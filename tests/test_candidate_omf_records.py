"""Candidate OMF-record extraction and relocation order from fresh MSC output."""
import copy
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
from compiler import compile_source
from prefix_proof import candidate_records, relocation_stream

SEGMENT = 'UNIT_TEXT'
START = 0x10000


def _body(tag, count):
    return ''.join(f'    r = r + ext_a(a + {tag * 100 + j}) * {j + 3};\n'
                   f'    b = b ^ ext_b(r, {j + tag});\n' for j in range(count))


def unit(*, sizes=(10,) * 5, f6=24, f7=True, call7=False, extra0='', tail6='', first6=None,
         body7='    return a + 1;\n'):
    """f0 calls f5 (and f7 with call7); f6 spans the record the short unit leaves open."""
    out = ['extern int far ext_a(int);', 'extern int far ext_b(int, int);',
           'int f5(int);', 'int f7(int);']
    for i, count in enumerate(sizes):
        calls = ('    r = r + f5(b);\n' if i == 0 else '') + ('    r = r + f7(a);\n' if i == 0 and call7 else '')
        out.append(f'int f{i}(int a, int b)\n{{\n    int r;\n    r = 0;\n' + (extra0 if i == 0 else '') +
                   calls + _body(i, count) + '    return r + b;\n}\n')
    out.append('int f5(int a)\n{\n    return ext_a(a) + 2;\n}\n')
    if f6:
        out.append('int f6(int a, int b)\n{\n    int r;\n    r = 0;\n' + (first6 or '') +
                   _body(6, f6) + tail6 + '    return r;\n}\n')
    if f7:
        out.append('int f7(int a)\n{\n' + body7 + '}\n')
    return '\n'.join(out).encode('ascii')


_CACHE = {}


def compiled(**kwargs):
    key = repr(sorted(kwargs.items()))
    if key not in _CACHE:
        _CACHE[key] = compile_source(unit(**kwargs), 'msc510-medium')[0]
    return _CACHE[key]


class CandidateRecords(unittest.TestCase):
    def test_records_come_from_the_candidate_object(self):
        obj = compiled()
        records = candidate_records(obj, SEGMENT)
        self.assertGreaterEqual(len(records), 4)
        self.assertEqual([f for r in records for f in r['fixups']], obj.linker_fixups)
        # FIXUPPs descend within each record; the stream keeps that order.
        for record in records:
            offsets = [f['offset'] for f in record['fixups']]
            self.assertEqual(offsets, sorted(offsets, reverse=True))
        stream = relocation_stream(obj.linker_fixups, START)
        self.assertNotEqual([r['load_offset'] for r in stream],
                            sorted(r['load_offset'] for r in stream))

    def test_missing_omf_fails_closed(self):
        obj = copy.copy(compiled())
        obj.omf_bytes = None
        with self.assertRaisesRegex(ValueError, 'emitted OMF'):
            candidate_records(obj, SEGMENT)


if __name__ == '__main__':
    unittest.main()
