from salome.shaper import model
import os

model.begin()
file_path = os.path.join(os.getenv("DATA_DIR"),"Shapes","Brep","box1.brep")
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Import_1 = model.addImport(Part_1_doc,file_path)
model.do()
    
### Create BoundingBox
Normal_1 = model.getNormal(Part_1_doc, model.selection("FACE", "box1_1/Shape_6"))
model.do()
model.end()
