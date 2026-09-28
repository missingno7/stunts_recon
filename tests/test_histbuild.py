"""Historical build entry-point planning, debt accounting and DOS response output."""
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))

from common import read_json
import histbuild
import reallink


class HistoricalBuildPlanningTests(unittest.TestCase):
    def test_manifest_summary_keeps_initialized_and_bss_debt_separate(self):
        summary = histbuild.ownership_summary(read_json(ROOT / 'layout/manifest.json'))
        self.assertEqual((summary['c_code_objects'], summary['asm_modules']), (35, 52))
        self.assertEqual(summary['raw_initialized_bytes'], 0)
        self.assertEqual(summary['raw_initialized_ranges'], [])
        self.assertEqual(summary['raw_bss_by_form'], {})

    def test_default_plan_uses_image_order_and_link_library_search(self):
        plan = histbuild.run_plan(reallink.LINK_OPTIONS)
        self.assertEqual(plan, {
            'runtime': 'libraries', 'partial': 'split', 'order': 'library',
            'library_order': 'combined', 'game_order': 'image', 'game_input': 'library',
            'link_options': reallink.LINK_OPTIONS,
        })

    def test_link_response_parser_and_makefile(self):
        response = ('E000+D001+E002+\r\nD003\r\nRESULT.EXE\r\nRESULT.MAP\r\n'
                    'GAME.LIB+LIBH.LIB /DOSSEG /NOI /NOD /MAP /CP:1 /ST:8000;\r\n')
        explicit, libraries = histbuild.parse_link_response(response)
        self.assertEqual(explicit, ['E000.OBJ', 'D001.OBJ', 'E002.OBJ', 'D003.OBJ'])
        self.assertEqual(libraries, ['GAME.LIB', 'LIBH.LIB'])
        template = histbuild.MAKEFILE_TEMPLATE.read_text(encoding='ascii')
        makefile = histbuild.render_makefile(template, explicit, ['G004.OBJ', 'D005.OBJ'], libraries)
        self.assertIn('RESULT.EXE: LINK.RSP $(EXPLICIT_OBJECTS) $(LINK_LIBRARIES)', makefile)
        self.assertIn('GAME.LIB: MLIBCR.LIB GAME.RSP $(GAME_OBJECTS)', makefile)
        self.assertIn('$(EXEPACK) RESULT.EXE PACKED.EXE', makefile)
        self.assertIn('D001.OBJ', makefile)
        self.assertNotIn('@EXPLICIT_OBJECTS@', makefile)

    def test_library_root_has_no_segments_bytes_or_fixups(self):
        blob = reallink.library_root_object(['_second', '_first', '_second'])
        from omf import OmfReader
        obj = OmfReader(communals=True).read(blob)
        self.assertEqual(obj.segment_defs, [])
        self.assertEqual(obj.externals, ['_second', '_first'])
        self.assertEqual(obj.linker_fixups, [])
        self.assertEqual(obj.publics, [])

    def test_checked_in_response_templates_render_complete_dos_files(self):
        game = reallink.render_historical_response('GAME.RSP.in', {
            '@LIBRARY_HEADER@': 'GAME.LIB',
            '@GAME_MODULES@': '+G001.OBJ &\r\n+G002.OBJ',
        })
        self.assertEqual(game, 'GAME.LIB\r\n+G001.OBJ &\r\n+G002.OBJ\r\nNUL;\r\n')
        link = reallink.render_historical_response('LINK.RSP.in', {
            '@OBJECT_MODULES@': 'E000+ROOT',
            '@OUTPUT_EXE@': 'RESULT.EXE',
            '@MAP_FILE@': 'RESULT.MAP',
            '@LIBRARIES_AND_OPTIONS@': 'GAME.LIB+LIBH.LIB /ST:8000',
        })
        self.assertEqual(link, 'E000+ROOT\r\nRESULT.EXE\r\nRESULT.MAP\r\n'
                               'GAME.LIB+LIBH.LIB /ST:8000;\r\n')
        self.assertNotRegex(game + link, r'@[A-Z_]+@')

    def test_tag_cannot_escape_build_output(self):
        with self.assertRaisesRegex(ValueError, 'tag must use'):
            histbuild._new_run_id('../outside')


if __name__ == '__main__':
    unittest.main()
