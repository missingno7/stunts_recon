"""Resolve reviewed original DGROUP addresses, including uninitialized storage."""
from common import ROOT, read_json, require


def resolve_symbols(names, image, relocations):
    layout = read_json(ROOT/'layout/data-symbols.json')
    require(layout['schema'] == 1, 'Unknown data-symbol schema')
    from common import sha
    require(layout['oracle_sha256'] == sha(image), 'Data symbols belong to another oracle')
    for anchor in layout['startup_anchors']:
        at = anchor['start']; expected = bytes.fromhex(anchor['hex'])
        require(image[at:at+len(expected)] == expected, 'DGROUP startup evidence changed')
    frame = layout['frame_load_address']
    site = layout['frame_relocation']
    require(site in relocations, 'DGROUP frame lacks required ordered-table relocation entry')
    at = site['load_offset']
    require(int.from_bytes(image[at:at+2], 'little') * 16 == frame, 'DGROUP relocated paragraph differs')
    require(layout['bss_start'] == frame + 0x55ca and layout['bss_end'] == frame + 0xad20,
            'DGROUP BSS does not agree with reviewed startup clear range')
    result = {}
    for name in names:
        require(name in layout['symbols'], 'Unknown data external: ' + name)
        symbol = layout['symbols'][name]
        address = symbol['load_address']
        require(0 <= address-frame < 65536, 'Data symbol outside DGROUP')
        if symbol['storage'] == 'bss':
            require(layout['bss_start'] <= address < layout['bss_end'], 'Symbol outside verified BSS')
        else:
            require(symbol['storage'] == 'initialized' and frame <= address < len(image),
                    'Symbol outside initialized DGROUP')
        require(symbol['references'], 'Data symbol needs original instruction evidence')
        for ref in symbol['references']:
            start = ref['start']; code = bytes.fromhex(ref['hex']); operand = ref['operand_offset']
            require(image[start:start+len(code)] == code, 'Data symbol instruction evidence changed')
            require(0 <= operand <= len(code)-2 and int.from_bytes(code[operand:operand+2], 'little') == address-frame,
                    'Data symbol disagrees with original address operand')
            require(not any(start+operand-1 <= r['load_offset'] < start+operand+2 for r in relocations),
                    'DGROUP offset unexpectedly has a load relocation')
        result[name] = {'group': 'DGROUP', 'frame_load_address': frame, 'load_address': address}
    return result
