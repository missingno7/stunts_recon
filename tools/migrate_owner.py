"""Journaled owner-id and source-file migration with byte-identity proof.

The operation changes reconstruction labels and paths only. It does not change
the oracle, object extents, recipe bindings, machine bytes, or relocation order.
"""
import argparse
import copy
import json
import re
from pathlib import Path

from common import ROOT, identity, json_bytes, read_json, require, sha, write_json
import promote as P
from transaction import (exclusive, ensure_consistent, prepare, apply, finish, rollback,
                          invalidate_receipts, lock_free_snapshot, publishing)


_OWNED_KINDS = ('MATCHING_ASM', 'MATCHING_ASM_DATA')
_TOKEN = lambda name: re.compile(r'(?<![A-Za-z0-9_])'+re.escape(name)+r'(?![A-Za-z0-9_])')


def _ordered_json_bytes(value):
    return (json.dumps(value, indent=2, ensure_ascii=False) + '\n').encode('utf-8')


def _replace_text(value, owner_names, paths):
    if not isinstance(value, str):
        return value
    for old, new in sorted(paths.items(), key=lambda item: -len(item[0])):
        value = value.replace(old, new)
    for old, new in sorted(owner_names.items(), key=lambda item: -len(item[0])):
        value = _TOKEN(old).sub(new, value)
    return value


def _rewrite_tree(value, owner_names, paths):
    if isinstance(value, dict):
        return {_replace_text(key, owner_names, paths): _rewrite_tree(item, owner_names, paths)
                for key, item in value.items()}
    if isinstance(value, list):
        return [_rewrite_tree(item, owner_names, paths) for item in value]
    return _replace_text(value, owner_names, paths)


def _relative(value):
    path = Path(value.replace('\\', '/'))
    require(not path.is_absolute() and '..' not in path.parts,
            'Migration mapping contains an unsafe project path')
    return path.as_posix()


def plan_migration(mapping_document, root=ROOT):
    """Build the complete source/recipe/layout/evidence migration in memory."""
    root = Path(root).resolve()
    document = (read_json(mapping_document) if not isinstance(mapping_document, dict)
                else copy.deepcopy(mapping_document))
    rows = document.get('mappings')
    require(document.get('schema') == 1 and isinstance(rows, list) and rows and
            document.get('mapping_count') == len(rows),
            'Owner migration mapping schema/count differs')

    manifest_path = root/'layout/manifest.json'
    manifest = json.loads(manifest_path.read_text(encoding='utf-8'))
    by_id = {row['id']: row for row in manifest['owners']}
    owner_names, path_names, pairs = {}, {}, []
    seen_new_ids, seen_new_paths = set(), set()
    for row in rows:
        old = row['current_owner']; new = row['proposed_owner']
        old_source = _relative(row['current_file']); new_source = _relative(row['proposed_file'])
        require(old not in owner_names and new not in seen_new_ids,
                'Owner migration IDs are not one-to-one')
        require(old_source not in path_names and new_source not in seen_new_paths,
                'Owner migration paths are not one-to-one')
        owner_names[old] = new
        path_names[old_source] = new_source
        old_recipe = 'recipes/'+old+'.json'; new_recipe = 'recipes/'+new+'.json'
        path_names[old_recipe] = new_recipe
        seen_new_ids.add(new); seen_new_paths.add(new_source)
        owner = by_id.get(old)
        require(owner is not None and owner['kind'] == 'MATCHING_ASM' and
                owner.get('recipe') == old_recipe and owner.get('name') == old,
                'Migration target is not one complete active ASM owner: '+old)
        recipe_file = root/old_recipe
        source_file = root/old_source
        require(recipe_file.is_file() and source_file.is_file(),
                'Migration recipe/source is missing: '+old)
        recipe = json.loads(recipe_file.read_text(encoding='utf-8'))
        require(recipe.get('kind') == 'asm' and recipe.get('id') == old and
                recipe.get('source') == old_source,
                'Migration recipe identity/source differs: '+old)
        require(not (root/new_recipe).exists() and not (root/new_source).exists(),
                'Migration destination already exists: '+new)
        pairs.append({'old_owner': old, 'new_owner': new,
                      'old_recipe': old_recipe, 'new_recipe': new_recipe,
                      'old_source': old_source, 'new_source': new_source,
                      'basis': row.get('basis', ''), 'owner': copy.deepcopy(owner),
                      'recipe': recipe, 'recipe_bytes': recipe_file.read_bytes(),
                      'source_bytes': source_file.read_bytes()})

    require(not (set(owner_names.values()) & set(by_id)),
            'A proposed owner ID is already active')
    new_manifest = _rewrite_tree(manifest, owner_names, path_names)
    new_recipes = {}
    changes = {}
    for pair in pairs:
        recipe = _rewrite_tree(pair['recipe'], owner_names, path_names)
        require(recipe.get('id') == pair['new_owner'] and
                recipe.get('source') == pair['new_source'],
                'Migrated recipe id/source was not updated')
        new_recipes[pair['new_recipe']] = recipe
        changes[pair['old_source']] = None
        changes[pair['new_source']] = pair['source_bytes']
        changes[pair['old_recipe']] = None
        changes[pair['new_recipe']] = _ordered_json_bytes(recipe)

    changes['layout/manifest.json'] = _ordered_json_bytes(new_manifest)
    # Rewrite direct references in other recipes and canonical layout/evidence
    # records. Historical documentation is retained; the new mapping is added
    # below with the provenance artifact.
    for folder in ('recipes', 'layout', 'evidence'):
        for path in sorted((root/folder).rglob('*')):
            if not path.is_file() or path.name == 'manifest.json' and folder == 'layout':
                continue
            relative = path.relative_to(root).as_posix()
            if relative in changes or path.suffix.lower() not in ('.json', '.md', '.txt'):
                continue
            raw = path.read_bytes()
            if path.suffix.lower() == '.json':
                try:
                    value = json.loads(raw.decode('utf-8'))
                except (UnicodeDecodeError, json.JSONDecodeError):
                    continue
                rewritten = _rewrite_tree(value, owner_names, path_names)
                after = _ordered_json_bytes(rewritten) if rewritten != value else raw
            else:
                text = raw.decode('utf-8')
                after = _replace_text(text, owner_names, path_names).encode('utf-8')
            if after != raw:
                changes[relative] = after

    link_path = 'layout/link-objects.json'
    link_data = json.loads((root/link_path).read_text(encoding='utf-8'))
    link_ids_changed = sorted((item['id'], owner_names[item['id']])
                              for item in link_data.get('objects', [])
                              if item.get('id') in owner_names)

    overrides = {path: value for path, value in new_recipes.items()}
    for relative, raw in changes.items():
        if relative.startswith('recipes/') and raw is not None:
            overrides[relative] = json.loads(raw.decode('utf-8'))
    sources = {pair['new_source']: pair['source_bytes'] for pair in pairs}
    return {'mapping_document': document, 'pairs': pairs, 'changes': changes,
            'manifest': new_manifest, 'recipe_overrides': overrides,
            'source_overrides': sources, 'link_object_ids_changed': link_ids_changed}


def _same_build(left, right):
    return (left.get('executable') == right.get('executable') and
            left.get('relocation_count') == right.get('relocation_count') and
            left.get('status') == right.get('status'))


def verify_plan(plan, before_inputs):
    """Compare every old/new OMF and staged whole image before publication."""
    objects = []
    for pair in plan['pairs']:
        old_recipe = pair['recipe']
        new_recipe = plan['recipe_overrides'][pair['new_recipe']]
        old_obj = P._cosmetic_compile(old_recipe, pair['source_bytes'])
        new_obj = P._cosmetic_compile(new_recipe, pair['source_bytes'])
        old_id, new_id = identity(old_obj), identity(new_obj)
        require(old_id == new_id,
                'Owner migration changed full OMF identity: '+pair['old_owner'])
        pair['source_identity'] = identity(pair['source_bytes'])
        pair['recipe_identity_before'] = identity(pair['recipe_bytes'])
        pair['recipe_identity_after'] = identity(_ordered_json_bytes(new_recipe))
        pair['object_identity_before'] = old_id
        pair['object_identity_after'] = new_id
        objects.append({'owner_before': pair['old_owner'], 'owner_after': pair['new_owner'],
                        'source_before': pair['old_source'], 'source_after': pair['new_source'],
                        'recipe_before': pair['old_recipe'], 'recipe_after': pair['new_recipe'],
                        'source': pair['source_identity'], 'object_before': old_id,
                        'object_after': new_id})

    baseline = P.build(publish=False, allow_pending=True)
    staged = P.build(plan['manifest'], plan['recipe_overrides'], publish=False,
                     source_overrides=plan['source_overrides'], allow_pending=True)
    require(_same_build(baseline, staged),
            'Owner migration changed the full executable or ordered relocation count')
    require(P.inputs() == before_inputs, 'Canonical inputs changed during migration verification')
    return {'objects': objects, 'baseline_executable': baseline['executable'],
            'staged_executable': staged['executable'],
            'relocation_count': baseline.get('relocation_count'),
            'link_object_ids_changed': plan['link_object_ids_changed'],
            'owner_count': len(objects), 'status': 'VERIFIED'}


def _provenance(plan, proof):
    path = ROOT/'evidence/module-owner-renames.json'
    current = json.loads(path.read_text(encoding='utf-8')) if path.exists() else {
        'schema': 'module-owner-renames-v1', 'migrations': []}
    require(current.get('schema') == 'module-owner-renames-v1' and
            isinstance(current.get('migrations'), list), 'Owner migration provenance schema differs')
    mapping_sha = sha(json_bytes(plan['mapping_document']))
    require(not any(row.get('mapping_sha256') == mapping_sha for row in current['migrations']),
            'This mapping already has provenance')
    by_owner = {row['owner_before']: row for row in proof['objects']}
    records = []
    for pair in plan['pairs']:
        record = by_owner[pair['old_owner']]
        records.append({'old_owner': pair['old_owner'], 'new_owner': pair['new_owner'],
                        'old_source': pair['old_source'], 'new_source': pair['new_source'],
                        'old_recipe': pair['old_recipe'], 'new_recipe': pair['new_recipe'],
                        'extent': [pair['owner']['start'], pair['owner']['end']],
                        'basis': pair['basis'], 'source': record['source'],
                        'object_before': record['object_before'],
                        'object_after': record['object_after']})
    current['migrations'].append({'mapping_sha256': mapping_sha,
                                  'executable_before': proof['baseline_executable'],
                                  'executable_after': proof['staged_executable'],
                                  'relocation_count': proof['relocation_count'],
                                  'link_object_ids_changed': proof['link_object_ids_changed'],
                                  'owners': records})
    return current


def _acceptance_section(plan, proof):
    lines = ['\n\n# Module owner and source migration (integ45)', '',
             '**Result.** The 18 source-backed ASM owner IDs, recipe paths, and source paths were migrated as one journaled transaction. The complete pinned-tool OMF object hash stayed identical for every row; the staged executable hash and ordered relocation count stayed identical to the pre-migration build.', '',
             '**Provenance.** `evidence/module-owner-renames.json` records each extent, original and migrated paths, source identity, full OMF identity, and whole executable identities. `tools/migrate_owner.py` performs the schema-aware rewrite under the acceptance writer lock and rolls back if the rebuilt image differs.', '',
             '| Previous owner | New owner | Previous source | New source | Basis |',
             '|---|---|---|---|---|']
    for pair in plan['pairs']:
        lines.append(f"| `{pair['old_owner']}` | `{pair['new_owner']}` | `{pair['old_source']}` | `{pair['new_source']}` | {pair['basis']} |")
    lines += ['', '**Physical link-object IDs.** `layout/link-objects.json` IDs are changed only when they exactly identify one migrated owner; a physical object that begins before or ends after a migrated source owner retains its physical extent identity.', '']
    return '\n'.join(lines) + '\n'


def _expected_inputs(before, changes):
    result = {path: digest for path, digest in before.items() if path not in changes or changes[path] is not None}
    # docs/ is journaled and published with the migration, but is intentionally
    # outside build_exact.inputs(): documentation does not affect the accepted
    # object/image input identity.
    result.update({path: sha(raw) for path, raw in changes.items()
                   if raw is not None and path.split('/', 1)[0] != 'docs'})
    return result


def migrate(mapping_document, *, publish=False):
    def inspect(before):
        plan = plan_migration(mapping_document)
        proof = verify_plan(plan, before)
        plan['proof'] = proof
        plan['changes']['evidence/module-owner-renames.json'] = json_bytes(_provenance(plan, proof))
        acceptance = (ROOT/'docs/acceptance.md').read_bytes()
        plan['changes']['docs/acceptance.md'] = acceptance + _acceptance_section(plan, proof).encode('utf-8')
        require(P.inputs() == before, 'Canonical inputs changed during migration verification')
        return plan

    if not publish:
        plan = lock_free_snapshot(inspect, P.inputs)
        report = {'status': 'VERIFIED_ONLY', 'mapping_count': len(plan['pairs']),
                  'proof': plan['proof'], 'changes': sorted(plan['changes'])}
        write_json(ROOT/'build/owner-migration/verify-only.json', report)
        return report

    with exclusive():
        ensure_consistent()
        before = P.inputs()
        plan = inspect(before)
        changes = plan['changes']
        expected = _expected_inputs(before, changes)
        with publishing():
            journal = prepare(changes)
            try:
                require(P.inputs() == before, 'Canonical inputs changed before owner migration')
                invalidate_receipts()
                apply(journal)
                require(P.inputs() == expected, 'Owner migration inputs differ from its plan')
                accepted = P.build(publish=False, allow_pending=True)
                require(accepted.get('executable') == plan['proof']['staged_executable'] and
                        accepted.get('relocation_count') == plan['proof']['relocation_count'],
                        'Published owner migration changed the complete executable')
                require(P.inputs() == expected,
                        'Inputs changed before owner migration provenance completed')
                finish()
            except BaseException:
                invalidate_receipts()
                rollback()
                raise
        report = {'status': 'PUBLISHED', 'mapping_count': len(plan['pairs']),
                  'proof': plan['proof'], 'inputs': expected,
                  'changed_paths': sorted(changes)}
        write_json(ROOT/'build/owner-migration/published.json', report)
        return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('mapping', type=Path, help='Reviewed owner mapping JSON')
    mode = parser.add_mutually_exclusive_group(required=True)
    mode.add_argument('--verify-only', action='store_true')
    mode.add_argument('--publish', action='store_true')
    args = parser.parse_args()
    report = migrate(args.mapping, publish=args.publish)
    print(report['status'], report['mapping_count'], 'owner/source migrations',
          report.get('proof', {}).get('relocation_count', ''))


if __name__ == '__main__':
    main()
