from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Fillet_1 = model.model.addFilletMultiRadiusBycurvAbs
                (Part_1_doc, [model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Top]")],
                 [0, 0.5, 1],[1, 0.5, 1], keepSubResults = True)
model.do()
model.end()
