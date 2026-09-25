#!/usr/bin/env python3
"""Diagnostic TU closure map and member-aware canonical MSC 5.10 workbench."""
from __future__ import annotations

import argparse
import bisect
import hashlib
import json
import os
import struct
import subprocess
import sys
import uuid
from collections import defaultdict, deque
from pathlib import Path

MODULE_DIR = Path(__file__).resolve().parent
ROOT = MODULE_DIR.parent if MODULE_DIR.name == "tools" else MODULE_DIR.parents[2]
HERE = MODULE_DIR
TOOLS = ROOT / "tools"
if str(TOOLS) not in sys.path:
    sys.path.insert(0, str(TOOLS))

from common import read_json, sha, write_json

WORKSPACE = Path(os.environ.get("TUBENCH_WORKSPACE", str(ROOT / "build/tubench")))
MAP_PATH = ROOT / "build/tumap.json"
MAP_TABLE = ROOT / "build/tumap.md"
VERIFIED_STATUSES = {
    "BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED",
    "BOUNDARIES_AND_EMISSION_BYTES_VERIFIED",
}


def _read_authority():
    from oracle import verify
    from mz import MZ

    oracle = verify(write=False)
    image = MZ.parse(oracle[1]).load_image(oracle[1])
    return oracle, image


def build_tu_map():
    """Find direct near-call components from locked bytes and enrich intervals."""
    oracle, image = _read_authority()
    functions_doc = read_json(ROOT / "evidence/functions.json")
    if functions_doc.get("load_sha256") != sha(image):
        raise RuntimeError("Function inventory is not anchored to the locked oracle load image")
    inventory = [row for row in functions_doc["functions"]
                 if row.get("status") in VERIFIED_STATUSES
                 and isinstance(row.get("start"), int)
                 and isinstance(row.get("end"), int)
                 and 0 <= row["start"] < row["end"] <= len(image)]
    for row in inventory:
        if not row.get("sha256") or sha(image[row["start"]:row["end"]]) != row["sha256"]:
            raise RuntimeError(f"Function extent hash differs from the locked image: {row['name']}")
    starts = defaultdict(list)
    for row in inventory:
        starts[row["start"]].append(row)
    sorted_starts = sorted(starts)

    def procedures_at(address):
        exact = starts.get(address, [])
        if exact:
            return exact
        index = bisect.bisect_right(sorted_starts, address) - 1
        if index < 0:
            return []
        candidates = starts[sorted_starts[index]]
        return [row for row in candidates if row["start"] <= address < row["end"]]

    # Decode each hash-verified procedure directly from the locked image. An E8
    # byte inside an immediate or displacement is not treated as a call.
    decoder_path = str(ROOT / "build/python")
    if decoder_path not in sys.path:
        sys.path.insert(0, decoder_path)
    import capstone
    if capstone.__version__ != "5.0.3":
        raise RuntimeError("Pinned instruction decoder differs")
    decoder = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_16)
    edges = []
    unassigned_near_calls = []
    adjacency = {row["name"]: set() for row in inventory}
    by_name = {row["name"]: row for row in inventory}
    decode_anomalies = []
    for caller in inventory:
        start, end = caller["start"], caller["end"]
        decoded = list(decoder.disasm(image[start:end], start))
        if not decoded or decoded[-1].address + decoded[-1].size != end:
            decode_anomalies.append({"caller": caller["name"], "start": start, "end": end,
                                     "decoded_end": (decoded[-1].address + decoded[-1].size
                                                     if decoded else start),
                                     "reason": "instruction decoder did not cover the verified extent"})
        for instruction in decoded:
            site = instruction.address
            if instruction.bytes[:1] != b"\xe8" or instruction.size != 3:
                continue
            callee_at = site + 3 + struct.unpack_from("<h", image, site + 1)[0]
            targets = procedures_at(callee_at)
            if not targets:
                unassigned_near_calls.append({
                    "caller": caller["name"], "site": site,
                    "target_offset": callee_at,
                    "reason": "near-call destination is outside half-open inventory procedure extents",
                })
            for callee in targets:
                if callee["name"] == caller["name"]:
                    continue
                # Separate evidence code segments cannot use an E8 near call.
                if callee.get("segment") != caller.get("segment"):
                    continue
                edge = {
                    "caller": caller["name"], "callee": callee["name"],
                    "site": site, "operand": site + 1,
                    "target_offset": callee_at,
                    "target_is_member_entry": callee_at == callee["start"],
                    "push_cs": site > start and image[site - 1] == 0x0E,
                    "encoding": "push_cs_call_near" if site > start and image[site - 1] == 0x0E else "call_near",
                }
                edges.append(edge)
                adjacency[caller["name"]].add(callee["name"])
                adjacency[callee["name"]].add(caller["name"])

    manifest = read_json(ROOT / "layout/manifest.json")
    owners = manifest.get("owners", [])
    accepted_names = {owner.get("name") for owner in owners if owner.get("kind") == "MATCHING_C"}
    relorder = {}  # No scratch relorder ledger is required for the canonical map.
    code_order = relorder.get("known_order_constraints", {}).get("code_frame_order", [])
    code_order_index = {name: index for index, name in enumerate(code_order)}

    components = []
    visited = set()
    for seed in sorted((name for name, neighbors in adjacency.items() if neighbors)):
        if seed in visited:
            continue
        pending = [seed]
        names = []
        while pending:
            name = pending.pop()
            if name in visited:
                continue
            visited.add(name)
            names.append(name)
            pending.extend(adjacency[name] - visited)
        members_connected = [by_name[name] for name in names]
        segments = {row.get("segment") for row in members_connected}
        if len(segments) != 1:
            # This should be impossible for near calls; keep an explicit record.
            continue
        segment = next(iter(segments))
        ordered_connected = sorted(members_connected, key=lambda row: row["start"])
        lo, hi = ordered_connected[0]["start"], ordered_connected[-1]["end"]
        members = sorted((row for row in inventory
                          if row.get("segment") == segment and lo <= row["start"] and row["end"] <= hi),
                         key=lambda row: row["start"])
        start, end = members[0]["start"], members[-1]["end"]
        names_in_interval = {row["name"] for row in members}
        direct_edges = [edge for edge in edges
                        if edge["caller"] in names_in_interval and edge["callee"] in names_in_interval]
        interval_owners = [owner for owner in owners
                           if owner.get("start", end) < end and start < owner.get("end", start)]
        accepted_owners_overlapping = [owner for owner in interval_owners
                                       if owner.get("kind") == "MATCHING_C"]
        accepted_owners = [owner for owner in accepted_owners_overlapping
                           if start <= owner.get("start", end) and owner.get("end", start) <= end]
        asm_detail = []
        c_compatible = []
        emission_only = []
        unknown = []
        for member in members:
            positive = []
            hint = "msc_c_compatible_shape" if member["name"] in accepted_names else None
            if positive:
                asm_detail.append({"name": member["name"], "markers": positive})
            elif hint == "msc_c_compatible_shape":
                c_compatible.append(member["name"])
            else:
                unknown.append(member["name"])
            if member.get("confidence") == "verified_emission_coordinates_cfg_unreviewed" or \
                    member.get("status") == "BOUNDARIES_AND_EMISSION_BYTES_VERIFIED":
                emission_only.append(member["name"])

        runtime = []
        relorder_intersections = []
        for obj in relorder.get("objects", []):
            code_range = obj.get("code_range")
            if isinstance(code_range, list) and len(code_range) == 2 and \
                    code_range[0] < end and start < code_range[1]:
                overlap = {"id": obj.get("id"), "kind": obj.get("kind"),
                           "code_range": code_range, "members": obj.get("members", []),
                           "confidence": obj.get("confidence")}
                relorder_intersections.append(overlap)
                if obj.get("kind") == "pinned_runtime_member":
                    runtime.append(overlap)
        boundary_evidence = [item for item in relorder.get("proposed_boundary_tests", [])
                             if item.get("at") is not None and start <= item["at"] <= end]
        try:
            order_segment = "seg" + str(int(segment[3:])).zfill(3)
            order_rank = code_order_index.get(order_segment)
        except (TypeError, ValueError, IndexError):
            order_segment, order_rank = segment, None

        ident = f"{segment}_{start:05x}_{end:05x}"
        components.append({
            "id": ident, "segment": segment, "interval": {"start": start, "end": end,
                                                               "bytes": end - start},
            "members": [{"name": row["name"], "stable_id": row.get("stable_id"),
                         "start": row["start"], "end": row["end"], "size": row["end"] - row["start"],
                         "status": row.get("status"), "confidence": row.get("confidence"),
                         "connected": row["name"] in names}
                        for row in members],
            "member_count": len(members), "member_bytes": sum(row["end"] - row["start"] for row in members),
            "gaps": [{"start": a["end"], "end": b["start"]}
                     for a, b in zip(members, members[1:]) if a["end"] != b["start"]],
            "connected_member_count": len(names), "near_call_edges": direct_edges,
            "accepted_owners_inside": [{"id": owner.get("id"), "name": owner.get("name"),
                                         "kind": owner.get("kind"), "start": owner.get("start"),
                                         "end": owner.get("end"), "recipe": owner.get("recipe")}
                                        for owner in accepted_owners],
            "accepted_owners_intersecting": [{"id": owner.get("id"), "name": owner.get("name"),
                                               "kind": owner.get("kind"), "start": owner.get("start"),
                                               "end": owner.get("end"),
                                               "fully_inside_interval": owner in accepted_owners}
                                              for owner in accepted_owners_overlapping],
            "all_owners_intersecting": [{"id": owner.get("id"), "kind": owner.get("kind"),
                                          "start": owner.get("start"), "end": owner.get("end")}
                                         for owner in interval_owners],
            "positive_asm_marker_members": asm_detail,
            "positive_marker_basis": {"status": "NOT_EVALUATED_IN_THIS_MAP"},
            "runtime_overlaps": runtime,
            "relorder_evidence": {
                "code_frame_order_entry": order_segment if order_rank is not None else None,
                "code_frame_order_rank": order_rank,
                "intersecting_objects": relorder_intersections,
                "nearby_boundary_tests": boundary_evidence,
                "global_constraints": relorder.get("known_order_constraints", {}),
            },
            "members_with_emission_level_boundaries": emission_only,
            "c_compatible_members_without_positive_asm_markers": c_compatible,
            "unknown_members": unknown,
            "ranking": {"c_compatible": len(c_compatible), "unknown": len(unknown),
                        "positive_asm": len(asm_detail), "smallness_bytes": end - start},
        })
    components.sort(key=lambda row: (-(row["ranking"]["c_compatible"] / max(1, row["member_count"])),
                                     row["ranking"]["unknown"], row["ranking"]["positive_asm"],
                                     row["ranking"]["smallness_bytes"], row["interval"]["start"]))
    document = {
        "schema": "tubench-map-v1", "authority": "Derived from locked oracle bytes plus read-only reviewed inventories/layouts; candidate TU membership remains a hypothesis.",
        "inputs": {"function_inventory": "evidence/functions.json", "manifest": "layout/manifest.json",
                   "decoder": "capstone==5.0.3", "oracle_load_sha256": functions_doc.get("load_sha256")},
        "inventory_procedures_scanned": len(inventory), "near_call_edges": len(edges),
        "push_cs_near_call_edges": sum(edge["push_cs"] for edge in edges),
        "near_call_decode_anomalies": decode_anomalies,
        "near_calls_without_inventory_callee": unassigned_near_calls,
        "closure_count": len(components), "closures": components,
    }
    write_json(MAP_PATH, document)
    rows = ["# TU candidates ranked for C reconstruction", "",
            "Rank favors accepted C members, then fewer unknown members and smaller extents. ASM markers and relorder are not evaluated.", "",
            "| Rank | ID | Segment interval | Bytes | Members | C-compatible | Unknown | ASM-marker members | Accepted owners | Emission-only boundaries |",
            "|---:|---|---:|---:|---:|---:|---:|---:|---:|---|"]
    for rank, item in enumerate(components, 1):
        start, end = item["interval"]["start"], item["interval"]["end"]
        rows.append(f"| {rank} | `{item['id']}` | `{start:05X}-{end:05X}` | {end-start} | {item['member_count']} | "
                    f"{item['ranking']['c_compatible']} | {item['ranking']['unknown']} | {item['ranking']['positive_asm']} | "
                    f"{len(item['accepted_owners_inside'])} | {', '.join(item['members_with_emission_level_boundaries']) or '-'} |")
    MAP_TABLE.write_text("\n".join(rows) + "\n", encoding="utf-8")
    return document


def _compile_in_worker(source_path: Path):
    """Invoke only the locked compiler; place every temporary under this worker."""
    import compiler
    from object_probe import read_object

    profile = "msc510-medium"
    config, runner = compiler.verify_toolchain(profile)
    if [str(flag).upper() for flag in config.get("flags", [])] != ["/AM", "/O", "/GS"]:
        raise RuntimeError("Pinned msc510-medium profile no longer matches /AM /O /Gs")
    source_path = source_path.expanduser().resolve()
    source = source_path.read_bytes()
    try:
        source_text = source.decode("ascii").replace("\r\n", "\n").replace("\r", "\n")
    except UnicodeDecodeError as error:
        raise RuntimeError("Historical C source must be ASCII") from error
    import re
    for pattern, message in ((r"^\s*#", "Preprocessor directives are unsupported in the standalone TU workbench"),
                             (r"\b(?:_asm|__asm|asm|__emit)\b", "Inline assembly/raw emission is forbidden")):
        if re.search(pattern, source_text, re.M):
            raise RuntimeError(message)
    run_dir = WORKSPACE / "compiler_runs" / (sha(source)[:12] + "_" + uuid.uuid4().hex[:8])
    run_dir.mkdir(parents=True, exist_ok=False)
    staged = source_text.replace("\n", "\r\n").encode("ascii")
    (run_dir / "UNIT.C").write_bytes(staged)
    tc = (ROOT / config["directory"]).resolve()
    argv = [runner["path"], "-e", "-v5.00", str(tc / config["executable"]), "/c"]
    argv += list(config["flags"])
    argv += ["UNIT.C"]
    env = {"PATH": str(tc), "MSDOS_PATH": str(tc), "TEMP": ".", "TMP": ".", "MSDOS_TEMP": "."}
    result = subprocess.run(argv, cwd=run_dir, env=env, stdout=subprocess.PIPE,
                            stderr=subprocess.STDOUT, timeout=60,
                            creationflags=getattr(subprocess, "CREATE_NO_WINDOW", 0))
    (run_dir / "compiler.log").write_bytes(result.stdout)
    obj_path = run_dir / "UNIT.OBJ"
    if result.returncode or not obj_path.is_file():
        raise RuntimeError(f"Compiler failed ({result.returncode}); see {run_dir / 'compiler.log'}")
    obj_bytes = obj_path.read_bytes()
    obj = read_object(obj_bytes, research_local_symbols=True)
    return source, run_dir, obj, obj_bytes


def _function_maps():
    rows = [row for row in read_json(ROOT / "evidence/functions.json")["functions"]
            if row.get("status") in VERIFIED_STATUSES and isinstance(row.get("start"), int)
            and isinstance(row.get("end"), int)]
    by_name = defaultdict(list)
    by_start = defaultdict(list)
    for row in rows:
        by_name[row["name"]].append(row)
        by_start[row["start"]].append(row)
    return rows, by_name, by_start


def _public_name_map(obj, code_segment):
    result = defaultdict(list)
    for row in obj.publics:
        if row.get("segment") != code_segment:
            continue
        name = row["name"]
        result[name].append(row)
        result[name.lstrip("_")].append(row)
    return result


def _choose_public(public_map, function_name):
    rows = public_map.get(function_name, [])
    unique = {(row["name"], row["offset"]): row for row in rows}
    if len(unique) == 1:
        return next(iter(unique.values()))
    if unique:
        return None
    # MSC 5.x stores at most 31 bytes of an external identifier, including
    # the leading underscore on its C public spelling.
    for candidate in (function_name[:31], ("_" + function_name)[:31],
                      function_name[:30], ("_" + function_name)[:31].lstrip("_")):
        rows = public_map.get(candidate, [])
        unique = {(row["name"], row["offset"]): row for row in rows}
        if len(unique) == 1:
            return next(iter(unique.values()))
    return None


def _symbol_alias(symbol_name, code_symbols, data_symbols, image, relocations, resolver_cache,
                  inventory_by_name):
    """Resolve independently grounded public aliases; return status + value."""
    name_options = [symbol_name]
    if symbol_name.startswith("_"):
        name_options.append(symbol_name[1:])
    else:
        name_options.append("_" + symbol_name)
    for name in name_options:
        if name in code_symbols:
            cache_key = ("code", name)
            if cache_key not in resolver_cache:
                try:
                    from code_symbols import resolve_code_symbols
                    resolver_cache[cache_key] = resolve_code_symbols({name}, image, relocations)[name]
                except Exception as error:  # explicit, diagnostic, fail closed
                    resolver_cache[cache_key] = {"error": str(error)}
            value = resolver_cache[cache_key]
            if "error" not in value:
                return {"status": "REVIEWED_CODE_ALIAS", "alias": name,
                        "symbol": value}, value
            return {"status": "CODE_ALIAS_PRESENT_BUT_UNRESOLVED", "alias": name,
                    "reason": value["error"]}, None
        if name in data_symbols:
            cache_key = ("data", name)
            if cache_key not in resolver_cache:
                try:
                    from data_symbols import resolve_symbols
                    resolver_cache[cache_key] = resolve_symbols({name}, image, relocations)[name]
                except Exception as error:
                    resolver_cache[cache_key] = {"error": str(error)}
            value = resolver_cache[cache_key]
            if "error" not in value:
                return {"status": "REVIEWED_DATA_ALIAS", "alias": name,
                        "symbol": value}, value
            return {"status": "DATA_ALIAS_PRESENT_BUT_UNRESOLVED", "alias": name,
                    "reason": value["error"]}, None
    # A function's reviewed entry coordinate is also an independent far-code
    # binding when the compact code-symbol map has no separate alias row.
    function_name = symbol_name.lstrip("_")
    rows = inventory_by_name.get(function_name, [])
    if len(rows) == 1:
        row = rows[0]
        frame = row.get("segment_paragraph")
        offset = row.get("segment_offset")
        if row.get("status") == "BOUNDARIES_AND_INSTRUCTION_ANCHORS_VERIFIED" and \
                isinstance(frame, int) and isinstance(offset, int) and \
                frame * 16 + offset == row.get("start") and row.get("sha256") and \
                sha(image[row["start"]:row["end"]]) == row["sha256"]:
            value = {"kind": "far-code", "frame_load_address": frame * 16,
                     "load_address": row["start"], "evidence_function": row["name"],
                     "stable_id": row.get("stable_id"), "evidence_status": row.get("status")}
            return {"status": "INVENTORY_FAR_CODE_PUBLIC", "alias": symbol_name,
                    "symbol": value}, value
    return {"status": "NO_LAYOUT_ALIAS"}, None


def _resolve_near_member(target_name, target_rows, compiled_public_map):
    options = [target_name, target_name.lstrip("_")]
    for option in options:
        matches = target_rows.get(option, [])
        if len(matches) == 1:
            return matches[0], "INVENTORY_MEMBER"
    truncated = target_name.lstrip("_")[:30]
    prefix_matches = [row for name, rows in target_rows.items()
                      if name[:30] == truncated for row in rows]
    if len(prefix_matches) == 1:
        return prefix_matches[0], "MSC_31_BYTE_IDENTIFIER_PREFIX"
    # Local OMF names can differ by their leading underscore. Match a unique
    # emitted member public back to its evidence function if available.
    pub = _choose_public(compiled_public_map, target_name.lstrip("_"))
    if pub:
        for row in target_rows.get(pub["name"].lstrip("_"), []):
            return row, "EMITTED_PUBLIC_TO_INVENTORY"
    return None, "NO_INVENTORY_TARGET"


def _first_mismatch(candidate: bytes, target: bytes):
    limit = min(len(candidate), len(target))
    for at in range(limit):
        if candidate[at] != target[at]:
            return {"relative_offset": at, "target_byte": f"{target[at]:02x}",
                    "candidate_byte": f"{candidate[at]:02x}"}
    if len(candidate) != len(target):
        return {"relative_offset": limit, "target_byte": (f"{target[limit]:02x}" if limit < len(target) else None),
                "candidate_byte": (f"{candidate[limit]:02x}" if limit < len(candidate) else None),
                "reason": "length boundary"}
    return None


def _select_members(map_doc, tu_id, interval, members_arg):
    all_rows, _, _ = _function_maps()
    by_name = {row["name"]: row for row in all_rows}
    if tu_id:
        tu = next((item for item in map_doc["closures"] if item["id"] == tu_id), None)
        if tu is None:
            raise ValueError(f"Unknown TU id: {tu_id}; see {MAP_TABLE}")
        selected = [by_name[row["name"]] for row in tu["members"] if row["name"] in by_name]
        return selected, tu["interval"], tu["id"]
    if interval:
        start, end = interval
        selected = [row for row in all_rows if start <= row["start"] and row["end"] <= end]
        return sorted(selected, key=lambda row: row["start"]), {"start": start, "end": end}, f"interval_{start:05x}_{end:05x}"
    if members_arg:
        names = [name.strip() for name in members_arg.split(",") if name.strip()]
        selected = []
        for name in names:
            if name not in by_name:
                raise ValueError(f"Unknown or unverified member: {name}")
            selected.append(by_name[name])
        selected.sort(key=lambda row: row["start"])
        return selected, {"start": selected[0]["start"], "end": selected[-1]["end"]}, "members"
    raise ValueError("Select members with --tu ID, --interval START END, or --members name,...")


def run_workbench(source_path, *, tu_id=None, interval=None, members_arg=None):
    map_doc = build_tu_map()
    selected, interval_info, selection_id = _select_members(map_doc, tu_id, interval, members_arg)
    source, run_dir, obj, obj_bytes = _compile_in_worker(Path(source_path))
    oracle, image = _read_authority()
    inventory_sha = read_json(ROOT / "evidence/functions.json").get("load_sha256")
    if inventory_sha != sha(image):
        raise RuntimeError("Function inventory is not anchored to the locked oracle load image")
    for target in selected:
        if not target.get("sha256") or sha(image[target["start"]:target["end"]]) != target["sha256"]:
            raise RuntimeError(f"Target member hash differs from the locked image: {target['name']}")
    relocations = oracle[2]["unpacked_mz"]["relocations"]
    code_symbols = read_json(ROOT / "layout/code-symbols.json").get("symbols", {})
    data_symbols = read_json(ROOT / "layout/data-symbols.json").get("symbols", {})

    selected_names = {row["name"] for row in selected}
    all_functions, all_targets, _ = _function_maps()
    for target in all_functions:
        if not target.get("sha256") or sha(image[target["start"]:target["end"]]) != target["sha256"]:
            raise RuntimeError(f"Target inventory hash differs from the locked image: {target['name']}")
    inventory_by_name = defaultdict(list)
    for row in all_functions:
        inventory_by_name[row["name"]].append(row)
    code_segments = [name for name in obj.segments if any(
        public.get("segment") == name for public in obj.publics)]
    code_segment = "UNIT_TEXT" if "UNIT_TEXT" in code_segments else (code_segments[0] if code_segments else None)
    segment_bytes = bytes(obj.segments.get(code_segment, b"")) if code_segment else b""
    emitted_publics = sorted((row for row in obj.publics if row.get("segment") == code_segment),
                             key=lambda row: row["offset"])
    public_map = _public_name_map(obj, code_segment) if code_segment else defaultdict(list)
    fixups = [fix for fix in obj.linker_fixups if fix.get("segment") == code_segment]
    original_edges = {}
    seen_original_edges = set()
    for closure in map_doc.get("closures", []):
        for edge in closure.get("near_call_edges", []):
            edge_key = (edge["caller"], edge["callee"], edge["site"])
            if edge_key in seen_original_edges:
                continue
            seen_original_edges.add(edge_key)
            original_edges.setdefault((edge["caller"], edge["callee"]), []).append(edge)
    for edge_rows in original_edges.values():
        edge_rows.sort(key=lambda edge: edge["site"])
    compiled_function_by_offset = {}
    for function in all_functions:
        public_row = _choose_public(public_map, function["name"])
        if public_row:
            compiled_function_by_offset.setdefault(public_row["offset"], []).append(function)

    # Match emitted near-reference occurrences to the original caller/callee
    # edges in source order. This lets a declaration-only near callee be checked
    # against its target CALL operand without requiring a compiled definition.
    near_groups = defaultdict(list)
    for fix in fixups:
        at = fix.get("offset", -1)
        if not (fix.get("self_relative") and fix.get("loc") == "offset16" and
                fix.get("width") == 2 and at > 0 and segment_bytes[at - 1] == 0xE8):
            continue
        caller_public = max((pub for pub in emitted_publics if pub["offset"] <= at - 1),
                            key=lambda pub: pub["offset"], default=None)
        callers = compiled_function_by_offset.get(caller_public["offset"], []) if caller_public else []
        callee, _ = _resolve_near_member(fix.get("target", ""), all_targets, public_map)
        if len(callers) == 1 and callee:
            near_groups[(callers[0]["name"], callee["name"])].append(fix)
    assigned_target_edges = {}
    near_assignment_errors = {}
    for key, near_rows in near_groups.items():
        near_rows.sort(key=lambda fix: fix["offset"])
        target_edges = original_edges.get(key, [])
        if len(near_rows) == len(target_edges):
            for fix, edge in zip(near_rows, target_edges):
                assigned_target_edges[id(fix)] = edge
        elif len(near_rows) == 1 and len(target_edges) == 1:
            assigned_target_edges[id(near_rows[0])] = target_edges[0]
        else:
            for fix in near_rows:
                near_assignment_errors[id(fix)] = {
                    "status": "AMBIGUOUS_TARGET_CALL_OCCURRENCE",
                    "candidate_calls": len(near_rows), "target_calls": len(target_edges),
                    "caller": key[0], "callee": key[1],
                }
    resolver_cache = {}
    patched = bytearray(segment_bytes)
    fixed_sites = {}
    fixup_details = []
    unresolved_by_offset = {}
    for fix in fixups:
        at, width = fix["offset"], fix["width"]
        row = {key: fix.get(key) for key in (
            "offset", "loc", "width", "self_relative", "target", "target_kind", "frame",
            "encoded_addend", "displacement", "frame_kind", "frame_method", "target_method")}
        row["end_offset"] = at + width
        row["predicted_value"] = None
        row["resolved"] = False
        if fix.get("self_relative") and fix.get("loc") == "offset16" and width == 2 and at > 0 and \
                segment_bytes[at - 1] == 0xE8:
            target_row, target_status = _resolve_near_member(fix.get("target", ""), all_targets, public_map)
            row["alias"] = {"status": target_status, "target": target_row["name"] if target_row else fix.get("target")}
            target_edge = assigned_target_edges.get(id(fix))
            if target_row and target_edge:
                row["alias"]["caller"] = target_edge["caller"]
                row["target_call_site"] = target_edge["site"]
                row["target_operand_offset"] = target_edge["operand"]
                row["target_public_offset"] = target_row["start"]
                addend = int.from_bytes(bytes.fromhex(fix.get("encoded_addend", "0000")), "little", signed=False)
                if target_edge.get("target_offset") == target_row["start"] and addend == 0 and \
                        int(fix.get("displacement", 0)) == 0 and image[target_edge["site"]] == 0xE8:
                    predicted = (target_row["start"] - target_edge["site"] - 3) & 0xFFFF
                    target_operand = image[target_edge["operand"]:target_edge["operand"] + 2]
                    if target_operand == struct.pack("<H", predicted):
                        struct.pack_into("<H", patched, at, predicted)
                        row["predicted_value"] = predicted
                        row["target_operand_bytes"] = target_operand.hex()
                        row["operand_bytes_match_target"] = bytes(patched[at:at + 2]) == target_operand
                        row["resolved"] = row["operand_bytes_match_target"]
                if not row["resolved"]:
                    row["alias"]["resolution_error"] = (
                        "target call is not a zero-addend call to the mapped public entry")
            elif id(fix) in near_assignment_errors:
                row["alias"] = near_assignment_errors[id(fix)]
        if not row["resolved"] and fix.get("target_kind") == "external" and not fix.get("self_relative"):
            alias, value = _symbol_alias(fix.get("target", ""), code_symbols, data_symbols,
                                         image, relocations, resolver_cache, inventory_by_name)
            row["alias"] = alias
            if value is not None:
                encoded = bytes.fromhex(fix.get("encoded_addend", ""))
                if fix.get("loc") == "pointer32" and width == 4 and value.get("kind") == "far-code":
                    addend = int.from_bytes(encoded[:2], "little") if len(encoded) >= 2 else 0
                    if addend == 0 and len(encoded) == 4:
                        struct.pack_into("<HH", patched, at,
                                         value["load_address"] - value["frame_load_address"],
                                         value["frame_load_address"] // 16)
                        row["predicted_value"] = [value["load_address"] - value["frame_load_address"],
                                                  value["frame_load_address"] // 16]
                        row["resolved"] = True
                        row["relocation_site"] = at + 2
                elif fix.get("loc") == "offset16" and width == 2 and value.get("group") == "DGROUP":
                    addend = int.from_bytes(encoded, "little") if encoded else 0
                    allowed = value.get("allowed_addends", [0])
                    target_offset = value["load_address"] - value["frame_load_address"]
                    if addend in allowed and 0 <= target_offset + addend <= 0xFFFF:
                        struct.pack_into("<H", patched, at, target_offset + addend)
                        row["predicted_value"] = target_offset + addend
                        row["resolved"] = True
        if not row["resolved"]:
            row.setdefault("alias", {"status": "UNRESOLVED_OR_UNSUPPORTED_FIXUP"})
            for pos in range(at, at + width):
                unresolved_by_offset[pos] = row["alias"]["status"]
        fixup_details.append(row)

    members_report = []
    for target in selected:
        name = target["name"]
        public = _choose_public(public_map, name)
        base = public["offset"] if public else None
        following = next((row["offset"] for row in emitted_publics if base is not None and row["offset"] > base),
                         len(segment_bytes) if base is not None else None)
        emitted_size = following - base if base is not None else None
        target_size = target["end"] - target["start"]
        target_bytes = image[target["start"]:target["end"]]
        compared_bytes = bytes(patched[base:min(base + target_size, len(patched))]) \
            if base is not None and base <= len(patched) else b""
        relevant_fixups = [row for row in fixup_details
                           if base is not None and base <= row["offset"] < base + max(target_size, emitted_size or 0)]
        unresolved_fixups = [row for row in relevant_fixups if not row["resolved"]]
        mismatch = _first_mismatch(compared_bytes, target_bytes) if public is not None else None
        has_exact_extent = emitted_size == target_size and len(compared_bytes) == target_size
        exact = has_exact_extent and mismatch is None and not unresolved_fixups
        if public is None:
            status = "DECLARED_ONLY_OR_MISSING_DEFINITION"
        elif exact:
            status = "EXACT"
        elif has_exact_extent and mismatch is None and unresolved_fixups:
            status = "INDETERMINATE_UNRESOLVED_FIXUPS"
        else:
            status = "DIFFER"
        if mismatch is not None:
            mismatch["target_offset"] = target["start"] + mismatch["relative_offset"]
            if base is not None:
                mismatch["candidate_offset"] = base + mismatch["relative_offset"]
                mismatch["unresolved_fixup_alias"] = unresolved_by_offset.get(base + mismatch["relative_offset"])
        members_report.append({
            "name": name, "stable_id": target.get("stable_id"),
            "target": {"start": target["start"], "end": target["end"], "size": target_size,
                       "sha256": target.get("sha256"), "boundary_status": target.get("status"),
                       "confidence": target.get("confidence")},
            "emitted": {"public": public.get("name") if public else None,
                        "offset": base, "size_to_next_public": emitted_size,
                        "target_sized_span_available": len(compared_bytes) == target_size},
            "status": status, "exact_bytes": exact,
            "first_mismatch": mismatch,
            "external_fixups": relevant_fixups,
            "unresolved_fixup_count": len(unresolved_fixups),
        })

    report = {
        "schema": "tubench-workbench-v1", "profile": "msc510-medium",
        "flags": ["/AM", "/O", "/Gs"],
        "source": str(Path(source_path).resolve()), "source_sha256": sha(source),
        "compile": {"status": "COMPILED", "work_directory": str(run_dir),
                    "object": {"path": str(run_dir / "UNIT.OBJ"), "size": len(obj_bytes), "sha256": sha(obj_bytes)},
                    "code_segment": code_segment, "code_segment_size": len(segment_bytes),
                    "publics": obj.publics, "externals": obj.externals,
                    "fixup_count": len(obj.linker_fixups),
                    "all_fixups_resolved_for_comparison": not unresolved_by_offset},
        "selection": {"id": selection_id, "interval": interval_info,
                      "member_count": len(selected), "members": [row["name"] for row in selected]},
        "members": members_report,
        "summary": {"exact": sum(row["exact_bytes"] for row in members_report),
                    "differ": sum(row["status"] == "DIFFER" for row in members_report),
                    "declared_only_or_missing": sum(row["status"] == "DECLARED_ONLY_OR_MISSING_DEFINITION" for row in members_report),
                    "indeterminate": sum(row["status"] == "INDETERMINATE_UNRESOLVED_FIXUPS" for row in members_report)},
        "authority": "Diagnostic member comparison only. Each near-call relocation is resolved against the locked target function public offsets; code/data aliases use reviewed layout resolvers. No ownership or whole-object acceptance is inferred.",
    }
    output = run_dir / "workbench.json"
    write_json(output, report)
    report["report_path"] = str(output)
    write_json(output, report)
    print("NAME\tEMITTED/TARGET\tSTATUS\tFIRST MISMATCH\tUNRESOLVED FIXUPS")
    for row in members_report:
        emitted_size = row["emitted"]["size_to_next_public"]
        emitted_value = "—" if emitted_size is None else str(emitted_size)
        target_value = row["target"]["size"]
        first = row["first_mismatch"]
        mismatch_value = "-" if first is None else f"+{first['relative_offset']:x} {first['target_byte']}!={first['candidate_byte']}"
        print(f"{row['name']}\t{emitted_value}/{target_value}\t{row['status']}\t{mismatch_value}\t{row['unresolved_fixup_count']}")
    print(f"report: {output}")
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("source", nargs="?", help="Whole-TU C source file")
    parser.add_argument("--map", "--build-map", dest="build_map", action="store_true",
                        help="Rebuild build/tumap.json and build/tumap.md")
    selection = parser.add_mutually_exclusive_group()
    selection.add_argument("--tu", help="TU closure ID from tumap.json")
    selection.add_argument("--interval", nargs=2, type=lambda value: int(value, 0), metavar=("START", "END"),
                           help="Target interval in load-image offsets (decimal or 0x-prefixed)")
    selection.add_argument("--members", help="Comma-separated evidence function names")
    args = parser.parse_args()
    if args.build_map:
        document = build_tu_map()
        print(f"TU closures: {document['closure_count']} from {document['near_call_edges']} near-call edges")
        print(f"JSON: {MAP_PATH}\nTable: {MAP_TABLE}")
        if not args.source:
            return 0
    if not args.source:
        parser.error("source is required unless --map is used alone")
    interval = tuple(args.interval) if args.interval else None
    run_workbench(args.source, tu_id=args.tu, interval=interval, members_arg=args.members)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
