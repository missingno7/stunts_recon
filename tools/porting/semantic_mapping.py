"""Address-grounded historical-function to current-C candidate inventory.

This is a scratch helper for semantic-audit integration. It does not certify
that a same-named implementation is equivalent. A registry alias is joined
only at the historical row's exact start address; absent that, a direct name
hit is reported as name-only evidence. Multiple definitions remain ambiguous.
"""
from __future__ import annotations

import json
import sys
from collections import defaultdict
from pathlib import Path


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
    paths = sorted((root / "src").rglob("*.c")) + sorted((root / "port").glob("*.c"))
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
