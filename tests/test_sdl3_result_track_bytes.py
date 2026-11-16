"""Original byte-compare flags versus the production result-screen adapter."""
import os
from pathlib import Path
import re
import subprocess
import tempfile
import unittest
from tests.test_sdl3_renderer_arithmetic import (
    ROOT,OracleImage,Uc,UC_ARCH_X86,UC_MODE_16,UC_X86_REG_AX,UC_X86_REG_CS,UC_X86_REG_EFLAGS)
from unicorn.x86_const import UC_X86_REG_BX,UC_X86_REG_DI,UC_X86_REG_ES
from port.game_abi import adapt_result_track_comparison

class ResultTrackByteTests(unittest.TestCase):
    def test_all_byte_pairs_match_original_cmp_and_adapter_changes_one_site(self):
        original=(ROOT/'game/frontend.c').read_text();adapted=adapt_result_track_comparison(original)
        helper=re.search(r'static int port_track_byte_equal[^}]+}',adapted).group()
        self.assertEqual(adapted.count('!port_track_byte_equal(trackFile[i], td14tb[i])'),1)
        image=OracleImage.load();self.assertEqual(image.load_image[0x399d:0x39a0],bytes.fromhex('263801'))
        compiler=Path('C:/msys64/mingw32/bin/gcc.exe')
        with tempfile.TemporaryDirectory(prefix='stunts-result-bytes-') as tmp:
            p=Path(tmp);source=p/'probe.c';exe=p/'probe.exe';output=p/'pairs.bin'
            source.write_text('#include <stdint.h>\n#include <stdio.h>\n'+helper+r'''
int main(int argc,char **argv){FILE *f=fopen(argv[1],"wb");if(!f)return 1;
for(unsigned a=0;a<256;++a)for(unsigned b=0;b<256;++b){
 unsigned char equal=port_track_byte_equal((char)a,(uint8_t)b);if(fwrite(&equal,1,1,f)!=1)return 2;}
return fclose(f)!=0;}
''')
            env=os.environ.copy();env['PATH']=str(compiler.parent)+os.pathsep+env.get('PATH','')
            result=subprocess.run([str(compiler),'-std=c11','-O2',str(source),'-o',str(exe)],env=env,capture_output=True,text=True,timeout=30)
            self.assertEqual(result.returncode,0,result.stderr)
            result=subprocess.run([str(exe),str(output)],env=env,capture_output=True,text=True,timeout=5)
            self.assertEqual(result.returncode,0,result.stderr);observed=output.read_bytes()
        self.assertEqual(len(observed),65536)
        machine=Uc(UC_ARCH_X86,UC_MODE_16);machine.mem_map(0,1<<20)
        machine.mem_write(0x10000,image.relocated(0x1000));machine.reg_write(UC_X86_REG_CS,0x1000)
        machine.reg_write(UC_X86_REG_ES,0x7000);machine.reg_write(UC_X86_REG_BX,0x100);machine.reg_write(UC_X86_REG_DI,0)
        for b in range(256):
            machine.mem_write(0x70100,bytes([b]))
            for a in range(256):
                machine.reg_write(UC_X86_REG_AX,a);machine.reg_write(UC_X86_REG_EFLAGS,2)
                machine.emu_start(0x1399d,0,count=1)
                self.assertEqual(observed[a*256+b],int(bool(machine.reg_read(UC_X86_REG_EFLAGS)&0x40)),(a,b))

if __name__=='__main__':unittest.main()
