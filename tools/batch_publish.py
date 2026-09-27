"""Strict batch publication: N candidates, one journaled transaction (integ28).

Every candidate is verified individually first: a fresh probe of its complete
contribution, its ownership change against the canonical manifest (with fresh
probes of every owner it subsumes) and, unless disabled for a research run, an
independent DOSBox-X compile/assembly with the same binders.  The surviving
candidates are then composed into ONE staged manifest and checked by ONE fresh
whole-image build (every active contribution recompiled, ordered relocations
compared).  A failing candidate is dropped and the rest are retried with a new
fresh build; nothing verified earlier is reused as a result.  Publication
writes all sources, recipes and the manifest under one journal, then the
canonical image is rebuilt freshly; any failure rolls the whole batch back.

Batch file: one candidate per line, `NAME CANDIDATE [RECIPE]`; `#` comments.
"""
import time
from pathlib import Path
from common import ROOT, identity, json_bytes, read_json, require, sha, write_json, atomic_bytes
import promote as P
from transaction import (exclusive, ensure_consistent, prepare, apply, finish, rollback,
                         invalidate_receipts, lock_free_snapshot, publishing)


def _log(message):
    print(time.strftime('%H:%M:%S'), message, flush=True)


def read_batch(path):
    entries = []
    for number, line in enumerate(Path(path).read_text(encoding='utf-8').splitlines(), 1):
        text = line.split('#', 1)[0].strip()
        if not text:
            continue
        parts = text.split()
        require(len(parts) in (2, 3), f'Batch line {number}: expected NAME CANDIDATE [RECIPE]')
        entries.append({'name': parts[0], 'candidate': parts[1],
                        'recipe': parts[2] if len(parts) == 3 else None})
    return entries


def _freeze(entries):
    import re
    frozen, names = [], set()
    for entry in entries:
        name = entry['name']
        require(re.fullmatch(r'[A-Za-z_][A-Za-z0-9_]*', name), 'Unsafe function name: ' + name)
        require(name not in names, 'Duplicate batch candidate: ' + name)
        names.add(name)
        candidate = Path(entry['candidate']).resolve()
        recipe_path = Path(entry['recipe']).resolve() if entry.get('recipe') else None
        frozen.append({'name': name, 'candidate': candidate, 'source': candidate.read_bytes(),
                       'recipe_path': recipe_path,
                       'recipe_data': recipe_path.read_bytes() if recipe_path else None})
    return frozen


def _unchanged(entry):
    return (entry['candidate'].read_bytes() == entry['source'] and
            (entry['recipe_path'] is None or entry['recipe_path'].read_bytes() == entry['recipe_data']))


def _overrides(entries):
    return ({'recipes/'+e['name']+'.json': e['recipe'] for e in entries},
            {e['destination']: e['source'] for e in entries})


def _union(manifest, entries, oracle, image, dropped, log):
    """Compose ownership changes in batch order; a conflicting one is dropped."""
    import memo
    union, kept = manifest, []
    with memo.session():
        for entry in entries:
            try:
                union = P.apply_ownership(union, entry['name'], entry['recipe'], oracle, image)
                kept.append(entry)
            except Exception as error:
                dropped[entry['name']] = {'stage': 'compose', 'error': str(error)}
                log(f"DROP {entry['name']} (ownership composition): {error}")
    return union, kept


def _check_inputs(before):
    from transaction import SnapshotChanged
    if P.inputs() != before:
        raise SnapshotChanged('Canonical inputs changed during batch staging; nothing was accepted. Retry.')


def stage_batch(entries, before, *, independent=True, log=None, builder=None, independent_check=None):
    """All batch gates except publication; returns (survivors, manifest, staged, dropped)."""
    import memo
    from oracle import verify
    from mz import MZ
    builder = builder or P.build
    log = log or _log
    dropped, checked = {}, []
    manifest = read_json(ROOT/'layout/manifest.json')
    oracle = verify(write=False)
    image = MZ.parse(oracle[1]).load_image(oracle[1])
    if independent and independent_check is None:
        from crosscheck_runner import independent_row as independent_check
    with memo.session():
        for entry in entries:
            started = time.time()
            try:
                recipe, destination, payload, fast, _ = P.check_candidate(
                    entry['name'], entry['source'], entry['recipe_data'], manifest, oracle, image)
                entry.update(recipe=recipe, destination=destination, payload=payload, fast=fast)
                if independent:
                    entry['independent'] = independent_check(recipe, entry['source'], oracle, image)
                checked.append(entry)
                log(f"OK   {entry['name']} individual{' + DOSBox-X' if independent else ''} "
                    f"({len(payload)} B, {time.time()-started:.1f} s)")
            except Exception as error:
                _check_inputs(before)
                dropped[entry['name']] = {'stage': 'individual', 'error': str(error)}
                log(f"DROP {entry['name']} (individual verification): {error}")
    survivors = checked
    while survivors:
        union, survivors = _union(manifest, survivors, oracle, image, dropped, log)
        if not survivors:
            break
        recipes, sources = _overrides(survivors)
        started = time.time()
        try:
            staged = builder(union, recipes, publish=False, source_overrides=sources)
            log(f"OK   union staged whole-image build of {len(survivors)} candidates "
                f"({time.time()-started:.1f} s)")
            require(P.inputs() == before and staged['inputs'] == before,
                    'Canonical inputs changed during batch acceptance')
            return survivors, union, staged, dropped
        except Exception as error:
            # A concurrent canonical change is never a candidate failure.
            _check_inputs(before)
            culprits = [e for e in survivors
                        if getattr(error, 'owner_recipe', None) == 'recipes/'+e['name']+'.json']
            if culprits:
                for e in culprits:
                    dropped[e['name']] = {'stage': 'union', 'error': str(error)}
                    log(f"DROP {e['name']} (union whole-image build): {error}")
                survivors = [e for e in survivors if e not in culprits]
                continue
            if len(survivors) == 1:
                dropped[survivors[0]['name']] = {'stage': 'union', 'error': str(error)}
                log(f"DROP {survivors[0]['name']} (union whole-image build): {error}")
                survivors = []
                break
            # Not attributable to one contribution (e.g. image-level order):
            # bisect for the longest passing prefix of the batch order, each
            # step a new fresh whole-image build; drop the first breaking one.
            log(f"BISECT union failure not attributable to one candidate: {error}")
            good, bad = 0, len(survivors)
            while bad - good > 1:
                middle = (good + bad) // 2
                trial, kept = _union(manifest, survivors[:middle], oracle, image, {}, log)
                _check_inputs(before)
                recipes, sources = _overrides(kept)
                try:
                    require(len(kept) == middle, 'Prefix composition differs')
                    builder(trial, recipes, publish=False, source_overrides=sources)
                    good = middle
                except Exception:
                    _check_inputs(before)
                    bad = middle
            culprit = survivors[bad-1]
            dropped[culprit['name']] = {'stage': 'union-bisect', 'error': str(error)}
            log(f"DROP {culprit['name']} (first candidate breaking the union build)")
            survivors = [e for e in survivors if e is not culprit]
    return [], manifest, None, dropped


def publish_batch(batch_entries, *, verify_only=False, independent=True, log=None):
    log = log or _log
    entries = _freeze(batch_entries)
    stamp = time.strftime('%Y%m%dT%H%M%S')
    summary = {'batch': stamp, 'verify_only': verify_only, 'independent': independent,
               'candidates': [e['name'] for e in entries]}
    started = time.time()
    if verify_only:
        def action(before):
            survivors, _, staged, dropped = stage_batch(entries, before, independent=independent, log=log)
            return survivors, staged, dropped, before
        survivors, staged, dropped, before = lock_free_snapshot(action, P.inputs)
        require(all(_unchanged(e) for e in entries), 'Candidate changed during batch verification')
        summary.update(status='VERIFIED_ONLY', inputs=before)
    else:
        with exclusive():
            ensure_consistent()
            before = P.inputs()
            survivors, union, staged, dropped = stage_batch(entries, before, independent=independent, log=log)
            require(all(_unchanged(e) for e in entries), 'Candidate changed during batch acceptance')
            if survivors:
                changes = {}
                for e in survivors:
                    changes[e['destination']] = e['source']
                    changes['recipes/'+e['name']+'.json'] = json_bytes(e['recipe'])
                changes['layout/manifest.json'] = json_bytes(union)
                expected = {**before, **{p: sha(raw) for p, raw in changes.items()}}
                with publishing():
                    rows = prepare(changes)
                    try:
                        require(P.inputs() == before, 'Canonical inputs changed before publication')
                        invalidate_receipts()
                        apply(rows)
                        require(P.inputs() == expected, 'Unexpected edits during publication')
                        artifact = {}
                        accepted = P.build(publish=False, allow_pending=True, artifact=artifact)
                        require(P.inputs() == expected and accepted['inputs'] == expected,
                                'Unexpected edits during canonical verification')
                        require(all(_unchanged(e) for e in entries),
                                'Candidate changed before publication completed')
                        finish()
                        require(P.inputs() == expected, 'Inputs changed before acceptance receipt publication')
                        atomic_bytes(ROOT/'build/exact/mcga.exe', artifact['executable'])
                        write_json(ROOT/'build/exact/acceptance.json', accepted)
                    except BaseException:
                        invalidate_receipts()
                        rollback()
                        raise
                log(f"PUBLISHED {len(survivors)} candidates; canonical fresh HYBRID_EXACT "
                    f"({accepted['relocation_count']} ordered relocations)")
                summary.update(inputs=expected, canonical=accepted['executable'],
                               matching_c_bytes=accepted['matching_c_bytes'],
                               matching_asm_bytes=accepted['matching_asm_bytes'],
                               raw_initialized_bytes=accepted['raw_initialized_bytes'])
            summary.update(status='PROMOTED' if survivors else 'NOTHING_PUBLISHED')
    summary.update(seconds=round(time.time()-started, 1),
                   published=[{'name': e['name'], 'bytes': len(e['payload']),
                               'source': identity(e['source']),
                               'independent': bool(e.get('independent'))} for e in survivors],
                   dropped=dropped)
    if staged is not None:
        summary['staged_whole_image'] = staged['executable']
    for e in survivors:
        write_json(ROOT/'build/acceptance'/e['name']/'report.json',
                   {'status': 'VERIFIED_ONLY' if verify_only else 'PROMOTED', 'function': e['name'],
                    'batch': stamp, 'source': identity(e['source']), 'bytes': len(e['payload']),
                    'whole_image': staged['executable'], 'relocation_count': staged['relocation_count'],
                    'fast': e['fast'], 'inputs': summary['inputs']})
    write_json(ROOT/'build/acceptance/_batches'/(stamp+'.json'), summary)
    return summary
