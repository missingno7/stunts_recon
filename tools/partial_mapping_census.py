"""Read-only first-divergence census for labeled but unmapped Restunts spans.

Restunts source is a clue, not the oracle. This reuses the importer's pinned
decoder and agreement predicate without relaxing any boundary rule or turning
a matching prefix into an accepted machine interval.
"""
import collections
import json
import runpy

from common import ROOT, read_json, require, sha, write_json
from mz import MZ
from oracle import verify


def run():
    inventory = read_json(ROOT/'recovery/restunts-inventory.json')
    partial = {(f['segment'], f['name'], f['line_start']): f
               for f in inventory['functions'] if f['status']=='PARTIAL_UNMAPPED'}
    require(len(partial)==sum(f['status']=='PARTIAL_UNMAPPED' for f in inventory['functions']),
            'Ambiguous partial function identity')
    # The existing importer body remains the authority for source parsing and
    # mnemonic/register agreement. Its transient output stays under build/.
    context = runpy.run_path(str(ROOT/'tools/restunts_base.py'))
    _, unpacked, _, _ = verify(write=False)
    image = MZ.parse(unpacked).load_image(unpacked)
    require(context['hashlib'].sha256(image).hexdigest()==inventory['load_sha256'],
            'Partial mapping source/oracle drift')
    md, agree = context['md'], context['agree']
    rows = []
    for f in context['funcs']:
        key = (f['segment'], f['name'], f['line_start'])
        if key not in partial:
            continue
        labels = f['labels']
        failures = []
        for index, label in enumerate(labels):
            following = labels[index+1] if index+1 < len(labels) else None
            source_items = f['items'][label['index']:following['index'] if following else len(f['items'])]
            start = label['ida']-65536
            end = following['ida']-65536 if following else None
            if context['decode_match'](start, source_items, end) is not None:
                continue
            decoded = list(md.disasm(image[start:min(len(image), start+15*len(source_items))],
                                     start, count=len(source_items))) if 0<=start<len(image) else []
            first = next((i for i,(src,ins) in enumerate(zip(source_items,decoded))
                          if not agree(src,ins)), None)
            if first is not None:
                src, ins = source_items[first], decoded[first]
                failure = {'kind':'FIRST_SOURCE_INSTRUCTION_DISAGREES', 'label_load_address':start,
                           'source_line':src['line'], 'instruction_index':first,
                           'source_instruction':src['source'],
                           'pristine_instruction':(ins.mnemonic+' '+ins.op_str).strip(),
                           'pristine_bytes':bytes(ins.bytes).hex(), 'pristine_load_address':ins.address}
            elif len(decoded)!=len(source_items):
                failure = {'kind':'DECODE_COUNT_DIFFERS', 'label_load_address':start,
                           'source_items':len(source_items), 'decoded_items':len(decoded)}
            else:
                finish = decoded[-1].address+decoded[-1].size if decoded else start
                failure = {'kind':'LABEL_END_DIFFERS' if end is not None and finish!=end else 'OTHER_AGREEMENT_FAILURE',
                           'label_load_address':start, 'source_items':len(source_items),
                           'decoded_end':finish, 'next_label':end}
            failures.append(failure)
        original = partial[key]
        rows.append({'id':original['unresolved_evidence_id'], 'name':f['name'],
                     'segment':f['segment'], 'source':original['provenance'],
                     'prefix_candidates':original.get('start_candidates',[]),
                     'verified_labels':original.get('verified_labels',0),
                     'label_count':original.get('label_count',len(labels)),
                     'failed_labels':len(failures), 'first_failure':failures[0] if failures else None,
                     'failure_kinds':dict(collections.Counter(x['kind'] for x in failures)),
                     'all_failures':failures,
                     'authority':'DIAGNOSTIC_ONLY'})
    require(len(rows)==len(partial), 'Partial mapping census omitted a current task')
    primary=collections.Counter((r['first_failure'] or {}).get('kind','PREFIX_OR_UNCLASSIFIED') for r in rows)
    signature=collections.Counter((r['first_failure']['source_instruction'].split()[0],
                                   r['first_failure']['pristine_instruction'].split()[0])
                                  for r in rows if r['first_failure'] and
                                  r['first_failure']['kind']=='FIRST_SOURCE_INSTRUCTION_DISAGREES')
    report={'schema':1,'authority':'RESEARCH_ONLY','oracle_load_sha256':sha(image),
            'partial_tasks':len(rows),'first_failure_kinds':dict(primary),
            'first_opcode_pairs':[{'source':a,'pristine':b,'tasks':n}
                                  for (a,b),n in signature.most_common()],
            'tasks':rows,
            'limitation':'Importer agreement checks mnemonic, explicit GPR and literal immediate. A first mismatch may reflect earlier source/emission drift; this does not establish a repaired boundary or code/data class.'}
    write_json(ROOT/'recovery/partial-mapping-census.json',report)
    print(json.dumps({'partial_tasks':len(rows),'first_failure_kinds':dict(primary),
                      'top_first_opcode_pairs':report['first_opcode_pairs'][:15]},indent=2))
    return report


if __name__=='__main__':
    run()
