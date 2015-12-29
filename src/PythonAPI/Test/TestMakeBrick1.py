import unittest

from config import examples

class MakeBrick1TestCase(unittest.TestCase):
    def test_main(self):
        examples.MakeBrick1.main()

if __name__ == "__main__":
    unittest.main()
