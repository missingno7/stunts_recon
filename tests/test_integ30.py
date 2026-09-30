"""integ30: one program-wide public name per address (names registry adopted by
every defining and referencing object), reviewed embedded publics of ASM groups,
the reviewed far-pointer table island of file_decomp_fatal with table-anchored
code aliases, string-table field anchors, and the pinned-library link helpers."""
import copy
import hashlib
import json
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))

from common import read_json


def _oracle():
    from oracle import verify
    from mz import MZ
    result = verify(write=False)
    return MZ.parse(result[1]).load_image(result[1]), result[2]['unpacked_mz']['relocations']


class NamesRegistryTests(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.image, cls.relocations = _oracle()
        cls.registry = read_json(ROOT / 'layout/names-registry.json')['names']

    def test_one_name_per_address_and_code_rows_have_grounded_targets(self):
        from function_evidence import current_inventory
        names = [row['name'] for row in self.registry.values()]
        self.assertEqual(len(names), len(set(names)))
        rows = {f['name']: f for f in current_inventory(self.image)['functions'] if f.get('name')}
        symbols = read_json(ROOT / 'layout/code-symbols.json')['symbols']
        for address, row in self.registry.items():
            self.assertLessEqual(len(row['name']), 30, row)
            if row['kind'] != 'code':
                continue
            inventory = rows.get(row['inventory_name'])
            if inventory is not None:
                self.assertEqual(inventory['start'], int(address), row)
            else:
                targets = {s['mapped_target']['start'] for s in symbols.values()
                           if (s.get('mapped_target') or {}).get('name') == row['inventory_name']}
                if targets:
                    self.assertEqual(targets, {int(address)}, row)
                else:
                    # A reviewed near-label alias can be an instruction entry
                    # inside a larger verified inventory extent. Ground it in
                    # the pinned label, exact owning extent, and decoder entry.
                    from code_symbols import _complete_target_owner
                    from common import identity
                    refs = read_json(ROOT / 'layout/references.json')['restunts']['evidence_files']
                    inventory = current_inventory(self.image)['functions']
                    decoder_path = str(ROOT / 'build/python')
                    if decoder_path not in sys.path:
                        sys.path.insert(0, decoder_path)
                    import capstone
                    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_16)
                    alias = '_' + row['name']
                    inventory_label = row['inventory_name']
                    self.assertRegex(inventory_label, r'^loc_[0-9A-Fa-f]+$', row)
                    label_address = int(inventory_label[4:], 16) - 0x10000
                    self.assertEqual(label_address, int(address), row)
                    proofs = set()
                    for owner in read_json(ROOT / 'layout/manifest.json')['owners']:
                        if not owner.get('recipe'):
                            continue
                        recipe = read_json(ROOT / owner['recipe'])
                        if alias in recipe.get('reviewed_near_targets', {}):
                            source_path = 'src/restunts/asmorig/seg012.asm'
                            pinned = refs[source_path]
                            from pinned_reference import check_reference_identity, reference_line
                            self.assertEqual(check_reference_identity(source_path), pinned, row)
                            line = recipe['reviewed_near_targets'][alias]['source_line']
                            self.assertEqual(reference_line(source_path, line).strip().lower(),
                                             (inventory_label + ':').lower(), row)
                            frame = recipe['original_frame_load_address']
                            extents = [f for f in inventory if
                                       f.get('start', 10**9) <= label_address < f.get('end', -1) and
                                       f.get('segment_paragraph', -1) * 16 == frame and
                                       f.get('status') in ('BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED',
                                                           'BOUNDARIES_AND_EMISSION_BYTES_VERIFIED') and
                                       hashlib.sha256(self.image[f['start']:f['end']]).hexdigest() == f.get('sha256') and
                                       any(_complete_target_owner(candidate, f)
                                           for candidate in read_json(ROOT / 'layout/manifest.json')['owners'])]
                            self.assertEqual(len(extents), 1, row)
                            extent = extents[0]
                            boundaries = {ins.address for ins in decoder.disasm(
                                self.image[extent['start']:extent['end']], extent['start'])}
                            self.assertIn(label_address, boundaries, row)
                            proofs.add(label_address)
                    self.assertEqual(proofs, {int(address)}, row)

    def test_registry_spelling_is_an_accepted_member_public(self):
        from asm_module import expected_publics, registry_publics
        self.assertEqual(registry_publics('plane_rotate_op'), {'_plnrotop'})
        self.assertIn('_plnrotop', expected_publics('plane_rotate_op', 'c'))
        self.assertIn('_main', expected_publics('ported_stuntsmain_', 'c'))
        self.assertEqual(registry_publics('no_such_entry'), set())
        self.assertNotIn('_plane_rotate_opx', expected_publics('plane_rotate_op', 'asm'))

    def test_accepted_objects_no_longer_use_superseded_spellings(self):
        declared = set()
        declared_by_owner = {}
        for owner in read_json(ROOT / 'layout/manifest.json')['owners']:
            if not owner.get('recipe') or owner['kind'] not in ('MATCHING_C', 'MATCHING_ASM',
                                                                  'MATCHING_C_DATA', 'MATCHING_ASM_DATA'):
                continue
            recipe = read_json(ROOT / owner['recipe'])
            decl = recipe.get('object_declarations') or recipe.get('binding', {}).get('declarations') or {}
            names = {p['name'] for p in decl.get('publics', [])} | set(decl.get('externals', []))
            names |= {row['name'] for row in recipe.get('communal_declarations', [])}
            declared |= names
            declared_by_owner[owner['id']] = names
            if recipe.get('public'):
                declared.add(recipe['public'])
        aliases = {}
        for name, symbol in read_json(ROOT / 'layout/data-symbols.json')['symbols'].items():
            aliases.setdefault(symbol['load_address'], set()).add(name)
        code_aliases = read_json(ROOT / 'layout/code-symbols.json')['symbols']
        for address, row in self.registry.items():
            if row.get('object_declared') is False:
                # integ33: a module-private label (no PUBDEF/EXTDEF anywhere) keeps its
                # registry spelling in source text only; no object may declare any alias.
                self.assertFalse(aliases.get(int(address), set()) & declared, row)
                continue
            self.assertIn('_' + row['name'], declared, row)
            if row['kind'] == 'code' and row['inventory_name'] != row['name']:
                old = '_' + row['inventory_name']
                if old in code_aliases:
                    # seg007 retains historical external spellings where the reviewed
                    # anchored code-symbol binding maps them to the normalized owner.
                    alias = code_aliases[old]
                    self.assertEqual(alias['mapped_target']['start'], int(address), row)
                    self.assertEqual(alias['mapped_target']['name'], row['inventory_name'], row)
                    declaring_owners = {owner for owner, names in declared_by_owner.items() if old in names}
                    if declaring_owners:
                        self.assertEqual(declaring_owners, {'obj_seg007'}, row)
                    else:
                        self.assertNotIn(old, declared, row)
                    continue
                self.assertNotIn(old, declared, row)
                self.assertNotIn(old[:31], declared - {'_' + row['name']}, row)


class EmbeddedPublicTests(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.image, cls.relocations = _oracle()

    def test_code_island_public_needs_its_reviewed_alias_inside_the_member(self):
        from multi_contribution import checked_members
        recipe = read_json(ROOT / 'recipes/sub_35C4E.json')
        checked_members(recipe, self.image)
        moved = copy.deepcopy(recipe)
        moved['embedded_publics'][0]['offset'] += 1
        with self.assertRaisesRegex(ValueError, 'code-island alias'):
            checked_members(moved, self.image)
        single = copy.deepcopy(recipe)
        single.pop('embedded_publics')
        with self.assertRaisesRegex(ValueError, 'Multi recipe needs members'):
            checked_members(single, self.image)

    def test_near_label_public_needs_pinned_declaration_and_instruction_boundary(self):
        from multi_contribution import checked_members
        recipe = read_json(ROOT / 'recipes/sub_35E08.json')
        checked_members(recipe, self.image)
        wrong_line = copy.deepcopy(recipe)
        wrong_line['embedded_publics'][0]['source_line'] += 1
        with self.assertRaisesRegex(ValueError, 'pinned source declaration'):
            checked_members(wrong_line, self.image)
        wrong_name = copy.deepcopy(recipe)
        wrong_name['embedded_publics'][0]['public'] = '_loc_35EDA'
        with self.assertRaisesRegex(ValueError, 'load address'):
            checked_members(wrong_name, self.image)


class FarPointerTableTests(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.image, cls.relocations = _oracle()

    def test_reviewed_island_verifies_file_decomp_fatal(self):
        from function_evidence import current_inventory, REVIEWED_FAR_POINTER_TABLES
        row = [f for f in current_inventory(self.image)['functions'] if f['name'] == 'file_decomp_fatal'][0]
        self.assertEqual(row['status'], 'BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED')
        self.assertEqual((row['start'], row['end']), REVIEWED_FAR_POINTER_TABLES['file_decomp_fatal']['extent'])

    def test_unreviewed_far_pointer_table_function_is_refused(self):
        import function_evidence
        document = read_json(ROOT / 'layout/function-evidence.json')
        row = [f for f in document['functions'] if f['name'] == 'file_decomp_fatal'][0]
        by = {r['load_offset']: bytes.fromhex(r['bytes']) for r in row['disassembly']}
        island = {row['data_islands'][0]['load_offset']: (bytes.fromhex(row['data_islands'][0]['hex']),
                                                        row['data_islands'][0])}
        self.assertEqual(function_evidence._checked_far_pointer_table(row, by, island, self.image),
                         [134992, 135009, 135025])
        other = {**row, 'name': 'sub_35C4E'}
        with self.assertRaisesRegex(ValueError, 'Unreviewed far-pointer table'):
            function_evidence._checked_far_pointer_table(other, by, island, self.image)
        bad_call = dict(by)
        bad_call[134847] = bytes.fromhex('2eff9fe624')
        with self.assertRaisesRegex(ValueError, 'indirect far CALL'):
            function_evidence._checked_far_pointer_table(row, bad_call, island, self.image)

    def test_table_anchored_code_alias_and_its_refusals(self):
        import code_symbols
        from code_symbols import resolve_code_symbols
        resolved = resolve_code_symbols({'_file_decomp_rle', '_file_decomp_vle'}, self.image, self.relocations)
        self.assertEqual(resolved['_file_decomp_rle']['table_sites'], [134916])
        self.assertEqual(resolved['_file_decomp_vle']['load_address'], 142716)
        layout = read_json(ROOT / 'layout/code-symbols.json')
        original = code_symbols.read_json
        for site, message in ((134920, 'reviewed far-pointer table'), (134914, 'reviewed far-pointer table')):
            altered = copy.deepcopy(layout)
            anchor = altered['symbols']['_file_decomp_rle']['table_anchors'][0]
            anchor['site'] = site
            anchor['hex'] = self.image[site:site + 4].hex()
            code_symbols.read_json = lambda path, a=altered: a if Path(path) == ROOT / 'layout/code-symbols.json' \
                else original(path)
            try:
                with self.assertRaises(ValueError):
                    resolve_code_symbols({'_file_decomp_rle'}, self.image, self.relocations)
            finally:
                code_symbols.read_json = original


class StringFieldTests(unittest.TestCase):

    def test_string_table_elements_are_anchored_fields(self):
        from data_symbols import resolve_symbols
        image, relocations = _oracle()
        allowed = resolve_symbols(['_aCar0'], image, relocations)['_aCar0']['allowed_addends']
        self.assertEqual(allowed, list(range(0, 75, 5)))
        base = resolve_symbols(['_loopSurface_ZBounds0'], image, relocations)['_loopSurface_ZBounds0']
        self.assertEqual(base['allowed_addends'], [0, 2, 6])


class PinnedLibraryLinkTests(unittest.TestCase):

    def test_library_public_names_reads_pinned_members(self):
        from omf import OmfReader
        from compiler import toolchain_path
        from reallink import library_public_names
        data = toolchain_path('toolchain/msc510/MLIBCR.LIB').read_bytes()
        members = dict(OmfReader().split_library(data))
        self.assertIn('__astart', library_public_names(members['dos\\crt0.asm']))
        self.assertEqual({'_exit', '__exit', '__cinit', '__ctermsub'} -
                         library_public_names(members['dos\\crt0dat.asm']), set())


if __name__ == '__main__':
    unittest.main()
