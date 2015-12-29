import unittest

from config import examples

class PlatineTestCase(unittest.TestCase):
    def test_main(self):
        examples.Platine.main()

if __name__ == "__main__":
    unittest.main()
