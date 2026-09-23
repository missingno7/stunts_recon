"""Conservative, evidence-based capability triage; never an origin classifier."""
import re


def recipe_matches_inventory(recipe, function):
    return (function['status']=='BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED'
            and all(recipe.get(k)==function.get(k) for k in ['stable_id','start','end'])
            and recipe.get('id')==function['name']
            and recipe.get('target')=={'size':function.get('size'),'sha256':function.get('sha256')})


def capabilities(function, disassembly, known_data_offsets):
    blockers, risks = [], []
    start, end = function.get('start'), function.get('end')
    if function['status'] != 'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED':
        return ['Unverified function boundaries'], []
    if function.get('relocation_sites'): blockers.append('MZ relocation binding is unsupported')
    if not disassembly: return blockers + ['No complete instruction evidence for triage'], []
    at = start; boundaries = {x['load_offset'] for x in disassembly}
    for row in disassembly:
        data = bytes.fromhex(row['bytes']); text = row['instruction'].strip().lower()
        if row['load_offset'] != at: blockers.append('Instruction evidence has a gap/overlap')
        at = row['load_offset'] + len(data)
        mnemonic = text.split(' ')[0]
        if mnemonic in ['call', 'lcall']: blockers.append('Calls need supervisor linking/TU review')
        if 'cs:[' in text: blockers.append('CS-relative storage is unsupported')
        if re.search(r',\s*ss\s*$',text): blockers.append('Explicit SS register value needs supervisor source review')
        if mnemonic in ['int', 'iret', 'in', 'out', 'cli', 'sti', 'hlt']:
            blockers.append('Hardware/interrupt semantics need supervisor review')
        if mnemonic.startswith('j') or mnemonic.startswith('loop'):
            target = None
            if len(data) == 2 and (data[0] == 0xeb or 0x70 <= data[0] <= 0x7f or 0xe0 <= data[0] <= 0xe3):
                target = at + int.from_bytes(data[1:], 'little', signed=True)
            elif len(data) == 3 and data[0] == 0xe9:
                target = at + int.from_bytes(data[1:], 'little', signed=True)
            if target is None or not start <= target < end:
                blockers.append('External/indirect jump needs supervisor linking/TU review')
            elif target not in boundaries:
                blockers.append('Branch target is not a proven instruction boundary')
        # Absolute default-DS operands definitely require external data evidence.
        for offset in re.findall(r'(?<!:)\[(0x[0-9a-f]+)\]', text):
            if int(offset, 16) not in known_data_offsets:
                blockers.append('Unregistered DGROUP address '+offset)
        # Register-displacement operands may be arrays or fields; don't guess.
        if re.search(r'\[(?:bx|si|di)[^\]]*0x[1-9a-f][0-9a-f]{3}', text):
            risks.append('Indexed data displacement needs source/symbol review')
    if at != end: blockers.append('Instruction evidence does not cover complete extent')
    if (end-start) % 2: risks.append('Odd extent: verify complete compiler alignment; never trim')
    return sorted(set(blockers)), sorted(set(risks))
