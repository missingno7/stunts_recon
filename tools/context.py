"""Small read-only evidence packet for a function, recipe, and scratch runs."""
import argparse
import json
from pathlib import Path

from common import ROOT, read_json, sha, identity, json_bytes

EVIDENCE = Path('evidence/functions.json')
MANIFEST = Path('layout/manifest.json')


def _inventory():
    return read_json(ROOT / EVIDENCE)


def _manifest():
    return read_json(ROOT / MANIFEST)


def _recipes(manifest):
    result = {}
    for owner in manifest.get('owners', []):
        if owner.get('kind') == 'MATCHING_C' and owner.get('recipe'):
            result[owner.get('name')] = owner
    return result


def _resolve(identifier, inventory):
    rows = inventory.get('functions', [])
    matches = [row for row in rows if identifier in (row.get('name'), row.get('stable_id'))]
    if not matches:
        # Manifest IDs include unresolved/raw extents, which are useful context
        # targets even when the source inventory has no function label for them.
        owners = _manifest().get('owners', [])
        matches = [row for row in owners if identifier in (row.get('id'), row.get('name'))]
    if len(matches) != 1:
        raise ValueError('Unknown or ambiguous function. Use an exact name or stable ID from evidence/functions.json or layout/manifest.json.')
    return matches[0]


def _search_history(name, limit=None):
    root = ROOT / 'build/search'
    found = []
    if root.is_dir():
        for path in root.glob('*/report.json'):
            try:
                report = read_json(path)
            except (OSError, ValueError):
                continue
            if (report.get('function') or {}).get('name') == name:
                found.append(report)
    found.sort(key=lambda report: report.get('created_utc', ''), reverse=True)
    count = len(found)
    if limit is not None:
        found = found[:limit]
    rows = []
    for report in found:
        candidate = report.get('candidate') or {}
        compiler = report.get('compiler') or {}
        comparison = report.get('comparison') or {}
        rows.append({
            'run_id': report.get('run_id'),
            'candidate_sha256': candidate.get('sha256'),
            'source_name': candidate.get('name'),
            'profile': compiler.get('profile'),
            'compile_status': compiler.get('status'),
            'output_identity': (report.get('observed_output') or {}).get('identity'),
            'diagnostic': comparison.get('summary'),
            'strict_recipe_result': report.get('recipe_check', {}).get('status'),
            'report': str((root / report.get('run_id', '') / 'report.json').relative_to(ROOT))
        })
    return rows, count


def _oracle_target(row):
    from oracle import verify
    from mz import MZ
    result = verify(write=False)
    image = MZ.parse(result[1]).load_image(result[1])
    start, end = row.get('start'), row.get('end')
    if not isinstance(start, int) or not isinstance(end, int) or not (0 <= start <= end <= len(image)):
        raise ValueError('Evidence range is outside the pristine oracle image')
    data = image[start:end]
    expected = row.get('sha256')
    if expected and sha(data) != expected:
        raise ValueError('Evidence function bytes do not match the locked pristine oracle')
    return data, result


def packet(identifier, expansions=()):
    """Return direct evidence and active-recipe context without workflow state."""
    inventory = _inventory()
    manifest = _manifest()
    row = _resolve(identifier, inventory)
    expansion = set(expansions)
    owner = _recipes(manifest).get(row.get('name'))
    recipe_path = owner.get('recipe') if owner else None
    recipe = read_json(ROOT / recipe_path) if recipe_path else None
    is_manifest_extent = row in manifest.get('owners', [])

    out = {
        'name': row.get('name', row.get('id')),
        'stable_id': row.get('stable_id', row.get('id')),
        'extent': {key: row.get(key) for key in ('start', 'end', 'size', 'sha256', 'segment', 'segment_offset') if key in row},
        'evidence_status': row.get('status', row.get('classification', row.get('kind', 'unclassified'))),
        'confidence': row.get('confidence'),
        'issues': row.get('issues', []),
        'origin': row.get('origin'),
        'provenance': row.get('provenance'),
        'source_locations': row.get('c_sources', []),
        'manifest': ({key: owner.get(key) for key in ('id', 'kind', 'classification', 'start', 'end', 'recipe') if key in owner}
                     if owner else ({key: row.get(key) for key in ('id', 'kind', 'classification', 'start', 'end') if key in row}
                                    if is_manifest_extent else None)),
        'recipe': ({'path': recipe_path, 'source': recipe.get('source'), 'profile': recipe.get('profile'),
                    'object_segment': recipe.get('object_segment'), 'public': recipe.get('public'),
                    'target': recipe.get('target'), 'expected_fixups': recipe.get('expected_fixups', []),
                    'expected_relocations': recipe.get('expected_relocations', [])}
                   if recipe else None),
        'evidence_inventory': {'path': str(EVIDENCE).replace('\\', '/'), 'sha256': sha((ROOT / EVIDENCE).read_bytes())},
        'expansion_options': ['--asm', '--callers', '--symbols', '--history', '--raw'],
        'authority': 'Evidence-qualified research context. Semantic source leads do not prove original translation-unit membership or machine-level callers.'
    }

    if 'asm' in expansion:
        raw, _ = _oracle_target(row)
        from diagnostics import decode
        out['assembly'] = decode(raw, row['start'])
        out['assembly_scope'] = 'Pristine bytes linearly decoded with the pinned diagnostic decoder. This is not a reviewed CFG, reachability analysis, or proof of C/assembly authorship.'

    if 'callers' in expansion:
        callers = []
        for candidate in inventory.get('functions', []):
            for call in candidate.get('calls', []):
                if call.get('target') == row.get('name'):
                    callers.append({'name': candidate.get('name'), 'stable_id': candidate.get('stable_id'),
                                    'confidence': call.get('confidence'), 'origin': candidate.get('origin')})
        out['callers'] = callers
        out['caller_scope'] = 'Reverse index of recorded semantic call leads; not independently verified machine-level call sites.'

    if 'symbols' in expansion:
        related = set()
        if recipe:
            related.update(recipe.get('binding', {}).get('declarations', {}).get('externals', []))
            related.update(f.get('target') for f in recipe.get('expected_fixups', []) if f.get('target'))
        code_symbols, data_symbols = {}, {}
        code_path, data_path = ROOT / 'layout/code-symbols.json', ROOT / 'layout/data-symbols.json'
        if code_path.is_file():
            code_layout = read_json(code_path)
            code_symbols = {name: code_layout.get('symbols', {}).get(name) for name in sorted(related)
                            if name in code_layout.get('symbols', {})}
        if data_path.is_file():
            data_layout = read_json(data_path)
            data_symbols = {name: data_layout.get('symbols', {}).get(name) for name in sorted(related)
                            if name in data_layout.get('symbols', {})}
        out['symbols'] = {
            'global_evidence': [g for g in inventory.get('globals', []) if g.get('name') in related],
            'code_bindings': code_symbols,
            'data_bindings': data_symbols,
            'recipe_symbols': sorted(related),
            'function_calls': row.get('calls', []),
            'scope': 'Only names present in evidence/functions.json or the selected recipe are shown; unresolved bindings remain unresolved.'
        }

    history, count = _search_history(out['name'], None if 'history' in expansion else 3)
    if history or 'history' in expansion:
        out['search_history'] = history
        out['omitted_search_runs'] = max(0, count - len(history))

    if 'raw' in expansion:
        raw, oracle_result = _oracle_target(row)
        out['raw_target'] = {'size': len(raw), 'sha256': sha(raw), 'bytes_hex': raw.hex(),
                             'oracle_load_image': oracle_result[2]['load_image']}

    return out


def listing(query=None):
    """Compact inventory/manifest view for selecting a target by evidence."""
    inventory = _inventory()
    manifest = _manifest()
    owners = manifest.get('owners', [])
    rows = []
    mapped_names = set()
    for function in inventory.get('functions', []):
        owner = next((item for item in owners if item.get('name') == function.get('name') or
                      item.get('id') == function.get('stable_id')), None)
        name = function.get('name')
        mapped_names.add(name)
        item = {'name': name, 'id': function.get('stable_id'), 'start': function.get('start'),
                'end': function.get('end'), 'size': function.get('size'),
                'evidence_status': function.get('status', function.get('confidence')),
                'owner_kind': owner.get('kind') if owner else 'UNMAPPED_EVIDENCE',
                'accepted': bool(owner and owner.get('kind') in ('MATCHING_C', 'KNOWN_TOOLCHAIN_LIBRARY')),
                'raw': bool(owner and owner.get('kind') == 'UNRESOLVED_RAW')}
        if not query or query.casefold() in str(item).casefold():
            rows.append(item)
    for owner in owners:
        if owner.get('kind') != 'UNRESOLVED_RAW' or owner.get('name') in mapped_names:
            continue
        item = {'name': owner.get('name', owner.get('id')), 'id': owner.get('id'),
                'start': owner.get('start'), 'end': owner.get('end'),
                'size': owner.get('end', 0) - owner.get('start', 0),
                'evidence_status': owner.get('classification', owner.get('kind')),
                'owner_kind': owner.get('kind'), 'accepted': False, 'raw': True}
        if not query or query.casefold() in str(item).casefold():
            rows.append(item)
    rows.sort(key=lambda row: (row.get('start') if isinstance(row.get('start'), int) else -1,
                               str(row.get('name') or '')))
    return {'functions': rows, 'count': len(rows),
            'authority': 'Inventory view only; accepted/raw reflect current manifest ownership labels and do not rank research priority or establish new eligibility.'}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('function', nargs='?', help='Exact name or stable ID; optional substring filter with --list')
    parser.add_argument('--list', action='store_true', help='List inventory functions and manifest ownership')
    for name in ('asm', 'callers', 'symbols', 'history', 'raw'):
        parser.add_argument('--' + name, action='store_true')
    args = parser.parse_args()
    if args.list:
        print(json.dumps(listing(args.function), indent=2))
        return
    if not args.function:
        parser.error('provide a function name/ID or use --list [NAME_PART]')
    result = packet(args.function, [name for name in ('asm', 'callers', 'symbols', 'history', 'raw') if getattr(args, name)])
    print(json.dumps(result, indent=2))


if __name__ == '__main__':
    main()
