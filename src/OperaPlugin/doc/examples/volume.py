from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Volume_1 = model.addVolume(Part_1_doc, "Air", [model.selection("SOLID", "Box_1_1")])

model.do()
model.end()
