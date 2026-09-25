"""Focused TU map and member comparison checks against the locked image."""
import io
import sys
import unittest
from contextlib import redirect_stdout
from pathlib import Path
from types import SimpleNamespace

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
from tubench import WORKSPACE, _own_data_placements, build_tu_map, run_workbench


class TUBench(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        WORKSPACE.mkdir(parents=True, exist_ok=True)

    def compare(self, source, **selection):
        with redirect_stdout(io.StringIO()):
            return run_workbench(source, **selection)

    def test_map_from_canonical_inputs(self):
        document = build_tu_map()
        self.assertEqual(document["closure_count"], 42)
        self.assertEqual(document["near_call_edges"], 435)
        self.assertTrue(any(row['name']=='mat_rot_zxy' for closure in document['closures']
                            for row in closure['members']))
        self.assertTrue(document["near_call_decode_anomalies"])
        self.assertFalse([row for row in document["near_call_decode_anomalies"]
                          if 95408 <= row["start"] < 107196])
        segment = next(row for row in document["closures"]
                       if row["segment"] == "seg008" and row["interval"]["start"] == 95408)
        self.assertEqual(segment["interval"], {"start": 95408, "end": 107196, "bytes": 11788})
        self.assertEqual(segment["member_count"], 62)
        self.assertIn("file_load_resource", [row['name'] for row in segment['members']])

    def test_exact_groups_and_mutated_member(self):
        names = "audio_enable_flag2,audio_disable_flag2,audio_toggle_flag2"
        source = ROOT / "src/audio_flag2_group.c"
        exact = self.compare(source, members_arg=names)
        self.assertEqual(exact["summary"]["exact"], 3)
        mouse = self.compare(ROOT / "src/mouse_set_pixratio.c", members_arg=
                             "mouse_set_pixratio,mouse_init,mouse_set_minmax,mouse_get_position,mouse_show_cursor,mouse_hide_cursor")
        self.assertEqual(mouse["summary"]["exact"], 6)
        mutated = WORKSPACE / "mutated_audio_flag2_group.c"
        mutated.write_text(source.read_text().replace("audioflag2 = 1;", "audioflag2 = 2;", 1), encoding="ascii")
        report = self.compare(mutated, members_arg=names)
        self.assertEqual(next(row for row in report["members"]
                              if row["name"] == "audio_enable_flag2")["status"], "DIFFER")

    def test_declared_only_callee_operand(self):
        source = WORKSPACE / "declared_only.c"
        source.write_text("""extern int near audioresource_get_chunk_index(unsigned int stride, int count,
    unsigned char *wanted, unsigned char far *records);
extern unsigned long far audioresource_get_dword(unsigned char far *address);
unsigned char far * far audioresource_find(unsigned char far *resource, unsigned char *name)
{
    unsigned int count;
    int index;
    unsigned long relative;
    unsigned char far *entry_offsets;
    unsigned char far *result;
    count = *(unsigned int far *)(resource + 4);
    index = audioresource_get_chunk_index(0, count, name, resource + 6);
    if (index >= 0) {
        entry_offsets = resource + 6 + ((count + index) * 4);
        relative = audioresource_get_dword(entry_offsets);
        result = resource + 6 + count * 8 + (unsigned int)relative;
    } else {
        result = 0;
    }
    return result;
}
""", encoding="ascii")
        report = self.compare(source, interval=(170708, 171140))
        self.assertTrue(all(row["status"] == "DECLARED_ONLY_OR_MISSING_DEFINITION"
                            for row in report["members"][:2]))
        find = next(row for row in report["members"] if row["name"] == "audioresource_find")
        near = next(row for row in find["external_fixups"]
                    if row["target"] == "_audioresource_get_chunk_index")
        self.assertTrue(near["resolved"] and near["self_relative"])
        self.assertEqual(near["target_operand_bytes"], "36ff")
        self.assertTrue(near["operand_bytes_match_target"])

    def test_tu_owned_data_uses_grounded_dgroup_base(self):
        obj=SimpleNamespace(segment_lengths={'UNIT_TEXT':10,'_DATA':4},
                            segments={'_DATA':b'abcd'})
        fix={'target_kind':'segment','target':'_DATA','loc':'offset16',
             'width':2,'self_relative':False,'frame_kind':'group',
             'frame':'DGROUP','offset':2,'encoded_addend':'0200'}
        publics=[{'name':'_member','offset':0,'segment':'UNIT_TEXT'}]
        functions=[{'name':'member','start':20,'end':30}]
        image=bytearray(120)
        image[22:24]=(22).to_bytes(2,'little')
        image[100:104]=b'abcd'
        placement=_own_data_placements(obj,[fix],publics,functions,image,80)['_DATA']
        self.assertEqual((placement['status'],placement['base'],placement['dgroup_offset']),
                         ('GROUNDED',100,20))
        image[100]=0
        self.assertEqual(_own_data_placements(obj,[fix],publics,functions,image,80)
                         ['_DATA']['status'],'UNRESOLVED')


if __name__ == "__main__":
    unittest.main()
