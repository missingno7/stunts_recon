"""Check the immutable third-party source boundary and offline provisioning."""
from pathlib import Path
import os
import tempfile
import unittest
from unittest.mock import patch

from port.dependencies import NUKED_OPL3_FILES, nuked_opl3_root


class Sdl3DependencyTests(unittest.TestCase):
    def test_provisioned_opl_source_matches_pinned_revision(self):
        directory = nuked_opl3_root(fetch=False)
        self.assertTrue((directory / "opl3.c").is_file())
        self.assertTrue((directory / "opl3.h").is_file())

    def test_offline_override_never_fetches_missing_files(self):
        with tempfile.TemporaryDirectory() as temp, \
                patch.dict(os.environ, {"NUKED_OPL3_ROOT": temp}), \
                patch("urllib.request.urlopen") as download:
            with self.assertRaisesRegex(RuntimeError, "dependency is missing"):
                nuked_opl3_root()
            download.assert_not_called()

    def test_modified_cached_source_is_rejected_and_preserved(self):
        with tempfile.TemporaryDirectory() as temp, \
                patch.dict(os.environ, {"NUKED_OPL3_ROOT": temp}), \
                patch("urllib.request.urlopen") as download:
            path = Path(temp) / next(iter(NUKED_OPL3_FILES))
            path.write_bytes(b"changed source\n")
            with self.assertRaisesRegex(RuntimeError, "hash mismatch"):
                nuked_opl3_root()
            self.assertEqual(path.read_bytes(), b"changed source\n")
            download.assert_not_called()
