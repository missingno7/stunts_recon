"""Address-grounded historical-function to current-C candidate inventory.

This is a scratch helper for semantic-audit integration. It does not certify
that a same-named implementation is equivalent. A registry alias is joined
only at the historical row's exact start address; absent that, a direct name
hit is reported as name-only evidence. Multiple definitions remain ambiguous.
"""
from __future__ import annotations

import json
import ast
import re
import sys
from collections import defaultdict
from pathlib import Path


# Explicit host additions, not guesses based on absence from the symbol map.
# Each policy names observable state and the ordering obligation it introduces.
HOST_POLICIES = {
    'config.c': ('User-selected startup settings; original has no JSON/config service.',
                 'frame rate, detail, renderer and protection policy', 'apply settings at startup before the guest uses them'),
    'diagnostics.c': ('Host crash/session reporting; no DOS game replacement.',
                     'host files and stop status', 'report failures without advancing guest clocks'),
    'trace.c': ('Host observation serialization added around guest events.',
                'trace output only', 'observation must not insert guest updates'),
    'trace_hooks.c': ('Host tracing callbacks added to generated game code.',
                     'trace output and optional test controls', 'preserve the wrapped guest call order'),
    'test_seed.c': ('Opt-in original-state fixture injection.',
                   'declared seed/state fixture', 'inject only at the named test boundary'),
    'input_script.c': ('Host scripted-input delivery, absent from the DOS executable.',
                      'queued input and test checkpoints', 'apply at the declared guest input boundary'),
    'boundary_record.c': ('Host bounded-call observer, absent from the DOS executable.',
                         'capture buffers and optional test stop', 'record calls without an extra guest call'),
    'sound_record.c': ('Host sound-request/resource observer.',
                      'capture identity tables and files', 'preserve driver-request consumption order'),
    'audio_sdl.c': ('SDL audio device/queue sink replaces physical output hardware.',
                   'host PCM queue and device state', 'device callback consumes PCM; guest ticks advance driver state'),
    'sdl_host.c': ('SDL event/window/thread boundary, absent from the DOS executable.',
                  'host events, focus and presentation', 'queue input before granting the next guest tick'),
    'main.c': ('Native process lifecycle wraps the retained guest main.',
               'startup configuration, host resources and stop status', 'start guest once; wait/grant/present in documented order'),
    'startup.c': ('Host entropy/config startup policy replaces timing-derived entropy.',
                  'prescribed or OS seed and configured startup values', 'set once before guest startup; fixture injection remains explicit'),
    'tick_clock.c': ('Host tick scheduling replaces PIC/PIT interrupt delivery.',
                     'callback counter, elapsed counter and callback order', 'advance/dispatch once per grant; preserve original byte guard'),
    'port_opl3.c': ('Host OPL synthesizer interface replaces physical AdLib hardware.',
                    'OPL register/output state', 'register writes precede rendering the corresponding PCM'),
    'pc_speaker.c': ('Host PIT/speaker synthesizer replaces physical speaker hardware.',
                     'PIT/gate/sample phase', 'driver writes precede rendering the corresponding PCM'),
}


HELPER_ORIGINALS = {
    'dos_83_filename': ['findfirst', 'findnext'],
    'dos_case_equal': ['findfirst', 'loadfileat'],
    'dos_pattern_match': ['findfirst', 'findnext'],
    'enumerate_matching_entry': ['findfirst', 'findnext'],
    'find_case_component': ['findfirst', 'loadfileat'],
    'saved_file_shadows': ['findfirst', 'findnext'],
    'port_call_read_line': ['instringz'],
    'port_parse_shape2d_helper3': ['parse_shape2d_helper3'],
    'port_random_test_rand': ['rand'],
    'port_fs_seek': ['lseek'], 'port_fs_exists': ['filesizez'],
    'mmgr_alloc_farmem': ['mmgr_alloc_resmem'], 'port_far_add': ['loadfileat', 'savefilez'],
    'port_audio_dispatch': ['audio_init_chunk', 'process_audio_chunk_event'],
    'file_load_binary_nofatal_thunk': ['loadfilez'],
    'sprite_copy_2_to_1_2': ['displaywindow'],
    'sprite_set_1_from_argptr': ['sprite_setup1_from_arg_pointer'],
    'stunts_div_u32_u16': ['audio_op_unk2'],
    'stunts_port_audio_init_chunk_7_view': ['audio_init_chunk'],
    'stunts_port_call_read_line_view': ['instringz'],
    'stunts_port_locate_shape_fatal_shape_view': ['locate_shape_fatal'],
    'stunts_port_read_file_with_retry_view': ['read_file_with_retry'],
    'stunts_port_send_audio_stop_event_value_view': ['send_audio_stop_event'],
    'stunts_port_timer_copy_counter_split_view': ['setticks'],
}
ADAPTER_SOURCES = {
    'adapt_renderer_word_arithmetic': ['game/polygon.c'],
    'adapt_preview_word_arithmetic': ['game/render.c'],
    'route_audio_vectors': ['lib/audiores.c', 'lib/audioply.c'],
    'adapt_aggregate_views': ['lib/audiores.c', 'lib/audioply.c', 'game/audioeng.c', 'game/trkscene.c', 'game/polygon.c'],
    'adapt_polygon_storage': ['game/polygon.c'], 'replace_seg007_asm': ['game/audioeng.c'],
}


def int86_call_sites(root=None):
    """Every retained source call, with a conservative scalar-use decision."""
    from semantic_audit import functions, mask_comments
    root = _root(root)
    rows = []
    for path in sorted(list((root / 'game').glob('*.c')) + list((root / 'lib').glob('*.c'))):
        text = path.read_text(encoding='latin-1')
        for name, fn in functions(text).items():
            for match in re.finditer(r'\bint86\s*\(', mask_comments(fn['body'])):
                line_start = fn['body'].rfind('\n', 0, match.start()) + 1
                prefix = fn['body'][line_start:match.start()].strip()
                rows.append(dict(source=path.relative_to(root).as_posix(), function=name,
                    line=fn['start_line'] + fn['body'].count('\n', 0, match.start()),
                    scalar_use='discarded' if not prefix else 'requires-review',
                    statement=fn['body'][line_start:fn['body'].find('\n', match.start())].strip()))
    return rows


def port_correspondence(root, layer, semantic, overlays=()):
    """Map helpers through actual callers/expansions; classify explicit additions.

    A caller scope is not an isolated-function equivalence certificate. Ambiguous
    rewrites remain unresolved instead of receiving an arbitrary original range.
    """
    from semantic_audit import functions, mask_comments
    root = _root(root)
    manifest = json.loads((root / 'layout/manifest.json').read_text())
    asm = json.loads((root / 'docs/porting/asm-migration.json').read_text())['routines']
    owners = {r['id']: r for r in manifest['owners']}
    by_name, by_source, by_address = defaultdict(list), defaultdict(list), {}
    def add(name, target, source=None):
        if not name: return
        accepted = by_address.get(target['start'])
        if accepted and accepted['end'] == target['end'] and accepted.get('owner') and not target.get('owner'):
            target = dict(target, **accepted)
        if target not in by_name[name]: by_name[name].append(target)
        if source and target not in by_source[source]: by_source[source].append(target)
        by_address[target['start']] = target
    for owner in manifest['owners']:
        if not owner.get('recipe'): continue
        recipe = json.loads((root / owner['recipe']).read_text())
        source = recipe.get('source')
        members = recipe.get('members', [])
        for member in members:
            start = member.get('start')
            if not isinstance(start, int): continue
            end = member.get('end')
            if not isinstance(end, int):
                end = min([owner['end']] + [m['start'] for m in members
                          if isinstance(m.get('start'), int) and m['start'] > start])
            if not owner['start'] <= start < end <= owner['end']: continue
            target = dict(start=start, end=end, owner=owner['id'],
                          basis='accepted recipe member/source interval', source=source,
                          original_name=member.get('name'), public=member.get('public'))
            for name in (member.get('name'), member.get('public'), (member.get('public') or '').lstrip('_')):
                add(name, target, source)
    for row in asm:
        ex = public_extent(row, owners, root)
        if ex:
            target = dict(ex, owner=row['owner_id'], original_name=row['routine'], public=row['public_symbol'])
            for name in (row['routine'], row['public_symbol'], row['public_symbol'].lstrip('_')):
                add(name, target)
    registry = json.loads((root / 'layout/names-registry.json').read_text())['names']
    for address, entry in registry.items():
        address = int(address)
        if entry.get('kind') != 'code': continue
        if address in by_address:
            add(entry.get('name'), by_address[address])
            add(entry.get('inventory_name'), by_address[address])
    for h in semantic['historical_mapping']:
        if not isinstance(h.get('address'), int) or not isinstance(h.get('end'), int): continue
        target = dict(start=h['address'], end=h['end'], basis='reviewed historical function inventory')
        for name in (h.get('original_name'), h.get('canonical_name'), h.get('registry_inventory_name')):
            add(name, target)

    overlay_paths = {r['source']: Path(r['overlay']) for r in overlays}
    defs, names = {}, defaultdict(list)
    paths = (list((root / 'port').rglob('*.c')) + list((root / 'port').rglob('*.h')) +
             list((root / 'tools/porting/port_include').rglob('*.h')) +
             list((root / 'tools/porting/host').rglob('*.h')))
    for path in paths:
        source = path.relative_to(root).as_posix()
        for name, fn in functions(path.read_text(encoding='latin-1')).items():
            key = (source, name)
            defs[key] = fn
            names[name].append(key)
    for source, path in overlay_paths.items():
        for name, fn in functions(path.read_text(encoding='latin-1')).items():
            key = (source, name)
            defs[key] = fn
            names[name].append(key)
    calls = defaultdict(set)
    for key, fn in defs.items():
        code = mask_comments(fn['body'])
        for name in set(re.findall(r'\b([A-Za-z_]\w*)\s*\(', code)):
            if (key[0], name) in defs and name != key[1]: calls[key].add((key[0], name))
            elif len(names[name]) == 1 and names[name][0] != key: calls[key].add(names[name][0])
        for name in re.findall(r'[,(&]\s*&?\s*([A-Za-z_]\w*)\s*[,)]', code):
            if (key[0], name) in defs and name != key[1]: calls[key].add((key[0], name))
    roots = defaultdict(list)
    for (source, name) in defs:
        match = re.match(r'(ad15_driver|pc15_entry|port_audio_driver)_([0-9a-f]{2})(?:_unlocked)?$', name)
        if match:
            drivers = ['AD15', 'PC15'] if match[1] == 'port_audio_driver' else [
                'AD15' if match[1] == 'ad15_driver' else 'PC15']
            for driver in drivers:
                asset = 'assets/' + driver + '.DRV'
                roots[source, name].append(dict(asset=asset, start=0, end=(root / asset).stat().st_size,
                    entry=int(match[2], 16), basis='original driver entry/caller and complete driver-image diagnostic scope'))
    for row in layer['rows']:
        candidates = by_name.get(row['name'], [])
        if row.get('original_address') in by_address:
            candidates = [by_address[row['original_address']]]
        for p in row.get('portable', []):
            if row['kind'] in ('asm', 'adapted_c', 'host'):
                roots[p['source'], p['name']].extend(candidates)
    # Also recover renamed historical C roots from the production overlay join.
    for fn in semantic.get('functions', []):
        if not fn.get('historical_source'): continue
        candidates = by_name.get(fn['historical_name'], [])
        for p in fn['portable_counterparts']:
            roots[p['source'], p['name']].extend(candidates)
    for key in defs:
        if roots[key] or Path(key[0]).name in HOST_POLICIES or key[0].startswith('port/render/'):
            continue
        candidates = by_name.get(key[1], [])
        if len({(t.get('asset'), t['start'], t['end']) for t in candidates}) == 1:
            roots[key].extend(candidates)
    inherited = defaultdict(dict)
    for parent, targets in roots.items():
        todo, visited = [parent], set()
        while todo:
            key = todo.pop()
            if key in visited: continue
            visited.add(key)
            for t in targets:
                inherited[key][t['start'], t['end']] = dict(t, caller_source=parent[0], caller=parent[1])
            todo.extend(calls[key] - visited)

    original_bodies = []
    for source in by_source:
        path = root / source
        if path.suffix == '.c' and path.is_file():
            for name, fn in functions(path.read_text(encoding='latin-1')).items():
                original_bodies.append((source, name, fn['body']))
    decisions = {}
    for row in layer['rows']:
        targets, reason, host_policy = [], '', None
        direct = by_name.get(row['name'], [])
        if row.get('original_address') in by_address:
            direct = [by_address[row['original_address']]]
        if row['kind'] in ('asm', 'host', 'adapted_c'):
            targets.extend(direct)
        for p in row.get('portable', []):
            source, name = p['source'], p['name']
            if row['kind'] == 'native_helper' and (source, name) not in defs:
                continue
            policy = HOST_POLICIES.get(Path(source).name) if source.startswith('port/') else None
            if source.startswith('port/render/') and source.endswith('.c'):
                policy = ('Enhanced rendering is an added host renderer, without a DOS entry counterpart.',
                          'native command buffers/viewport; faithful indexed path remains available',
                          'native rendering consumes guest scene snapshots; preserve faithful simulation/publication')
            if source in ('port/render/viewport.h', 'port/render/pattern.h'):
                policy = ('Host presentation/expanded-view helper with no separate original entry.',
                          'viewport, input coordinates or pattern phase', 'preserve the faithful logical grid and original pattern phase')
            if source == 'port/video.c' and name.startswith(('port_video_', 'dump_')):
                policy = ('Host SDL presentation/capture lifecycle, not a guest raster replacement.',
                          'published framebuffer, palette and host texture state', 'publish completed guest frames without an extra guest update')
            if source == 'port/input.c' and name.startswith(('port_input_init', 'port_input_shutdown',
                    'port_input_queue_', 'port_input_set_queue_', 'port_input_enable_test_',
                    'port_input_type_test_', 'port_input_test_')):
                policy = ('Host input lifecycle/queue/test control, absent from the DOS entry points.',
                          'keyboard/mouse queues and optional test input', 'deliver queued input before the granted tick')
            if source == 'port/input.c' and name == 'test_text_scan':
                policy = ('Host test text-to-scan conversion; no guest routine counterpart.',
                          'opt-in scripted keyboard input', 'enqueue bytes at the declared test input boundary')
            if source == 'port/projection.c' and name == 'port_projection_snapshot':
                policy = ('Host renderer snapshot of guest projection words.',
                          'copied render command state; no write to projection words', 'snapshot at the render boundary without advancing the guest')
            if source == 'port/random.c' and name == 'port_random_test_set_state':
                policy = ('Host explicit/OS startup seed injection.',
                          'MSC-compatible generator state', 'set the prescribed state before guest startup')
            if source == 'port/memory.c' and name in ('port_memory_init', 'port_memory_stats'):
                policy = ('Host allocation address-space lifecycle/diagnostics.',
                          'guest address arena and allocation counters', 'initialize before guest startup; observation does not allocate guest memory')
            if source == 'port/file.c' and name.startswith('port_runtime_'):
                policy = ('Host asset/save-root configuration around DOS relative filenames.',
                          'virtual asset/save current directory', 'configure roots before guest I/O; preserve immutable assets')
            if source == 'port/audio.c' and (name in ('ad15_write_register', 'record_request', 'port_audio_backend_name',
                    'port_audio_driver_entry_33_supported', 'port_audio_silent_load', 'port_audio_is_active',
                    'port_audio_is_silent') or name.startswith('port_audio_pc15_')):
                policy = ('Host sound-device bridge, capability setting or diagnostic counter.',
                          'device selection, PCM/register sink or capture counters', 'do not advance driver state from observation/device callbacks')
            if policy and not targets:
                host_policy = policy
                continue
            if not targets and len(direct) == 1:
                targets.extend(direct)
            if source == 'port/timer.c' and name.startswith('port_') and not targets:
                host_policy = ('Host tick handshake/barrier added around the guest.',
                               'input grant and callback timing', 'one acknowledged grant per guest yield')
                continue
            if not targets:
                targets.extend(inherited.get((source, name), {}).values())
            if not targets:
                for target_name in HELPER_ORIGINALS.get(name, []): targets.extend(by_name.get(target_name, []))
            if not targets and len(calls.get((source, name), ())) == 1:
                for callee in calls[source, name]: targets.extend(roots.get(callee, []))
        if row['kind'] == 'semantic_contract':
            for name in row.get('routines', []): targets.extend(by_name.get(name, []))
            for name in row.get('routines', []):
                for key in names.get(name, []): targets.extend(inherited.get(key, {}).values())
            assets = {'pc15-sample-trace': ['PC15'],
                      'high-risk-uncovered-driver-and-timer': ['MT15', 'TD15']}
            for driver in assets.get(row['name'], []):
                asset = 'assets/' + driver + '.DRV'
                targets.append(dict(asset=asset, start=0, end=(root / asset).stat().st_size,
                    basis='original driver image for explicitly registered contract'))
            if row['name'] == 'audio-opl-wrapper':
                host_policy = HOST_POLICIES['port_opl3.c']
            if row['name'] == 'vga-raster-status-model':
                targets.extend(by_name.get('vsyncstate', []))
            if row['name'] == 'original-replay-goldens':
                targets.extend(by_name.get('replay', []) + by_name.get('movesim', []))
            if row['name'] == 'evaluation-track-byte-equality':
                targets.append(dict(start=0x399d, end=0x39a0,
                    basis='registered locked CMP ES:[BX+DI],AL instruction window'))
        if row['kind'] == 'build_adapter' and row.get('portable'):
            p = row['portable'][0]
            source = p['source']
            text = (root / source).read_text(encoding='utf-8')
            tree = ast.parse(text)
            fn = next((n for n in ast.walk(tree) if isinstance(n, ast.FunctionDef) and
                       n.name == row['name'] and n.lineno == row.get('function_line', p['line'])), None)
            if fn is None:
                fn = next((n for n in ast.walk(tree) if isinstance(n, ast.FunctionDef) and n.name == row['name']), None)
            strings = [n.value for n in ast.walk(fn) if isinstance(n, ast.Constant) and isinstance(n.value, str)] if fn else []
            for outer in ast.walk(tree):
                if isinstance(outer, ast.FunctionDef) and outer.lineno <= p['line'] <= outer.end_lineno:
                    for csource in ADAPTER_SOURCES.get(outer.name, []): targets.extend(by_source.get(csource, []))
            if source.startswith('tools/porting/'):
                if row['name'] in ('commentless', 'function_headers', '_masked', 'mask', 'source_parser_text', 'norm_proto'):
                    host_policy = ('Build-time lexical/declaration scanner; no emitted guest instruction counterpart.',
                                   'no runtime state; parsed declarations only', 'scanner operates before compiling the guest overlay')
                else:
                    for csource, ts in by_source.items():
                        if csource and csource.endswith('.c'): targets.extend(ts)
            if source == 'port/build.py' and row['name'] == 'run':
                host_policy = ('Host compiler/process command formatting; no guest source replacement.',
                               'build output only', 'compile/link before executing the guest')
            # Exact original function names and complete old-source anchors.
            for s in strings:
                if s in by_name: targets.extend(by_name[s])
                for called in re.findall(r'\b([A-Za-z_]\w*)\\?\(', s):
                    targets.extend(by_name.get(called, []))
                if len(s.strip()) >= 24:
                    for csource, name, body in original_bodies:
                        if s in body: targets.extend(by_name.get(name, []))
            if not targets and any(x in source for x in ('boundary_adapter', 'sound_adapter')):
                host_policy = ('Build-time observation wrapper, not a DOS implementation.',
                               'recording hooks and optional stop controls', 'preserve the wrapped call and argument evaluation order')
            if not targets:
                for s in strings:
                    if s.endswith('.c') and '\n' not in s:
                        for csource, ts in by_source.items():
                            if Path(csource).name == Path(s).name: targets.extend(ts)
        if row['kind'] == 'semantic_macro':
            name = row['name'].lstrip('_')
            for key, fn in defs.items():
                if re.search(r'\b' + re.escape(row['name']) + r'\b', mask_comments(fn['body'])):
                    targets.extend(inherited.get(key, {}).values())
            for source, fname, body in original_bodies:
                if re.search(r'\b' + re.escape(name) + r'\b', mask_comments(body), re.I):
                    targets.extend(by_name.get(fname, []))
            if not targets:
                host_policy = ('Compile-time compatibility/observation definition with no retained guest expansion.',
                               'no standalone guest storage', 'no standalone runtime call; inspect expansion users before changing the macro')
        if row['kind'] == 'semantic_data' or row['name'] == 'msc-ctype-byte-data':
            symbol = json.loads((root / 'layout/data-symbols.json').read_text())['symbols']['__ctype']
            targets.append(dict(start=symbol['load_address'], end=symbol['load_address'] + 257,
                                data=True, basis='locked MSC 257-byte ctype table'))
        if targets:
            unique = {(t.get('asset'), t['start'], t['end']): t for t in targets}
            decisions[row['id']] = dict(classification='mapped', targets=list(unique.values()),
                reason='Original public/member or actual caller/expansion scope; scope is not an isolated equivalence proof.')
        elif host_policy:
            decisions[row['id']] = dict(classification='host-only', targets=[], reason=host_policy[0],
                class4=host_policy[1], class6=host_policy[2])
        else:
            decisions[row['id']] = dict(classification='unmapped', targets=[],
                reason='No accepted entry, resolvable caller/expansion, or explicit host-addition policy for ' + row['id'])
    for row in layer['rows']:
        decision = decisions[row['id']]
        if decision['classification'] != 'unmapped' or row['kind'] != 'build_adapter' or not row.get('portable'): continue
        source = row['portable'][0]['source']
        candidates = [t for other in layer['rows'] if other['kind'] == 'build_adapter' and other.get('portable') and
            other['portable'][0]['source'] == source and other['id'] != row['id']
            for t in decisions[other['id']]['targets']]
        if candidates:
            unique = {(t.get('asset'), t['start'], t['end']): t for t in candidates}
            decision.update(classification='mapped', targets=list(unique.values()),
                            reason='Shared rewrite helper; original scopes of concrete adapters in this module.')
    return decisions


def public_extent(row, owners, root=None):
    """Resolve a public entry from accepted member placement, never a null join.

    Runtime inventory rows have no target_extent. Their pinned OMF public
    offsets and the accepted owner's placement still identify the entry. The
    next public is an interval bound, not a control-flow/function boundary.
    """
    if row.get("target_extent"):
        return dict(row["target_extent"], basis="asm-migration target extent")
    owner = owners.get(row.get("owner_id"), {})
    public = row.get("module_entry_offset") or {}
    declarations = owner.get("binding", {}).get("declarations", {})
    if not declarations.get('publics') and owner.get('publics'):
        declarations = dict(declarations, publics=owner['publics'])
    if not public and owner.get("recipe"):
        recipe = json.loads((_root(root) / owner['recipe']).read_text(encoding='utf-8'))
        declarations = recipe.get('object_declarations', {})
        names = {row.get('public_symbol')}
        names.update(m.get('public') for m in recipe.get('members', [])
                     if m.get('name') == row.get('routine'))
        matches = [p for p in declarations.get('publics', [])
                   if (p.get('name') in names or p.get('name', '').lstrip('_') ==
                       row.get('public_symbol', '').lstrip('_')) and
                   p.get('segment') == recipe.get('object_segment')]
        if len(matches) == 1:
            public = matches[0]
    offset = public.get("offset")
    start, end = owner.get("start"), owner.get("end")
    if not all(isinstance(x, int) for x in (start, end, offset)):
        return None
    exports = declarations.get("publics", [])
    if not any((p.get("name") == public.get('name', row.get("public_symbol"))) and
               p.get("offset") == offset and p.get("segment") == public.get("segment")
               for p in exports):
        return None
    following = [p["offset"] for p in exports if p.get("segment") == public.get("segment")
                 and isinstance(p.get("offset"), int) and p["offset"] > offset]
    bound = min([end] + [start + x for x in following])
    if not start <= start + offset < bound <= end:
        return None
    return dict(start=start + offset, end=bound, size=bound - start - offset,
                basis="accepted owner placement + OMF public; next-public interval, CFG unproved")


def _root(root=None):
    return Path(root).resolve() if root else Path(__file__).resolve().parents[2]


def _c_functions(root: Path):
    # Reuse the repository's declaration scanner so candidate matching has
    # the same function-definition boundaries as semantic_audit.py.
    sys.path.insert(0, str(root / "tools/porting"))
    from host_probe_declarations import (  # pylint: disable=import-outside-toplevel
        mask_comments, top_level_spans, function_headers,
    )

    found = defaultdict(list)
    paths = sorted((root / "game").rglob("*.c")) + sorted((root / "lib").rglob("*.c")) + sorted((root / "port").glob("*.c"))
    for path in paths:
        text = path.read_text(encoding="latin-1")
        code = mask_comments(text)
        for start, end, kind in top_level_spans(text):
            if kind != "function":
                continue
            brace = code.find("{", start, end)
            headers = function_headers(text[start:end])
            if brace < 0 or len(headers) != 1:
                continue
            name, signature = headers[0]
            found[name].append({
                "source": path.relative_to(root).as_posix(),
                "name": name,
                "line": text.count("\n", 0, brace) + 1,
                "signature": signature,
            })
    return found


def build_mapping(root=None, definitions=None, renames=None):
    """Return one explicit mapping row per evidence/functions.json function.

    `address`/`end` are load offsets from the frozen function inventory;
    `extent_bytes` is present only when both bounds are supplied. Portable
    candidates are exact C-definition names drawn from the evidence function
    name and exact-start names-registry aliases. No fuzzy/substring joins are
    performed. `mapping_status` is one of address_alias_unique,
    name_only_unique, ambiguous, unmapped, or unresolved_address.
    """
    root = _root(root)
    evidence = json.loads((root / "evidence/functions.json").read_text(encoding="utf-8"))
    registry = json.loads((root / "layout/names-registry.json").read_text(encoding="utf-8"))
    defs = definitions if definitions is not None else _c_functions(root)
    renames = renames or {}
    output = []
    for row in evidence["functions"]:
        start, end = row.get("start"), row.get("end")
        alias = registry.get("names", {}).get(str(start)) if start is not None else None
        if alias and alias.get("kind") != "code":
            alias = None
        exact_names = []
        if alias:
            exact_names.extend([alias.get("name"), alias.get("inventory_name")])
        exact_names.append(row.get("name"))
        exact_names = list(dict.fromkeys(n for n in exact_names if n))

        candidates = []
        for candidate_name in exact_names:
            for definition in defs.get(renames.get(candidate_name, candidate_name), []):
                candidate = dict(definition)
                candidate["match_name"] = candidate_name
                candidate["implementation_kind"] = (
                    "portable_manual_c" if candidate["source"].startswith("port/")
                    else "accepted_historical_c"
                )
                candidate["match_basis"] = (
                    "exact_start_registry_canonical" if alias and candidate_name == alias.get("name")
                    else "exact_start_registry_inventory" if alias and candidate_name == alias.get("inventory_name")
                    else "historical_name_exact"
                )
                key = (candidate["source"], candidate["name"])
                if all((c["source"], c["name"]) != key for c in candidates):
                    candidates.append(candidate)

        if start is None:
            mapping_status = "unresolved_address"
        elif len(candidates) > 1:
            mapping_status = "ambiguous"
        elif len(candidates) == 1 and alias:
            mapping_status = "address_alias_unique"
        elif len(candidates) == 1:
            mapping_status = "name_only_unique"
        else:
            mapping_status = "unmapped"

        if mapping_status == "ambiguous":
            mapping_reason = "multiple exact-name definitions; no candidate selected"
        elif mapping_status == "unmapped":
            mapping_reason = "no exact-name C definition among historical and exact-start registry names"
        elif mapping_status == "unresolved_address":
            mapping_reason = "historical function row has no start address; any name hit remains an ungrounded candidate"
        elif mapping_status == "name_only_unique":
            mapping_reason = "one exact historical-name C definition, but no exact-start registry alias"
        else:
            mapping_reason = "one exact-name C definition joined through names-registry at exact start"

        output.append({
            "address": start,
            "end": end,
            "extent_bytes": end - start if isinstance(start, int) and isinstance(end, int) and end >= start else None,
            "original_name": row.get("name"),
            "canonical_name": alias.get("name") if alias else None,
            "registry_inventory_name": alias.get("inventory_name") if alias else None,
            "registry_basis": alias.get("basis") if alias else None,
            "evidence_status": row.get("status"),
            "evidence_source": row.get("source"),
            "c_source_leads": row.get("c_sources", []),
            "portable_names": candidates,
            "mapping_status": mapping_status,
            "mapping_reason": mapping_reason,
        })
    return output


if __name__ == "__main__":
    rows = build_mapping()
    counts = {}
    for row in rows:
        counts[row["mapping_status"]] = counts.get(row["mapping_status"], 0) + 1
    print(json.dumps({"function_rows": len(rows), "mapping_status": counts}, sort_keys=True))
