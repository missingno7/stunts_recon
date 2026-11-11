import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))


class PromoteImports(unittest.TestCase):
    def test_publish_path_uses_transaction_prepare(self):
        # A same-named import (e.g. preprocessor.prepare) must not shadow the
        # journaled publisher's prepare(changes); --verify-only never reaches it.
        import promote
        import transaction
        self.assertIs(promote.prepare, transaction.prepare)


if __name__ == '__main__':
    unittest.main()
