import os
import sys
import tempfile
import unittest
from pathlib import Path
from unittest import mock

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
import toolcache


class ToolCache(unittest.TestCase):
    def setUp(self):
        self.base = Path(tempfile.mkdtemp())
        self.store = mock.patch.object(toolcache, 'STORE', self.base / 'store'); self.store.start()
        self.env = mock.patch.dict(os.environ, {'STUNTS_FRESH': '0'}); self.env.start()
        self.runs = 0

    def tearDown(self):
        self.store.stop(); self.env.stop()

    def work(self, source=b'int x;'):
        work = Path(tempfile.mkdtemp(dir=self.base)); (work / 'UNIT.C').write_bytes(source)
        return work

    def execute(self, work, code=0):
        def run():
            self.runs += 1
            (work / 'UNIT.OBJ').write_bytes(b'object' + (work / 'UNIT.C').read_bytes())
            return code, b'console'
        return run

    def call(self, source=b'int x;', flags=('/AM',), code=0):
        work = self.work(source)
        result = toolcache.run('test', work, {'UNIT.C': source}, {'flags': list(flags)}, self.execute(work, code))
        return work, result

    def test_identical_request_restores_outputs_without_running(self):
        _, first = self.call()
        work, second = self.call()
        self.assertEqual((self.runs, first, second), (1, (0, b'console'), (0, b'console')))
        self.assertEqual((work / 'UNIT.OBJ').read_bytes(), b'objectint x;')

    def test_source_or_parameter_change_runs_again(self):
        self.call(); self.call(source=b'int y;'); self.call(flags=('/AL',))
        self.assertEqual(self.runs, 3)

    def test_failure_is_reproduced_with_its_exit_code(self):
        self.call(code=2)
        self.assertEqual(self.call(code=2)[1], (2, b'console'))
        self.assertEqual(self.runs, 1)

    def test_fresh_never_reads_or_writes_the_store(self):
        self.call()
        with mock.patch.dict(os.environ, {'STUNTS_FRESH': '1'}):
            self.call(); self.call(source=b'int z;')
        self.assertEqual(self.runs, 3)
        self.call(source=b'int z;')
        self.assertEqual(self.runs, 4)

    def test_damaged_entry_is_discarded_and_run_again(self):
        self.call()
        stored = next((self.base / 'store').rglob('UNIT.OBJ')); stored.write_bytes(b'tampered')
        work, _ = self.call()
        self.assertEqual(self.runs, 2)
        self.assertEqual((work / 'UNIT.OBJ').read_bytes(), b'objectint x;')


if __name__ == '__main__':
    unittest.main()
