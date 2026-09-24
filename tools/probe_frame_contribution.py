"""Bounded research check of one complete set_frame_callback OMF contribution.

This checks original operand evidence and full candidate shape. It never binds
an object, creates a recipe, reopens a task, or authorizes acceptance.
"""
from common import ROOT, require, sha, write_json
from mz import MZ
from object_probe import read_object
from oracle import verify


OBJECT = ROOT / 'build/private/research-batches/set_frame_callback/a058284caef3/frame-direct-pointer.obj'
OBJECT_SHA = 'fd3f9f5921964435405c6e30e51b599f3d67bf6621f0b320caaf3cab5c8e0758'
START, END = 75098, 75126
EXPECTED_FIXUPS = [
    (24, 'offset16', '_byte_442E4', '748b'),
    (15, 'pointer32', '_timer_reg_callback', '8a18a21e'),
    (10, 'base16', '_frame_callback', 'b711'),
    (7, 'loader-offset16', '_frame_callback', '260a'),
    (2, 'offset16', '_word_46468', 'f8ac'),
]
# These pristine instructions are outside the candidate target. They corroborate
# addresses, not original PUBDEF names, data ownership, or historical TU scope.
INDEPENDENT_SITES = [
    ('frame_callback pointer in remove_frame_callback', 75140, 'b8260abab711'),
    ('timer_reg_callback call in another function', 93133, '9a8a18a21e'),
    ('word_46468 increment in frame_callback', 75338, 'ff06f8ac'),
    ('byte_442E4 comparison in frame_callback', 75170, '803e748b00'),
]


def run():
    raw = OBJECT.read_bytes()
    require(sha(raw) == OBJECT_SHA, 'Archived candidate object changed')
    obj = read_object(raw)
    _, unpacked, oracle, _ = verify(write=False)
    image = MZ.parse(unpacked).load_image(unpacked)
    code = obj.segment_bytes('UNIT_TEXT')
    target = image[START:END]
    require(obj.segment_lengths.get('UNIT_TEXT') == len(code) == len(target) == 28,
            'Incomplete code extent')
    require(all(name == 'UNIT_TEXT' or size == 0
                for name, size in obj.segment_lengths.items()), 'Additional contribution')
    require(obj.publics == [{'name': '_set_frame_callback', 'segment': 'UNIT_TEXT', 'offset': 0}],
            'Public order/offset changed')
    require(len(obj.linker_fixups) == len(EXPECTED_FIXUPS), 'Fixup count changed')
    covered = set()
    fixups = []
    for fix, (offset, loc, symbol, expected) in zip(obj.linker_fixups, EXPECTED_FIXUPS):
        width = len(bytes.fromhex(expected))
        require((fix['segment'], fix['offset'], fix['loc'], fix['target'], fix['width']) ==
                ('UNIT_TEXT', offset, loc, symbol, width), 'Ordered fixup differs')
        require(fix['frame_method'] == 5 and fix['frame_kind'] == 'target' and
                fix['target_method'] == 2 and fix['target_kind'] == 'external' and
                not fix['self_relative'] and fix['displacement'] == 0 and
                bytes.fromhex(fix['encoded_addend']) == bytes(width),
                'Unsupported fixup mode/addend')
        require(target[offset:offset + width].hex() == expected,
                'Pristine operand differs')
        covered.update(range(offset, offset + width))
        fixups.append({'offset': offset, 'loc': loc, 'target': symbol,
                       'pristine_operand': expected})
    require(len(covered) == 12 and all(code[i] == target[i]
            for i in range(28) if i not in covered), 'Nonfixup byte mismatch')
    sites = []
    for description, at, expected in INDEPENDENT_SITES:
        require(image[at:at + len(expected)//2].hex() == expected,
                'Independent pristine address evidence changed')
        sites.append({'description': description, 'load_offset': at, 'hex': expected})
    relocs = [r['load_offset'] for r in oracle['unpacked_mz']['relocations']
              if START <= r['load_offset'] < END]
    require(relocs == [START + 17, START + 10], 'Original ordered MZ relocations differ')
    report = {
        'schema': 1, 'authority': 'RESEARCH_ONLY_COMPLETE_SHAPE_AND_OPERAND_CHECK',
        'task_id': 'load_1255a', 'object_sha256': sha(raw),
        'oracle_load_sha256': sha(image), 'candidate_extent': [START, END],
        'segment_lengths': obj.segment_lengths, 'ordered_publics': obj.publics,
        'ordered_fixups': fixups, 'ordered_original_mz_relocations': relocs,
        'independent_pristine_sites': sites, 'nonfixup_equal_bytes': 16,
        'unresolved_fixup_bytes': 12,
        'result': 'FULL_SHAPE_AND_OPERAND_CONSISTENT_RESEARCH_ONLY',
        'missing_proof': ['original data ownership/PUBDEFs',
                          'historical LINK differential for this complete five-fixup mode',
                          'fresh native whole-image acceptance'],
    }
    write_json(ROOT / 'recovery/experiments/set-frame-contribution-20260924.json', report)
    print(report['result'])
    return report


if __name__ == '__main__':
    run()
