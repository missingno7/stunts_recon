from pathlib import Path
import importlib.util,shutil,tempfile,unittest
ROOT=Path(__file__).resolve().parents[1]
SCRIPT=ROOT/'tools/porting/host_probe.py'
spec=importlib.util.spec_from_file_location('host_probe',SCRIPT)
probe=importlib.util.module_from_spec(spec); spec.loader.exec_module(probe)
GCC=shutil.which('gcc')
if not GCC and probe.DEFAULT_GCC.is_file(): GCC=str(probe.DEFAULT_GCC)

class HostProbeTests(unittest.TestCase):
    def test_source_markers_find_port_boundaries(self):
        markers=probe.source_markers('int x; /* PORT: 16-bit width */\n/* PLATFORM(video): BIOS call. */')
        self.assertTrue(markers['port_tags'])
        self.assertTrue(markers['platform_tags'])
        self.assertTrue(markers['markers']['implicit int widths'])

    @unittest.skipUnless(GCC, 'GCC is not installed; host compile smoke test skipped')
    def test_one_source_compiles_through_host_probe(self):
        out_root=ROOT/'build'/'porting'
        out_root.mkdir(parents=True,exist_ok=True)
        with tempfile.TemporaryDirectory(prefix='host-probe-test-',dir=out_root) as temp:
            work=Path(temp); src=work/'probe.c'; objdir=work/'out'; objdir.mkdir()
            src.write_text('I16 probe(void) { return (I16)sizeof(I16); }\n',encoding='utf-8')
            result=probe.run_one(Path(GCC),src,1,objdir,ROOT,ROOT/'tools/porting/host/compat.h',ROOT/'tools/porting/host/include')
            self.assertEqual(result['status'],{'syntax':'ok','object':'ok'})
            self.assertTrue((objdir/'001_probe.o').is_file())

if __name__=='__main__': unittest.main()
