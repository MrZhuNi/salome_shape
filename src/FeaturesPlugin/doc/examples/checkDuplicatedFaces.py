from salome.shaper import model
import os

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
### Create Box
Box_1 = model.addBox(Part_1_doc, 100, 50, 100)
### Create Point
Point_1 = model.addPoint(Part_1_doc, 100, 0, 0)
### Create Point
Point_2 = model.addPoint(Part_1_doc, 250, 50, 100)
### Create Box
Box_2 = model.addBox(Part_1_doc, model.selection("VERTEX", "all-in-Point_1"), model.selection("VERTEX", "all-in-Point_2"))
### Create Point
Point_3 = model.addPoint(Part_1_doc, 100, 50, 250)
### Create Box
Box_3 = model.addBox(Part_1_doc, model.selection("VERTEX", "[Box_1_1/Back][Box_1_1/Left][Box_1_1/Top]"), model.selection("VERTEX", "Point_3"))
### Create Compound
Compound_1_objects = [model.selection("SOLID", "Box_1_1"), 
                      model.selection("SOLID", "Box_2_1"), 
                      model.selection("SOLID", "Box_3_1")]
Compound_1 = model.addCompound(Part_1_doc, Compound_1_objects)
### Create Duplicated_faces
Duplicated_faces_1 = model.getDuplicatedFaces(Part_1_doc, model.selection("COMPOUND", "Compound_1_1"), 50, 0.001, "mygroup")

model.do()
model.end()
