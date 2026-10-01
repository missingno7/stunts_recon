"""Pinned source dependencies for the modern host, outside historical assets."""
from __future__ import annotations

import hashlib
import os
from pathlib import Path
import urllib.request


ROOT = Path(__file__).resolve().parents[1]
NUKED_OPL3_COMMIT = "765ec962e473aeb767e4cba74ffdc8f588ffbfe8"
NUKED_OPL3_FILES = {
    "opl3.c": "59eb873fdb6d52bc7977a0fcbb97c1bae03dd71cc88e3acddbcc01b7121bfe03",
    "opl3.h": "a84266b8d71a4929f15f573afbe407fd24310b77757d855835dacabf68763679",
    "LICENSE": "20c17d8b8c48a600800dfd14f95d5cb9ff47066a9641ddeab48dc54aec96e331",
}


def nuked_opl3_root(*, fetch: bool = True) -> Path:
    """Verify a local cache or fetch the exact upstream source revision once.

    NUKED_OPL3_ROOT permits offline provisioning with the same pinned files.
    Never replace an existing file that fails verification.
    """
    supplied = os.environ.get("NUKED_OPL3_ROOT")
    directory = (Path(supplied) if supplied else
                 ROOT / "build" / "dependencies" / "nuked-opl3").resolve()
    for name, expected in NUKED_OPL3_FILES.items():
        path = directory / name
        if path.is_file():
            contents = path.read_bytes()
        elif fetch and not supplied:
            url = ("https://raw.githubusercontent.com/nukeykt/Nuked-OPL3/"
                   f"{NUKED_OPL3_COMMIT}/{name}")
            print(f"Fetching pinned Nuked OPL source: {name}")
            try:
                with urllib.request.urlopen(url, timeout=30) as response:
                    contents = response.read()
            except OSError as error:
                raise RuntimeError(
                    "Nuked OPL source is unavailable; provision opl3.c, opl3.h "
                    f"and LICENSE at commit {NUKED_OPL3_COMMIT} in {directory} "
                    "or set NUKED_OPL3_ROOT to a verified offline copy"
                ) from error
            actual = hashlib.sha256(contents).hexdigest()
            if actual != expected:
                raise RuntimeError(f"Downloaded Nuked OPL {name} hash mismatch: {actual}")
            directory.mkdir(parents=True, exist_ok=True)
            path.write_bytes(contents)
        else:
            raise RuntimeError(f"Pinned Nuked OPL dependency is missing: {path}")
        actual = hashlib.sha256(contents).hexdigest()
        if actual != expected:
            raise RuntimeError(f"Pinned Nuked OPL {path} hash mismatch: {actual}")
    return directory


if __name__ == "__main__":
    print(nuked_opl3_root())
