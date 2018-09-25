# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Compound_1_objects = [model.selection("FACE", "Box_1_1/Front"),
                      model.selection("EDGE", "Box_1_1/Left&Box_1_1/Top"),
                      model.selection("VERTEX", "Box_1_1/Back&Box_1_1/Right&Box_1_1/Top"),
                      model.selection("EDGE", "Box_1_1/Back&Box_1_1/Top")]
Compound_1 = model.addCompound(Part_1_doc, Compound_1_objects)
model.do()
model.end()
