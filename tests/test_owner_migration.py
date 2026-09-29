"""Schema-aware owner migration keeps source bytes and updates bound references."""
import json
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT/'tools'))

import migrate_owner


class OwnerMigrationPlanTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.root = Path(self.temp.name)
        for directory in ('asm', 'recipes', 'layout', 'evidence'):
            (self.root/directory).mkdir(parents=True, exist_ok=True)
        old = 'old_timer_module'
        self.source = b'_loc_1234:\n jmp _loc_1234\n'
        (self.root/f'asm/{old}.ASM').write_bytes(self.source)
        recipe = {'id': old, 'stable_id': 'multi_old_timer_module', 'kind': 'asm',
                  'source': f'asm/{old}.ASM', 'profile': 'masm510-game',
                  'start': 10, 'end': 20}
        (self.root/f'recipes/{old}.json').write_text(json.dumps(recipe, indent=2)+'\n', encoding='utf-8')
        manifest = {'owners': [
            {'id': old, 'name': old, 'kind': 'MATCHING_ASM',
             'recipe': f'recipes/{old}.json', 'start': 10, 'end': 20},
            {'id': old+':_DATA', 'kind': 'MATCHING_ASM_DATA', 'parent': old,
             'start': 30, 'end': 32}]}
        (self.root/'layout/manifest.json').write_text(json.dumps(manifest, indent=2)+'\n', encoding='utf-8')
        (self.root/'layout/link-objects.json').write_text(json.dumps({'objects': [{'id': old}]}, indent=2)+'\n', encoding='utf-8')
        (self.root/'layout/names-registry.json').write_text(json.dumps({'names': {
            '10': {'kind': 'code', 'name': 'timer_start', 'basis': 'module old_timer_module'}}}, indent=2)+'\n', encoding='utf-8')
        (self.root/'layout/code-symbols.json').write_text(json.dumps({'symbols': {
            '_timer_start': {'module_entry': {'owner': old}}}}, indent=2)+'\n', encoding='utf-8')
        (self.root/'evidence/module-reference.json').write_text(json.dumps({'owner': old}, indent=2)+'\n', encoding='utf-8')
        (self.root/'recipes/consumer.json').write_text(json.dumps({
            'id': 'consumer', 'kind': 'asm', 'source': 'asm/consumer.ASM',
            'external_owner': old, 'profile': 'masm510-game'}, indent=2)+'\n', encoding='utf-8')
        self.mapping = {'schema': 1, 'mapping_count': 1, 'mappings': [{
            'current_owner': old, 'current_file': f'asm/{old}.ASM',
            'proposed_owner': 'timer_interrupt_runtime',
            'proposed_file': 'asm/timer_interrupt_runtime.ASM',
            'basis': 'Timer setup and callback dispatch.'}]}

    def tearDown(self):
        self.temp.cleanup()

    def test_plan_renames_owner_paths_and_all_identity_references(self):
        plan = migrate_owner.plan_migration(self.mapping, self.root)
        self.assertEqual(len(plan['pairs']), 1)
        changes = plan['changes']
        self.assertIsNone(changes['asm/old_timer_module.ASM'])
        self.assertEqual(changes['asm/timer_interrupt_runtime.ASM'], self.source)
        self.assertIsNone(changes['recipes/old_timer_module.json'])
        recipe = json.loads(changes['recipes/timer_interrupt_runtime.json'])
        self.assertEqual(recipe['id'], 'timer_interrupt_runtime')
        self.assertEqual(recipe['stable_id'], 'multi_old_timer_module')
        self.assertEqual(recipe['source'], 'asm/timer_interrupt_runtime.ASM')
        manifest = json.loads(changes['layout/manifest.json'])
        self.assertEqual(manifest['owners'][0]['id'], 'timer_interrupt_runtime')
        self.assertEqual(manifest['owners'][0]['recipe'], 'recipes/timer_interrupt_runtime.json')
        self.assertEqual(manifest['owners'][1]['id'], 'timer_interrupt_runtime:_DATA')
        self.assertEqual(manifest['owners'][1]['parent'], 'timer_interrupt_runtime')
        link = json.loads(changes['layout/link-objects.json'])
        self.assertEqual(link['objects'][0]['id'], 'timer_interrupt_runtime')
        names = json.loads(changes['layout/names-registry.json'])
        self.assertIn('timer_interrupt_runtime', names['names']['10']['basis'])
        symbols = json.loads(changes['layout/code-symbols.json'])
        self.assertEqual(symbols['symbols']['_timer_start']['module_entry']['owner'],
                         'timer_interrupt_runtime')
        evidence = json.loads(changes['evidence/module-reference.json'])
        self.assertEqual(evidence['owner'], 'timer_interrupt_runtime')
        consumer = json.loads(changes['recipes/consumer.json'])
        self.assertEqual(consumer['external_owner'], 'timer_interrupt_runtime')

    def test_expected_inputs_excludes_untracked_docs_but_keeps_new_source(self):
        before = {'asm/old_timer_module.ASM': 'old', 'tools/migrate_owner.py': 'tool'}
        changes = {'asm/old_timer_module.ASM': None,
                   'asm/timer_interrupt_runtime.ASM': b'new source',
                   'docs/acceptance.md': b'new docs'}
        expected = migrate_owner._expected_inputs(before, changes)
        self.assertEqual(set(expected), {'tools/migrate_owner.py', 'asm/timer_interrupt_runtime.ASM'})
        self.assertEqual(expected['asm/timer_interrupt_runtime.ASM'],
                         migrate_owner.sha(b'new source'))

    def test_acceptance_section_is_readable_markdown(self):
        plan = {'pairs': [{'old_owner': 'old_timer_module',
                           'new_owner': 'timer_interrupt_runtime',
                           'old_source': 'asm/old_timer_module.ASM',
                           'new_source': 'asm/timer_interrupt_runtime.ASM',
                           'basis': 'Timer setup and callbacks.'}]}
        text = migrate_owner._acceptance_section(plan, {})
        self.assertIn('# Module owner and source migration (integ45)\n', text)
        self.assertIn('| Previous owner | New owner |', text)
        self.assertIn('\n| `old_timer_module` | `timer_interrupt_runtime` |', text)


if __name__ == '__main__':
    unittest.main()
