###
### SHAPER component
###

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
aPart1 = model.addParameter(Part_1_doc, "Doing", "123", "Longueur de la pièce")
try:
  aPart2 = model.addParameter(Part_1_doc, "Doing", "323", "Long")
except SyntaxError as anError:
  assert(anError != "")
  assert(str(anError).find("Variable name is not unique.") != -1)

model.end()
