"""Fail-closed research probe for Restunts 16-bit DW OFFSET emission spans.

A matching table is source-emission evidence only. The importer may use it
with independent anchors to verify an emission interval; it does not prove
CFG/code-data classification, binder support, or queue eligibility.
"""
import collections
import re

from common import ROOT, read_json, require, sha, write_json
from mz import MZ
from oracle import verify


SYMBOL = r"[A-Za-z_?@][A-Za-z0-9_?@$]*"
TABLE_START = re.compile(r"^\s*(?P<label>" + SYMBOL + r")\s+dw\s+offset\s+(?P<target>" + SYMBOL + r")\s*(?:;.*)?$", re.I)
TABLE_NEXT = re.compile(r"^\s*dw\s+offset\s+(?P<target>" + SYMBOL + r")\s*(?:;.*)?$", re.I)
LABEL = re.compile(r"^\s*(?P<label>" + SYMBOL + r")\s*:\s*(?:;.*)?$", re.I)
NUMERIC_TABLE = re.compile(r"^off_([0-9a-f]+)$", re.I)
NUMERIC_TARGET = re.compile(r"^(?:loc|locret)_([0-9a-f]+)$", re.I)


def table_groups(lines, line_start, line_end):
    """Return complete adjacent directive groups, with a possible next label."""
    groups = []
    at = line_start - 1
    while at < line_end:
        first = TABLE_START.fullmatch(lines[at].strip())
        if first is None:
            at += 1
            continue
        begin = at
        targets = [first.group("target")]
        at += 1
        while at < line_end:
            continuation = TABLE_NEXT.fullmatch(lines[at].strip())
            if continuation is None:
                break
            targets.append(continuation.group("target"))
            at += 1
        next_at = at
        while next_at < line_end and (not lines[next_at].strip() or lines[next_at].lstrip().startswith(";")):
            next_at += 1
        following = LABEL.fullmatch(lines[next_at].strip()) if next_at < line_end else None
        groups.append({"label": first.group("label"), "targets": targets,
                       "line_start": begin + 1, "line_end": at,
                       "following_label": following.group("label") if following else None})
    return groups


def evaluate(group, frame_paragraph, definitions, image, relocations, verified_anchors,
             function_start=None, function_end=None):
    """Use only an independently anchored segment frame and numeric local labels."""
    row = dict(group)
    match = NUMERIC_TABLE.fullmatch(group["label"])
    if match is None:
        row.update(state="UNRESOLVED_TABLE_COORDINATE")
        return row
    start = int(match.group(1), 16) - 0x10000
    end = start + 2 * len(group["targets"])
    row.update(start=start, end=end, size=end-start)
    if frame_paragraph is None or frame_paragraph < 0 or frame_paragraph * 16 > start:
        row.update(state="UNPROVEN_SEGMENT_FRAME")
        return row
    if not (0 <= start < end <= len(image)) or (function_start is not None and start < function_start) or (
            function_end is not None and end > function_end):
        row.update(state="TABLE_OUTSIDE_CANDIDATE_INTERVAL")
        return row
    if len(definitions.get(group["label"].lower(), [])) != 1:
        row.update(state="AMBIGUOUS_TABLE_LABEL")
        return row
    words = []
    for target in group["targets"]:
        target_match = NUMERIC_TARGET.fullmatch(target)
        if target_match is None or len(definitions.get(target.lower(), [])) != 1:
            row.update(state="UNRESOLVED_TARGET", unresolved_target=target)
            return row
        address = int(target_match.group(1), 16) - 0x10000
        word = address - frame_paragraph * 16
        if not (0 <= word <= 0xffff):
            row.update(state="TARGET_OUTSIDE_SEGMENT", unresolved_target=target)
            return row
        words.append(word)
    predicted = b"".join(word.to_bytes(2, "little") for word in words)
    actual = image[start:end]
    row.update(frame_paragraph=frame_paragraph, predicted_hex=predicted.hex(),
               pristine_hex=actual.hex(), predicted_words=words,
               literal_equal=predicted == actual)
    if any(start <= offset < end for offset in relocations):
        row.update(state="MZ_RELOCATION_INSIDE_TABLE")
        return row
    if predicted != actual:
        row.update(state="WORD_BYTES_DIFFER")
        return row
    following = group["following_label"]
    following_match = NUMERIC_TARGET.fullmatch(following) if following else None
    row["following_label_at_end"] = bool(following_match and
                                          int(following_match.group(1), 16) - 0x10000 == end)
    row["verified_following_anchor"] = row["following_label_at_end"] and end in verified_anchors
    row["state"] = "EXACT_BRACKETED_TABLE_BYTES" if row["verified_following_anchor"] else "EXACT_UNBRACKETED_TABLE_BYTES"
    return row


def run():
    inventory = read_json(ROOT / "recovery/restunts-inventory.json")
    _, unpacked, oracle, _ = verify(write=False)
    image = MZ.parse(unpacked).load_image(unpacked)
    require(sha(image) == inventory["load_sha256"], "Oracle/inventory identity drift")
    relocations = {r["load_offset"] for r in oracle["unpacked_mz"]["relocations"]}
    segments = {s["name"]: s for s in inventory["segments"]}
    anchors = collections.defaultdict(set)
    for anchor in inventory["anchors"]:
        anchors[(anchor["segment"], anchor["function"])].add(anchor["load_offset"])
    by_segment = {}
    rows = []
    partial = [f for f in inventory["functions"] if f["status"] == "PARTIAL_UNMAPPED"]
    emission = [f for f in inventory["functions"] if f["status"] == "BOUNDARIES_AND_EMISSION_BYTES_VERIFIED"]
    for function in partial + emission:
        segment = segments[function["segment"]]
        if segment["name"] not in by_segment:
            path = ROOT / segment["source"]
            data = path.read_bytes()
            require(sha(data) == segment["sha256"], "Restunts source identity drift")
            lines = data.decode("latin1").splitlines()
            definitions = collections.defaultdict(list)
            for number, line in enumerate(lines, 1):
                match = LABEL.fullmatch(line.strip())
                if match:
                    definitions[match.group("label").lower()].append(number)
                table = TABLE_START.fullmatch(line.strip())
                if table:
                    definitions[table.group("label").lower()].append(number)
            by_segment[segment["name"]] = (lines, definitions)
        lines, definitions = by_segment[segment["name"]]
        frames = segment["frame_candidates"]
        frame = frames[0] if len(frames) == 1 and segment.get("segment_paragraph") == frames[0] else None
        for group in table_groups(lines, function["line_start"], function["line_end"]):
            result = evaluate(group, frame, definitions, image, relocations,
                              anchors[(segment["name"], function["name"])],
                              function.get("start"), function.get("end"))
            rows.append({"function": function["name"],
                         "function_id": function["unresolved_evidence_id"],
                         "segment": segment["name"],
                         "frame_evidence_count": len(segment["far_call_evidence"]),
                         "function_status": function["status"], **result})
    counts = collections.Counter(row["state"] for row in rows)
    report = {"schema": 1, "authority": "RESEARCH_ONLY",
              "oracle_load_sha256": sha(image),
              "source_inventory_sha256": sha((ROOT / "recovery/restunts-inventory.json").read_bytes()),
              "partial_tasks": len(partial),
              "emission_tasks_scanned": len(emission),
              "tasks_with_table_directives": len({(r["segment"], r["function"]) for r in rows}),
              "table_groups": len(rows),
              "states": dict(sorted(counts.items())),
              "exact_bracketed_tasks": sorted({r["function"] for r in rows
                  if r["state"] == "EXACT_BRACKETED_TABLE_BYTES"}),
              "exact_bracketed_task_ids": sorted({r["function_id"] for r in rows
                  if r["state"] == "EXACT_BRACKETED_TABLE_BYTES"}),
              "exact_bracketed_bytes": sum(r["size"] for r in rows
                  if r["state"] == "EXACT_BRACKETED_TABLE_BYTES"),
              "tables": rows,
              "limitation": "Exact source-emission bytes do not prove the rest of a function, CFG reachability, original TU membership, or C eligibility."}
    write_json(ROOT / "recovery/table-offset-census.json", report)
    print({k: report[k] for k in ("partial_tasks", "tasks_with_table_directives", "table_groups", "states")})
    return report


if __name__ == "__main__":
    run()
