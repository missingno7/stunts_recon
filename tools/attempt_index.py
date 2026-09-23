"""Compact searchable view of immutable attempts, never an acceptance authority.

Routine controls still hash all archived attempts conservatively. This index saves
context reads, not correctness checks. Full validation rebuilds/checks the view.
"""
from common import ROOT, read_json, write_json, identity


def generate(write=True):
    tasks={}
    for folder in ['attempts','experiments']:
        paths=(ROOT/'recovery'/folder).glob('*/*/report.json')
        for path in sorted(paths,key=lambda p:(p.parent.parent.name,not p.parent.name.startswith('prior_'),p.parent.name)):
            r=read_json(path);task=r.get('task',path.parent.parent.name)
            diagnostic=r.get('diagnostics',{});comparison=diagnostic.get('comparison',r.get('comparison',{}))
            row={'path':path.relative_to(ROOT).as_posix(),'identity':identity(path.read_bytes()),
                 'kind':folder,'epoch':r.get('epoch'),'status':r.get('status',r.get('scope')),
                 'hypothesis':r.get('hypothesis',r.get('prediction')),
                 'category':diagnostic.get('category'),'error':r.get('error'),
                 'sizes':diagnostic.get('emitted_sizes',r.get('emitted_sizes')),
                 'first_difference':comparison.get('first_differing_instruction'),
                 'same_effective_as':r.get('same_effective_as',r.get('same_object_as',[]))}
            tasks.setdefault(task,[]).append(row)
    result={'schema':1,'authority':'Derived search view; immutable archives/control_inputs remain authoritative','tasks':tasks}
    if write:write_json(ROOT/'recovery/attempt-index.json',result)
    return result
