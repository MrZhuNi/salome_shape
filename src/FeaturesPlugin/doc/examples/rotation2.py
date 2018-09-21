# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Cone_1 = model.addCone(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 10, 5, 10)
Rotation_1 = model.addRotation(Part_1_doc, [model.selection("SOLID", "Cone_1_1")], model.selection("VERTEX", "PartSet/Origin"), model.selection("VERTEX", "Cone_1_1/Face_1&Cone_1_1/Face_3"), model.selection("VERTEX", "Cone_1_1/Face_1&Cone_1_1/Face_2"))
model.do()
model.end()
