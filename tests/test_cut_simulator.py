"""MSC 5.10 CODE record-cut rule against freshly compiled OMF objects."""
import sys
import unittest
from pathlib import Path

ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
from common import read_json
from compiler import compile_source
from diagnostics import _decode
from cut_simulator import emission_unit_ends, observed_code_records, simulate


def compiled_code(source, profile='msc510-medium'):
    """Compile source and return code bytes, fixups, instruction ends, observed records."""
    obj, receipt = compile_source(source, profile)
    data = (Path(receipt['work_directory'])/'UNIT.OBJ').read_bytes()
    segments = [s for s in obj.segment_defs if s['class'] == 'CODE']
    assert len(segments) == 1
    segment = segments[0]
    code = obj.segment_bytes(segment['name'])
    fixups = [f for f in obj.linker_fixups if f['segment'] == segment['name']]
    # MSC switch-table words carry an offset16 FIXUPP to the module's own
    # segment at the word itself; an instruction never starts with a field.
    table = {f['offset'] for f in fixups if f['loc'] == 'offset16' and
             f['target_kind'] == 'segment' and f['target'] == segment['name']}
    ends, pos = [], 0
    while pos < len(code):
        if pos in table:
            pos += 2
        else:
            rows = _decode(code[pos:pos+16], pos)
            assert rows, f'undecodable CODE at {pos}'
            pos += len(bytes.fromhex(rows[0]['bytes']))
        ends.append(pos)
    records = observed_code_records(data, segment['index'])
    assert records and records[0]['offset'] == 0
    assert all(a['offset'] + a['size'] == b['offset'] for a, b in zip(records, records[1:]))
    assert records[-1]['offset'] + records[-1]['size'] == len(code)
    return code, fixups, ends, records, table


def cut_input(code, fixups, ends, *, units=True, relocations=()):
    member = {'name':'unit', 'length':len(code), 'instruction_ends':ends,
              'fixups':[{'offset':f['offset'], 'width':f['width'], 'kind':f['loc']} for f in fixups],
              'relocations':list(relocations)}
    if units:
        member['code_hex'] = code.hex()
    return {'schema':'msc510-code-cut-input-v1', 'members':[member]}


def calls_probe(count):
    return ('extern void e(void);\nint f(int a)\n{\n    int i;\n' + '    e();\n'*count +
            '    i = a * 3;\n    i = i ^ a;\n    e();\n    return i;\n}\n').encode('ascii')


def switch_probe(functions, cases):
    lines = ['extern int g(int);', 'extern int h;']
    for k in range(functions):
        lines.append(f'int s{k}(int a, int b)\n{{\n    int r;\n    r = 0;\n    if (a > b) {{')
        lines += [f'        r = r * {3+j} + g(a + {j});\n        h = h ^ r;' for j in range(12)]
        lines.append('    }\n    switch (a) {')
        lines += [f'    case {j}: r = g(b + {j}) + {k*j+1}; break;' for j in range(cases)]
        lines.append('    }\n    return r + b;\n}')
    return ('\n'.join(lines) + '\n').encode('ascii')


def old_heuristic_ends(ends, fixups, length, limit=947):
    """Falsified rule kept only for the negative test: last safe boundary <= 947."""
    interior = {p for f in fixups for p in range(f['offset']+1, f['offset']+f['width'])}
    safe = [e for e in ends if e not in interior]
    out, start = [], 0
    while start < length:
        start = [e for e in safe if start < e <= start+limit][-1]
        out.append(start)
    return out


class CutRuleCompiledObjects(unittest.TestCase):
    def assert_rule(self, code, fixups, ends, records):
        observed = [r['offset'] + r['size'] for r in records]
        result = simulate(cut_input(code, fixups, ends))
        self.assertEqual(result['cut_basis'], 'msc510-threshold-rule')
        self.assertEqual([r['end'] for r in result['records']], observed)
        self.assertEqual([r['fixup_count'] for r in result['records']],
                         [r['fixups'] for r in records])
        document = cut_input(code, fixups, ends)
        document['members'][0].pop('code_hex')
        document['members'][0]['unit_ends'] = emission_unit_ends(code, ends)
        document['observed_record_ends'] = observed
        checked = simulate(document)
        self.assertEqual(checked['rule_check'], {'consistent':True, 'violations':[]})
        return result

    def test_accepted_recipes_reproduce_records_and_relocation_order(self):
        for name in ('init_main', 'rectlist_core_group'):
            with self.subTest(recipe=name):
                recipe = read_json(ROOT/'recipes'/f'{name}.json')
                code, fixups, ends, records, _ = compiled_code(
                    (ROOT/recipe['source']).read_bytes(), recipe['profile'])
                self.assertEqual(len(code), recipe['end'] - recipe['start'])
                self.assertGreater(len(records), 1)
                expected = {r['load_offset']:r for r in recipe['expected_relocations']}
                relocations = []
                for f in fixups:
                    site = f['offset'] + (2 if f['loc'] == 'pointer32' else 0)
                    if f['loc'] in ('pointer32', 'base16'):
                        relocations.append({**expected[recipe['start']+site], 'site':site})
                self.assertEqual(len(relocations), len(expected))
                result = simulate(cut_input(code, fixups, ends, relocations=relocations))
                self.assertEqual([r['end'] for r in result['records']],
                                 [r['offset'] + r['size'] for r in records])
                self.assertEqual([r['load_offset'] for r in result['exepack_order_relocations']],
                                 [r['load_offset'] for r in recipe['expected_relocations']])
                self.assert_rule(code, fixups, ends, records)

    def test_ninety_ninth_fixup_closes_short_record(self):
        code, fixups, ends, records, _ = compiled_code(calls_probe(99))
        result = self.assert_rule(code, fixups, ends, records)
        first = result['records'][0]
        self.assertLess(first['code_bytes'], 944)
        self.assertEqual((first['fixup_count'], first['reason']), (99, ['fixup-count-limit']))
        # Negative: 98 FIXUPPs stay below both limits and close only at object end.
        code, fixups, ends, records, _ = compiled_code(calls_probe(97))
        self.assertEqual(len(records), 1)
        self.assertEqual(self.assert_rule(code, fixups, ends, records)['records'][0]['reason'],
                         ['object-end'])

    def test_switch_table_word_closes_record(self):
        code, fixups, ends, records, table = compiled_code(switch_probe(3, 28))
        observed = [r['offset'] + r['size'] for r in records]
        self.assertTrue(any(end in table for end in observed))
        self.assert_rule(code, fixups, ends, records)

    def test_long_branch_pair_is_one_unit(self):
        code, fixups, ends, records, _ = compiled_code(switch_probe(4, 32))
        self.assert_rule(code, fixups, ends, records)
        observed = [r['offset'] + r['size'] for r in records]
        # Negative: splitting jcc $+3 / JMP near pairs changes a cut.
        naive = simulate(cut_input(code, fixups, ends, units=False))
        self.assertNotEqual([r['end'] for r in naive['records']], observed)
        document = cut_input(code, fixups, ends, units=False)
        document['observed_record_ends'] = observed
        self.assertFalse(simulate(document)['rule_check']['consistent'])

    def test_old_947_heuristic_is_falsified(self):
        code, fixups, ends, records, _ = compiled_code(switch_probe(3, 24))
        observed = [r['offset'] + r['size'] for r in records]
        self.assertNotEqual(old_heuristic_ends(ends, fixups, len(code)), observed)
        self.assert_rule(code, fixups, ends, records)


class CutRuleInputs(unittest.TestCase):
    def member(self, **extra):
        return {'name':'m', 'length':1900, 'instruction_ends':list(range(1, 1901)),
                'fixups':[{'offset':i*12, 'width':2, 'kind':'offset16'} for i in range(1, 74)],
                'relocations':[], **extra}

    def test_threshold_and_field_boundaries(self):
        document = {'schema':'msc510-code-cut-input-v1', 'members':[self.member()]}
        self.assertEqual([r['end'] for r in simulate(document)['records']], [944, 1888, 1900])
        document['observed_record_ends'] = [944, 1888, 1900]
        self.assertTrue(simulate(document)['rule_check']['consistent'])
        document['observed_record_ends'] = [947, 1891, 1900]
        checked = simulate(document)
        self.assertEqual(checked['cut_basis'], 'observed-omf-ledata')
        self.assertFalse(checked['rule_check']['consistent'])
        document['members'][0]['fixups'].append({'offset':945, 'width':4, 'kind':'pointer32'})
        with self.assertRaisesRegex(ValueError, 'safe ascending ends'):
            simulate(document)

    def test_rejects_overflow_and_bad_units(self):
        member = self.member(instruction_ends=[940, 950, 1900])
        with self.assertRaisesRegex(ValueError, '949-byte'):
            simulate({'schema':'msc510-code-cut-input-v1', 'members':[member]})
        observed = {'schema':'msc510-code-cut-input-v1', 'members':[self.member()],
                    'observed_record_ends':[950, 1900]}
        with self.assertRaisesRegex(ValueError, 'record maximum'):
            simulate(observed)
        with self.assertRaisesRegex(ValueError, 'instruction end'):
            simulate({'schema':'msc510-code-cut-input-v1', 'members':[
                self.member(instruction_ends=list(range(2, 1901, 2)), unit_ends=[3, 1900])]})
        many = {'name':'m', 'length':500, 'instruction_ends':[500],
                'fixups':[{'offset':i*4, 'width':2, 'kind':'offset16'} for i in range(100)]}
        with self.assertRaisesRegex(ValueError, 'past the limit'):
            simulate({'schema':'msc510-code-cut-input-v1', 'members':[many]})

    def test_emission_unit_pairs(self):
        code = bytes([0x74, 0x03, 0xE9, 0x10, 0x00, 0x75, 0x02, 0xE9, 0x10, 0x00, 0x90])
        self.assertEqual(emission_unit_ends(code, [2, 5, 7, 10, 11]), [5, 7, 10, 11])


if __name__=='__main__':
    unittest.main()
