import unittest

from config import examples

class MakeBrick2TestCase(unittest.TestCase):
    def test_main(self):
        examples.MakeBrick2.main()

if __name__ == "__main__":
    unittest.main()
