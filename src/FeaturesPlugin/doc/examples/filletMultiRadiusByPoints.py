from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
### Create Point
Point_1 = model.addPoint(Part_1_doc, model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Top]"), 0.5, True, False)
Point_2 = model.addPoint(Part_1_doc, model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Top]"), 0.2, True, False)
Fillet_1 = model.addFilletMultiRadiusByPoints(Part_1_doc, 
                model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Top]"), 
                [model.selection("VERTEX", "Point_1"), model.selection("VERTEX", "Point_2")], 
                [1, 0.5, 0.5, 2], keepSubResults = True)

model.do()
model.end()
