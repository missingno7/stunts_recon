"""integ38: whole seg012 modules owning their DGROUP data (timerint,
scalblit), the in-image prefix of the first _BSS contribution
(BSS_IN_IMAGE) with the word fill before it, DGROUP-framed external addends
and the FONTHDR font-header STRUC."""
import copy
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))

from common import read_json  # noqa: E402


class Oracle:
    image = None
    relocations = None

    @classmethod
    def load(cls):
        if cls.image is None:
            from oracle import verify
            from mz import MZ
            result = verify(write=False)
            cls.image = MZ.parse(result[1]).load_image(result[1])
            cls.relocations = result[2]['unpacked_mz']['relocations']
        return cls


class BssInImageTests(unittest.TestCase):

    def setUp(self):
        from build_exact import validate_layout
        self.validate = validate_layout
        self.manifest = read_json(ROOT / 'layout/manifest.json')
        self.size = len(Oracle.load().image)


    def test_row_must_name_first_accepted_bss_owner(self):
        bad = copy.deepcopy(self.manifest)
        bad['owners'][-1]['bss_owner'] = 'carsim:_BSS'
        bad['owners'][-1]['id'] = 'carsim:_BSS@image'
        with self.assertRaisesRegex(ValueError, 'first accepted _BSS owner'):
            self.validate(bad, self.size)

    def test_raw_bss_placeholder_cannot_own_image_bytes(self):
        bad = copy.deepcopy(self.manifest)
        row = bad['bss_owners'][0]
        bad['bss_owners'][0] = {'id': row['id'], 'kind': 'UNRESOLVED_RAW', 'start': row['start'],
                                'end': row['end'], 'segment': '_BSS'}
        with self.assertRaisesRegex(ValueError, 'first accepted _BSS owner'):
            self.validate(bad, self.size)

    def test_row_must_end_the_image(self):
        bad = copy.deepcopy(self.manifest)
        bad['owners'][-1]['extra'] = True
        with self.assertRaisesRegex(ValueError, 'first accepted _BSS owner'):
            self.validate(bad, self.size)

    def test_word_fill_before_bss(self):
        from link_fill import checked_word_fill
        image = Oracle.load().image
        fill = next(o for o in self.manifest['owners'] if o['start'] == 199993)
        receipt = checked_word_fill(fill, self.manifest, image)
        self.assertEqual(receipt['next_segment']['owner'], 'frontend:_BSS')
        self.assertEqual(receipt['next_segment']['alignment'], 'word')
        bad = copy.deepcopy(self.manifest)
        bad['owners'][-1]['bss_owner'] = 'race:_BSS'
        fill = next(o for o in bad['owners'] if o['start'] == 199993)
        with self.assertRaisesRegex(ValueError, 'first _BSS contribution'):
            checked_word_fill(fill, bad, image)


if __name__ == '__main__':
    unittest.main()
