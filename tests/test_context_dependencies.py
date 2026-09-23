"""Dependency separation is a freshness optimization, never a mutation waiver."""
import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch
ROOT=Path(__file__).resolve().parents[1];sys.path.insert(0,str(ROOT/'tools'))
from common import write_json,read_json
import build_exact
import workflow


class DependenciesTests(unittest.TestCase):
    def test_unrelated_candidate_does_not_invalidate_production(self):
        with tempfile.TemporaryDirectory(dir=ROOT/'build') as d:
            root=Path(d)
            for folder in ['tools','src','asm','include','recipes','layout','recovery/candidates']:(root/folder).mkdir(parents=True)
            manifest={'owners':[{'kind':'MATCHING_C','recipe':'recipes/active.json'}]}
            write_json(root/'layout/manifest.json',manifest)
            write_json(root/'recipes/active.json',{'source':'src/active.c'})
            (root/'src/active.c').write_text('accepted');(root/'recovery/candidates/other.c').write_text('old')
            with patch('build_exact.ROOT',root):
                first=build_exact.production_inputs();broad=build_exact.inputs()
                (root/'recovery/candidates/other.c').write_text('new')
                write_json(root/'recipes/inactive.json',{'anything':'new'})
                self.assertEqual(first,build_exact.production_inputs());self.assertNotEqual(broad,build_exact.inputs())
                for name in ['include/a.h','layout/evidence.json','src/active.c','tools/binder.py']:
                    before=build_exact.production_inputs();(root/name).write_text('changed')
                    self.assertNotEqual(before,build_exact.production_inputs())
                override={'recipes/active.json':{'source':'recovery/candidates/other.c'}}
                self.assertIn('recovery/candidates/other.c',build_exact.production_inputs(manifest,override))

    def test_shared_snapshot_detects_mutation_and_path_escape(self):
        with tempfile.TemporaryDirectory(dir=ROOT/'build') as d:
            root=Path(d)
            with patch('workflow.ROOT',root),patch('workflow.project_path',side_effect=lambda p:(root/p).resolve()):
                ref=workflow.save_snapshot({'tools/x':'hash'})
                self.assertEqual(workflow.load_snapshot(ref),{'tools/x':'hash'})
                (root/ref['path']).write_text('{}')
                with self.assertRaises(ValueError):workflow.load_snapshot(ref)
                with self.assertRaises(ValueError):workflow.load_snapshot({'path':'../outside','sha256':'x'})

    def test_headers_and_symbol_evidence_change_hypothesis_identity(self):
        recipe={'source':'recovery/candidates/f.c','profile':'fixture'}
        snap={'tools/x.py':'a','include/types.h':'b','layout/code-symbols.json':'c','recovery/candidates/other.c':'d'}
        with patch('workflow.inputs',return_value=snap):first=workflow.hypothesis_key(recipe,b'source')
        for name in ['include/types.h','layout/code-symbols.json']:
            with patch('workflow.inputs',return_value={**snap,name:'changed'}):
                self.assertNotEqual(first,workflow.hypothesis_key(recipe,b'source'))
        with patch('workflow.inputs',return_value={**snap,'recovery/candidates/other.c':'changed'}):
            self.assertEqual(first,workflow.hypothesis_key(recipe,b'source'))

    def test_changed_selected_evidence_fails(self):
        from oracle import verify
        from mz import MZ
        from function_evidence import reviewed_functions
        result=verify(write=False);image=MZ.parse(result[1]).load_image(result[1])
        self.assertIn('is_facing_camera',reviewed_functions(image))
        data=read_json(ROOT/'layout/function-evidence.json');data['functions'][0]['padding_offsets']=[]
        with patch('function_evidence.read_json',return_value=data),self.assertRaises(ValueError):reviewed_functions(image)

    def test_unequal_extent_has_diagnostic_and_is_still_failure(self):
        from probe_module import probe,ProbeFailure
        recipe=read_json(ROOT/'recipes/is_facing_camera.json')
        # A deliberately undersized fixture, independent of the current hypothesis.
        with tempfile.TemporaryDirectory(dir=ROOT/'build') as d:
            source=Path(d)/'fixture.c';source.write_text('int is_facing_camera(void) { return 0; }\n')
            recipe['source']=source.relative_to(ROOT).as_posix()
            with self.assertRaises(ProbeFailure) as caught:probe(recipe)
        self.assertEqual(caught.exception.details['category'],'EXTENT_MISMATCH')
        self.assertIn('first_differing_instruction',caught.exception.details['comparison'])

    def test_control_mutation_during_fast_cannot_receive_pass(self):
        import check_candidate
        from contextlib import ExitStack
        recipe={'source':'unused'}
        with ExitStack() as stack:
            stack.enter_context(patch('check_candidate.read_json',side_effect=[recipe,{'tier':'CHEAP'}]))
            stack.enter_context(patch('check_candidate.check_scope'))
            stack.enter_context(patch('check_candidate.check_workflow_scope'))
            stack.enter_context(patch('check_candidate.inputs',return_value={}))
            stack.enter_context(patch('check_candidate.control_inputs',side_effect=[{'block':'old'},{'block':'changed'}]))
            stack.enter_context(patch('check_candidate.probe',return_value=(b'x',{})))
            with self.assertRaisesRegex(ValueError,'controls changed during FAST'):check_candidate.check('fixture')

    def test_existing_writer_lock_refuses_compilation(self):
        import grind
        with tempfile.TemporaryDirectory(dir=ROOT/'build') as d:
            root=Path(d);(root/'build').mkdir();(root/'build/grind.lock').write_text('active writer')
            recipe={'id':'fixture','source':'recovery/candidates/f.c'}
            with patch('grind.ROOT',root),patch('grind.read_json',return_value=recipe),\
                 patch('grind.require_eligible',return_value={'history':[]}),\
                 patch('grind.project_path',return_value=root/'build/grind.lock'),\
                 patch('grind.hypothesis_key',return_value='key'),patch('grind.check') as compile_check:
                with self.assertRaises(FileExistsError):grind.run('fixture','test writer exclusion')
                compile_check.assert_not_called()
                self.assertEqual((root/'build/grind.lock').read_text(),'active writer')


if __name__=='__main__':unittest.main()
