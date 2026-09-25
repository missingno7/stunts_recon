"""One pinned absolute MSC runtime public used by huge-pointer shifts."""
from common import ROOT, read_json, require, sha
from compiler import verify_toolchain
from omf import OmfReader


def ahshift(image, relocations):
    config, _ = verify_toolchain('msc510-medium')
    library = 'toolchain/msc510/MLIBCR.LIB'
    pinned, = [f for f in config['files'] if f['path'] == library]
    data = (ROOT/library).read_bytes()
    require(sha(data) == pinned['sha256'], 'Pinned runtime library changed')
    modules = [(n, raw) for n, raw in OmfReader().split_library(data)
               if n == 'dos\\diffhlp.asm']
    require(len(modules) == 1 and sha(modules[0][1]) ==
            '0c6ab7581510ce610a1ffca5953f0909b34a493f915354fa82d78dded36a4956',
            'Pinned runtime absolute-public module changed')
    obj = OmfReader().read(modules[0][1])
    require(obj.publics == [{'name':'__AHINCR','segment':'?0','offset':4096},
                            {'name':'__AHSHIFT','segment':'?0','offset':12}]
            and not obj.segment_defs and not obj.linker_fixups,
            'Runtime __AHSHIFT is not the pinned absolute public')
    # Independently observed MOV CX,000Ch in update_gamestate.
    require(image[28768:28771] == bytes.fromhex('b90c00') and
            not any(28769 <= r['load_offset'] < 28771 for r in relocations),
            'Original huge-pointer shift immediate or relocation changed')
    from function_evidence import current_inventory
    inv = current_inventory(image)
    found = [f for f in inv['functions'] if f.get('name')=='update_gamestate'
             and f.get('start') <= 28768 and 28771 <= f.get('end',0)
             and f['status']=='BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED'
             and sha(image[f['start']:f['end']])==f['sha256']]
    require(len(found)==1, 'Original __AHSHIFT instruction lacks verified function extent')
    return {'kind':'absolute-runtime-word','value':12,'public':'__AHSHIFT',
            'module_sha256':sha(modules[0][1]),'anchor_load_offset':28769}
