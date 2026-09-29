from pathlib import Path
import importlib.util,json,shutil,subprocess,sys,tempfile,unittest
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

    @unittest.skipUnless(GCC, 'GCC is not installed; port-mode regression skipped')
    def test_compat_and_strict_central_modes_compile_all_sources(self):
        for mode in ('compat', 'strict-central'):
            with self.subTest(mode=mode):
                run = subprocess.run(
                    [sys.executable, str(SCRIPT), '--mode', mode, '--gcc', GCC],
                    cwd=ROOT, capture_output=True, text=True, timeout=900, check=False)
                self.assertEqual(run.returncode, 0, run.stdout + run.stderr)
                results_path = ROOT/'build'/'porting'/'host-probe'/mode/'results.json'
                data = json.loads(results_path.read_text(encoding='utf-8'))
                self.assertEqual(data['source_count'], 38, run.stdout)
                self.assertEqual(data['syntax_failures'], 0, run.stdout)
                self.assertEqual(data['object_failures'], 0, run.stdout)
                self.assertTrue(all(row['status'] == {'syntax': 'ok', 'object': 'ok'}
                                    for row in data['results']), run.stdout)

if __name__=='__main__': unittest.main()
