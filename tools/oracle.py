"""Original assets -> DSI decoding -> DIF -> packed MZ -> pristine EXEPACK image."""
import argparse
import dataclasses
import struct
from common import ROOT, identity, read_json, write_json, require
from dsi import decode
from exepack import unpack
from mz import MZ

NAMES = ('MCGA.HDR', 'EGA.CMN', 'MCGA.DIF', 'MCGA.COD')

def apply_dif(common, patch):
    out, at, target, records = bytearray(common), 0, -1, []
    while True:
        require(at + 2 <= len(patch), 'DIF missing terminator')
        word = struct.unpack_from('<H', patch, at)[0]
        at += 2
        if word == 0:
            require(at == len(patch), 'Trailing DIF bytes')
            return bytes(out), records
        target += word & 0x7fff
        count = 4 if word & 0x8000 else 2
        require(0 <= target and target + count <= len(out), 'DIF destination outside common component')
        require(at + count <= len(patch), 'Truncated DIF replacement')
        out[target:target + count] = patch[at:at + count]
        records.append({'decoded_dif_offset': at, 'packed_load_offset': target, 'length': count})
        at += count

def construct(asset_dir=ROOT / 'assets'):
    found = {}
    for path in asset_dir.iterdir():
        if path.is_file() and path.name.upper() in NAMES:
            require(path.name.upper() not in found, 'Duplicate case-insensitive oracle asset')
            found[path.name.upper()] = path.read_bytes()
    require(set(found) == set(NAMES), 'Missing required MCGA asset')
    header = found['MCGA.HDR']
    require(len(header) == 30 and header[:2] == b'MZ', 'Unexpected distributed header')
    decoded = {name: decode(found[name]) for name in NAMES if name != 'MCGA.HDR'}
    common, dif_records = apply_dif(decoded['EGA.CMN'], decoded['MCGA.DIF'])
    header_size = struct.unpack_from('<H', header, 8)[0] * 16
    require(header_size >= len(header), 'Invalid distributed header size')
    packed = header + bytes(header_size - len(header)) + common + decoded['MCGA.COD']
    mz = MZ.parse(packed)
    require(len(packed) == mz.declared_size, 'Component lengths disagree with MZ declared size')
    unpacked, exepack = unpack(packed)
    um = MZ.parse(unpacked)
    report = {'schema': 1, 'inputs': {n: identity(found[n]) for n in NAMES},
              'decoded_components': {n: identity(b) for n, b in decoded.items()},
              'packed': identity(packed), 'unpacked': identity(unpacked),
              'load_image': identity(um.load_image(unpacked)),
              'packed_mz': dataclasses.asdict(mz), 'unpacked_mz': dataclasses.asdict(um),
              'exepack_stub': exepack['stub'], 'exepack_header': exepack['header'],
              'packed_header_padding': {'start': 30, 'end': header_size, 'policy': 'deterministic zero; not distributed bytes'},
              'unpacked_header_policy': 'synthetic minimal paragraph-aligned header; preserved entry/stack and decoded relocation pairs/order; reconstructed sizes/minalloc/checksum/padding'}
    return packed, unpacked, report, {'dif': dif_records, 'exepack': exepack['commands']}

def verify(write=True, asset_dir=ROOT / 'assets'):
    lock = read_json(ROOT / 'layout/oracle.lock.json')
    result = construct(asset_dir)
    require(result[2] == lock, 'Oracle identity/structure differs from immutable lock')
    if write:
        out = ROOT / 'build/oracle'
        out.mkdir(parents=True, exist_ok=True)
        (out / 'mcga-packed.exe').write_bytes(result[0])
        (out / 'mcga-unpacked.exe').write_bytes(result[1])
        (out / 'load-image.bin').write_bytes(MZ.parse(result[1]).load_image(result[1]))
        write_json(out / 'report.json', result[2])
        write_json(out / 'transforms.json', result[3])
    return result

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('command', choices=['verify'])
    parser.parse_args()
    result = verify()
    print('PASS: pristine MCGA oracle', result[2]['load_image'])

if __name__ == '__main__':
    main()
