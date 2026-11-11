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
        wanted = {'keyboard_input_callbacks': {'_joystick_enabled'}, 'projection_vector_window': {'_projection_x_scale', '_projection_y_scale'},
                  'input_keyboard_joystick_services': {'_callbackflags2'}, 'timer_counter_deadline_helpers': {'_line_input_font_header'}}
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
