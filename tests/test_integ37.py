"""integ37: seg005/seg006 complete static sets, the MSC 5.10 translation-unit
static flush rule (diagnostic model) and pinned data-only runtime members."""
import json
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))

from common import read_json  # noqa: E402
import bss_link  # noqa: E402

FIXTURES = ROOT / 'tests' / 'fixtures'


class StaticSetTests(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.manifest = read_json(ROOT / 'layout/manifest.json')
        cls.registry = read_json(ROOT / 'layout/names-registry.json')['names']
        cls.symbols = read_json(ROOT / 'layout/data-symbols.json')['symbols']

    def test_seg005_and_seg006_bss_rows(self):
        rows = {r['id']: r for r in self.manifest['bss_owners']}
        for obj, start, end in (('obj_seg005', 200042, 200332), ('obj_seg006', 200332, 203476)):
            row = rows[obj + ':_BSS']
            self.assertEqual((row['start'], row['end'], row['kind'], row['placement']),
                             (start, end, 'MATCHING_C_DATA', 'link-module-order-v1'))
            self.assertNotIn('raw_bss_' + obj, rows)
            recipe = read_json(ROOT / ('recipes/%s.json' % obj))
            bss = recipe['secondary_dgroup_segments']['_BSS']
            self.assertEqual((bss['start'], bss['end']), (start, end))
            self.assertTrue(any('compiler-hash-constrained' in n for n in recipe['review_notes']))

    def test_registry_rows_are_module_private_static_names(self):
        texts = {o: (ROOT / ('src/%s.c' % o)).read_text() for o in ('obj_seg005', 'obj_seg006')}
        rows = {int(a): r for a, r in self.registry.items() if 200042 <= int(a) < 203476}
        self.assertEqual(len(rows), 57)
        for address, row in rows.items():
            self.assertIs(row['object_declared'], False)
            obj = 'obj_seg005' if address < 200332 else 'obj_seg006'
            source_identifier = row.get('source_identifier', row['name'])
            if source_identifier != row['name']:
                self.assertTrue(row['basis'].startswith('integ43'), row)
                self.assertTrue(address >= 200332)
            else:
                self.assertTrue(row['basis'].startswith('integ37'), row)
            self.assertRegex(texts[obj], r'\bstatic\b[^;\n]*\b%s\b' % source_identifier)
        # interior elements [2]/[3] of camera_buttons_pressed[9] have no row
        self.assertEqual(rows[200298]['name'], 'camera_buttons_pressed')
        self.assertNotIn(200300, rows)
        self.assertNotIn(200301, rows)
        for name in ('shape3d_unused', 'poly_padding_bytes', 'unused_40E73'):
            row = next(r for r in rows.values() if r['name'] == name)
            self.assertIn('unreferenced storage', row['basis'])

    def test_superseded_clone_aliases_are_gone(self):
        for old in ('last_tachometer', 'camera_buttons_active', 'replay_back_pending', 'replay_forward_pending',
                    'projected_point_table', 'vector_angle_sine_b', 'polyinfo_insert_result'):
            self.assertNotIn('_' + old, self.symbols)
        # the grounded reference labels stay
        for label in ('_byte_40E6C', '_byte_40E6D', '_mat_y200', '_polyinfoptrs'):
            self.assertIn(label, self.symbols)
        for label, address in (('_off_3F3C8', 193480), ('_word_31854', 202836),
                               ('_word_3186A', 202858), ('_word_31878', 202872),
                               ('_byte_31882', 202882)):
            self.assertEqual(self.symbols[label]['load_address'], address)
            self.assertNotIn('clone_of', self.symbols[label])


class StaticFlushModelTests(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.corpus = json.loads((FIXTURES / 'msc510_static_flush_fixtures.json').read_text())

    def test_compiled_fixtures(self):
        self.assertEqual([f['fixture'] for f in self.corpus['fixtures']], ['e%d' % i for i in range(1, 11)])
        for fx in self.corpus['fixtures']:
            offsets, length = bss_link.static_layout(fx['items'])
            for name, want in fx['observed_offsets'].items():
                self.assertEqual(offsets[name], want, (fx['fixture'], name))
            self.assertEqual(length, fx['bss_length'], fx['fixture'])

    def test_seg005_complete_layout(self):
        fx = self.corpus['seg005']
        offsets, length = bss_link.static_layout(fx['items'])
        self.assertEqual(offsets, fx['observed_offsets'])
        self.assertEqual(length, 290)

    def test_flush_rule_distinguishes_the_flat_model(self):
        import msc_static_model as model
        # e8: a nested block follows its enclosing block although its bucket is lower
        self.assertLess(model.block_bucket('aa'), model.block_bucket('zz'))
        fx = next(f for f in self.corpus['fixtures'] if f['fixture'] == 'e8')
        self.assertEqual(fx['observed_offsets'], {'zz': 0, 'aa': 2})
        # e7: a static declared after a definition is not flushed with the earlier set
        flat = bss_link.static_order(['zz', 'aa'], {'zz': 2, 'aa': 2}, {'zz': 'short', 'aa': 'short'})
        self.assertEqual(flat, {'aa': 0, 'zz': 2})
        fx = next(f for f in self.corpus['fixtures'] if f['fixture'] == 'e7')
        self.assertEqual(fx['observed_offsets'], {'zz': 0, 'bb': 2, 'aa': 4})


def _image():
    from oracle import verify
    from mz import MZ
    oracle = verify(write=False)
    mz = MZ.parse(oracle[1])
    return mz.load_image(oracle[1]), mz.relocations


def _member_blob(module):
    from omf import OmfReader
    import runtime_binding
    archive = runtime_binding._toolchain_path('toolchain/msc510/MLIBCR.LIB').read_bytes()
    return dict(OmfReader().split_library(archive))[module]


class OmfReaderTests(unittest.TestCase):

    def test_lidata_fixups_apply_to_every_repetition(self):
        from omf import OmfReader
        from object_probe import read_object
        blob = _member_blob('cmiscdat.asm')
        obj = OmfReader().read(blob)
        self.assertEqual([(f['segment'], f['offset'], f['loc'], f['target']) for f in obj.linker_fixups],
                         [('_DATA', 4 * i, 'pointer32', '__fptrap') for i in range(5)])
        with self.assertRaisesRegex(ValueError, 'iterated LIDATA'):
            read_object(blob)
        self.assertEqual(len(read_object(blob, iterated_fixups=True).linker_fixups), 5)

    def test_comdef_names_take_their_external_indices(self):
        from omf import OmfReader, MatchError
        from object_probe import read_object
        blob = _member_blob('_file.c')
        with self.assertRaises(MatchError):
            OmfReader().read(blob)
        obj = OmfReader(communals=True).read(blob)
        self.assertEqual(obj.externals, ['__acrtused', '__bufin', '__bufout', '__buferr'])
        self.assertEqual(obj.external_scopes, ['external', 'communal', 'communal', 'communal'])
        self.assertEqual([(c['name'], c['kind'], c['length']) for c in obj.communals],
                         [('__bufin', 'near', 512), ('__bufout', 'near', 512), ('__buferr', 'near', 512)])
        with self.assertRaisesRegex(ValueError, 'COMDEF'):
            read_object(blob)
        with self.assertRaisesRegex(ValueError, 'reviewed communal'):
            read_object(blob, communals=['__bufin'])


class DataOnlyMemberTests(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        cls.manifest = read_json(ROOT / 'layout/manifest.json')
        cls.image, cls.relocations = _image()

    def member(self, member_id):
        import copy
        return copy.deepcopy(next(m for m in self.manifest['runtime_data_members'] if m['id'] == member_id))

    def bind(self, member, manifest=None):
        from library import bind_library
        return bind_library(member, self.image, self.relocations, manifest=manifest or self.manifest)

    def test_published_members_and_rows(self):
        members = {m['id']: m for m in self.manifest['runtime_data_members']}
        self.assertEqual(sorted(members), ['library__cflush_192062', 'library__file_192064',
                                           'library_cmiscdat_192388', 'library_ctype_192414'])
        rows = {o['id']: (o['start'], o['end']) for o in self.manifest['owners']
                if o['kind'] == 'KNOWN_TOOLCHAIN_LIBRARY_DATA' and o['parent'] in members}
        self.assertEqual(rows, {'library__cflush_192062:_DATA': (192062, 192064),
                                'library__cflush_192062:XP': (199722, 199726),
                                'library__file_192064:_DATA': (192064, 192346),
                                'library_cmiscdat_192388:_DATA': (192388, 192414),
                                'library_ctype_192414:_DATA': (192414, 192671)})
        for member in members.values():
            payload, receipt = self.bind(member)
            self.assertEqual(payload, b'')
            for name, raw in receipt['binding']['secondary_payloads'].items():
                start = member['binding']['storage'][name]['start']
                self.assertEqual(bytes.fromhex(raw), self.image[start:start + len(raw) // 2])
        self.assertEqual(len(members['library_cmiscdat_192388']['expected_relocations']), 5)
        self.assertEqual(members['library__file_192064']['omf_policy'],
                         {'communals': ['__bufin', '__bufout', '__buferr']})

    def test_refusals(self):
        import copy
        from build_exact import validate_layout
        moved = self.member('library_ctype_192414')
        moved['binding']['storage']['_DATA']['anchor']['offset'] = 2
        with self.assertRaisesRegex(ValueError, 'alias placement differs'):
            self.bind(moved)
        coded = self.member('library_cmiscdat_192388')
        coded['segment'] = '_TEXT'
        with self.assertRaisesRegex(ValueError, 'Invalid data-only runtime member form'):
            self.bind(coded)
        raw = self.member('library__cflush_192062')
        raw['binding']['storage']['XP']['ownership'] = 'proven-raw'
        with self.assertRaisesRegex(ValueError, 'must be linked'):
            self.bind(raw)
        manifest = copy.deepcopy(self.manifest)
        row = next(o for o in manifest['owners'] if o['id'] == 'library_ctype_192414:_DATA')
        manifest['owners'][manifest['owners'].index(row)] = {'id': 'raw_x', 'kind': 'UNRESOLVED_RAW',
                                                            'start': row['start'], 'end': row['end']}
        with self.assertRaisesRegex(ValueError, 'lacks exactly one owned row'):
            validate_layout(manifest, len(self.image))

    def test_link_object_redeclares_only_the_communals(self):
        import reallink
        from omf import OmfReader
        blob, info = reallink.data_member_object(self.member('library__file_192064'))
        obj = OmfReader().read(blob)
        self.assertEqual(obj.externals, ['__acrtused', '__bufin', '__bufout', '__buferr'])
        self.assertEqual(info['comdef_as_extdef'], ['__bufin', '__bufout', '__buferr'])
        original = OmfReader(communals=True).read(_member_blob('_file.c'))
        self.assertEqual(obj.linker_fixups, original.linker_fixups)
        self.assertEqual(obj.segments, original.segments)


class LinkFillTests(unittest.TestCase):

    def test_dosseg_lead_and_new_word_fills(self):
        import copy
        from link_fill import checked_fill, DOSSEG_LEAD_BASIS
        manifest = read_json(ROOT / 'layout/manifest.json')
        image, _ = _image()
        rows = {o['start']: o for o in manifest['owners'] if o['kind'] == 'LINK_FILL'}
        lead = rows[117842]
        self.assertEqual((lead['end'], lead['basis'], lead['object']), (117858, DOSSEG_LEAD_BASIS, 'obj_seg009'))
        self.assertEqual(checked_fill(lead, manifest, image)['text_frame'], 117840)
        for start in (192387, 192671):
            checked_fill(rows[start], manifest, image)
        bad = copy.deepcopy(manifest)
        row = next(o for o in bad['owners'] if o['start'] == 117842)
        row['object'] = 'library_dos_crt0_117858'
        with self.assertRaisesRegex(ValueError, 'named code contribution'):
            checked_fill(row, bad, image)


class NameBindingTests(unittest.TestCase):

    def test_ctype_macros_reference_the_pinned_table(self):
        for obj in ('obj_seg000', 'obj_seg008', 'obj_seg031'):
            recipe = read_json(ROOT / ('recipes/%s.json' % obj))
            self.assertIn('__ctype', recipe['object_declarations']['externals'])
            self.assertNotIn('_g_ascii_props', recipe['object_declarations']['externals'])
            fixes = [f for f in recipe['expected_fixups'] if f['target'] == '__ctype']
            self.assertTrue(fixes and all(f['encoded_addend'] == '0100' for f in fixes))
            self.assertIn('#define tolower(c)', (ROOT / recipe['source']).read_text())
        from data_symbols import resolve_symbols
        image, relocations = _image()
        symbol = resolve_symbols({'__ctype'}, image, relocations)['__ctype']
        self.assertEqual((symbol['load_address'], max(symbol['allowed_addends'])), (192414, 256))
        registry = read_json(ROOT / 'layout/names-registry.json')['names']
        self.assertEqual(registry['192414']['name'], '_ctype')

    def test_no_accepted_object_names_bytes_inside_null(self):
        symbols = read_json(ROOT / 'layout/data-symbols.json')['symbols']
        manifest = read_json(ROOT / 'layout/manifest.json')
        for owner in manifest['owners']:
            if 'recipe' not in owner:
                continue
            recipe = read_json(ROOT / owner['recipe'])
            decl = recipe.get('object_declarations') or recipe.get('binding', {}).get('declarations') or {}
            for name in decl.get('externals', []):
                address = symbols.get(name, {}).get('load_address', 0)
                self.assertFalse(178032 <= address < 178098, (owner['id'], name))

    def test_asm_data_publics_are_group_relative(self):
        from assembler import assemble_source
        from omf import OmfReader
        # the data publics that other objects reach with target/external frames
        wanted = {'keyboard_input_callbacks': {'_byte_3FE00'}, 'projection_vector_window': {'_projection_x_scale', '_projection_y_scale'},
                  'input_keyboard_joystick_services': {'_callbackflags2'}, 'timer_counter_deadline_helpers': {'_line_input_screen_rect'}}
        for name, publics in wanted.items():
            recipe = read_json(ROOT / ('recipes/%s.json' % name))
            obj, receipt = assemble_source((ROOT / recipe['source']).read_bytes(), recipe['profile'])
            data = (Path(receipt['work_directory']) / 'UNIT.OBJ').read_bytes()
            group = {}
            for kind, body in OmfReader.records(data):
                if kind != 0x90:
                    continue
                at = 2
                while at < len(body):
                    n = body[at]
                    group[body[at + 1:at + 1 + n].decode()] = body[0]
                    at += 1 + n + 3
            self.assertEqual({p: group[p] for p in publics}, {p: 1 for p in publics}, name)


class RuntimeCommonAndBegdataTests(unittest.TestCase):

    def test_owned_overlay_and_linked_null(self):
        import copy
        import runtime_binding
        from omf import OmfReader
        manifest = read_json(ROOT / 'layout/manifest.json')
        rows = {o['id']: (o['start'], o['end']) for o in manifest['owners']
                if o['kind'] == 'KNOWN_TOOLCHAIN_LIBRARY_DATA'}
        self.assertEqual(rows['library_abort_123846:PAD'], (199973, 199992))
        self.assertEqual(rows['library_dos_nmsghdr_119038:EPAD'], (199992, 199993))
        self.assertEqual(rows['library_chksum_118488:NULL'], (178032, 178098))
        self.assertEqual(rows['library_chksum_118488:MSG'], (199903, 199940))
        owners = {o['id']: o for o in manifest['owners']}
        abort = owners['library_abort_123846']
        obj = OmfReader().read(_member_blob('abort.asm'))
        self.assertTrue(runtime_binding.common_overlay_owner(abort, 'PAD', obj, manifest))
        crt0msg = owners['library_dos_crt0msg_118410']
        msg_obj = OmfReader().read(_member_blob('dos\\crt0msg.asm'))
        self.assertFalse(runtime_binding.common_overlay_owner(crt0msg, 'PAD', msg_obj, manifest))
        later = copy.deepcopy(manifest)
        next(o for o in later['owners'] if o['id'] == 'library_chksum_118488')['start'] = 124000
        self.assertFalse(runtime_binding.common_overlay_owner(abort, 'PAD', obj, later))

    def test_dseg_is_linked_as_begdata(self):
        import reallink
        text, changes = reallink.adapt_asm("DSEG segment byte public 'STUNTSD'\nDSEG ends\n", 'S012_TEXT')
        self.assertIn("'BEGDATA'", text)
        self.assertEqual(reallink.RUNTIME_UNMODELLED_CLASSES, ())


if __name__ == '__main__':
    unittest.main()
