"""Synthetic localization, durable information flow, and acceptance isolation."""
import copy
import json
import sys
import tempfile
import unittest
from pathlib import Path
from types import SimpleNamespace
from unittest.mock import patch

ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
from common import identity,write_json,read_json,sha
from diagnostics import compare_streams,compact,format_summary
import attempt_index
import context
import probe_module

PREFIX='558bec83ec10'
SUFFIX='8b46020346048946065b5dc3'


def compare(a,b,**kwargs):return compare_streams(bytes.fromhex(a),bytes.fromhex(b),**kwargs)
def classes(r):return {c['class'] for i in r['islands'] for c in i['classifications']}


class AlignmentTests(unittest.TestCase):
    def test_identical_and_deterministic(self):
        r=compare(PREFIX+SUFFIX,PREFIX+SUFFIX)
        self.assertEqual(r,compare(PREFIX+SUFFIX,PREFIX+SUFFIX))
        self.assertEqual(r['counts']['mismatch_islands'],0)
        self.assertEqual(r['counts']['exact_instructions'],r['counts']['target_instructions'])

    def test_register_substitution(self):
        r=compare(PREFIX+'8bd8'+SUFFIX,PREFIX+'8bf0'+SUFFIX)
        self.assertIn('REGISTER_ALLOCATION',classes(r))
        self.assertTrue(r['islands'][0]['resumes_exact'])

    def test_stack_slots(self):
        r=compare(PREFIX+'8946f48956f6'+SUFFIX,PREFIX+'8946f88956fa'+SUFFIX)
        self.assertEqual(r['islands'][0]['classifications'][0]['class'],'STACK_SLOT_ALLOCATION')
        self.assertEqual(r['islands'][0]['classifications'][0]['confidence'],'high')

    def test_insert_then_realign(self):
        r=compare(PREFIX+SUFFIX,PREFIX+'40'+SUFFIX)
        self.assertIn('EXTRA_INSTRUCTIONS',classes(r))
        self.assertTrue(r['islands'][0]['resumes_exact'])
        self.assertNotEqual(r['exact_suffix']['target']['bytes'],r['exact_suffix']['candidate']['bytes'])

    def test_delete_then_realign(self):
        r=compare(PREFIX+'40'+SUFFIX,PREFIX+SUFFIX)
        self.assertIn('MISSING_INSTRUCTIONS',classes(r))
        self.assertTrue(r['islands'][0]['resumes_exact'])

    def test_branch_encoding_and_position_independent_decode(self):
        r=compare(PREFIX+'eb00'+SUFFIX,PREFIX+'e90000'+SUFFIX)
        self.assertIn('BRANCH_ENCODING',classes(r))
        self.assertEqual(r['counts']['decoded_exact_pairs'],1)
        self.assertTrue(r['islands'][0]['resumes_exact'])

    def test_branch_layout(self):
        r=compare(PREFIX+'7400'+SUFFIX,PREFIX+'740140'+SUFFIX)
        self.assertIn('BRANCH_TARGET_OR_LAYOUT',classes(r))
        self.assertTrue(r['exact_suffix'])

    def test_repeated_epilogue_is_not_anchor(self):
        r=compare('b801005dc3b802005dc3','b803005dc3')
        self.assertIsNone(r['exact_suffix'])
        self.assertEqual(r['counts']['anchors'],0)
        self.assertTrue(any('Repeated terminal' in n for n in r['notes']))

    def test_fixups_never_become_exact(self):
        fix=[{'offset':len(bytes.fromhex(PREFIX))+1,'width':4,'segment':'UNIT_TEXT','target':'helper','loc':'pointer32'}]
        r=compare(PREFIX+'9a881cc51c'+SUFFIX,PREFIX+'9a00000000'+SUFFIX,fixups=fix)
        self.assertEqual(r['counts']['fixup_normalized_pairs'],1)
        self.assertIn('UNRESOLVED_FIXUP_OPERANDS',classes(r))
        self.assertTrue(r['islands'][0]['resumes_exact'])
        zero=compare(PREFIX+'9a00000000'+SUFFIX,PREFIX+'9a00000000'+SUFFIX,fixups=fix)
        self.assertEqual(zero['counts']['fixup_normalized_pairs'],1)
        self.assertEqual(zero['counts']['exact_instructions'],zero['counts']['target_instructions']-1)
        bound=compare(PREFIX+'9a881cc51c'+SUFFIX,PREFIX+'9a881cc51c'+SUFFIX,fixups=fix,bound=True)
        self.assertEqual(bound['counts']['mismatch_islands'],0)

    def test_invalid_fixup_and_undecoded_tail(self):
        r=compare('b80100','b80200',fixups=[{'offset':0,'width':1}])
        self.assertEqual(len(r['fixup_normalization']['ignored']),1)
        self.assertEqual(r['counts']['fixup_normalized_pairs'],0)
        r=compare('550f','550f')
        self.assertFalse(r['decode_complete'])
        self.assertEqual(r['islands'][-1]['target']['bytes'],[1,2])

    def test_immediate_width_control_and_unclassified(self):
        self.assertIn('IMMEDIATE_VALUE',classes(compare(PREFIX+'b80100'+SUFFIX,PREFIX+'b80200'+SUFFIX)))
        self.assertIn('WIDTH_OR_EXTENSION',classes(compare(PREFIX+'b001'+SUFFIX,PREFIX+'b80100'+SUFFIX)))
        self.assertIn('CONTROL_FLOW_SHAPE',classes(compare('74005dc3','75005dc3')))
        self.assertIn('LOCAL_CODEGEN_UNCLASSIFIED',classes(compare('01d8','29d8')))

    def test_compact_bounded_and_formatter(self):
        r=compare(PREFIX+'40'+SUFFIX,PREFIX+SUFFIX)
        r['islands']=r['islands']*100;r['anchors']=r['anchors']*100
        summary=compact(r)
        self.assertEqual(len(summary['islands']),12)
        self.assertGreater(summary['omitted_islands'],0)
        self.assertNotIn('target_instructions',summary)
        self.assertNotIn('alignment',summary)
        self.assertIn('Exact stream resumes',format_summary(summary))


class IntegrationTests(unittest.TestCase):
    def probe_fixture(self,target,candidate,diagnostic_broken=False):
        with tempfile.TemporaryDirectory(dir=ROOT/'build') as d:
            path=Path(d);source=path/'source.c';source.write_text('fixture')
            recipe={'start':0,'end':len(target),'target':identity(target),'expected_relocations':[],
                    'source':str(source),'profile':'fixture','object_segment':'UNIT_TEXT'}
            obj=SimpleNamespace(segments={'UNIT_TEXT':candidate},segment_lengths={'UNIT_TEXT':len(candidate)},
                segment_length=lambda s:len(candidate),publics=[],externals=[],linker_fixups=[])
            from contextlib import ExitStack
            with ExitStack() as stack:
                stack.enter_context(patch('probe_module.MZ.parse',return_value=SimpleNamespace(load_image=lambda _:target)))
                stack.enter_context(patch('probe_module.compile_source',return_value=(obj,{'work_directory':str(path)})))
                stack.enter_context(patch('probe_module.resolve_recipe_symbols',return_value={}))
                bind=stack.enter_context(patch('probe_module.bind_contribution',return_value=(candidate,{'generated_relocations':[]})))
                if diagnostic_broken:stack.enter_context(patch('diagnostics.diagnose',side_effect=RuntimeError('broken decoder')))
                with self.assertRaises(probe_module.ProbeFailure) as caught:
                    probe_module.probe(recipe,(None,b'',{'unpacked_mz':{'relocations':[]}}))
                return caught.exception.details,bind.called

    def test_unequal_extent_cannot_bind_or_pass(self):
        details,bound=self.probe_fixture(b'\x55\xc3',b'\xc3')
        self.assertEqual(details['category'],'EXTENT_MISMATCH');self.assertFalse(bound)
        self.assertIn('match_summary',details['comparison'])

    def test_equal_wrong_byte_uses_bound_diagnostic_and_fails(self):
        details,bound=self.probe_fixture(b'\x55\xc3',b'\x53\xc3')
        self.assertEqual(details['category'],'BYTE_MISMATCH');self.assertTrue(bound)
        self.assertEqual(details['comparison']['comparison'],'bound payload')
        self.assertEqual(details['object_comparison']['comparison'],'unbound object')

    def test_diagnostic_bug_cannot_change_failure(self):
        for candidate,category in [(b'\xc3','EXTENT_MISMATCH'),(b'\x53\xc3','BYTE_MISMATCH')]:
            details,_=self.probe_fixture(b'\x55\xc3',candidate,True)
            self.assertEqual(details['category'],category)
            self.assertIn('diagnostic_error',details['comparison'])

    def test_index_and_default_context_surface_summary(self):
        with tempfile.TemporaryDirectory(dir=ROOT/'build') as d:
            root=Path(d);source=b'int fixture;';summary=compact(compare(PREFIX+'8946f4'+SUFFIX,PREFIX+'8946f8'+SUFFIX))
            report={'task':'fixture','source':identity(source),'diagnostics':{'comparison':{
                'match_summary':summary,'full_diagnostic':'full.json','full_diagnostic_identity':identity(b'full'),
                'target_instructions':['must not leak'],'candidate_instructions':['must not leak']}}}
            write_json(root/'recovery/attempts/fixture/0001/report.json',report)
            with patch('attempt_index.ROOT',root):ledger=attempt_index.generate()
            entry=ledger['tasks']['fixture'][0]
            self.assertEqual(entry['match_summary']['counts'],summary['counts'])
            self.assertNotIn('must not leak',json.dumps(ledger))
            recipe={'source':'candidate.c','profile':'fixture','expected_fixups':[],'expected_relocations':[]}
            (root/'candidate.c').write_bytes(source);write_json(root/'recipe.json',recipe)
            write_json(root/'layout/toolchain.json',{'profiles':{'fixture':{'flags':[]}}})
            card={'recipe':'recipe.json','evidence':{'provenance':'fixture'},'tier':'SUPERVISOR','capability_blockers':[],'risks':[]}
            write_json(root/'card.json',card)
            write_json(root/'recovery/context-index.json',{'tasks':[{'id':'id','name':'fixture','card':'card.json','card_sha256':sha((root/'card.json').read_bytes())}],
                'workflow_fingerprint':'fingerprint','oracle':{},'attempt_index_sha256':sha((root/'recovery/attempt-index.json').read_bytes())})
            with patch('context.ROOT',root),patch('workflow.workflow_inputs',return_value={}),\
                 patch('workflow.fingerprint',return_value='fingerprint'),patch('workflow.state',return_value={'reason':None,'blocked':True,'remaining':2}):
                packet=context.packet('id')
            self.assertEqual(packet['match_diagnosis']['match_summary']['counts'],summary['counts'])
            self.assertTrue(packet['match_diagnosis']['source_matches_current'])
            self.assertIn('patterns',packet['match_diagnosis']['match_summary'])
            self.assertNotIn('islands',packet['match_diagnosis']['match_summary'])
            self.assertIn('--islands',packet['match_diagnosis']['drill_down'])
            self.assertNotIn('assembly',packet)
            write_json(root/'recovery/attempts/fixture/0002/report.json',{
                'task':'fixture','status':'FAILED','diagnostics':{'category':'BYTE_MISMATCH',
                'comparison':{'diagnostic_error':'decoder unavailable'}}})
            with patch('attempt_index.ROOT',root):attempt_index.generate()
            index=read_json(root/'recovery/context-index.json')
            index['attempt_index_sha256']=sha((root/'recovery/attempt-index.json').read_bytes())
            write_json(root/'recovery/context-index.json',index)
            with patch('context.ROOT',root),patch('workflow.workflow_inputs',return_value={}),\
                 patch('workflow.fingerprint',return_value='fingerprint'),patch('workflow.state',return_value={'reason':None,'blocked':True,'remaining':1}):
                newer=context.packet('id')['match_diagnosis']['newer_observations_without_summary']
            self.assertEqual(newer[0]['category'],'BYTE_MISMATCH')
            self.assertEqual(newer[0]['diagnostic_error'],'decoder unavailable')

    def test_anchor_loss_is_diagnostic_and_comparable(self):
        old=compact(compare(PREFIX+SUFFIX,PREFIX+SUFFIX))
        new=compact(compare(PREFIX+SUFFIX,PREFIX+'90'))
        loss=attempt_index.lost_anchors(old,new)
        self.assertGreater(loss['lost_bytes'],0)
        new['omitted_anchors']=1
        self.assertIsNone(attempt_index.lost_anchors(old,new))
        new['omitted_anchors']=0
        new['engine_sha256']='different'
        self.assertIsNone(attempt_index.lost_anchors(old,new))

    def test_history_orders_supervisor_before_newer_attempt(self):
        with tempfile.TemporaryDirectory(dir=ROOT/'build') as d:
            root=Path(d)
            old=compact(compare(PREFIX+SUFFIX,PREFIX+SUFFIX))
            new=compact(compare(PREFIX+SUFFIX,PREFIX+'90'))
            for folder,name,stamp,summary in [('diagnostics','observation','2026-09-23T10:00:00+00:00',old),
                                             ('attempts','0002','2026-09-23T11:00:00+00:00',new)]:
                write_json(root/'recovery'/folder/'fixture'/name/'report.json',
                    {'task':'fixture','started_utc':stamp,'diagnostics':{'comparison':{'match_summary':summary}}})
            with patch('attempt_index.ROOT',root):rows=attempt_index.generate(False)['tasks']['fixture']
            self.assertEqual([r['kind'] for r in rows],['diagnostics','attempts'])
            self.assertGreater(rows[-1]['anchor_regression']['lost_bytes'],0)

    def test_spill_and_return_patterns_are_observations(self):
        r=compare(PREFIX+SUFFIX,PREFIX+'8946fe8b46fe'+SUFFIX)
        self.assertIn('TEMPORARY_OR_SPILL',classes(r))
        self.assertIn('EPILOGUE_OR_RETURN_LOWERING',classes(compare('c20200','c20400')))

    def test_archive_copies_verified_full_artifact(self):
        with tempfile.TemporaryDirectory(dir=ROOT/'build') as d:
            destination=Path(d);original=destination/'original.json';original.write_bytes(b'{}')
            details={'comparison':{'full_diagnostic':str(original),'full_diagnostic_identity':identity(b'{}')}}
            probe_module.archive_diagnostics(details,destination)
            self.assertEqual((destination/'comparison-full.json').read_bytes(),b'{}')
            details['comparison']['full_diagnostic_identity']=identity(b'wrong')
            probe_module.archive_diagnostics(details,destination)
            self.assertIn('archive_error',details['comparison'])


if __name__=='__main__':unittest.main()
