"""The game macros require MSC byte classifications, not the host CRT ABI."""
import json
import os
from pathlib import Path
import subprocess
import tempfile
import unittest
from tests.test_sdl3_polygon_raster import ROOT,OracleImage,host_compiler

class OriginalCtypeTests(unittest.TestCase):
    def test_all_eof_and_byte_classifications_match_the_locked_runtime(self):
        symbols=json.loads((ROOT/'layout/data-symbols.json').read_text())['symbols']
        symbol=symbols['__ctype'];self.assertEqual(symbol['width'],257)
        expected=OracleImage.load(ROOT/'assets').load_image[symbol['load_address']:symbol['load_address']+257]
        with tempfile.TemporaryDirectory(prefix='stunts-msc-ctype-') as tmp:
            p=Path(tmp);source=p/'probe.c';exe=p/'probe.exe';out=p/'table.bin'
            source.write_text('#include <stdio.h>\n#include "'+(ROOT/'port/msc_ctype.h').as_posix()+'"\n'+r'''
int main(int argc,char **argv){FILE *f=fopen(argv[1],"wb");if(!f)return 1;
 int ok=fwrite(port_msc_ctype,1,sizeof(port_msc_ctype),f)==257;return fclose(f)||!ok;}
''')
            cc=host_compiler();self.assertIsNotNone(cc);env=os.environ.copy()
            env['PATH']=os.pathsep.join([str(Path(cc).parent),env.get('PATH','')])
            result=subprocess.run([cc,'-std=c11','-Werror',str(source),'-o',str(exe)],env=env,capture_output=True,text=True,timeout=30)
            self.assertEqual(result.returncode,0,result.stderr)
            result=subprocess.run([str(exe),str(out)],env=env,capture_output=True,text=True,timeout=5)
            self.assertEqual(result.returncode,0,result.stderr)
            self.assertEqual(out.read_bytes(),expected,'all 257 original MSC table bytes')
            self.assertEqual(expected[ord('d')+1]&2,2,'the file-dialog initial-letter lookup')

if __name__=='__main__':unittest.main()
