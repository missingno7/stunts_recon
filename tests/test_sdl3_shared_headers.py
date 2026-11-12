"""Port preparation must see headers before selecting/removing legacy views."""
from pathlib import Path
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT/'tools/porting'))
from project_headers import expand_project_headers


class SharedHeaderTests(unittest.TestCase):
    def test_nested_guards_preserve_one_scalar_anchor_and_declarations(self):
        with tempfile.TemporaryDirectory() as d:
            root = Path(d); (root/'include').mkdir()
            (root/'include/stunts_types.h').write_text('#define I16 short\n')
            (root/'include/vec3.h').write_text('#ifndef VEC3_H\n#define VEC3_H\n#include "stunts_types.h"\nstruct VECTOR { I16 x,y,z; };\n#endif\n')
            (root/'include/math3d.h').write_text('#ifndef MATH3D_H\n#define MATH3D_H\n#include "vec3.h"\nextern I16 radius(struct VECTOR *);\n#endif\n')
            text = expand_project_headers('#include "stunts_types.h"\n#include "math3d.h"\n#include "vec3.h"\nint body;\n',root)
            self.assertEqual(text.count('#include "stunts_types.h"'),1)
            self.assertEqual(text.count('struct VECTOR {'),2)
            self.assertEqual(text.count('#ifndef VEC3_H'),2)
            self.assertIn('extern I16 radius',text)
            self.assertIn('int body;',text)
            self.assertNotIn('#include "math3d.h"',text)

    def test_cycle_and_escape_are_refused(self):
        with tempfile.TemporaryDirectory() as d:
            root=Path(d);(root/'include').mkdir()
            (root/'include/a.h').write_text('#include "b.h"\n')
            (root/'include/b.h').write_text('#include "a.h"\n')
            for source in ('#include "a.h"\n','#include "../escape.h"\n'):
                with self.assertRaises(ValueError):expand_project_headers(source,root)

    def test_existing_units_expose_their_shared_types_and_imports(self):
        import host_probe_declarations as declarations
        import cdecls
        unit='lib/whlhelp3.c'
        text=expand_project_headers((ROOT/unit).read_text(),ROOT)
        parsed=cdecls.Unit(declarations.source_parser_text(text),unit)
        self.assertIn('Point',parsed.typedefs)
        self.assertIn('fixedmult',parsed.decls)
        self.assertIn('ellipse',parsed.decls)


if __name__=='__main__':unittest.main()
