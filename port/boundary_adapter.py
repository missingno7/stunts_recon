"""Optional observations around production C bodies, using reviewed bounded ABIs.

Generated wrappers preserve signatures and call the unchanged implementation.
Only outermost crossings into the replacement layer are comparable; nested
implementation calls need not have the original ASM's decomposition.
"""
import re
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
SCENARIO_PHASES={
 'game/frontend.c':{'run_intro':(1,[]),'select':(2,[]),'getprotection':(3,['selection']),
    'selectcar':(7,['caridptr','materialofs','transmissionofs','opponenttype']),
    'selectopponent':(8,[]),'teditmain':(9,['restart']),'evaluation':(14,[]),
    'drawhighs':(15,[]),'addhighs':(16,['score','text','carStyle'])},
 'lib/lineedit.c':{'get_pstring':(4,['flags','buffer','pendingKey','bufferLimit','maxWidth','x','y','readCallback','timerOffset','timerSegment'])},
 'game/race.c':{'run_game':(5,[]),'doreplay':(19,['mode','frame_index','frame_offset']),
    'savereplay':(24,['filename'])},'game/trackedt.c':{'trackedit':(10,[])},
 'game/uires.c':{'floadreq':(11,['path','selected_name','attributes','heading']),
    'fsavereq':(12,['name','directory','title']),
    'messagebox':(13,['type','check','message','x','y','frame_arg','disabled','initial']),
    'sprite_blit_to_video':(17,['sprite','mode']),'mykeywait':(18,['timeout']),
    'clear_keyboardbuffer':(23,[])}}
CONTRACTS=[
 dict(name='port_call_read_line',original_name='instringz',source='legacy_views.c',
      signature='int16_t port_call_read_line(char *buffer, int16_t max_length, int16_t x, int16_t y, int32_t timeout)',
      params=['buffer','max_length','x','y','timeout'],args=[None,'max_length','x','y','timeout','(timeout >> 16)'],ret=16,
      regions=[('text','buffer','strlen(buffer)+1u',1),('buffer','buffer','(uint16_t)max_length+1u',0)]),
 dict(name='savefilez',source='file.c',signature='int savefilez(const char *filename, const void *source, unsigned long length)',
      params=['filename','source','length'],args=[None,None,'length','(length >> 16)'],ret=16,far=[1],
      regions=[('filename','filename','filename ? strlen(filename)+1u : 0',1)]),
 dict(name='intsin',source='sincos.c',signature='int16_t intsin(uint16_t angle)',params=['angle'],args=['angle'],ret=16,
      regions=[('sine_table','s_sine_table',514,1)],globals=[('_sinetable',514,1)]),
 dict(name='intcos',source='sincos.c',signature='int16_t intcos(uint16_t angle)',params=['angle'],args=['angle'],ret=16,
      regions=[('sine_table','s_sine_table',514,1)],globals=[('_sinetable',514,1)]),
 dict(name='fixedmult',source='sincos.c',signature='int16_t fixedmult(int16_t left, int16_t right)',params=['left','right'],args=['left','right'],ret=16),
 dict(name='intatan',source='polang.c',signature='int16_t intatan(int16_t z_arg, int16_t x_arg)',params=['z_arg','x_arg'],args=['z_arg','x_arg'],ret=16,
      regions=[('atan_table','atantable',257,1)],globals=[('_atantable',257,1)]),
 dict(name='inthypot',source='polang.c',signature='uint16_t inthypot(int16_t z_arg, int16_t y_arg)',params=['z_arg','y_arg'],args=['z_arg','y_arg'],ret=16),
 dict(name='mat_vec',source='matrix.c',signature='void mat_vec(const int16_t *input, const int16_t *matrix, int16_t *output)',params=['input','matrix','output'],args=[None,None,None],ret=0,
      regions=[('input','input',6,1),('matrix','matrix',18,1),('output','output',6,0)]),
 dict(name='transmult',source='matrix.c',signature='void transmult(const int16_t *right_matrix, const int16_t *left_matrix, int16_t *output_matrix)',params=['right_matrix','left_matrix','output_matrix'],args=[None,None,None],ret=0,
      regions=[('right','right_matrix',18,1),('left','left_matrix',18,1),('output','output_matrix',18,0)]),
 dict(name='zclipline',source='matrix.c',signature='void zclipline(int16_t *first, int16_t *second, int16_t *output, int16_t target_depth)',params=['first','second','output','target_depth'],args=[None,None,None,'target_depth'],ret=0,
      regions=[('first','first',6,1),('second','second',6,1),('output','output',6,0)]),
 dict(name='mysetseeds',source='random.c',signature='void mysetseeds(const uint8_t *seed)',params=['seed'],args=[None],ret=0,
      regions=[('seed','seed',6,1),('randomseeds','randomseeds',6,0)],globals=[('_randomseeds',6,0)]),
 dict(name='mygetseeds',source='random.c',signature='void mygetseeds(uint8_t *seed_out)',params=['seed_out'],args=[None],ret=0,
      regions=[('seed','seed_out',6,0),('randomseeds','randomseeds',6,1)],globals=[('_randomseeds',6,1)]),
 dict(name='myrandom',source='random.c',signature='int16_t myrandom(void)',params=[],args=[],ret=16,
      regions=[('randomseeds','randomseeds',6,1)],globals=[('_randomseeds',6,1)]),
 *[dict(name=n,source='input.c',signature=f'int16_t {n}(void)',params=[],args=[],ret=16)
   for n in ('normkey','normkeyi')],
 dict(name='joystate',source='input.c',signature='int16_t joystate(void)',params=[],args=[],ret=16,
      regions=[('keys','s_key_down',90,1)],globals=[('_kbinput',90,1)]),
]

# Opt-in diagnostic spans. The original uses FAR shape pointers and a NEAR
# vertex list; descriptor offsets are grounded in patlines.ASM's SPRITE.
GRAPHICS_CONTRACTS=[]
for n in ('movcfxya','movpackcfxya','andcfxy','orcfxy','movpackcf','movcf'):
    xy=n not in ('movpackcf','movcf')
    regions=[('shape','shape',8 if n in ('andcfxy','orcfxy') else 12 if not xy else 4,1),
             ('clip','s_sprite1.words2',10,1),
             ('row9','port_boundary_graphics_row9()','port_boundary_graphics_row9_size()',1),
             ('corner','&graphics_corner',8,1)]
    if n!='movpackcfxya' and n!='movpackcf':regions.append(('source_pixels','&graphics_source',8,1))
    GRAPHICS_CONTRACTS.append(dict(name=n,source='sprite.c',signature='void '+n+'(const PortShape2D *shape'+(', int16_t x, int16_t y' if xy else '')+')',
        params=['shape']+(['x','y'] if xy else []),args=[None]+(['x','y'] if xy else []),ret=0,far=[0],graphics=True,
        regions=regions,globals=[('_sprite1',10,1,12)]))
for n in ('drawpolypt','drawpolyp','preRender_patterned','preRender_line','clippix'):
    if n=='preRender_line':params=['x1','y1','x2','y2','color'];pointer=None
    elif n=='clippix':params=['left','right','top','bottom'];pointer=None
    else:params=(['pattern_bits'] if n=='preRender_patterned' else [])+['color','point_count','points'];pointer='points'
    regions=[('clip','s_sprite1.words2',10,1),('row9','port_boundary_graphics_row9()','port_boundary_graphics_row9_size()',1),('corner','&graphics_corner',8,1)]
    if pointer:regions.insert(0,('points','points','(uint16_t)point_count*4u',1))
    GRAPHICS_CONTRACTS.append(dict(name=n,source='sprite.c',signature='void '+n+'('+', '.join('const int16_t *points' if p=='points' else 'int16_t '+p for p in params)+')',
        params=params,args=[None if p=='points' else p for p in params],ret=0,graphics=True,
        regions=regions,globals=[('_sprite1',10,1,12)]))

def instrument(source,text):
    wrappers=[]
    for c in CONTRACTS+GRAPHICS_CONTRACTS:
        if c['source']!=source:continue
        name=c['name'];signature=c['signature'];ret_type=signature.split(name+'(')[0].strip()
        # Rename only the definition; internal call decomposition stays visible
        # to the wrapper's depth guard, with no changes to the function body.
        pattern=r'(?m)^([^;{}\n]*\b)'+name+r'(\s*\([^;{}]*\)\s*\{)'
        text,count=re.subn(pattern,lambda m:m[1]+'port_boundary_impl_'+name+m[2],text,count=1)
        if count!=1:raise ValueError('missing production boundary definition: '+name)
        values=[f'(uint16_t){a}' if a else '0' for a in c['args']]
        regions=c.get('regions',[])
        lines=[signature+' {','    PortBoundaryCall boundary_call;']
        if c.get('graphics'):
            call='port_boundary_impl_'+name+'('+','.join(c['params'])+')'
            lines+=['    if(!port_boundary_graphics_enabled()){'+call+';return;}',
                    '    uint64_t graphics_corner=port_boundary_graphics_corner();']
            if any(r[0]=='source_pixels' for r in regions):lines+=['    uint64_t graphics_source=port_boundary_graphics_source(shape);']
        if values:lines+=['    uint32_t boundary_args[]={'+','.join(values)+'};']
        if regions:lines+=['    PortBoundaryRegion boundary_regions[]={'+','.join(
            '{"%s",%s,%s,%u}'%r for r in regions)+'};']
        mask=sum(1<<i for i,a in enumerate(c['args']) if a is None)
        lines += [f'    int active=port_boundary_begin(&boundary_call,"{name}",'+
            ('boundary_args' if values else 'NULL')+f',{len(values)},{mask},'+
            ('boundary_regions' if regions else 'NULL')+f',{len(regions)});']
        call='port_boundary_impl_'+name+'('+','.join(c['params'])+')'
        if c['ret']:lines += [f'    {ret_type} result={call};',
            '    port_boundary_end(&boundary_call,(uint16_t)result,active);','    return result;']
        else:
            lines += ['    '+call+';']
            if c.get('graphics'):
                lines+=['    graphics_corner=port_boundary_graphics_corner();']
                if any(r[0]=='source_pixels' for r in regions):lines+=['    graphics_source=port_boundary_graphics_source(shape);']
            lines+=['    port_boundary_end(&boundary_call,UINT32_MAX,active);']
        wrappers.append('\n'.join(lines+['}']))
    return '#include "boundary_record.h"\n'+text+'\n'+'\n'.join(wrappers) if wrappers else text

def phase_adapt(source,text):
    from semantic_audit import functions
    from host_probe_declarations import mask_comments
    def prepend(name,statement):
        nonlocal text
        body=functions(text)[name]['body']
        brace=mask_comments(body).index('{');at=text.index(body)
        text=text[:at]+body[:brace+1]+'\n    '+statement+'\n'+body[brace+1:]+text[at+len(body):]
    phases={'game/frontend.c':{'run_intro':1,'select':2,'getprotection':3},
            'lib/lineedit.c':{'get_pstring':4},'game/race.c':{'run_game':5}}
    for name,phase in phases.get(source,{}).items():
        prepend(name,'port_input_script_phase('+str(phase)+');')
    if source=='game/uires.c':prepend('mykey','port_input_script_ui_poll();')
    if source=='game/race.c':
        prepend('moveeverything','port_input_script_step((uint16_t)tmr2);')
        pattern=r'(?m)^(void\s+far\s+)dodash(\s*\(I16 mode\))'
        text,count=re.subn(pattern,r'\1port_dodash_impl\2',text,count=1)
        if count!=1:raise ValueError('missing dashboard completion boundary')
        text+='\nextern void port_video_publish(const char *);\nvoid dodash(I16 mode) {\n'
        text+='    port_dodash_impl(mode);\n    if(mode==2) port_video_publish("race_frame_complete");\n}\n'
    for name,(phase,params) in SCENARIO_PHASES.get(source,{}).items():
        f=functions(text)[name];signature=f['signature'].rstrip(';')
        body=f['body'];at=text.index(body)
        replacement,count=re.subn(r'\b'+name+r'\s*\(', 'port_scope_impl_'+name+'(',body,count=1)
        text=text[:at]+replacement+text[at+len(body):]
        if count!=1:raise ValueError('missing scoped scenario entry: '+name)
        ret=signature.split(name+'(')[0].strip()
        call='port_scope_impl_'+name+'('+','.join(params)+')'
        expression=str(phase)+'+(unsigned)mode' if name=='doreplay' else str(phase)
        text+='\n'+signature+' {\n    port_input_script_scope_enter('+expression+');\n'
        if name=='evaluation':
            text+='    if(port_input_script_evaluation_score()){\n'
            text+='        race_stats.game_total_finish=(I16)port_input_script_evaluation_score();\n'
            text+='        race_stats.game_penalty=0;\n    }\n'
        text+=('    '+call+';\n    port_input_script_scope_leave();\n' if ret=='void' or ret=='void far' else
               '    '+ret+' result='+call+';\n    port_input_script_scope_leave();\n    return result;\n')+'}\n'
    return ('extern void port_input_script_phase(unsigned);\nextern void port_input_script_step(uint16_t);\n'
            'extern void port_input_script_scope_enter(unsigned);\nextern void port_input_script_scope_leave(void);\n'
            'extern void port_input_script_ui_poll(void);\nextern unsigned port_input_script_evaluation_score(void);\n'+text)
