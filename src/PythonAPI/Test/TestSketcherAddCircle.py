import unittest
from config import model
from TestSketcher import SketcherTestCase

class SketcherAddCircle(SketcherTestCase):    
    def test_add_cricle(self):
        circle = self.sketch.addCircle(0, 10, 20)
        model.do()
        self.assertEqual(circle.center().x(), 0.0)        
        self.assertEqual(circle.center().y(), 10.0)
        self.assertEqual(circle.radius(), 20.0)
        
    def test_modify_circle(self):
        circle = self.sketch.addCircle(0, 10, 20)
        model.do()
        circle.setCenter(10, 10)
        circle.setRadius(30)
        model.do()
        self.assertEqual(circle.center().x(), 10.0)        
        self.assertEqual(circle.center().y(), 10.0)
        self.assertEqual(circle.radius(), 30.0)
        
    
if __name__ == "__main__":
    unittest.main(verbosity=2)