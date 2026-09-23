"""Non-accepting instruction diagnostics. Full artifacts stay in the probe directory."""
import sys
from pathlib import Path
from common import ROOT, identity, write_json


def decode(data, start=0):
    sys.path.insert(0, str(ROOT/'build/python'))
    import capstone
    if capstone.__version__ != '5.0.3':
        raise ValueError('Diagnostic decoder requires Capstone 5.0.3')
    rows = []
    for ins in capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_16).disasm(data, start):
        # Capstone 5 prints the 32-bit aliases even in this 16-bit mode.
        mnemonic = {b'\x98':'cbw', b'\x99':'cwd'}.get(bytes(ins.bytes), ins.mnemonic)
        rows.append({'load_offset':ins.address, 'bytes':ins.bytes.hex(),
                     'instruction':mnemonic+' '+ins.op_str})
    return rows


def diagnose(target, candidate, receipt, fixups, bound=False):
    """Never supplies payloads to the binder or changes mismatch acceptance."""
    work = Path(receipt['work_directory'])
    report = {'target':identity(target), 'candidate':identity(candidate),
              'comparison':('bound complete contribution; every byte and extent must match' if bound else
                            'unbound object; relocation operands are expected to differ'),
              'fixups':fixups}
    (work/'candidate-code.bin').write_bytes(candidate)
    try:
        left, right = decode(target), decode(candidate)
        report.update(target_instructions=left, candidate_instructions=right)
        prefix = 0
        while prefix < min(len(left),len(right)) and left[prefix] == right[prefix]:
            prefix += 1
        report['first_differing_instruction'] = {'index':prefix,
            'target':left[prefix] if prefix<len(left) else None,
            'candidate':right[prefix] if prefix<len(right) else None}
        report['decoded_bytes'] = [sum(len(bytes.fromhex(x['bytes'])) for x in rows) for rows in (left,right)]
    except (ImportError, ValueError) as error:
        report['decoder_unavailable'] = str(error)
    destination=work/('diagnostic-bound.json' if bound else 'diagnostic.json')
    write_json(destination, report)
    return {k:v for k,v in report.items() if k not in ('target_instructions','candidate_instructions','fixups')} | {
        'full_diagnostic':str(destination), 'full_diagnostic_identity':identity(destination.read_bytes()),
        'omitted':'Full instruction lists and fixups are in full_diagnostic; diagnostic only.'}
