import unittest

from config import examples

class MakeBrick3TestCase(unittest.TestCase):
    def test_main(self):
        examples.MakeBrick3.main()

if __name__ == "__main__":
    unittest.main()
