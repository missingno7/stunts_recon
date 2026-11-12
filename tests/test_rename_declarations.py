"""Combined source/name transactions keep exact objects and monotone COMDEFs."""
import copy
import hashlib
import json
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
import rename
from common import json_bytes
from omf import OmfReader
from object_probe import read_object
from tests.test_rename_transaction import (object_fixture, communal_fixture, fixture_files,
                                           record, name, ROW)


def manifest():
    return {'owners': [{'id': 'owner', 'kind': 'MATCHING_C'},
                       {'id': 'other', 'kind': 'MATCHING_C'}],
            'bss_owners': [{'kind': 'LINK_COMMUNAL', 'communals': [
                {'name': '_shared', 'size': 4, 'address': 100, 'declarers': ['other'],
                 'size_basis': {'kind': 'declared-type', 'declaration': 'long shared;'}}]}]}


class CommunalAdditionTests(unittest.TestCase):
    def allowed(self, before, after, doc=None):
        return rename.allowed_communal_additions(before[0], after[0], {}, doc or manifest(), ['owner'])

    def test_accept_exact_existing_storage_and_explicit_declaration(self):
        before, after = object_fixture(), communal_fixture([('_shared', 4)])
        additions = self.allowed(before, after)
        self.assertEqual(additions, {'_shared': 4})
        self.assertIsNone(rename.compare_objects(before, after, {}, communal_additions=additions))
        self.assertIsNotNone(rename.compare_objects(before, after, {}))

    def test_refuse_unknown_communal(self):
        with self.assertRaisesRegex(ValueError, 'new communal'):
            self.allowed(object_fixture(), communal_fixture([('_unknown', 4)]))

    def test_refuse_wrong_added_size(self):
        with self.assertRaisesRegex(ValueError, 'size or kind'):
            self.allowed(object_fixture(), communal_fixture([('_shared', 2)]))

    def test_refuse_changed_existing_size(self):
        with self.assertRaisesRegex(ValueError, 'existing COMDEF'):
            self.allowed(communal_fixture([('_shared', 4)]), communal_fixture([('_shared', 2)]))

    def test_refuse_drop_of_declarer(self):
        with self.assertRaisesRegex(ValueError, 'drop a communal declarer'):
            self.allowed(communal_fixture([('_shared', 4)]), object_fixture())

    def test_refuse_without_accepted_declarer_elsewhere(self):
        for declarers, kind in [(['owner'], 'MATCHING_C'), (['other'], 'UNRESOLVED_RAW')]:
            doc = manifest();doc['bss_owners'][0]['communals'][0]['declarers'] = declarers
            doc['owners'][1]['kind'] = kind
            with self.assertRaisesRegex(ValueError, 'accepted declarer elsewhere'):
                self.allowed(object_fixture(), communal_fixture([('_shared', 4)]), doc)

    def test_refuse_changed_kind_or_type(self):
        for key, value in [('kind', 'far'), ('type_index', 1)]:
            after = communal_fixture([('_shared', 4)]);after[0].communals[0][key] = value
            with self.assertRaisesRegex(ValueError, 'size or kind'):
                self.allowed(object_fixture(), after)

    def test_payload_extent_and_named_fixup_stay_strict(self):
        before = object_fixture();after = communal_fixture([('_shared', 4)])
        for field in ['payload', 'extent', 'fixup', 'public', 'group']:
            with self.subTest(field=field):
                changed = copy.deepcopy(after)
                if field == 'payload':changed[0].segments['CODE']=b'\x90'+changed[0].segments['CODE'][1:]
                if field == 'extent':changed[0].segment_lengths['CODE'] += 1
                if field == 'fixup':changed[0].linker_fixups[0]['target'] = '_wrong'
                if field == 'public':changed[0].publics[0]['offset'] += 1
                if field == 'group':changed[0].groups.append({'name': 'new'})
                self.assertIsNotNone(rename.compare_objects(before, changed, {}, communal_additions={'_shared': 4}))

    def test_existing_external_can_become_communal_without_losing_fixup(self):
        before = object_fixture(external='_shared')
        raw = b''.join(record(0xB0, name('_shared')+b'\0\x62\x04') if k == 0x8C else record(k, b)
                       for k, b in OmfReader.records(before[1]))
        after = read_object(raw, communals=True), raw
        self.assertIsNone(rename.compare_objects(before, after, {}, communal_additions=self.allowed(before, after)))

    def test_derive_declarers_only_and_refuse_inventory_size_or_loss(self):
        doc = manifest();result = rename.manifest_with_linked_declarers(doc, {'_shared': [('other', 4), ('owner', 4)]})
        expected = copy.deepcopy(doc);expected['bss_owners'][0]['communals'][0]['declarers'] = ['other', 'owner']
        self.assertEqual(result, expected)
        for declared in [{}, {'_new': [('other', 4)]}, {'_shared': [('other', 2)]}, {'_shared': [('owner', 4)]}]:
            with self.assertRaises(ValueError):rename.manifest_with_linked_declarers(doc, declared)

    def test_read_actual_linked_records_and_identity(self):
        with tempfile.TemporaryDirectory() as work:
            root = Path(work);link = root/'link';link.mkdir()
            raw = communal_fixture([('_shared', 4)])[1];(link/'M001.OBJ').write_bytes(raw)
            (link/'objects.json').write_bytes(json_bytes([{'file': 'M001.OBJ', 'unit': 'c_owner', 'sha256': hashlib.sha256(raw).hexdigest()}]))
            report = root/'report.json';report.write_bytes(json_bytes({'accepted_objects': [{'unit': 'c_owner', 'piece': 'owner'}]}))
            self.assertEqual(rename.linked_communal_declarers(report), {'_shared': [('owner', 4)]})
            (link/'M001.OBJ').write_bytes(raw+b'bad')
            with self.assertRaisesRegex(ValueError, 'identity'):rename.linked_communal_declarers(report)

    def test_historical_inventories_and_independent_fixtures_keep_names(self):
        files = fixture_files()
        for p in ['evidence/functions.json', 'layout/function-evidence.json', 'tests/fixtures/a.json']:
            files[p] = b'{"public":"_old_function"}\n'
        changes = rename.make_changes(files, [ROW])
        for p in ['evidence/functions.json', 'layout/function-evidence.json', 'tests/fixtures/a.json']:
            self.assertNotIn(p, changes)

    def test_combined_sources_are_renamed_and_whole_gates_are_mandatory(self):
        for status in ['ok', 'image', 'bss', 'historical']:
            with self.subTest(status=status), tempfile.TemporaryDirectory() as work:
                root=Path(work);files=fixture_files();doc=manifest();doc['owners'][0].update(recipe='recipes/owner.json')
                files['layout/manifest.json']=json_bytes(doc)
                for p,b in files.items():(root/p).parent.mkdir(parents=True,exist_ok=True);(root/p).write_bytes(b)
                def compiler(recipe, source):
                    raw = communal_fixture([('_shared',4)])[1] if b'long shared;' in source else object_fixture()[1]
                    raw=b''.join(record(k,b'\0\1'+name(recipe['public'])+b'\0\0\0' if k==0x90 else b) for k,b in OmfReader.records(raw))
                    return read_object(raw,communals=True),raw,{}
                image={'status':'HYBRID_EXACT','bss_real_link':{'status':'PLACED'},'historical':{'status':'BUILT_EXACT'}}
                if status=='image':image['status']='DIFFERENT'
                if status=='bss':image['bss_real_link']['status']='REFUSED'
                if status=='historical':image['historical']['status']='BUILT_NOT_EXACT'
                engine=rename.ProofEngine(root,files,compiler,image=lambda:image,
                    declarations={'src/owner.c':b'long shared;\n'+files['src/owner.c']})
                result=engine.trial([ROW],whole=True)
                self.assertEqual(bool(result['failures']),status!='ok')
                self.assertIn(b'reviewed_function',(root/'src/owner.c').read_bytes())
                if status=='ok':
                    self.assertEqual(json.loads(engine.current['layout/manifest.json'])['bss_owners'][0]['communals'][0]['declarers'],['other','owner'])


if __name__ == '__main__': unittest.main()
