import unittest

import ModelAPI
import ModelHighAPI
import model

class FeaturesFixture(unittest.TestCase):

    def setUp(self):
        model.begin()
        # Create part
        partset = model.moduleDocument()
        self.part = model.addPart(partset).document()
        model.do()
        self.feature = model.addPoint(self.part, 0, 0, 0)

    def tearDown(self):
        model.end()
        model.reset()


class RefAttrTestCase(FeaturesFixture):

    def test_create_default(self):
        ModelHighAPI.ModelHighAPI_RefAttr()

    def test_create_from_attribute(self):
        print(self.feature.x())
        ModelHighAPI.ModelHighAPI_RefAttr(self.feature.x())

    def test_create_from_object(self):
        ModelHighAPI.ModelHighAPI_RefAttr(self.feature.feature())

    def test_create_from_None(self):
        ModelHighAPI.ModelHighAPI_RefAttr(None)


if __name__ == "__main__":
    unittest.main()
