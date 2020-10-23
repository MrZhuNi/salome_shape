from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Import_1 = model.addImportStep(Part_1_doc,
              "/SOURCES/SAMPLES/Shapes/Step/black_and_white.step", True, True, True)
model.do()
model.end()
