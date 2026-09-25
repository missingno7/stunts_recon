"""Pinned include resolution and frozen recipe closure checks."""
import json
import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from common import identity
import preprocessor
from compiler import compile_source


class PreprocessorTests(unittest.TestCase):
    def test_intrinsic_and_function_pragmas_are_frozen_and_compiled(self):
        source = (b'#include <conio.h>\n#pragma intrinsic(inp, outp)\n'
                  b'int port_read(void) { return inp(0x3da); }\n')
        expanded, closure = preprocessor.prepare(source, 'msc510-medium')
        self.assertIn(b'#pragma intrinsic(inp, outp)', expanded)
        self.assertEqual(closure[-1], {'pragma':'intrinsic','names':['inp','outp'],
                                      'path':'<source>','line':2})
        obj, receipt = compile_source(source, 'msc510-medium')
        self.assertEqual(receipt['preprocessor_closure'], closure)
        self.assertIn(b'\xec', obj.segment_bytes('UNIT_TEXT'))
        forced = b'#include <conio.h>\n#pragma function(inp)\nint port_read(void) { return inp(0x3da); }\n'
        obj, receipt = compile_source(forced, 'msc510-medium')
        self.assertEqual(receipt['preprocessor_closure'][-1]['pragma'], 'function')
        self.assertNotIn(b'\xec', obj.segment_bytes('UNIT_TEXT'))
        math = b'#include <math.h>\n#pragma intrinsic(sqrt)\ndouble root(double x) { return sqrt(x); }\n'
        obj, receipt = compile_source(math, 'msc510-medium')
        self.assertGreater(obj.segment_length('UNIT_TEXT'), 0)
        self.assertEqual(receipt['preprocessor_closure'][-1]['names'], ['sqrt'])
        with self.assertRaisesRegex(ValueError, 'closure'):
            preprocessor.check_recipe({'preprocessor_closure':[]}, closure)
        for bad in (b'#pragma intrinsic(disable)\n',
                    b'#pragma function(other)\n',
                    b'#pragma intrinsic(inp); junk\n'):
            with self.subTest(bad=bad), self.assertRaisesRegex(ValueError, 'Unsupported historical pragma'):
                preprocessor.prepare(bad, 'msc510-medium')

    def test_pack_directives_compile_and_enter_frozen_closure(self):
        source = (b'#pragma pack(1)\nstruct packed { char a; int b; };\n'
                  b'#pragma pack()\nint packed_size(void) { return sizeof(struct packed); }\n')
        expanded, closure = preprocessor.prepare(source, 'msc510-medium')
        self.assertEqual(expanded, source)
        self.assertEqual(closure, [
            {'pragma':'pack', 'value':1, 'path':'<source>', 'line':1},
            {'pragma':'pack', 'value':None, 'path':'<source>', 'line':3}])
        obj, receipt = compile_source(source, 'msc510-medium')
        self.assertEqual(receipt['preprocessor_closure'], closure)
        self.assertIn(b'\xb8\x03\x00', obj.segment_bytes('UNIT_TEXT'))
        with self.assertRaisesRegex(ValueError, 'closure'):
            preprocessor.check_recipe({'preprocessor_closure':[]}, closure)
        with self.assertRaisesRegex(ValueError, 'Unsupported historical pragma'):
            preprocessor.prepare(b'#pragma optimize("", off)\n', 'msc510-medium')
        with self.assertRaisesRegex(ValueError, 'Unsupported historical pragma'):
            preprocessor.prepare(b'#pragma pack(8)\n', 'msc510-medium')

    def test_real_pinned_dos_header_and_existing_source(self):
        expanded, closure = preprocessor.prepare(b'#include <dos.h>\nint x;\n', 'msc510-medium')
        self.assertIn(b'#define FP_SEG', expanded)
        self.assertEqual([row['path'] for row in closure], ['toolchain/msc510/INCLUDE/DOS.H'])
        self.assertEqual(preprocessor.prepare((ROOT/'src/copy_string.c').read_bytes(),
                                              'msc510-medium')[1], [])

    def test_tamper_outside_root_and_recipe_mismatch(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            include = root/'toolchain/msc510/INCLUDE'
            include.mkdir(parents=True)
            header = include/'PIN.H'
            header.write_bytes(b'#define VALUE 7\n')
            row = {'path':'toolchain/msc510/INCLUDE/PIN.H', **identity(header.read_bytes())}
            lock = {'profiles':{'msc510-medium':{'directory':'toolchain/msc510',
                                                'files':[row]}}}
            (root/'layout').mkdir()
            (root/'layout/toolchain.json').write_text(json.dumps(lock))
            project = root/'include'
            project.mkdir()
            (project/'local.h').write_bytes(b'#ifndef LOCAL_H\n#define LOCAL_H\n#undef VALUE\n#endif\n')
            with patch.object(preprocessor, 'ROOT', root):
                expanded, closure = preprocessor.prepare(b'#include <pin.h>\n#if VALUE\nint x;\n#else\nint y;\n#endif\n',
                                                         'msc510-medium')
                self.assertIn(b'#define VALUE 7', expanded)
                preprocessor.check_recipe({'preprocessor_closure':closure}, closure)
                with self.assertRaisesRegex(ValueError, 'closure'):
                    preprocessor.check_recipe({'preprocessor_closure':[]}, closure)
                with self.assertRaisesRegex(ValueError, 'allowed root'):
                    preprocessor.prepare(b'#include "../outside.h"\n', 'msc510-medium')
                with patch('preprocessor.subprocess.run') as git:
                    git.return_value.returncode = 0
                    _, local_closure = preprocessor.prepare(b'#include "local.h"\n', 'msc510-medium')
                    self.assertEqual(local_closure[0]['path'], 'include/local.h')
                    git.return_value.returncode = 1
                    with self.assertRaisesRegex(ValueError, 'not tracked'):
                        preprocessor.prepare(b'#include "local.h"\n', 'msc510-medium')
                with self.assertRaisesRegex(ValueError, 'hash mismatch'):
                    header.write_bytes(b'#define VALUE 8\n')
                    preprocessor.prepare(b'#include <pin.h>\n', 'msc510-medium')


if __name__ == '__main__':
    unittest.main()
