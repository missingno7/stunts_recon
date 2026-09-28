"""The cosmetic gate compares every recipe object attached to a source file."""
import json
import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT/'tools'))

import promote


class CosmeticSourceFileTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(dir=ROOT/'build')
        self.root = Path(self.temp.name)
        for directory in ('asm', 'recipes', 'layout', 'build/workers/integ45'):
            (self.root/directory).mkdir(parents=True, exist_ok=True)
        self.source_path = self.root/'asm/full_module.ASM'
        self.source_path.write_bytes(b'PROC original_label\n')
        self.candidate = self.root/'build/workers/integ45/full_module.ASM'
        self.candidate.write_bytes(b'PROC readable_label\n')
        for name in ('whole_module', 'member_fragment'):
            (self.root/f'recipes/{name}.json').write_text(json.dumps({
                'id': name, 'kind': 'asm', 'profile': 'masm510-game',
                'source': 'asm/full_module.ASM'}), encoding='utf-8')
        (self.root/'layout/manifest.json').write_text(json.dumps({'owners': [
            {'id': 'whole_module', 'kind': 'MATCHING_ASM',
             'recipe': 'recipes/whole_module.json'},
            {'id': 'whole_module:_DATA', 'kind': 'MATCHING_ASM_DATA',
             'parent': 'whole_module'}]}), encoding='utf-8')

    def tearDown(self):
        self.temp.cleanup()

    def test_module_source_compares_active_group_and_inactive_fragment_recipes(self):
        calls = []

        def compile_obj(recipe, source):
            calls.append((recipe['id'], source))
            return ('OBJ:'+recipe['id']).encode('ascii')

        with patch.object(promote, 'ROOT', self.root), \
             patch.object(promote, 'inputs', return_value={'fixture': 'stable'}), \
             patch.object(promote, '_cosmetic_compile', side_effect=compile_obj):
            result = promote.check_cosmetic(
                'asm/full_module.ASM', self.candidate, self.candidate.read_bytes(),
                {'fixture': 'stable'})

        self.assertEqual(result['function'], 'source:asm/full_module.ASM')
        self.assertEqual(result['active_owners'], ['whole_module', 'whole_module:_DATA'])
        self.assertEqual({row['recipe'] for row in result['comparisons']}, {
            'recipes/whole_module.json', 'recipes/member_fragment.json'})
        self.assertEqual(len(calls), 4)
        self.assertEqual(self.source_path.read_bytes(), b'PROC original_label\n')

    def test_module_source_rejects_any_recipe_object_mismatch(self):
        def compile_obj(recipe, source):
            if recipe['id'] == 'member_fragment' and b'readable' in source:
                return b'changed fragment object'
            return b'exact object'

        with patch.object(promote, 'ROOT', self.root), \
             patch.object(promote, 'inputs', return_value={'fixture': 'stable'}), \
             patch.object(promote, '_cosmetic_compile', side_effect=compile_obj):
            with self.assertRaisesRegex(ValueError, 'member_fragment'):
                promote.check_cosmetic(
                    'asm/full_module.ASM', self.candidate, self.candidate.read_bytes(),
                    {'fixture': 'stable'})


if __name__ == '__main__':
    unittest.main()
