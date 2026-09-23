"""Dangerous grouping controls and whole-probe acceptance independence."""
import sys
import unittest
from pathlib import Path
from unittest.mock import patch
ROOT=Path(__file__).resolve().parents[1];sys.path.insert(0,str(ROOT/'tools'))
from diagnostics import compare_streams,compact,format_summary
import test_diagnostics


def result(a,b,**kwargs):return compare_streams(bytes.fromhex(a),bytes.fromhex(b),**kwargs)
def supported(report):return [f for f in report['patterns']['families'] if f['state']=='SUPPORTED_GROUPING']


class PatternTests(unittest.TestCase):
    def test_cross_island_mapping_and_all_observations_traceable(self):
        r=result('558bec8946fc40bb010083c302894efcc3','558bec8946fe40bb010083c302894efec3')
        f=supported(r)[0];self.assertEqual(f['kind'],'BP_DISPLACEMENT')
        self.assertGreater(len(f['islands']),1);self.assertEqual(f['contradictions'],[])
        p=r['patterns'];ids={o['id'] for o in p['observations']}
        covered={s for f in supported(r) for s in f['sites']}|set(p['residuals']+p['corresponding_branches']+p['unresolved_fixups'])
        self.assertEqual(ids,covered)
        self.assertEqual(compact(compact(r))['patterns'],compact(r)['patterns'])

    def test_inconsistent_mapping_and_unchanged_reused_home_reject_family(self):
        for candidate in ['8946fe894efac3','8946fe894efe8b46fcc3']:
            target='8946fc894efcc3' if len(candidate)==14 else '8946fc894efc8b46fcc3'
            r=result(target,candidate)
            self.assertFalse(supported(r))
            self.assertTrue(r['patterns']['families'][0]['contradictions'])
            self.assertEqual(len(r['patterns']['residuals']),len(r['patterns']['observations']))

    def test_segment_and_frame_registers_not_allocation(self):
        for a,b in [('8cd88cd8c3','8cc08cc0c3'),('8bc48bc4c3','8bc58bc5c3')]:
            r=result(a,b);self.assertFalse(supported(r))
            classes={c['class'] for island in r['islands'] for c in island['classifications']}
            self.assertNotIn('REGISTER_ALLOCATION',classes)
            self.assertIn('CALL_FRAME_OR_SEGMENT_REGISTER',classes)

    def test_positive_bp_is_only_displacement_not_parameter_or_local(self):
        r=result('8b46068b4e06c3','8b46088b4e08c3')
        f=supported(r)[0];self.assertEqual(f['kind'],'BP_DISPLACEMENT')
        self.assertIn('not labelled locals or parameters',f['limit'])

    def test_wrong_immediate_and_width_remain_residual(self):
        for a,b in [('b80100b90100c3','b80200b90200c3'),('8a46fc8a4efcc3','8b46fe8b4efec3')]:
            r=result(a,b);self.assertFalse(supported(r));self.assertEqual(len(r['patterns']['residuals']),2)

    def test_wrong_symbol_remains_explicit_even_with_equal_numeric_bytes(self):
        r=result('9a00000000c3','9a00000000c3',fixups=[{'offset':1,'width':4,'target':'WRONG_SYMBOL'}])
        p=r['patterns'];self.assertFalse(supported(r));self.assertEqual(len(p['unresolved_fixups']),1)
        self.assertEqual(p['observations'][0]['unresolved_fixups'][0]['target'],'WRONG_SYMBOL')
        self.assertEqual(r['counts']['fixup_normalized_pairs'],1)

    def test_same_encoding_different_destination_is_separate_from_bytes(self):
        # Insert INC before the identical branch but leave its destination block unmoved.
        r=result('eb029090b80100c3','40eb0290b80100c3')
        branches=[o for o in r['patterns']['observations'] if o.get('branch')]
        self.assertTrue(any(o['byte_equal'] and o['branch']['state']!='CORRESPONDING_DESTINATION' for o in branches))

    def test_genuinely_different_anchored_branch_target(self):
        r=result('740090b8010040c3','740190b8010040c3')
        branch=next(o['branch'] for o in r['patterns']['observations'] if o.get('branch'))
        self.assertEqual(branch['state'],'DIFFERENT_ALIGNED_DESTINATION')

    def test_structural_destination_not_high_confidence(self):
        r=result('7400b80100c3','7400b80200c3')
        # Same position/encoding is no new operand difference, but changing its
        # displacement to a structurally paired destination cannot be strong proof.
        r=result('7400b80100c3','740140b80200c3')
        branch=next(o['branch'] for o in r['patterns']['observations'] if o.get('branch'))
        self.assertEqual(branch['state'],'ALIGNED_DESTINATION_UNCERTAIN')
        self.assertEqual(branch['confidence'],'low')

    def test_implicit_register_effects_and_segment_override_retained(self):
        r=result('f7e3c3','f7e6c3')
        self.assertIn('ax',r['target_instructions'][0]['implicit_reads'])
        self.assertIn('dx',r['target_instructions'][0]['implicit_writes'])
        r=result('268b4600268b4600c3','3e8b46003e8b4600c3')
        self.assertFalse(supported(r));self.assertTrue(r['patterns']['residuals'])
        r=result('8bd88bd8d7c3','8bf08bf0d7c3')
        self.assertFalse(supported(r))  # unchanged XLAT implicitly consumes BX
        self.assertTrue(r['patterns']['families'][0]['contradictions'])

    def test_repeated_code_and_incomplete_decode_do_not_create_family(self):
        r=result('b801005dc3b802005dc3','b803005dc3')
        self.assertFalse(supported(r));self.assertIsNone(r['exact_suffix'])
        r=result('8946fc894efc0f','8946fe894efe0f')
        self.assertFalse(r['decode_complete']);self.assertFalse(supported(r))
        self.assertIn('undecoded',r['patterns']['residuals'])

    def test_disabled_or_misleading_diagnostics_cannot_accept(self):
        fixture=test_diagnostics.IntegrationTests()
        for output in ({},{'match_summary':{'fake_similarity':1,'acceptance':'PASS'}}):
            with patch('diagnostics.diagnose',return_value=output):
                for candidate,category in [(b'\xc3','EXTENT_MISMATCH'),(b'\x53\xc3','BYTE_MISMATCH')]:
                    details,_=fixture.probe_fixture(b'\x55\xc3',candidate)
                    self.assertEqual(details['category'],category)
        self.assertIn('unavailable',format_summary(None).lower())


if __name__=='__main__':unittest.main()
