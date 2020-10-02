from salome.shaper import model

import tempfile

def getTmpFileName(ext):
  tempdir = tempfile.gettempdir()
  tmp_file = tempfile.NamedTemporaryFile(suffix="_%s"%ext, prefix='shaper_', dir=tempdir, delete=False)
  tmp_filename = tmp_file.name
  return tmp_filename

tmp_file1 = getTmpFileName("expRoot1")
tmp_file1 += ".C"

tmp_file2 = getTmpFileName("expRoot2")
tmp_file2 += ".root"

tmp_file3 = getTmpFileName("mat")
tmp_file3 += ".txt"

print(tmp_file1)
print(tmp_file2)
print(tmp_file3)

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Box_2 = model.addBox(Part_1_doc, 10, 10, 10)
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_2_1")], model.selection("EDGE", "PartSet/OX"), 20)
Group_1 = model.addGroup(Part_1_doc, "Solids", [model.selection("SOLID", "Box_1_1")])
Group_1.setName("air")
Group_1.result().setName("air")
model.do()

content = "mat vide vide 0. 0. 1\n"
content += "medium source SOURCE 1 vide\n"
content += "medium alu ALU 1 vide\n"
content += "medium steel STEEL 1 vide\n"
content += "medium eau EAU 1 vide\n"
content += "medium detects DETECTS 1 vide\n"
content += "medium air AIR 1 vide"
fic_mat = open(tmp_file3, "a")
fic_mat.write(content)
fic_mat.close()


Export_1 = model.exportToROOT(Part_1_doc, tmp_file1, "manager_name", "manager_title", tmp_file3, tmp_file2, model.selection("SOLID", "Box_1_1"))


model.end()


if salome.sg.hasDesktop():
  salome.sg.updateObjBrowser()
