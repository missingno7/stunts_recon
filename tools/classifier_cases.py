#!/usr/bin/env python3
"""Known-case and metamorphic checks for classify.py (diagnostic tool; grants nothing).

Each case states the expected (primary_state, comparison) for named members and the
evidence it rests on.  Run: python build/workers/classifier/validate_cases.py [--quick]
"""
import json
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[0]
sys.path.insert(0, str(HERE))
import classify as C  # noqa: E402

FIX = ROOT / 'build/workers/classifier/fixtures'
PFS = ROOT / 'build/workers/s008obj/parse_filepath_separators.c'


def variant(name, old, new):
    text = PFS.read_text()
    assert old in text, (name, old)
    path = FIX / f'pfs_{name}.c'
    path.write_text(text.replace(old, new, 1))
    return path


CASES = [
    # (label, source, selection kwargs, register, {member: (primary or None, comparison or None, required field category or None)})
    ('accepted control', PFS, {'function': 'parse_filepath_separators'}, None,
     {'parse_filepath_separators': ('ACCEPTED', 'BYTE_EXACT', None)}),
    ('metamorphic: constant changed', 'pfs_const', {'function': 'parse_filepath_separators'}, None,
     {'parse_filepath_separators': ('ACCEPTED', 'CODEGEN_MISMATCH', None)}),
    ('metamorphic: far callee renamed', 'pfs_callee', {'function': 'parse_filepath_separators'}, None,
     {'parse_filepath_separators': ('ACCEPTED', 'BLOCKED_SYMBOL', 'SYMBOL_NAME_DIFFERS')}),
    ('metamorphic: callee renamed to another known function', 'pfs_wrongcall', {'function': 'parse_filepath_separators'}, None,
     {'parse_filepath_separators': ('ACCEPTED', 'CODEGEN_MISMATCH', 'CALL_TARGET_DIFFERS')}),
    ('lsapply bto_final (code exact, one unknown DGROUP alias)', ROOT / 'build/workers/lsapply/bto_final.c',
     {'members': 'build_track_object,subst_hillroad_track'}, None,
     {'build_track_object': ('BLOCKED_SYMBOL', 'BLOCKED_SYMBOL', 'SYMBOL_UNKNOWN_TARGET')}),
    ('lsapply dp_final (was class-2 alias; registry since corrected)', ROOT / 'build/workers/lsapply/dp_final.c',
     {'function': 'detect_penalty'}, None, {'detect_penalty': ('EXACT_OPEN_RECORD', 'BYTE_EXACT', None)}),
    ('seg008 TU data placement', ROOT / 'build/workers/lsapply/seg008_final.c',
     {'members': 'do_fileselect_dialog,file_load_3dres,do_dea_textres'}, None,
     {'do_fileselect_dialog': ('BLOCKED_TU_DATA', 'BLOCKED_TU_DATA', 'TU_DATA_PLACEMENT')}),
    ('s008obj wai/sav members: first mismatch is the literal, but genuine code differs later',
     ROOT / 'build/workers/s008obj/seg008_waitflag_candidate.c', {'members': 'show_waiting,do_savefile_dialog'}, None,
     {'show_waiting': ('CODEGEN_MISMATCH', 'CODEGEN_MISMATCH', None),
      'do_savefile_dialog': ('CODEGEN_MISMATCH', 'CODEGEN_MISMATCH', None)}),
    ('seg001 members', ROOT / 'build/workers/obj-s001/seg001_merged_best.c',
     {'members': 'player_op,update_grip,car_car_speed_adjust_maybe'}, None,
     {'player_op': ('CODEGEN_MISMATCH', 'CODEGEN_MISMATCH', None),
      'update_grip': ('CODEGEN_MISMATCH', 'CODEGEN_MISMATCH', None)}),
    ('seg029 before flags (HEAD register: sweep PLAUSIBLE /Oal)', ROOT / 'build/workers/tuflags3/seg029/seg029_tu.c',
     {'object_id': 'obj_seg029'}, FIX / 'toolchain-hypotheses.HEAD.json',
     {'audioresource_compare_chunknames': ('PROFILE_UNCERTAIN', 'CODEGEN_MISMATCH', None),
      'audioresource_copy_n_bytes': ('PROFILE_UNCERTAIN', 'CODEGEN_MISMATCH', None)}),
    ('seg029 after flags (register SUPPORTED /Ox, not yet production-reviewed)',
     ROOT / 'build/workers/tuflags3/seg029/seg029_tu.c', {'object_id': 'obj_seg029'}, None,
     {'audioresource_compare_chunknames': ('BYTE_EXACT', 'BYTE_EXACT', None),
      'audioresource_find': ('CODEGEN_EXACT', 'CODEGEN_EXACT', 'SYMBOL_CONSISTENT'),
      'audioresource_copy_n_bytes': ('BOUNDARY_UNCERTAIN', 'BYTE_EXACT', None)}),
    ('hook: symbol receipts turn the unknown alias into CODEGEN_EXACT (never BYTE_EXACT)',
     ROOT / 'build/workers/lsapply/bto_final.c', {'members': 'build_track_object,subst_hillroad_track'},
     {'symbol_receipts': FIX / 'hook_receipts.json'},
     {'build_track_object': ('CODEGEN_EXACT', 'CODEGEN_EXACT', 'RECEIPT_CONSISTENT')}),
    ('hook: prefix proof marks detect_penalty RECORD_CLOSED_EXACT', ROOT / 'build/workers/lsapply/dp_final.c',
     {'function': 'detect_penalty'}, {'record_proof': FIX / 'hook_record_proof.json'},
     {'detect_penalty': ('RECORD_CLOSED_EXACT', 'BYTE_EXACT', None)}),
]


def main():
    quick = '--quick' in sys.argv
    FIX.mkdir(exist_ok=True)
    sources = {
        'pfs_const': variant('const', "== '.'", "== ','"),
        'pfs_callee': FIX / 'pfs_callee.c',
        'pfs_wrongcall': FIX / 'pfs_wrongcall.c',
    }
    (FIX / 'pfs_callee.c').write_text(PFS.read_text().replace('strlen', 'strlen_unknown'))
    (FIX / 'pfs_wrongcall.c').write_text(PFS.read_text().replace('strlen', 'toupper'))
    # Hook fixtures (synthetic, exercise the interfaces only; they prove nothing about the image).
    (FIX / 'hook_receipts.json').write_text(json.dumps({'symbols': {
        '_loopBase_InnXBounds0': {'load_address': 189890, 'kind': 'dgroup', 'note': 'synthetic hook test'}}}))
    (FIX / 'hook_record_proof.json').write_text(json.dumps({'members': {'detect_penalty': 'RECORD_CLOSED_EXACT'},
                                                            'note': 'synthetic hook test'}))
    contexts = {}
    results = []
    failures = 0
    for label, src, sel, register, expect in CASES:
        if quick and 'seg001' in label:
            continue
        src = sources.get(src, src)
        ctx_args = register if isinstance(register, dict) else {'register_path': register}
        ctx = contexts.setdefault(json.dumps(ctx_args, default=str), C.Context(**ctx_args))
        result = C.classify_source(src, ctx=ctx, with_mismatch_detail=False, **sel)
        got = {m['name']: m for m in result.get('members', [])}
        for name, (primary, comparison, category) in expect.items():
            m = got.get(name)
            ok = m is not None and (primary is None or m['primary_state'] == primary) and \
                (comparison is None or m.get('comparison') == comparison) and \
                (category is None or category in (m.get('fixups') or {}).get('categories', {}))
            failures += not ok
            row = {'case': label, 'member': name, 'ok': ok, 'expected': [primary, comparison, category],
                   'got': [m and m['primary_state'], m and m.get('comparison'),
                           m and (m.get('fixups') or {}).get('categories')] if m else result.get('error')}
            results.append(row)
            print(('PASS' if ok else 'FAIL'), label, '|', name, '|', row['got'])
    (ROOT / 'build/classifier/out').mkdir(parents=True, exist_ok=True)
    (ROOT / 'build/classifier/out/validate_cases.json').write_text(json.dumps(results, indent=1, default=str))
    print(f'{len(results) - failures}/{len(results)} expectations met')
    sys.exit(1 if failures else 0)


if __name__ == '__main__':
    main()
