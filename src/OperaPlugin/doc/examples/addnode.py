from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

Box_1 = model.addBox(Part_1_doc, 0, 0, 0, 20, 20, 20)
Box_2 = model.addBox(Part_1_doc, 0, 0, 0, 10, 10, 10)

AddNode_1 = model.addAddNode(Part_1_doc, model.selection("SOLID", "Box_1_1"), [model.selection("SOLID", "Box_2_1")])

model.do()
model.end()
