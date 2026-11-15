"""Real race arguments: original capture, original machine code, production raster."""
import json
import os
from pathlib import Path
import struct
import subprocess
import tempfile
import unittest
from tests.test_sdl3_polygon_raster import (
    ROOT,host_compiler,RealModeRunner,OracleImage,SymbolMap,MemoryWrite,MemoryRegion,RoutineCase)


class RaceHorizonTests(unittest.TestCase):
    def test_captured_single_surviving_scanline_against_original_machine(self):
        fixture=json.loads((ROOT/'tools/porting/step_oracle_cases/race-horizon.json').read_text())
        cases=fixture['cases'];self.assertEqual(len(cases),27)
        compiler=host_compiler();self.assertIsNotNone(compiler)
        with tempfile.TemporaryDirectory(prefix='stunts-race-horizon-') as tmp:
            folder=Path(tmp);source=folder/'probe.c';exe=folder/'probe.exe';output=folder/'pixels.bin'
            source.write_text('#define main original_probe_main\n#include "'+
                (ROOT/'tests/sdl3/raster_harness.c').as_posix()+'"\n#undef main\n'+r'''
int main(int argc,char **argv){
    FILE *out=fopen(argv[1],"wb");int color,count,clip[5],v;int16_t points[128];
    if(!out)return 2;
    while(scanf("%d %d",&color,&count)==2){
        if(count<3||count>64)return 3;
        for(int i=0;i<5;++i)if(scanf("%d",&clip[i])!=1)return 4;
        for(int i=0;i<count*2;++i){if(scanf("%d",&v)!=1)return 5;points[i]=(int16_t)v;}
        memset(test_video,0,sizeof(test_video));port_sprite_init();
        clippix(clip[0],clip[1],clip[2],clip[3]);
        for(int i=0;i<320;++i){if(scanf("%d",&v)!=1)return 6;test_video[9*320+i]=(uint8_t)v;}
        drawpolypt(color,count,points);
        if(fwrite(test_video,1,64000,out)!=64000)return 7;
    }
    return fclose(out)!=0;
}
''')
            env=os.environ.copy();env['PATH']=os.pathsep.join([str(Path(compiler).parent),env.get('PATH','')])
            result=subprocess.run([compiler,'-std=c11','-O2','-ffunction-sections','-fdata-sections',
                '-DPORT_BUILD','-Iport',str(source),'-Wl,--gc-sections','-o',str(exe)],
                cwd=ROOT,env=env,capture_output=True,text=True,timeout=45)
            self.assertEqual(result.returncode,0,result.stderr)
            words=[]
            for c in cases:
                words.extend([*c['args'],*c['clip'],*c['points'],*bytes.fromhex(c['row9_before'])])
            result=subprocess.run([str(exe),str(output)],input=' '.join(map(str,words)),
                env=env,capture_output=True,text=True,timeout=10)
            self.assertEqual(result.returncode,0,result.stderr);actual=output.read_bytes()
            runner=RealModeRunner(OracleImage.load(ROOT/'assets'),instruction_budget=100000)
            routine=SymbolMap().resolve('drawpolypt');cs,_=routine.far_at(0x1000)
            data=json.loads((ROOT/'layout/data-symbols.json').read_text())['symbols']
            sprite_offset=data['_sprite1']['load_address']-routine.code_base
            for i,c in enumerate(cases):
                with self.subTest(frame=c['frame'],points=c['points']):
                    sprite=[0]*15;sprite[1]=0xa000;sprite[5]=0xc000
                    sprite[6:11]=c['clip'];sprite[12:15]=[320,0,320]
                    args=[*c['args'],0xd000];stack=[0,0x7000,*args]
                    initial=bytearray(65536);initial[9*320:10*320]=bytes.fromhex(c['row9_before'])
                    case=RoutineCase('captured-horizon','drawpolypt',args=args,call='far',
                        registers={'ds':0x3b77,'ss':0x3b77,'sp':0xf000},memory=[
                            MemoryWrite(cs,sprite_offset,struct.pack('<15H',*sprite)),
                            MemoryWrite(cs,0xc000,struct.pack('<200H',*(y*320 for y in range(200)))),
                            MemoryWrite(0x3b77,0xd000,struct.pack('<'+'h'*len(c['points']),*c['points'])),
                            MemoryWrite(0xa000,0,bytes(initial)),
                            MemoryWrite(0x3b77,0xf000,struct.pack('<5H',*stack))],
                        compare=(MemoryRegion('pixels',0xa000,0,64000),),result_registers=())
                    expected=runner.call(case).memory['pixels']
                    self.assertEqual(expected[9*320:10*320].hex(),c['row9_after'],
                        'isolated original must reproduce captured original row')
                    self.assertEqual(actual[i*64000:(i+1)*64000],expected,'all 64000 pixels')
            self.assertEqual(len(actual),len(cases)*64000)

if __name__=='__main__':unittest.main()
