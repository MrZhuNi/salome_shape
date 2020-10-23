from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Export_1 = model.exportToSTL(Part_1_doc, "/dn48/newgeom/vsv/test.stl",
                              [model.selection("SOLID", "Box_1_1")],0.0001,0.5,"True","False")
model.do()
model.end()
