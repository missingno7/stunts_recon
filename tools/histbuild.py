"""Build the reconstructed DOS program through the pinned historical toolchain.

This is the human entry point for a fresh source/assembly build and one real
LINK/EXEPACK run.  The independent game-library order comes from image-derived
DGROUP anchors; the oracle is used only after linking for comparison.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import re
import shutil
import sys
from collections import Counter
from datetime import datetime, timezone
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))

import reallink


MAKEFILE_TEMPLATE = ROOT / 'historical' / 'MAKEFILE.in'
RAW_KINDS = {'raw-code', 'raw-data', 'bss'}
SUMMARY_KINDS = (
    'MATCHING_C', 'MATCHING_C_DATA', 'MATCHING_ASM', 'MATCHING_ASM_DATA',
    'KNOWN_TOOLCHAIN_LIBRARY', 'KNOWN_TOOLCHAIN_LIBRARY_DATA', 'LINK_FILL',
    'BSS_IN_IMAGE', 'UNRESOLVED_RAW',
)


def sha(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def json_write(path: Path, value) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(value, indent=2, sort_keys=True) + '\n', encoding='utf-8')


def relative(path: Path) -> str:
    return path.resolve().relative_to(ROOT.resolve()).as_posix()


def ownership_summary(manifest: dict) -> dict:
    """Count each initialized interval once and keep raw BSS distinct."""
    by_kind = Counter()
    for owner in manifest.get('owners', []):
        by_kind[owner['kind']] += owner['end'] - owner['start']
    raw_rows = [o for o in manifest.get('owners', []) if o['kind'] == 'UNRESOLVED_RAW']
    raw_bss = [o for o in manifest.get('bss_owners', []) if o['kind'] == 'UNRESOLVED_RAW']
    bss_by_form = Counter()
    for row in raw_bss:
        bss_by_form[row.get('raw_form', 'unclassified')] += row['end'] - row['start']
    return {
        'owner_bytes': {kind: by_kind.get(kind, 0) for kind in SUMMARY_KINDS},
        'c_code_objects': sum(o['kind'] == 'MATCHING_C' for o in manifest.get('owners', [])),
        'asm_modules': sum(o['kind'] == 'MATCHING_ASM' for o in manifest.get('owners', [])),
        'pinned_runtime_code_members': sum(
            o['kind'] == 'KNOWN_TOOLCHAIN_LIBRARY' for o in manifest.get('owners', [])),
        'raw_initialized_bytes': sum(o['end'] - o['start'] for o in raw_rows),
        'raw_initialized_ranges': [
            {'id': o['id'], 'start': o['start'], 'end': o['end'],
             'bytes': o['end'] - o['start']} for o in sorted(raw_rows, key=lambda x: x['start'])],
        'raw_bss_bytes': sum(bss_by_form.values()),
        'raw_bss_by_form': dict(sorted(bss_by_form.items())),
        'raw_bss_ranges': [
            {'id': o['id'], 'start': o['start'], 'end': o['end'],
             'bytes': o['end'] - o['start'], 'raw_form': o.get('raw_form')}
            for o in sorted(raw_bss, key=lambda x: x['start'])],
    }


def parse_link_response(response: str) -> tuple[list[str], list[str]]:
    """Return explicit OBJ and library filenames from LINK's response text."""
    lines = [line.strip() for line in response.replace('\r', '').split('\n') if line.strip()]
    try:
        output_at = lines.index('RESULT.EXE')
    except ValueError as error:
        raise ValueError('LINK.RSP has no RESULT.EXE output row') from error
    explicit = []
    for line in lines[:output_at]:
        explicit.extend(token for token in line.rstrip('+').split('+') if token)
    if output_at + 2 >= len(lines):
        raise ValueError('LINK.RSP is missing its map or library row')
    library_field = lines[output_at + 2].split(';', 1)[0].split()[0]
    libraries = [x for x in library_field.split('+') if x]
    if not explicit or not libraries:
        raise ValueError('LINK.RSP must name explicit objects and libraries')
    return [name if name.upper().endswith('.OBJ') else name + '.OBJ' for name in explicit], libraries


def _macro(name: str, values: list[str], width: int = 78) -> str:
    """Wrap a DOS NMAKE macro with standard backslash continuations."""
    line = name + ' ='
    rows = []
    current = '    '
    for value in values:
        addition = value if current.strip() == '' else ' ' + value
        if len(current) + len(addition) > width and current.strip():
            rows.append(current.rstrip() + ' \\')
            current = '    ' + value
        else:
            current += addition
    if current.strip():
        rows.append(current.rstrip())
    return line + ' \\\n' + '\n'.join(rows) + '\n'


def render_makefile(template: str, explicit_objects: list[str], game_objects: list[str],
                    libraries: list[str]) -> str:
    replacements = {
        '@EXPLICIT_OBJECTS@': _macro('EXPLICIT_OBJECTS', explicit_objects).split(' =', 1)[1].lstrip(),
        '@GAME_OBJECTS@': _macro('GAME_OBJECTS', game_objects).split(' =', 1)[1].lstrip(),
        '@LINK_LIBRARIES@': _macro('LINK_LIBRARIES', libraries).split(' =', 1)[1].lstrip(),
    }
    output = template
    for token, value in replacements.items():
        output = output.replace(token, value)
    unresolved = re.findall(r'@[A-Z_]+@', output)
    if unresolved:
        raise ValueError(f'unexpanded MAKEFILE tokens: {unresolved}')
    return output.replace('\r\n', '\n').replace('\n', '\r\n')


def run_plan(link_options: list[str]) -> dict:
    """Fixed independent-order configuration used by the entry point."""
    return {
        'runtime': 'libraries', 'partial': 'split', 'order': 'library',
        'library_order': 'combined', 'game_order': 'image', 'game_input': 'library',
        'link_options': list(link_options),
    }


def _new_run_id(label: str | None) -> str:
    if label is None:
        label = 'build'
    if not re.fullmatch(r'[A-Za-z0-9_-]{1,24}', label):
        raise ValueError('tag must use 1..24 ASCII letters, digits, hyphens or underscores')
    stamp = datetime.now(timezone.utc).strftime('%Y%m%dT%H%M%S%fZ')
    return f'{label}-{stamp}'


def _capture_commands(link_report: dict) -> list[dict]:
    commands = []
    for row in link_report.get('accepted_objects', []):
        info = row.get('info', {})
        if info.get('invocation_kind') not in ('C', 'ASM'):
            continue
        commands.append({
            'kind': info['invocation_kind'], 'unit': row.get('unit'), 'piece': row.get('piece'),
            'source': info.get('source'), 'profile': info.get('profile'),
            'flags': info.get('flags', []), 'command': info.get('command'),
            'working_directory': info.get('working_directory'), 'cached': info.get('cached', False),
            'segments': row.get('segments', {}),
        })
    return commands


def _compare(linked_path: Path, packed_path: Path, link_report: dict) -> dict:
    from mz import MZ
    from exepack import unpack

    ctx = reallink.load_inputs()
    oracle_image = ctx.image[:reallink.IMAGE_INIT_END]
    linked_bytes = linked_path.read_bytes()
    linked_mz = MZ.parse(linked_bytes)
    linked_image = linked_mz.load_image(linked_bytes)
    if (reallink.INIT_DATA_END <= len(linked_image) < reallink.IMAGE_INIT_END and
            not any(ctx.image[len(linked_image):reallink.IMAGE_INIT_END])):
        linked_image += bytes(reallink.IMAGE_INIT_END - len(linked_image))
    sites = [r['load_offset'] for r in linked_mz.relocations]
    expected_sites = list(ctx.relocs)
    bank_sites = reallink.bank_partition(sites)
    positions_equal = sum(a == b for a, b in zip(bank_sites, expected_sites))
    per_bank = {}
    for bank in range(4):
        actual = [s for s in bank_sites if s >> 16 == bank]
        expected = [s for s in expected_sites if s >> 16 == bank]
        per_bank[str(bank)] = {
            'linked_relocations': len(actual), 'oracle_relocations': len(expected),
            'ordered_equal': actual == expected,
            'positions_equal': sum(a == b for a, b in zip(actual, expected)),
        }
    packed = packed_path.read_bytes()
    oracle_packed = (ROOT / 'build' / 'oracle' / 'mcga-packed.exe').read_bytes()
    unpacked, _trace = unpack(packed)
    unpacked_mz = MZ.parse(unpacked)
    unpacked_image = unpacked_mz.load_image(unpacked)
    header_fields = ('cs', 'ip', 'ss', 'sp', 'minalloc', 'maxalloc')
    oracle_header = ctx.oracle['unpacked_mz']
    header = {field: getattr(linked_mz, field) for field in header_fields}
    expected_header = {field: oracle_header[field] for field in header_fields}
    return {
        'load_image': {
            'equal': linked_image == oracle_image,
            'linked_size': len(linked_image), 'oracle_size': len(oracle_image),
            'mismatch_bytes': sum(a != b for a, b in zip(linked_image, oracle_image)) +
                              abs(len(linked_image) - len(oracle_image)),
            'first_mismatch': reallink.first_diff(linked_image, oracle_image),
        },
        'header': {'equal': header == expected_header, 'linked': header, 'oracle': expected_header},
        'relocations': {
            'set_equal': set(sites) == set(expected_sites),
            'ordered_equal': bank_sites == expected_sites,
            'linked_count': len(sites), 'oracle_count': len(expected_sites),
            'positions_equal': positions_equal,
            'banks': per_bank,
            'library_order_positions_equal': link_report.get('relocation_order_positions_equal'),
            'library_order_equal': link_report.get('relocation_order_equal'),
            'library_banks': link_report.get('relocation_unit_runs_by_bank', {}),
        },
        'packed': {
            'equal': packed == oracle_packed,
            'size': len(packed), 'oracle_size': len(oracle_packed),
            'mismatch_bytes': sum(a != b for a, b in zip(packed, oracle_packed)) +
                              abs(len(packed) - len(oracle_packed)),
            'first_mismatch': reallink.first_diff(packed, oracle_packed),
            'unpacked_image_equal': unpacked_image == ctx.image,
            'unpacked_relocation_order_equal':
                [r['load_offset'] for r in unpacked_mz.relocations] == expected_sites,
        },
    }


def _stage_dos_bundle(run_root: Path, link_dir: Path, object_manifest: list[dict],
                      link_response: str) -> dict:
    dos = run_root / 'dos'
    dos.mkdir(parents=True, exist_ok=True)
    explicit, libraries = parse_link_response(link_response)
    for name in explicit:
        shutil.copy2(link_dir / name, dos / name)
    game_objects = [row['file'] for row in object_manifest if row['group'] == 'game-library']
    for name in game_objects:
        shutil.copy2(link_dir / name, dos / name)
    # Stage only inputs. Leaving prior GAME.LIB/EXE outputs here can make
    # NMAKE regard every target as current and skip the historical commands.
    for name in ('LIBH.LIB', 'MLIBCR.LIB', 'GAME.RSP', 'LINK.RSP'):
        source = link_dir / name
        if source.exists():
            shutil.copy2(source, dos / name)
    template = MAKEFILE_TEMPLATE.read_text(encoding='ascii')
    makefile = render_makefile(template, explicit, game_objects, libraries)
    (dos / 'MAKEFILE').write_bytes(makefile.encode('ascii'))
    (dos / 'README.TXT').write_text(
        'Generated DOS-side relink bundle. Run NMAKE /F MAKEFILE from this directory with the pinned '
        'MSC 5.10 LINK, LIB and EXEPACK directories on PATH.\r\n'
        'The authoritative full fresh build, including every CL/MASM invocation and raw-debt generation, '
        'is python tools/histbuild.py from the repository root.\r\n', encoding='ascii')
    return {
        'directory': relative(dos), 'makefile': relative(dos / 'MAKEFILE'),
        'link_response': relative(dos / 'LINK.RSP'),
        'explicit_objects': len(explicit),
        'game_library_objects': len(game_objects),
        'libraries': libraries,
    }


def build(tag: str | None = None, emit=print) -> dict:
    run_id = _new_run_id(tag)
    run_root = ROOT / 'build' / 'histbuild' / run_id
    run_root.mkdir(parents=True, exist_ok=False)
    log_path = run_root / 'build.log'
    logs = []

    def report_log(*items):
        line = ' '.join(str(x) for x in items)
        logs.append(line)
        emit(line)

    manifest = json.loads((ROOT / 'layout' / 'manifest.json').read_text(encoding='utf-8'))
    ownership = ownership_summary(manifest)
    plan = run_plan(reallink.LINK_OPTIONS)
    runtime_plan = reallink.runtime_link_plan(reallink.load_inputs())
    runtime_plan_blockers = [
        {'owner': owner, 'reason': row['reason']}
        for owner, row in runtime_plan.items() if not row['linked']]
    if runtime_plan_blockers:
        json_write(run_root / 'report.json', {
            'status': 'BLOCKED_RUNTIME_PLAN', 'tag': run_id, 'plan': plan,
            'runtime_plan_blockers': runtime_plan_blockers, 'ownership': ownership,
        })
        raise RuntimeError(f'pinned runtime storage cannot be represented by LINK: {runtime_plan_blockers}')
    isolated_reallink_out = run_root / 'reallink'
    prior_out = reallink.OUT
    try:
        reallink.OUT = isolated_reallink_out
        link_report = reallink.run(
            runtime=plan['runtime'], partial=plan['partial'], link_options=plan['link_options'],
            order=plan['order'], tag=run_id, log=report_log,
            library_order=plan['library_order'], game_order=plan['game_order'],
            game_input=plan['game_input'])
    except Exception as error:
        report_log('build failed during compile/library/link:', error)
        log_path.write_text('\n'.join(logs) + '\n', encoding='utf-8')
        json_write(run_root / 'report.json', {
            'status': 'FAILED', 'tag': run_id, 'error': str(error),
            'ownership': ownership, 'plan': plan,
        })
        raise
    finally:
        reallink.OUT = prior_out

    library = link_report.get('library', {})
    link_dir = isolated_reallink_out / 'link-library'
    if library.get('link_returncode') != 0 or not (link_dir / 'RESULT.EXE').exists():
        raise RuntimeError(f'LINK 3.65 failed; see {relative(link_dir / "link.log")}')
    if (plan['game_input'] == 'library' and
            library.get('libraries', {}).get('GAME.LIB', {}).get('returncode') != 0):
        raise RuntimeError(f'LINK library construction failed; see {relative(link_dir / "GAME.RSP")}')

    config, _ = reallink.verify_toolchain('msc510-medium')
    tool_directory = (ROOT / config['directory']).resolve()
    exepack_rc, exepack_output, exepack_argv = reallink.run_dos(
        tool_directory / 'EXEPACK.EXE', ['RESULT.EXE', 'PACKED.EXE'], link_dir, 300)
    (link_dir / 'exepack.log').write_text(exepack_output, encoding='ascii', errors='replace')
    if exepack_rc != 0 or not (link_dir / 'PACKED.EXE').exists():
        raise RuntimeError(f'EXEPACK failed; see {relative(link_dir / "exepack.log")}')

    comparison = _compare(link_dir / 'RESULT.EXE', link_dir / 'PACKED.EXE', library)
    object_manifest = json.loads((link_dir / 'objects.json').read_text(encoding='utf-8'))
    commands = _capture_commands(link_report)
    counts = Counter(c['kind'] for c in commands)
    expected_c = sum(row.get('kind') in ('c', 'far-data', 'data-module')
                     for row in link_report.get('order', []))
    expected_asm = sum(row.get('kind') == 'asm' for row in link_report.get('order', []))
    if counts.get('C', 0) != expected_c or counts.get('ASM', 0) != expected_asm:
        raise RuntimeError(
            f'fresh command coverage differs: C {counts.get("C", 0)}/{expected_c}, '
            f'ASM {counts.get("ASM", 0)}/{expected_asm}')
    if any(c['cached'] for c in commands):
        raise RuntimeError('fresh historical build unexpectedly used a cached compiler/assembler object')

    json_write(run_root / 'commands.json', commands)
    json_write(run_root / 'ownership.json', ownership)
    debt_objects = [o for o in object_manifest if o['object_kind'] == 'raw-debt']
    raw_object_bytes = (ownership['raw_initialized_bytes'] +
                        ownership['raw_bss_by_form'].get('object-bss', 0) +
                        ownership['raw_bss_by_form'].get('communal-unit', 0))
    if raw_object_bytes == 0 and debt_objects:
        raise RuntimeError('raw-debt OMF objects exist with no raw-owned object bytes')
    json_write(run_root / 'raw-debt-objects.json', debt_objects)
    link_response = (link_dir / 'LINK.RSP').read_text(encoding='ascii')
    bundle = _stage_dos_bundle(run_root, link_dir, object_manifest, link_response)
    mismatching_banks = [bank for bank, row in comparison['relocations']['banks'].items()
                         if not row['ordered_equal']]
    exact = (comparison['load_image']['equal'] and comparison['header']['equal'] and
             comparison['relocations']['set_equal'] and comparison['relocations']['ordered_equal'] and
             comparison['packed']['equal'])
    report = {
        'schema': 'historical-build-v1', 'status': 'BUILT_EXACT' if exact else 'BUILT_WITH_ORDER_RESIDUAL',
        'tag': run_id, 'entrypoint': 'python tools/histbuild.py',
        'plan': plan, 'order_basis': link_report.get('order_basis'),
        'source_build': {
            'fresh': True, 'c_objects_compiled': counts.get('C', 0), 'c_objects_expected': expected_c,
            'asm_modules_assembled': counts.get('ASM', 0), 'asm_modules_expected': expected_asm,
            'commands': relative(run_root / 'commands.json'),
            'profiles': dict(sorted(Counter(c['profile'] for c in commands).items())),
            'pinned_runtime_libraries': [
                {'path': row.get('pinned', name), 'sha256': row.get('sha256'), 'link_search': True}
                for name, row in library.get('libraries', {}).items()
                if name in ('MLIBCR.LIB', 'LIBH.LIB')],
            'runtime_library_modules_available': library.get('runtime_library_modules'),
            'runtime_library_modules_placed': library.get('runtime_members_placed'),
            'runtime_storage_owners': len(runtime_plan),
            'runtime_storage_declarations': sum(len(row['data']) for row in runtime_plan.values()),
            'runtime_storage_declaration_bytes': sum(
                end - start for row in runtime_plan.values() for _segment, start, end in row['data']),
            'runtime_units_not_loaded': library.get('library_units_not_loaded', []),
        },
        'link': {'runs': 1, 'returncode': library.get('link_returncode'),
                 'command': library.get('link_command'), 'response': relative(link_dir / 'LINK.RSP'),
                 'options': plan['link_options'], 'library_report': library},
        'exepack': {'runs': 1, 'returncode': exepack_rc, 'command': exepack_argv,
                    'log': relative(link_dir / 'exepack.log')},
        'comparison': comparison,
        'oracle_order_residual': {
            'needed_for_exact_relocation_order': not comparison['relocations']['ordered_equal'],
            'independent_order_positions_equal': comparison['relocations']['positions_equal'],
            'oracle_order_positions': comparison['relocations']['oracle_count'],
            'positions_remaining': (comparison['relocations']['oracle_count'] -
                                    comparison['relocations']['positions_equal']),
            'banks_not_exact': mismatching_banks,
            'note': ('The LINK order is derived from image DGROUP anchors; oracle relocation data is '
                     'read only after linking. Non-exact banks require the oracle-derived diagnostic order '
                     'for byte-exact relocation/EXEPACK reproduction.') if mismatching_banks else None,
        },
        'ownership': ownership,
        'raw_debt_objects': {
            'manifest': relative(run_root / 'raw-debt-objects.json'),
            'objects': debt_objects,
        },
        'dos_bundle': bundle,
        'reallink_report': link_report.get('path'),
        'report': relative(run_root / 'report.json'),
    }
    json_write(run_root / 'report.json', report)
    report_log('LINK 3.65: one run; EXEPACK: one run')
    report_log('fresh builds: C', counts.get('C', 0), '/', expected_c,
               'ASM', counts.get('ASM', 0), '/', expected_asm)
    report_log('oracle: load image', comparison['load_image']['equal'],
               'packed EXE', comparison['packed']['equal'],
               'relocation positions', comparison['relocations']['positions_equal'], '/',
               comparison['relocations']['oracle_count'], 'exact banks',
               [b for b, row in comparison['relocations']['banks'].items() if row['ordered_equal']])
    report_log('ownership bytes:', ownership['owner_bytes'])
    report_log('raw initialized:', ownership['raw_initialized_bytes'],
               'raw BSS object:', ownership['raw_bss_by_form'].get('object-bss', 0),
               'raw BSS fill:', ownership['raw_bss_by_form'].get('link-word-fill', 0))
    report_log('raw debt OMF objects:', len(debt_objects),
               ', '.join(o['file'] for o in debt_objects) if debt_objects else '(none)')
    report_log('DOS bundle:', bundle['directory'], 'report:', relative(run_root / 'report.json'))
    log_path.write_text('\n'.join(logs) + '\n', encoding='utf-8')
    return report


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--tag', help='short label included in the unique output directory under build/histbuild/')
    args = parser.parse_args(argv)
    try:
        report = build(args.tag)
    except Exception as error:
        print(f'FAILED: {error}', file=sys.stderr)
        return 1
    print('STATUS:', report['status'])
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
