"""Research-only historical LINK differential for the complete frame callback object.

The freshly compiled candidate object is never modified. Synthetic provider definitions
test linker arithmetic; they make no claim about original Stunts data ownership.
"""
import re
import shutil
import struct
import subprocess
from copy import deepcopy
from pathlib import Path

from common import ROOT, identity, read_json, require, sha, write_json
from compiler import compile_source, verify_toolchain
from linker_probe import provider
from mz import MZ
from object_probe import read_object
from omf import OmfReader

def fresh_candidate():
    recipe = read_json(ROOT/'recipes/set_frame_callback.json')
    obj, receipt = compile_source((ROOT/recipe['source']).read_bytes(), recipe['profile'])
    require_complete_shape(obj)
    return obj, (Path(receipt['work_directory'])/'UNIT.OBJ').read_bytes()


PROVIDER_SOURCE = (b'unsigned int word_46468=0; unsigned char byte_442E4=0; '
                   b'void far frame_callback(void) {} '
                   b'void far timer_reg_callback(void) {}\n')
FIXUP_SHAPE = [
    (24, 'offset16', '_byte_442E4'),
    (15, 'pointer32', '_timer_reg_callback'),
    (10, 'base16', '_frame_callback'),
    (7, 'loader-offset16', '_frame_callback'),
    (2, 'offset16', '_word_46468'),
]


def public(mapping, name):
    rows = re.findall(r'^\s*([0-9A-F]+):([0-9A-F]+)\s+' + re.escape(name) + r'\s*$',
                      mapping, re.M)
    # /MAP lists each public in both by-name and by-value sections.
    require(len(rows) == 2 and len(set(rows)) == 1,
            'Missing or ambiguous LINK public ' + name)
    segment, offset = (int(value, 16) for value in rows[0])
    return {'segment': segment, 'offset': offset, 'load_address': 16*segment + offset}


def require_complete_shape(candidate):
    require(candidate.segment_lengths.get('UNIT_TEXT') == 28 and
            candidate.publics == [{'name': '_set_frame_callback',
                                   'segment': 'UNIT_TEXT', 'offset': 0}] and
            all(name == 'UNIT_TEXT' or size == 0
                for name, size in candidate.segment_lengths.items()),
            'Fresh frame contribution is incomplete')
    require([(f['offset'], f['loc'], f['target']) for f in candidate.linker_fixups]
            == FIXUP_SHAPE, 'Fresh ordered fixups changed')


def negative_controls(candidate):
    rows = []
    for name, altered in [('reversed_order', list(reversed(candidate.linker_fixups))),
                          ('omitted_last', candidate.linker_fixups[:-1])]:
        changed = deepcopy(candidate)
        changed.linker_fixups = altered
        try:
            require_complete_shape(changed)
        except ValueError as error:
            rows.append({'mutation': name, 'rejected': True, 'reason': str(error)})
        else:
            require(False, 'Negative control was accepted: ' + name)
    return rows


def padded_library_member():
    row = next(o for o in read_json(ROOT / 'layout/manifest.json')['owners'] if o['id']=='library_lmul')
    archive = (ROOT / row['library']).read_bytes()
    require(sha(archive) == row['library_sha256'], 'Pinned padding library changed')
    matches = [body for name, body in OmfReader().split_library(archive)
               if name == row['module'] and sha(body) == row['module_sha256']]
    require(len(matches) == 1, 'Missing pinned padding member')
    return matches[0], row['module_sha256']


def experiment(profile='msc510-medium', provider_first=False, padded=False):
    candidate, candidate_bytes = fresh_candidate()
    require_complete_shape(candidate)
    synthetic, receipt = compile_source(PROVIDER_SOURCE, profile)
    require(not synthetic.linker_fixups and synthetic.segment_lengths.get('UNIT_TEXT') == 4 and
            synthetic.segment_lengths.get('_DATA') == 3,
            'Synthetic provider shape changed')
    work = Path(receipt['work_directory'])
    shutil.copyfile(work / 'UNIT.OBJ', work / 'SYMS.OBJ')
    (work / 'FRAME.OBJ').write_bytes(candidate_bytes)
    aux = provider(0)
    (work / 'AUX.OBJ').write_bytes(aux)
    config, runner = verify_toolchain(profile)
    tc = (ROOT / config['directory']).resolve()
    member_sha = None
    if padded:
        member, member_sha = padded_library_member()
        (work / 'MUL.OBJ').write_bytes(member)
    order = ('SYMS.OBJ+FRAME.OBJ+AUX.OBJ' if provider_first else
             'FRAME.OBJ+SYMS.OBJ+AUX.OBJ')
    if padded:
        order = 'MUL.OBJ+' + order
    command = [runner['path'], '-e', '-v5.00', str(tc / 'LINK.EXE'),
               f'/NOD /MAP {order},RESULT.EXE,RESULT.MAP;']
    result = subprocess.run(command, cwd=work,
        env={'PATH': str(tc), 'MSDOS_PATH': str(tc), 'TEMP': '.', 'TMP': '.',
             'MSDOS_TEMP': '.'}, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
        timeout=60, creationflags=getattr(subprocess, 'CREATE_NO_WINDOW', 0))
    (work / 'link.log').write_bytes(result.stdout)
    require(result.returncode == 0 and (work / 'RESULT.EXE').is_file(),
            'Historical LINK failed: ' + result.stdout.decode('ascii', 'replace'))
    linked_exe = (work / 'RESULT.EXE').read_bytes()
    mz = MZ.parse(linked_exe)
    image = mz.load_image(linked_exe)
    mapping = (work / 'RESULT.MAP').read_text()
    names = ['_set_frame_callback', '_frame_callback', '_timer_reg_callback',
             '_word_46468', '_byte_442E4']
    publics = {name: public(mapping, name) for name in names}
    group = re.findall(r'^\s*([0-9A-F]+):0\s+DGROUP\s*$', mapping, re.M)
    require(len(group) == 1, 'Missing or ambiguous LINK DGROUP frame')
    group_frame = int(group[0], 16) * 16
    start = publics['_set_frame_callback']['load_address']
    require(start + 28 <= len(image), 'Linked frame contribution outside image')
    raw = candidate.segment_bytes('UNIT_TEXT')
    expected = bytearray(raw)
    bindings = []
    for fix in candidate.linker_fixups:
        require(fix['frame_method'] == 5 and fix['frame_kind'] == 'target' and
                fix['target_method'] == 2 and fix['target_kind'] == 'external' and
                not fix['self_relative'] and fix['displacement'] == 0 and
                bytes.fromhex(fix['encoded_addend']) == bytes(fix['width']),
                'Unsupported fixture fixup semantics')
        symbol = publics[fix['target']]
        at = fix['offset']
        if fix['loc'] == 'pointer32':
            value = struct.pack('<HH', symbol['offset'], symbol['segment'])
        elif fix['loc'] == 'base16':
            value = struct.pack('<H', symbol['segment'])
        elif fix['loc'] == 'loader-offset16':
            value = struct.pack('<H', symbol['offset'])
        elif fix['loc'] == 'offset16':
            value = struct.pack('<H', symbol['load_address'] - group_frame)
        else:
            require(False, 'Unexpected fixture mode')
        require(len(value) == fix['width'] and raw[at:at + len(value)] == bytes(len(value)),
                'Unexpected object addend/width')
        expected[at:at + len(value)] = value
        bindings.append({'offset': at, 'loc': fix['loc'], 'target': fix['target'],
                         'map_derived_value': value.hex()})
    expected_relocations = [
        {'segment': start // 16, 'offset': start % 16 + at,
         'load_offset': start + at} for at in (17, 10)]
    require(mz.relocations == expected_relocations,
            'LINK MZ relocation order/coordinates differ')
    observed = image[start:start + 28]
    require(observed == expected,
            'Untouched historical LINK contribution differs from map-derived arithmetic: '
            + observed.hex() + ' != ' + expected.hex())
    require(identity((work / 'FRAME.OBJ').read_bytes()) == identity(candidate_bytes) and
            identity((work / 'SYMS.OBJ').read_bytes()) == receipt['object'] and
            identity((work / 'AUX.OBJ').read_bytes()) == identity(aux),
            'Historical LINK input object changed')
    if padded:
        require(sha((work / 'MUL.OBJ').read_bytes()) == member_sha,
                'Historical LINK padding member changed')
    verify_toolchain(profile)
    return {'profile': profile, 'provider_first': provider_first, 'padded': padded,
            'authority': 'RESEARCH_ONLY_HISTORICAL_LINK_DIFFERENTIAL',
            'fresh_candidate_object': identity(candidate_bytes),
            'synthetic_provider_source': PROVIDER_SOURCE.decode('ascii'),
            'synthetic_provider_compiler': receipt,
            'synthetic_aux_provider_object': identity(aux),
            'padding_library_member_sha256': member_sha,
            'link_command': command, 'link_log': result.stdout.decode('ascii', 'replace'),
            'map': mapping, 'map_publics': publics, 'dgroup_frame': group_frame,
            'ordered_fixups': candidate.linker_fixups, 'map_derived_bindings': bindings,
            'source_start': start, 'linked_contribution': observed.hex(),
            'ordered_mz_relocations': mz.relocations,
            'executable': identity(linked_exe), 'historical_link_equal': True}


def run():
    candidate, _ = fresh_candidate()
    require_complete_shape(candidate)
    report = {'schema': 1, 'authority': 'RESEARCH_ONLY_HISTORICAL_LINK_DIFFERENTIAL',
              'negative_controls': negative_controls(candidate),
              'cases': [experiment(provider_first=False), experiment(provider_first=True),
                        experiment(provider_first=False, padded=True)],
              'limitation': 'Synthetic providers prove LINK arithmetic for this object shape, '
                            'not original Stunts PUBDEF/data ownership/TU or native acceptance.'}
    write_json(ROOT / 'build/frame-link-probe.json', report)
    print('Historical frame LINK differential: build/frame-link-probe.json')
    return report


if __name__ == '__main__':
    run()
