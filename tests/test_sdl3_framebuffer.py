"""Exact indexed-screen regressions from a fresh seeded SDL3 startup run."""
from __future__ import annotations

import unittest

from tests.sdl3_screen_regression import (
    SCREEN_NAMES,
    capture_startup_screens,
    find_exact_screens,
    fresh_capture_directory,
)


class Sdl3FramebufferTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls._temporary, cls.capture_dir = fresh_capture_directory()
        try:
            cls.run_output = capture_startup_screens(cls.capture_dir)
            cls.matches, cls.diagnostics = find_exact_screens(cls.capture_dir)
        except Exception:
            cls._temporary.cleanup()
            raise

    @classmethod
    def tearDownClass(cls) -> None:
        cls._temporary.cleanup()

    def assert_screen_matches_portforge(self, name: str) -> None:
        self.assertIn(name, SCREEN_NAMES)
        self.assertIn(
            name, self.matches,
            f"{name} screen did not match the immutable PortForge reference; "
            f"{self.diagnostics[name]}; captures={len(list(self.capture_dir.glob('*.fbr')))}; "
            f"run output={self.run_output!r}",
        )

    def test_splash_matches_portforge_exactly(self) -> None:
        self.assert_screen_matches_portforge("splash")

    def test_title_matches_portforge_exactly(self) -> None:
        self.assert_screen_matches_portforge("title")

    def test_menu_matches_portforge_exactly(self) -> None:
        self.assert_screen_matches_portforge("menu")


if __name__ == "__main__":
    unittest.main()
