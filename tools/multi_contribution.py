"""Strict complete adjacent C functions with internal near CALL fixups."""
import copy, struct
from common import ROOT, identity, read_json, require, sha
from binder import bind_contribution, fixup_relocation_sites, link_order_sites
from object_probe import declared_externals
from code_symbols import resolve_recipe_symbols
from function_evidence import current_inventory, reviewed_functions
from secondary_contribution import bind_secondary
def _checked_asm_near_labels(recipe, image):
    labels=recipe.get('reviewed_near_labels',{})
    if not labels:return {}
    require(recipe.get('kind')=='asm', 'Near labels require ASM contribution')
    path='src/restunts/asmorig/seg012.asm'
    pinned=read_json(ROOT/'layout/references.json')['restunts']['evidence_files'][path]
    from pinned_reference import check_reference_identity, reference_line
    require(check_reference_identity(path)==pinned,
            'Reviewed near-label source differs')
    import re, sys
    decoder_path=str(ROOT/'build/python')
    if decoder_path not in sys.path:sys.path.insert(0,decoder_path)
    import capstone
    decoder=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_16)
    starts=set()
    reviewed=reviewed_functions(image)
    ordered=recipe['members']
    for index,member in enumerate(ordered):
        # Whole-module entries carry no member end: the next entry bounds it.
        member_end=member.get('end')
        if member_end is None:
            member_end=(ordered[index+1]['start'] if index+1<len(ordered) else recipe['end'])
        selected=reviewed.get(member.get('name'))
        if selected and (selected['start'],selected['end'])==(
                member['start'],member_end):
            starts.update(row['load_offset'] for row in selected['disassembly'])
        else:
            starts.update(ins.address for ins in decoder.disasm(
                image[member['start']:member_end],member['start']))
    result={}
    for offset,proof in labels.items():
        require(type(offset) is str and offset.isdecimal() and
                set(proof)=={'label','source_line'}, 'Invalid near-label proof')
        at=recipe['start']+int(offset);label=proof['label'];line=proof['source_line']
        match=re.fullmatch(r'loc_([0-9A-Fa-f]+)',label)
        require(match is not None and type(line) is int and
                reference_line(path,line).strip().lower()==(label+':').lower() and
                int(match.group(1),16)-0x10000==at and at in starts,
                'Near label lacks pinned source and original instruction boundary')
        result[int(offset)]=label
    return result
def _checked_object_tail(recipe, image, inventory, members):
    """A reviewed MSC C object tail after the last member (integ25).

    /Ol objects can end with extra 90 pads and 1-2 declared-uninitialised
    CODE bytes (a SEGDEF longer than its LEDATA).  The tail belongs to the
    complete object, not to a function: it must follow an unconditional
    transfer ending the last member, hold only 90 pads then zero bytes in the
    oracle, overlap no inventory row, and be emitted exactly (sparse_zero
    initialised prefix plus the declared zero bytes).  Returns the object end."""
    tail=recipe.get('object_tail')
    if tail is None:
        fill=recipe.get('link_fill',{})
        if (recipe.get('kind','c')=='c' and fill.get('basis')=='link-code-word-alignment-v1'):
            from asm_module import _ends_with_return
            require(set(fill)=={'end','basis'} and type(fill['end']) is int and
                    recipe['end']+1==fill['end']==members[-1]['end'] and
                    image[recipe['end']]==0 and _ends_with_return(image,recipe['end']),
                    'Terminal C member is not clipped by a proven LINK CODE WORD fill')
            return recipe['end']
        return members[-1]['end']
    from asm_module import _ends_with_return
    start,end=tail.get('start'),tail.get('end')
    fill,zeros=tail.get('fill_nops'),tail.get('declared_uninitialized')
    require(recipe.get('kind','c')=='c' and
            set(tail)=={'start','end','kind','fill_nops','declared_uninitialized'} and
            tail['kind']=='msc-code-object-tail-v1' and
            type(start) is int and type(end) is int and type(fill) is int and type(zeros) is int and
            start==members[-1]['end'] and end==start+fill+zeros and 0<fill+zeros<=4 and
            zeros>=1 and image[start:start+fill]==bytes([0x90])*fill and
            image[start+fill:end]==bytes(zeros) and
            (_ends_with_return(image,start) or
             # integ31: the last member may keep its own MSC word-alignment 90
             # pad inside its reviewed inventory extent (audio_driver_func1E:
             # RETF, 90); the tail is then only the declared zero bytes.
             (fill==0 and image[start-1]==0x90 and _ends_with_return(image,start-1) and
              any(f.get('name')==members[-1]['name'] and f.get('end')==start and
                  start-1 in f.get('padding_offsets',()) for f in inventory['functions']))) and
            not any(type(f.get('start')) is int and type(f.get('end')) is int and
                    f['start']<end and start<f['end'] for f in inventory['functions']) and
            recipe.get('sparse_zero',{}).get(recipe['object_segment'])==
            {'initialized_prefix':end-zeros-recipe['start'],'declared_length':end-recipe['start']},
            'Object tail lacks reviewed MSC pad/uninitialised-byte proof')
    return end


def checked_members(recipe, image):
    if 'module_proof' in recipe:
        # One complete grounded ASM module: entries, not member extents.
        from asm_module import checked_module
        checked_module(recipe, image)
        require(identity(image[recipe['start']:recipe['end']])==recipe['target'],
                'Whole target identity differs')
        reviewed_functions(image)
        return
    base_inv=read_json(ROOT/'evidence/functions.json')
    inv=current_inventory(image)
    require(inv['load_sha256']==sha(image),'Inventory/oracle identity differs')
    members=recipe['members']
    # A one-member ASM recipe is admitted only to declare a reviewed embedded
    # code-island public inside its own procedure (integ30, `_incnums`).
    least=1 if recipe.get('embedded_publics') else 2
    require(type(members) is list and len(members)>=least, 'Multi recipe needs members')
    tail_end=_checked_object_tail(recipe,image,inv,members)
    require(len(members)>=least and recipe['start']==members[0]['start'] and recipe['end']==tail_end,'Bad multi interval')
    require(identity(image[recipe['start']:recipe['end']])==recipe['target'],'Whole target identity differs')
    for i,m in enumerate(members):
        require(i==0 or members[i-1]['end']==m['start'],'Member gap/overlap')
        rows=[f for f in base_inv['functions'] if f.get('name')==m['name']]
        if not rows:
            # A reviewed new inventory entry exists only in the current overlay.
            rows=[f for f in inv['functions'] if f.get('name')==m['name']]
        require(len(rows)==1,'Missing/ambiguous member')
        f=rows[0]
        verified = f['status']=='BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED' or (
            f['status']=='BOUNDARIES_AND_EMISSION_BYTES_VERIFIED' and
            f.get('start_evidence') and f.get('end_evidence') and
            sha(image[f['start']:f['end']])==f['sha256'] and
            (not f.get('bytes_hex') or bytes.fromhex(f['bytes_hex'])==image[f['start']:f['end']]))
        if not verified:
            rows=[row for row in inv['functions'] if row.get('name')==m['name']]
            require(len(rows)==1,'Missing/ambiguous reviewed member')
            f=rows[0]
            verified=(f['status']=='BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED' or
                      f['status']=='BOUNDARIES_AND_EMISSION_BYTES_VERIFIED' and
                      f.get('start_evidence') and f.get('end_evidence') and
                      sha(image[f['start']:f['end']])==f['sha256'])
        require(verified,'Unverified member boundary/emission bytes')
        from asm_module import expected_publics
        expected_public = ({f['name']} if f.get('local_symbol') else
                           expected_publics(f['name'], recipe.get('kind', 'c')))
        require(all(m.get(k)==f.get(k) for k in ('stable_id','start','end')) and
                m['public'] in expected_public and
                bool(m.get('local_symbol'))==bool(f.get('local_symbol')) and
                m['target']=={'size':f['size'],'sha256':f['sha256']},
                'Member evidence differs')
        require(identity(image[f['start']:f['end']])==m['target'],'Member bytes differ')
    _checked_embedded_islands(recipe, image)
    reviewed_functions(image)


def _checked_embedded_islands(recipe, image):
    """integ30: reviewed embedded publics of an ASM group.

    (a) A labelled CS data island strictly inside a member procedure may be
    declared PUBLIC under its reviewed code-island alias (layout/data-symbols.json,
    storage code_island, independently anchored by an original operand), which
    must name exactly that address.  (b) An interior code label `_loc_XXXXX`
    strictly inside a member may be declared PUBLIC when the pinned seg012.asm
    listing declares `loc_XXXXX:` at the recorded line, the label names this
    load address, and it is an original instruction boundary of the member (the
    near-target proof used by the referencing module).  Neither binds anything
    nor is a function entry."""
    rows=recipe.get('embedded_publics',[])
    if not rows:
        return
    import re
    symbols=read_json(ROOT/'layout/data-symbols.json')['symbols']
    for p in rows:
        require(recipe.get('kind')=='asm' and isinstance(p,dict) and
                set(p) in ({'public','offset'},{'public','offset','source_line'}) and
                type(p['offset']) is int,'Embedded public shape differs')
        at=recipe['start']+p['offset']
        members=[m for m in recipe['members'] if m['start']<at<m['end']]
        require(len(members)==1 and at not in {m['start'] for m in recipe['members']},
                'Embedded public lies outside a member procedure')
        if 'source_line' not in p:
            alias=symbols.get(p['public'])
            require(alias is not None and alias.get('storage')=='code_island' and
                    alias.get('load_address')==at and 'clone_of' not in alias,
                    'Embedded island public lacks its reviewed code-island alias')
            continue
        label=re.fullmatch(r'_loc_([0-9A-Fa-f]+)',p['public'])
        require(label is not None and int(label.group(1),16)-0x10000==at,
                'Embedded label public does not name its load address')
        path='src/restunts/asmorig/seg012.asm'
        pinned=read_json(ROOT/'layout/references.json')['restunts']['evidence_files'][path]
        from pinned_reference import check_reference_identity, reference_line
        require(check_reference_identity(path)==pinned,'Embedded label reference source differs')
        line=p['source_line']
        require(type(line) is int and
                reference_line(path,line).strip().lower()==(p['public'][1:]+':').lower(),
                'Embedded label lacks pinned source declaration')
        from asm_module import _instruction_starts
        member=members[0]
        starts,_=_instruction_starts(image,member['start'],member['end'])
        require(at in starts,'Embedded label is not an original instruction boundary')


def _code_frame(recipe, image):
    """The C object's original code frame: the inventory segment paragraph of
    every member (integ25 switch-table rule)."""
    frame=recipe.get('original_frame_load_address')
    paragraphs={f.get('segment_paragraph') for f in current_inventory(image)['functions']
                if f.get('name') in {m['name'] for m in recipe['members']}}
    require(type(frame) is int and paragraphs=={frame//16} and frame%16==0 and
            frame<=recipe['start'] and recipe['end']<=frame+65536,
            "C group frame differs from its members' code segment")
    return frame


def _bind_own_pointers(obj, recipe, image, linked, fixes, pubs, rows):
    """Adjacent MOV AX,offset / MOV DX,seg words naming one public of this C
    object: the offset is the public inside the members' verified code frame,
    the segment word its paragraph with its own MZ relocation (checked with the
    whole candidate FIXUPP stream)."""
    frame=_code_frame(recipe,image)
    linked=bytearray(linked)
    pairs={}
    for f in fixes:
        at=f['offset']
        require(f['target_kind']=='external' and f['target_method']==2 and
                f['loc'] in ('loader-offset16','offset16','base16') and f['width']==2 and
                (f['frame_method'],f['frame_kind'],f['frame'],f['frame_index'])==
                (5,'target',f['target'],0) and f['displacement']==0 and
                f['encoded_addend']=='0000' and 1<=at<=len(linked)-2 and
                0xb8<=linked[at-1]<=0xbf and linked[at:at+2]==bytes(2) and
                1<=f['target_index']<=len(obj.externals) and
                obj.externals[f['target_index']-1]==f['target'],
                'Unsupported own-public pointer datum')
        pairs.setdefault(f['target'],{'offset':[],'base':[]})[
            'base' if f['loc']=='base16' else 'offset'].append(at)
        value=(frame//16 if f['loc']=='base16' else recipe['start']-frame+pubs[f['target']])
        require(0<=value<=65535,'Own-public pointer overflow')
        struct.pack_into('<H',linked,at,value)
        rows.append({'offset':at,'target':f['target'],'linked_value':value})
    for fields in pairs.values():
        require(fields['base'] and sorted(x-3 for x in fields['base'])==sorted(fields['offset']),
                'Own-public pointer words are not adjacent MOV offset/segment pairs')
    return linked


def bind_multi(obj, recipe, image, relocations):
    seg=recipe['object_segment']; length=recipe['end']-recipe['start']
    require(obj.segment_length(seg)==length and len(obj.segment_bytes(seg))==length,'Incomplete CODE extent')
    secondary=recipe.get('secondary_dgroup_segments',{})
    require(all(n==seg or n in secondary or z==0 for n,z in obj.segment_lengths.items()),
            'Unowned data/BSS')
    pubs={m['public']:m['start']-recipe['start'] for m in recipe['members']}
    embedded={p['public']:p['offset'] for p in
              recipe.get('module_proof',{}).get('embedded_publics',[])}
    # Reviewed module data labels outside every procedure (asm_module, integ26).
    embedded.update({p['public']:p['offset'] for p in
                     recipe.get('module_proof',{}).get('data_publics',[])})
    # Reviewed code-island publics of an ASM group (integ30).
    embedded.update({p['public']:p['offset'] for p in recipe.get('embedded_publics',[])})
    require(not set(embedded)&set(pubs),'Embedded public duplicates a member')
    near_labels=_checked_asm_near_labels(recipe,image)
    # A whole C object may define global data in its own TU-owned DGROUP
    # segments (integ26); those publics are checked by bind_secondary
    # against the complete placed segment and the reviewed alias registry.
    # integ31: a whole ASM module's own _DATA labels likewise (asm012_133660).
    data_publics=[p for p in obj.publics if
                  p['segment']!=seg and p['segment'] in secondary]
    require(len(pubs)==len(recipe['members']) and
            len(obj.publics)==len(pubs)+len(embedded)+len(data_publics) and
            {p['name']:p['offset'] for p in obj.publics if p['segment']==seg}=={**pubs,**embedded},
            'Public offsets differ')
    require({p['name'] for p in obj.local_publics} ==
            {m['public'] for m in recipe['members'] if m.get('local_symbol')},
            'Local helper publics must be complete reviewed members')
    require(recipe['object_declarations']=={'segments':obj.segment_defs,'groups':obj.groups,'publics':obj.publics,'externals':obj.externals},'Full declarations differ')
    require(obj.linker_fixups==recipe['expected_fixups'],'Ordered FIXUPP differs')
    require(declared_externals(obj) <= set(pubs) | {'__acrtused'} | {f['target'] for f in obj.linker_fixups}, 'Unexpected external declaration')
    # CODE words only: FIXUPPs inside owned data segments (a module's own
    # far callback pointer, integ31) are bound by bind_secondary.
    local=[f for f in obj.linker_fixups if recipe.get('kind')=='asm' and
           f['segment']==seg and
           f['target_kind']=='segment' and f['target']==seg and
           f['loc']=='offset16']
    local_far=[f for f in obj.linker_fixups if recipe.get('kind')=='asm' and
               f['segment']==seg and
               f['target_kind']=='segment' and f['target']==seg and
               f['loc']=='pointer32']
    internal=[f for f in obj.linker_fixups if f['target'] in pubs and
              f not in local and f not in local_far]
    # A far pointer to the TU's own public (MOV AX,offset / MOV DX,seg, e.g.
    # add_exit_handler(audiodrv_atexit)): offset and base16 words (integ26).
    own_pointer=[f for f in internal if not f['self_relative'] and recipe.get('kind','c')=='c']
    internal=[f for f in internal if f not in own_pointer]
    # CODE words naming the TU's own data; FIXUPPs inside the data segments
    # themselves (pointer tables) are bound by bind_secondary (integ26).
    # A base16 word naming the TU's own _DATA is the DGROUP paragraph of a
    # `_loadds` DS reload; it stays with the composed binder (integ26) even
    # when the object now owns a nonempty _DATA (integ31, seg028).
    own_data=[f for f in obj.linker_fixups if f['segment']==seg and
              f['target_kind']=='segment' and f['target'] in secondary and
              not (f['loc']=='base16' and f['target']=='_DATA' and recipe.get('kind','c')=='c')]
    external=[f for f in obj.linker_fixups if f['segment']==seg and
              f not in internal and f not in own_pointer and f not in local and
              f not in local_far and f not in own_data]
    near_external=(recipe.get('kind')=='asm' and
                   any(f['self_relative'] and f['target_kind']=='external'
                       for f in external))
    require(all(f['self_relative'] for f in internal) and
            all(not f['self_relative'] or
                (near_external and f['target_kind']=='external' and f['loc']=='offset16')
                for f in external), 'Unsupported relative target')
    local_far_sites={recipe['start']+f['offset']+2 for f in local_far}
    own_base_sites={recipe['start']+f['offset'] for f in own_pointer if f['loc']=='base16'}
    external_relocations=[r for r in recipe['expected_relocations']
                          if r['load_offset'] not in local_far_sites|own_base_sites]
    require(len(local_far_sites)==len(local_far) and
            sorted(r['load_offset'] for r in recipe['expected_relocations']
                   if r['load_offset'] in local_far_sites)==sorted(local_far_sites),
            'Local far transfer relocation sites differ')
    if external:
        require('external_binding' in recipe,'External binding absent')
        view=copy.copy(obj); view.publics=[{'name':recipe['members'][0]['public'],'segment':seg,'offset':0}]
        view.local_publics=[]; view.local_externals=[]
        # integ33: an EXTDEF named only by an owned data segment's FIXUPPs (a
        # `dw seg` far-data word) is bound by bind_secondary, not by the CODE binder.
        data_only_ext=({f['target'] for f in obj.linker_fixups if f['segment'] in secondary and
                        f['target_kind']=='external'} -
                       {f['target'] for f in obj.linker_fixups if f['segment']==seg})
        view.externals=[n for n in obj.externals if (n not in pubs or n==view.publics[0]['name'])
                        and n not in data_only_ext]
        view.segment_lengths={**obj.segment_lengths,
                              **{name:0 for name in secondary}}
        # The CODE binder sees owned secondary segments as zero-length, as
        # before they were owned (a `_loadds` base16 still names DGROUP).
        view.segment_defs=[{**d,'length':0} if d['name'] in secondary else d
                           for d in obj.segment_defs]
        view.linker_fixups=[]
        for f in external:
            row=dict(f)
            if f['target_kind']=='external':
                require(1<=f['target_index']<=len(obj.externals) and
                        obj.externals[f['target_index']-1]==f['target'],
                        'Original external target index differs')
                row['target_index']=view.externals.index(f['target'])+1
            if f['frame_method']==2 and f['frame_kind']=='external':
                require(1<=f['frame_index']<=len(obj.externals) and
                        obj.externals[f['frame_index']-1]==f['frame'],
                        'Original external frame index differs')
                row['frame_index']=view.externals.index(f['frame'])+1
            view.linker_fixups.append(row)
        sub={'id':recipe['members'][0].get('name',recipe.get('id','')),'start':recipe['start'],'end':recipe['end'],'object_segment':seg,'public':view.publics[0]['name'],'expected_fixups':view.linker_fixups,'expected_relocations':external_relocations,'binding':{'mode':recipe['external_binding']['mode'],'declarations':{'segments':view.segment_defs,'groups':view.groups,'publics':view.publics,'externals':view.externals}}}
        if 'reviewed_near_targets' in recipe:
            sub['reviewed_near_targets']=recipe['reviewed_near_targets']
        if 'reviewed_code_offsets' in recipe:
            sub['reviewed_code_offsets']=recipe['reviewed_code_offsets']
        if 'negative_folded_index_bindings' in recipe:
            sub['negative_folded_index_bindings']=recipe['negative_folded_index_bindings']
        if recipe.get('kind')=='asm':
            sub['kind']='asm'
            sub['original_frame_load_address']=recipe['original_frame_load_address']
        elif any(f['target_kind']=='segment' and f['target'] in (seg,'_DATA')
                 for f in view.linker_fixups):
            # (A `_loadds` DGROUP base16 to the zero-length _DATA, integ26,
            # uses the same composed C binder and placement.)
            # A complete C object with MSC switch tables (offset16 words into
            # its own CODE segment, integ25): the composed C rule binds them at
            # the object's original code frame, which must be the inventory
            # segment paragraph of every member.  Their relocation-free words
            # and the object's ordered FIXUPP stream are checked as usual.
            frame=recipe.get('original_frame_load_address')
            paragraphs={f.get('segment_paragraph') for f in current_inventory(image)['functions']
                        if f.get('name') in {m['name'] for m in recipe['members']}}
            require(type(frame) is int and paragraphs=={frame//16} and frame%16==0,
                    'C switch-table group frame differs from its members\' code segment')
            sub['original_frame_load_address']=frame
        symbols=resolve_recipe_symbols(sub,image,relocations)
        payload,receipt=bind_contribution(view,sub,symbols)
    else:
        require(external_relocations==[] and
                (not recipe.get('external_binding') or
                 recipe['external_binding'].get('mode')=='asm-local-code-offset16-v1' and local),
                'Unexpected external obligations')
        payload,receipt=obj.segment_bytes(seg),{'mode':'no-external-fixups','generated_relocations':[]}
    linked=bytearray(payload); rows=[]
    linked, data_payloads, secondary_rows, data_relocations=bind_secondary(
        obj,recipe,image,relocations,linked,own_data)
    linked=bytearray(linked)
    for f in local_far:
        at=f['offset']; definition,=[d for d in obj.segment_defs if d['name']==seg]
        frame=recipe['original_frame_load_address']
        require(f['segment']==seg and f['width']==4 and not f['self_relative'] and
                f['target_method']==0 and f['target_index']==definition['index'] and
                (f['frame_method'],f['frame_kind'],f['frame'],f['frame_index'])==
                (0,'segment',seg,definition['index']) and
                f['encoded_addend']=='00000000' and
                type(f['displacement']) is int and 0<=f['displacement']<length and
                1<=at<=length-4 and linked[at-1] in (0x9a,0xea) and
                linked[at:at+4]==bytes(4) and frame%16==0 and
                frame<=recipe['start'] and recipe['end']<=frame+65536,
                'Unsupported same-module far CALL/JMP')
        value=(recipe['start']-frame+f['displacement'],frame//16)
        require(0<=value[0]<=65535,'Local far target offset overflow')
        struct.pack_into('<HH',linked,at,*value)
        rows.append({'offset':at,'target':seg,'linked_value':value})
    if own_pointer:
        linked=_bind_own_pointers(obj,recipe,image,linked,own_pointer,pubs,rows)
    for f in internal+local:
        at=f['offset']
        require(f['segment']==seg and f['loc']=='offset16' and f['width']==2 and
                f['encoded_addend']=='0000' and
                0<=at<=length-2 and
                linked[at:at+2]==bytes(2),
                'Unsupported internal code offset fixup')
        if f['target_kind']=='external':
            require(f['target_method']==2 and
                    (f['frame_method'],f['frame_kind'],f['frame'],f['frame_index'])==
                    (5,'target',f['target'],0) and
                    1<=f['target_index']<=len(obj.externals) and
                    obj.externals[f['target_index']-1]==f['target'] and f['displacement']==0,
                    'Unsupported external internal CALL datum')
            target_offset=pubs[f['target']]
            target_name=f['target']
        else:
            definition,=[d for d in obj.segment_defs if d['name']==seg]
            require(recipe.get('kind')=='asm' and f['target_kind']=='segment' and
                    f['target_method']==0 and f['target_index']==definition['index'] and
                    (f['frame_method'],f['frame_kind'],f['frame'],f['frame_index'])==
                    (0,'segment',seg,definition['index']) and
                    type(f['displacement']) is int and 0<=f['displacement']<length,
                    'Unsupported ASM same-module code datum')
            target_offset=f['displacement']
            target_name=next((name for name,offset in pubs.items() if offset==target_offset),
                             near_labels.get(target_offset))
        if f['self_relative']:
            require(1<=at and linked[at-1] in (0xe8,0xe9),
                    'Local relative fixup lacks near CALL/JMP')
            value=target_offset-(at+2)
            require(-32768<=value<=32767,'Internal displacement overflow')
            struct.pack_into('<h',linked,at,value)
        else:
            require(f in local and recipe['original_frame_load_address']<=recipe['start'] and
                    recipe['end']<=recipe['original_frame_load_address']+65536,
                    'Local absolute offset requires complete ASM module placement')
            value=recipe['start']-recipe['original_frame_load_address']+target_offset
            require(0<=value<=65535,'Local code offset overflow')
            struct.pack_into('<H',linked,at,value)
        rows.append({'offset':at,'target':target_name,'linked_value':value,
                     **({'displacement':value} if f['self_relative'] else {})})
    require(receipt['generated_relocations']==external_relocations,
            'External relocation order differs')
    # The complete object's MZ entries (external, same-module far transfers
    # and any other segment words) follow its own FIXUPP order as the pinned
    # LINK emits it; the recipe/oracle order must equal that candidate stream.
    candidate_sites=link_order_sites(fixup_relocation_sites(obj.linker_fixups,recipe['start'],seg))
    require([r['load_offset'] for r in recipe['expected_relocations']]==candidate_sites,
            'Group relocation order differs from its candidate FIXUPP order')
    generated=recipe['expected_relocations']
    return bytes(linked),{'mode':'multi-function-complete-v2','internal_calls':rows,
        'secondary_dgroup_fixups':secondary_rows,
        'secondary_payloads':{name:raw.hex() for name,raw in data_payloads.items()},
        'secondary_generated_relocations':data_relocations,
        'external':receipt,'generated_relocations':generated}
