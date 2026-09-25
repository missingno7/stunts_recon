"""Reconcile the foreman's cohort budget against archived compiler receipts.

Only the foreman runs this writer. It refuses to overwrite a divergent count or
accept a batch beyond its task allocation or the shared ceiling.
"""

import json
from pathlib import Path

from aggregate import collect


HERE = Path(__file__).resolve().parent
LEDGER = HERE / "ledger.json"


def main():
    report = collect()
    ledger = json.loads(LEDGER.read_text())
    total = 0
    for task, data in report["tasks"].items():
        processes = data["hypothesis_compiler_processes"]
        row = ledger["tasks"][task]
        if processes < row["processes_recorded"]:
            raise ValueError(f"{task}: archived receipts fell below recorded count")
        if processes > row["allocation"]:
            raise ValueError(f"{task}: {processes} processes exceed allocation {row['allocation']}")
        row["processes_recorded"] = processes
        row["meaningful_rounds_recorded"] = data["rounds"]
        total += processes
    if total > ledger["shared_hypothesis_compiler_ceiling"]:
        raise ValueError("cohort compiler processes exceed shared ceiling")
    ledger["hypothesis_compiler_processes_recorded"] = total
    LEDGER.write_text(json.dumps(ledger, indent=2) + "\n")
    print(json.dumps({"cohort_processes": total,
                      "shared_ceiling": ledger["shared_hypothesis_compiler_ceiling"],
                      "task_processes": {task: row["processes_recorded"]
                                         for task, row in ledger["tasks"].items()}}, indent=2))


if __name__ == "__main__":
    main()
