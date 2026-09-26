"""integ23: CRT startup binder forms and runtime far-code aliases."""
import copy
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT/'tools'))
from common import read_json, sha
from omf import OmfReader
from object_probe import read_object
from oracle import verify
from mz import MZ
from library import bind_library
from promote_runtime import replace_raw_library
from runtime_binding import declarations, crt_order_placements

LIB = 'toolchain/msc510/MLIBCR.LIB'
NMSGHDR = 'library_dos_nmsghdr_119038'
CRT0_RECORD_POLICY = {
    'mode':'crt0-comment-checksum-v1',
    'module_sha256':'d5b8b4a264adea82a75056189745d9d786e81192af65e9d4713e4ab0a687a973',
    'record_offset':466, 'kind':136, 'size':4,
    'body_sha256':'c6ef173229e1869cd33073657375349eaf71a2329203c009ecd0c9c7c8410aad',
    'checksum':209}
X_SEGMENTS = ['CDATA','XIFB','XIF','XIFE','XIB','XI','XIE','XPB','XP','XPE',
              'XCB','XC','XCE','XCFB','XCF','XCFE']
CFLUSH = {'library':LIB, 'module':'_cflush.asm',
          'module_sha256':'3f282e819bfa93fd9587ddd3978dc7a40695ba767652caaf32188bf01f2b7c24',
          'segment':'XP', 'length':4, 'targets':['_flushall']}
CINITTM = 'f5c04c990a9c3d54e205d78ee8bdae18bf0426d5fb38eb1cc5868b46ddfe7334'


def storage(start, end, anchor):
    return {'start':start, 'end':end, 'ownership':'proven-raw', 'anchor':anchor}


class Integ23StartupBinder(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        exe = verify(write=False)[1]; mz = MZ.parse(exe)
        cls.image = mz.load_image(exe); cls.relocs = mz.relocations
        cls.manifest = read_json(ROOT/'layout/manifest.json')
        archive = (ROOT/LIB).read_bytes()
        cls.archive_sha = sha(archive)
        cls.modules = OmfReader().split_library(archive)

    def row(self, module, start, end, binding, record_policy=None):
        blobs = [b for n,b in self.modules if n == module]
        self.assertEqual(len(blobs), 1)
        sparse = binding.get('sparse_zero')
        obj = read_object(blobs[0], record_policy=record_policy, sparse_zero=sparse)
        row = {'id':'library_'+module.replace('\\','_').split('.')[0]+f'_{start}', 'name':module,
               'classification':'RUNTIME_LIBRARY', 'kind':'KNOWN_TOOLCHAIN_LIBRARY',
               'profile':'msc510-medium', 'library':LIB, 'library_sha256':self.archive_sha,
               'module':module, 'module_sha256':sha(blobs[0]), 'segment':'_TEXT',
               'start':start, 'end':end, 'publics':obj.publics, 'externals':obj.externals,
               'expected_fixups':obj.linker_fixups,
               'binding':{'mode':'runtime-owner-group-v2', 'declarations':declarations(obj),
                          'data_bindings':{}, 'storage':{}, **copy.deepcopy(binding)}}
        if record_policy: row['record_policy'] = record_policy
        spans = [(start,end)] + [(s['start'],s['end']) for s in row['binding']['storage'].values()]
        row['expected_relocations'] = [r for r in self.relocs
                                       if any(a-1 <= r['load_offset'] < b for a,b in spans)]
        return row

    def startup(self):
        crt0 = self.row('dos\\crt0.asm', 117858, 118055, {
            'storage':{'_DATA':storage(191860,191948,{'kind':'data-alias','symbol':'_word_3ED74','offset':0}),
                       'STACK':storage(222352,224400,{'kind':'mz-stack-v1'})},
            'sparse_zero':{'_DATA':{'initialized_ranges':[[0,10],[86,88]],'declared_length':88}},
            'external_code_offsets':[{'segment':'_DATA','offset':4,'target':'__exit'},
                                     {'segment':'_TEXT','offset':175,'target':'_exit'}],
            'dosseg_boundaries':{'_edata':{'kind':'bss-class-start-v1',
                                           'after':{'owner':NMSGHDR,'segment':'EPAD'},
                                           'alignment':2,'addends':[0]},
                                 '_end':{'kind':'stack-class-start-v1','addends':[0,-2]}}},
            CRT0_RECORD_POLICY)
        dat = self.row('dos\\crt0dat.asm', 118056, 118410, {
            'storage':{'_DATA':storage(191948,192028,{'kind':'data-alias','symbol':'_word_2EDEB','offset':31}),
                       'CDATA':storage(199708,199722,{'kind':'dgroup-order-v1'})},
            'sparse_zero':{'CDATA':{'initialized_prefix':2,'declared_length':14}},
            'external_code_offsets':[{'segment':'_TEXT','offset':26,'target':'__cintDIV'}],
            'dgroup_order_proof':{'kind':'crt-dgroup-data-order-v1',
                                  'upper_neighbour':{'owner':NMSGHDR,'segment':'HDR'},
                                  'segments':X_SEGMENTS, 'intervening':[CFLUSH]}})
        fp = self.row('crt0fp.asm', 118446, 118452, {
            'storage':{'MSG':storage(199864,199903,{'kind':'unique-literal'}),
                       'PAD':storage(199973,199975,{'kind':'common-v1'})}})
        envp = self.row('dos\\stdenvp.asm', 118928, 119038, {
            'data_bindings':{'__psp':'_word_2EDEB','_environ':'_word_3EE0C'}})
        alloc = self.row('dos\\stdalloc.asm', 119124, 119190, {
            'data_bindings':{'__abrktb':'_crtsp1','__asizds':'_word_3ED74','__psp':'_word_2EDEB'}})
        return [crt0, dat, fp, envp, alloc]

    def staged(self, rows):
        manifest = copy.deepcopy(self.manifest)
        for row in rows:
            # A member published since this fixture was written is staged
            # afresh from raw ownership of the same extent.
            for index, owner in enumerate(manifest['owners']):
                if owner['id'] == row['id']:
                    self.assertEqual((owner['start'], owner['end']), (row['start'], row['end']))
                    manifest['owners'][index] = {'id':'raw_%05x_%05x' % (row['start'], row['end']),
                                                 'kind':'UNRESOLVED_RAW', 'start':row['start'],
                                                 'end':row['end']}
            manifest = replace_raw_library(manifest, row)
        return manifest

    def bind(self, row, rows):
        return bind_library(row, self.image, self.relocs, manifest=self.staged(rows))

    def test_crt0_binds_code_offsets_dosseg_bounds_and_main(self):
        rows = self.startup()
        payload, proof = self.bind(rows[0], rows)
        self.assertEqual(payload, self.image[117858:118055])
        linked = {(f['segment'],f['offset']):f for f in proof['binding']['fixups']}
        self.assertEqual(linked[('_TEXT',107)]['address'], 199994)   # _edata: after accepted EPAD
        self.assertEqual(linked[('_TEXT',110)]['address'], 222352)   # _end: MZ STACK start
        self.assertEqual(linked[('_TEXT',33)]['address'], 222350)    # _end-2
        self.assertEqual((linked[('_TEXT',152)]['address'], linked[('_TEXT',152)]['frame']), (0,0))
        self.assertEqual(linked[('_DATA',4)]['address'], 118056+219)  # __exit
        for row in rows[2:]:
            self.assertEqual(self.bind(row, rows)[0], self.image[row['start']:row['end']])

    def test_external_code_offsets_need_exact_review(self):
        rows = self.startup()
        for mutate, message in [
                (lambda b: b['external_code_offsets'].pop(), 'explicit table proof'),
                (lambda b: b['external_code_offsets'][1].update(target='__exit'), 'differs from its review'),
                (lambda b: b['external_code_offsets'].append(
                    {'segment':'_TEXT','offset':196,'target':'_exit'}), 'Unused runtime external code-offset'),
                (lambda b: b['external_code_offsets'][0].update(offset=6), 'Runtime absolute code offset')]:
            with self.subTest(message=message):
                changed = copy.deepcopy(rows)
                mutate(changed[0]['binding'])
                with self.assertRaisesRegex(ValueError, message):
                    self.bind(changed[0], changed)

    def test_dosseg_boundaries_are_independently_grounded(self):
        rows = self.startup()
        cases = [
            (lambda b: b['dosseg_boundaries']['_end']['addends'].remove(-2), 'addend is not reviewed'),
            (lambda b: b['dosseg_boundaries']['_edata']['after'].update(owner='library_crt0fp_118446'),
             'another accepted runtime owner'),
            (lambda b: b['dosseg_boundaries']['_edata']['after'].update(segment='PAD'),
             'final pinned non-BSS'),
            (lambda b: b['dosseg_boundaries']['_edata'].update(alignment=4), '_edata proof differs'),
            (lambda b: b['storage']['STACK']['anchor'].update(kind='unique-literal'), 'STACK|literal'),
            (lambda b: b['dosseg_boundaries'].pop('_edata'), 'Runtime|DGROUP|group public')]
        for mutate, message in cases:
            with self.subTest(message=message):
                changed = copy.deepcopy(rows)
                mutate(changed[0]['binding'])
                with self.assertRaisesRegex(ValueError, message):
                    self.bind(changed[0], changed)

    def test_dosseg_end_comes_from_mz_stack_header(self):
        from runtime_binding import dosseg_boundary
        row = self.row('chkstk.asm', 118452, 118488, {
            'dosseg_boundaries':{'_end':{'kind':'stack-class-start-v1','addends':[256]}}})
        obj = read_object(next(b for n,b in self.modules if n == 'chkstk.asm'))
        self.assertEqual(dosseg_boundary(row, obj, '_end', 256, {}, self.image, self.relocs), 222352+256)
        with self.assertRaisesRegex(ValueError, 'addend is not reviewed'):
            dosseg_boundary(row, obj, '_end', 0, {}, self.image, self.relocs)
        with self.assertRaisesRegex(ValueError, 'Unreviewed DOSSEG'):
            dosseg_boundary(row, obj, '_edata', 0, {}, self.image, self.relocs)
        wrong = copy.deepcopy(row); wrong['binding']['dosseg_boundaries']['_end']['kind'] = 'bss-class-start-v1'
        with self.assertRaisesRegex(ValueError, 'MZ-grounded'):
            dosseg_boundary(wrong, obj, '_end', 256, {}, self.image, self.relocs)
        crt0 = self.startup()[0]
        blob = next(b for n,b in self.modules if n == 'dos\\crt0.asm')
        crt0_obj = read_object(blob, record_policy=CRT0_RECORD_POLICY,
                               sparse_zero=crt0['binding']['sparse_zero'])
        moved = {'STACK':storage(222368,224416,{'kind':'mz-stack-v1'})}
        with self.assertRaisesRegex(ValueError, 'STACK declaration differs'):
            dosseg_boundary(crt0, crt0_obj, '_end', 0, moved, self.image, self.relocs)

    def test_crt_data_sections_follow_order_neighbour_and_intervening(self):
        rows = self.startup(); dat = rows[1]
        blob = next(b for n,b in self.modules if n == 'dos\\crt0dat.asm')
        obj = read_object(blob, sparse_zero=dat['binding']['sparse_zero'])
        manifest = self.staged(rows)
        places = crt_order_placements(dat, obj, self.image, self.relocs, manifest)
        self.assertEqual(places['CDATA'], {'start':199708, 'end':199722})
        for name in ('XIFB','XIF','XIFE','XIB','XI','XIE','XPB'):
            self.assertEqual(places[name], {'start':199722, 'end':199722})
        self.assertEqual(places['XP'], {'start':199722, 'end':199726})
        for name in ('XPE','XCB','XC','XCE','XCFB','XCF','XCFE'):
            self.assertEqual(places[name], {'start':199726, 'end':199726})
        cases = [
            (lambda p: p.update(intervening=[]), 'Unexplained relocated'),
            (lambda p: p['intervening'][0].update(targets=['__inittime']), 'far-pointer table'),
            (lambda p: p['intervening'][0].update(module='_cinittm.asm', module_sha256=CINITTM,
                                                  segment='XI', targets=['__inittime']),
             'unique verified inventory entry'),
            (lambda p: p['intervening'][0].update(segment='XI'), 'declaration differs'),
            (lambda p: p['intervening'][0].update(module_sha256='0'*64), 'missing/ambiguous'),
            (lambda p: p.update(segments=X_SEGMENTS[1:]+X_SEGMENTS[:1]), 'pinned SEGDEF order'),
            (lambda p: p['upper_neighbour'].update(owner='library_dos_crt0msg_118410', segment='MSG'),
             'order|far pointer|Unexplained'),
            (lambda p: p['upper_neighbour'].update(owner='library_crt0fp_118446', segment='MSG'),
             'another accepted runtime owner'),
            (lambda p: p['upper_neighbour'].update(segment='EPAD'), 'independently anchored')]
        for mutate, message in cases:
            with self.subTest(message=message):
                changed = copy.deepcopy(dat)
                mutate(changed['binding']['dgroup_order_proof'])
                with self.assertRaisesRegex(ValueError, message):
                    crt_order_placements(changed, obj, self.image, self.relocs, manifest)
        moved = copy.deepcopy(rows)
        moved[1]['binding']['storage']['CDATA'].update(start=199706, end=199720)
        with self.assertRaisesRegex(ValueError, 'DGROUP-order placement differs'):
            self.bind(moved[1], moved)

    def test_crt0dat_bss_sections_fail_closed_without_neighbour(self):
        rows = self.startup()
        with self.assertRaisesRegex(ValueError, 'lacks independently grounded placement: XOB'):
            self.bind(rows[1], rows)

    def fflush(self, aliases=('_write',)):
        row = self.row('fflush.c', 120142, 120254, {'raw_file_publics':['__iob','__iob2']})
        if aliases is not None: row['binding']['code_aliases'] = list(aliases)
        return row

    def test_runtime_far_code_alias_into_raw_code(self):
        row = self.fflush()
        payload, proof = self.bind(row, [row])
        self.assertEqual(payload, self.image[120142:120254])
        call = next(f for f in proof['binding']['fixups'] if f['target'] == '_write')
        self.assertEqual((call['address'], call['frame']), (122664, 117840))
        for aliases, message in [(None, 'group public missing'),
                                 (('_write','_fflush'), 'Unused runtime code-alias'),
                                 (('_write','_no_such_alias'), 'Unknown far code symbol|Unused')]:
            with self.subTest(aliases=aliases):
                bad = self.fflush(aliases)
                with self.assertRaisesRegex(ValueError, message):
                    self.bind(bad, [bad])
        member = self.fflush(); member['binding']['mode'] = 'runtime-member-v1'
        with self.assertRaisesRegex(ValueError, 'code-alias review'):
            self.bind(member, [member])


if __name__ == '__main__':
    unittest.main()
