import copy
import sys
import tempfile
import unittest
from contextlib import ExitStack
from pathlib import Path
from unittest.mock import patch
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
from common import read_json, write_json, sha
from triage import capabilities, recipe_matches_inventory
import workflow
import grind
from probe_module import ProbeFailure


class TriageTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.inventory=read_json(ROOT/'recovery/restunts-inventory.json')
    def inspect(self,name):
        f=next(f for f in self.inventory['functions'] if f['name']==name)
        small=next(f for f in self.inventory['small_candidates'] if f['name']==name)
        return capabilities(f,small['disassembly'],set())
    def test_external_jumps_are_not_ordinary_leaf_work(self):
        for name in ['mmgr_release','mmgr_get_chunk_size']:
            blocked,_=self.inspect(name)
            self.assertTrue(any('External/indirect jump' in b for b in blocked))
    def test_cs_storage_needs_supervisor(self):
        blocked,_=self.inspect('sprite_set_1_size')
        self.assertIn('CS-relative storage is unsupported',blocked)
    def test_cs_string_copy_is_not_misreported_as_dgroup(self):
        for name in ['sprite_copy_both_to_arg','sprite_copy_arg_to_both']:
            blocked,_=self.inspect(name)
            self.assertIn('CS-relative storage is unsupported',blocked)
            self.assertIn('Unregistered absolute address expression 0x5f20',blocked)
            self.assertNotIn('Unregistered DGROUP address 0x5f20',blocked)
    def test_unregistered_absolute_global_needs_review(self):
        blocked,_=self.inspect('nopsub_kb_set_readchar_callback')
        self.assertIn('Unregistered DGROUP address 0x468c',blocked)
    def test_odd_extent_is_risk_not_origin_or_automatic_block(self):
        blocked,risks=self.inspect('file_get_res_shape_count')
        self.assertFalse(blocked);self.assertTrue(any('Odd extent' in x for x in risks))
    def test_far_pointer_register_access_is_supported(self):
        blocked,_=self.inspect('copy_string');self.assertFalse(blocked)
    def test_recipe_must_match_current_inventory_mapping(self):
        f=next(f for f in self.inventory['functions'] if f['name']=='parse_shape2d_helper')
        recipe={'id':f['name'],'stable_id':f['stable_id'],'start':f['start'],'end':f['end'],
                'target':{'size':f['size'],'sha256':f['sha256']}}
        self.assertTrue(recipe_matches_inventory(recipe,f))
        for key,value in [('start',f['start']+1),('end',f['end']+1),('stable_id','wrong'),('id','wrong'),('target',{'size':f['size'],'sha256':'0'*64})]:
            self.assertFalse(recipe_matches_inventory({**recipe,key:value},f))


class LedgerTests(unittest.TestCase):
    def setUp(self):
        self.temp=tempfile.TemporaryDirectory(dir=ROOT/'build');self.addCleanup(self.temp.cleanup)
        self.root=Path(self.temp.name)
        for folder in ['recipes','recovery/candidates','recovery/attempts','layout','tools']:
            (self.root/folder).mkdir(parents=True,exist_ok=True)
        self.recipe={'id':'candidate','start':1,'end':3,'source':'recovery/candidates/candidate.c','profile':'fixture'}
        write_json(self.root/'recipes/candidate.json',self.recipe)
        write_json(self.root/'recovery/blockers.json',{'attempts':[]})
        self.source=self.root/self.recipe['source'];self.source.write_text('first hypothesis')
        self.stack=ExitStack();self.addCleanup(self.stack.close)
        def snapshot():
            return {p.relative_to(self.root).as_posix():sha(p.read_bytes())
                    for folder in ['recipes','layout','tools','recovery/candidates']
                    for p in (self.root/folder).rglob('*') if p.is_file()}
        self.snapshot=snapshot
        for module in [workflow,grind]:
            self.stack.enter_context(patch.object(module,'ROOT',self.root))
            self.stack.enter_context(patch.object(module,'inputs',snapshot))
        self.stack.enter_context(patch.object(grind,'project_path',side_effect=lambda p:self.root/p))
        self.stack.enter_context(patch('reconstruction_factory.refresh',return_value={}))
    def failure(self,category='BYTE_MISMATCH'):
        return ProbeFailure('fixture mismatch',{'category':category,'receipt':{'object':{'size':3,'sha256':'object'}}})
    def test_three_failures_archive_and_block_a_fourth(self):
        with patch.object(grind,'check',side_effect=self.failure()) as check:
            for i in range(3):
                self.source.write_text('hypothesis '+str(i))
                self.assertEqual(grind.run('candidate','specific source change')['status'],'FAILED')
            self.source.write_text('fourth')
            with self.assertRaisesRegex(ValueError,'blocked'):grind.run('candidate','too many')
            self.assertEqual(check.call_count,3)
        self.assertEqual(workflow.state('candidate')['remaining'],0)
        self.assertEqual(len(list((self.root/'recovery/attempts/candidate').glob('*/source.c'))),3)
        self.assertEqual(workflow.attempts('candidate')[1]['same_object_as'],[1])
        self.assertFalse((self.root/'build/grind.lock').exists())
    def test_unchanged_failure_does_not_recompile(self):
        with patch.object(grind,'check',side_effect=self.failure()) as check:
            grind.run('candidate','first')
            with self.assertRaisesRegex(ValueError,'Unchanged'):grind.run('candidate','same source renamed')
            self.assertEqual(check.call_count,1)
    def test_unsupported_object_escalates_immediately(self):
        with patch.object(grind,'check',side_effect=self.failure('UNSUPPORTED_OBJECT')):
            grind.run('candidate','unsupported record')
        self.assertTrue(workflow.state('candidate')['blocked'])
    def test_unsupported_source_is_not_an_infrastructure_retry(self):
        from compiler import compile_source, CompileFailure
        with self.assertRaises(CompileFailure) as caught:
            compile_source(b'#include <stdio.h>\n','msc510-medium')
        self.assertEqual(caught.exception.category,'UNSUPPORTED_SOURCE')
        with patch.object(grind,'check',side_effect=self.failure('UNSUPPORTED_SOURCE')):
            grind.run('candidate','unsupported preprocessor dependency')
        self.assertTrue(workflow.state('candidate')['blocked'])
    def test_infrastructure_failure_does_not_spend_source_budget(self):
        with patch.object(grind,'check',side_effect=ValueError('toolchain hash mismatch')):
            r=grind.run('candidate','first')
        self.assertEqual(r['status'],'ERROR');self.assertEqual(workflow.state('candidate')['remaining'],3)
    def test_legacy_interval_block_cannot_be_bypassed_by_renaming(self):
        write_json(self.root/'recovery/blockers.json',{'attempts':[{'name':'different_name','interval':[0,4],'reason':'blocked bytes'}]})
        with self.assertRaisesRegex(ValueError,'blocked bytes'):workflow.require_eligible(self.recipe)
    def test_new_escalation_follows_overlapping_interval_after_rename(self):
        with patch.object(grind,'check',side_effect=self.failure('UNSUPPORTED_OBJECT')):
            grind.run('candidate','unsupported')
        renamed={**self.recipe,'id':'another_name'}
        with self.assertRaisesRegex(ValueError,'Overlapping failed interval'):workflow.require_eligible(renamed)
    def test_new_budget_follows_overlapping_interval_after_rename(self):
        with patch.object(grind,'check',side_effect=self.failure()):
            for i in range(3):
                self.source.write_text('hypothesis '+str(i));grind.run('candidate','bounded attempt')
        with self.assertRaisesRegex(ValueError,'Overlapping failed interval'):
            workflow.require_eligible({**self.recipe,'id':'renamed'})
    def test_workflow_fingerprint_includes_blockers_not_build_identity(self):
        before=self.snapshot();first=workflow.fingerprint(workflow.workflow_inputs())
        write_json(self.root/'recovery/blockers.json',{'attempts':[{'name':'candidate','reason':'new'}]})
        self.assertEqual(before,self.snapshot())
        self.assertNotEqual(first,workflow.fingerprint(workflow.workflow_inputs()))
    def test_fast_pass_can_be_promoted_with_same_hypothesis(self):
        with patch.object(grind,'check',return_value={'status':'FAST_PASS_ONLY','receipt':{}}):
            grind.run('candidate','first')
        with patch.object(grind,'check',return_value={'status':'PROMOTED','fast':{}}) as check:
            self.assertEqual(grind.run('candidate','promote proven source',True)['status'],'PROMOTED')
            self.assertTrue(check.call_args.kwargs['promote'])
    def test_reopen_keeps_history_and_records_new_epoch(self):
        write_json(self.root/'recovery/restunts-inventory.json',{'functions':[{'name':'candidate','start':1,'end':3}]})
        with patch.object(grind,'check',side_effect=self.failure('BINDING_REVIEW_REQUIRED')):
            grind.run('candidate','first')
        self.assertTrue(workflow.state('candidate')['blocked'])
        grind.reopen('candidate','New independently verified binder capability')
        self.assertEqual(workflow.state('candidate')['epoch'],1)
        self.assertFalse(workflow.state('candidate')['blocked'])
        self.assertEqual(len(workflow.attempts('candidate')),1)


if __name__=='__main__':unittest.main()
