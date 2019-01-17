## Copyright (C) 2014-2017  CEA/DEN, EDF R&D
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##
## See http:##www.salome-platform.org/ or
## email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
##

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Param_H = model.addParameter(Part_1_doc, "height", "3")
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [], model.selection(), 0, "height", [model.selection("SOLID", "Box_1_1")])
Sketch_1 = model.addSketch(Part_1_doc, model.selection("FACE", "Box_1_1/Top"))
SketchLine_1 = Sketch_1.addLine(6, 1, 4, 1)
SketchLine_1.setName("SketchLine_3")
SketchLine_1.result().setName("SketchLine_3")
SketchLine_2 = Sketch_1.addLine(5, -1, 4, 1)
SketchLine_2.setName("SketchLine_2")
SketchLine_2.result().setName("SketchLine_2")
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.endPoint())
SketchLine_3 = Sketch_1.addLine(5, -1, 6, 1)
SketchLine_3.setName("SketchLine_4")
SketchLine_3.result().setName("SketchLine_4")
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.startPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_3.endPoint())
ExtrusionCut_1.setNestedSketch(Sketch_1)
Group_1 = model.addGroup(Part_1_doc, [model.selection("FACE", "ExtrusionCut_1_1/Modified_Face&Box_1_1/Left")])
model.do()
groupSelectionList = Group_1.feature().selectionList("group_list")
assert(groupSelectionList.size() == 1)
# cut the face fully
Param_H.setValue(10)
model.do()
assert(groupSelectionList.size() == 2)
# merge the face back
Param_H.setValue(2)
# split again
Param_H.setValue(10)
model.do()
assert(groupSelectionList.size() == 2)
# merge the face back
Param_H.setValue(2)
model.end()
assert(groupSelectionList.size() == 1)

assert(model.checkPythonDump())
