import unittest
import model
from TestSketcher import SketcherTestCase

class SketcherSetHorizontal(SketcherTestCase):
    def runTest(self):
        line = self.sketch.addLine(0, 0, 1, 1)
        self.sketch.setHorizontal(line)
        model.do()
        self.assertEqual(line.startPoint().y(), line.endPoint().y())

if __name__ == "__main__":
    unittest.main()