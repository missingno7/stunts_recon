"""Optional observations around production C bodies, using reviewed bounded ABIs.

Generated wrappers preserve signatures and call the unchanged implementation.
Only outermost crossings into the replacement layer are comparable; nested
implementation calls need not have the original ASM's decomposition.
"""
import re
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
CONTRACTS=[
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

def instrument(source,text):
    wrappers=[]
    for c in CONTRACTS:
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
        if values:lines+=['    uint32_t boundary_args[]={'+','.join(values)+'};']
        if regions:lines+=['    PortBoundaryRegion boundary_regions[]={'+','.join(
            '{"%s",%s,%u,%u}'%r for r in regions)+'};']
        mask=sum(1<<i for i,a in enumerate(c['args']) if a is None)
        lines += [f'    int active=port_boundary_begin(&boundary_call,"{name}",'+
            ('boundary_args' if values else 'NULL')+f',{len(values)},{mask},'+
            ('boundary_regions' if regions else 'NULL')+f',{len(regions)});']
        call='port_boundary_impl_'+name+'('+','.join(c['params'])+')'
        if c['ret']:lines += [f'    {ret_type} result={call};',
            '    port_boundary_end(&boundary_call,(uint16_t)result,active);','    return result;']
        else:lines += ['    '+call+';','    port_boundary_end(&boundary_call,UINT32_MAX,active);']
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
    if source=='game/race.c':
        prepend('moveeverything','port_input_script_step((uint16_t)tmr2);')
    return 'extern void port_input_script_phase(unsigned);\nextern void port_input_script_step(uint16_t);\n'+text
