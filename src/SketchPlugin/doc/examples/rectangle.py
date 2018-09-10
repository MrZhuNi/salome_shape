# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
Rectangle_1 = Sketch_1.addRectangle(5.598878525298213, 8.556502484736781, 31.35634838639225, 78.97905438572354)
model.do()
model.end()
