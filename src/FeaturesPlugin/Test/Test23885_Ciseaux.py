# Copyright (C) 2021  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

import sys
import salome

salome.salome_init()
import salome_notebook
notebook = salome_notebook.NoteBook()
sys.path.insert(0, r'/export/home/Shaper/Build')

###
### SHAPER component
###

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

### Create Sketch
Sketch_1 = model.addSketch(Part_1_doc, model.standardPlane("XOY"))

### Create SketchProjection
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()

### Create SketchProjection
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_1 = SketchProjection_2.createdFeature()
SketchLine_1.setName("SketchLine_2")
SketchLine_1.result().setName("SketchLine_2")

### Create SketchArc
SketchArc_1 = Sketch_1.addArc(5.769187715431377, -24.3252229816311, 1.288150416501811, 0.2699046806844986, 5.769187715431377, -49.3252229816311, False)
Sketch_1.setRadius(SketchArc_1.results()[1], 25)

### Create SketchLine
SketchLine_2 = Sketch_1.addLine(5.769187715431377, -49.3252229816311, 110.7691877154314, -49.3252229816311)
SketchLine_2.setName("SketchLine_3")
SketchLine_2.result().setName("SketchLine_3")
Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_2.startPoint())
Sketch_1.setHorizontal(SketchLine_2.result())
Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_2.startPoint())

### Create SketchArc
SketchArc_2 = Sketch_1.addArc(110.7691877154314, -44.3252229816311, 110.7691877154314, -49.3252229816311, 115.7691877154314, -44.3252229816311, False)
Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchArc_2.startPoint())
Sketch_1.setRadius(SketchArc_2.results()[1], 5)
Sketch_1.setHorizontalDistance(SketchArc_2.endPoint(), SketchArc_1.endPoint(), 110)

### Create SketchLine
SketchLine_3 = Sketch_1.addLine(115.7691877154314, -44.3252229816311, 115.7691877154314, -29.3252229816311)
SketchLine_3.setName("SketchLine_4")
SketchLine_3.result().setName("SketchLine_4")
Sketch_1.setCoincident(SketchArc_2.endPoint(), SketchLine_3.startPoint())
Sketch_1.setVertical(SketchLine_3.result())

### Create SketchArc
SketchArc_3 = Sketch_1.addArc(110.7691877154314, -29.3252229816311, 115.7691877154314, -29.3252229816311, 110.7691877154314, -24.3252229816311, False)
Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchArc_3.startPoint())
Sketch_1.setRadius(SketchArc_3.results()[1], 5)
Sketch_1.setVerticalDistance(SketchArc_3.endPoint(), SketchLine_2.endPoint(), 25)

### Create SketchLine
SketchLine_4 = Sketch_1.addLine(110.7691877154314, -24.3252229816311, 98.79659432401075, -24.3252229816311)
SketchLine_4.setName("SketchLine_5")
SketchLine_4.result().setName("SketchLine_5")
Sketch_1.setCoincident(SketchArc_3.endPoint(), SketchLine_4.startPoint())
Sketch_1.setHorizontal(SketchLine_4.result())

### Create SketchArc
SketchArc_4 = Sketch_1.addArc(98.79659432401074, 5.674777018368899, 80.85123450211243, -18.3661060948322, 98.79659432401075, -24.3252229816311, False)
SketchArc_4.setName("SketchArc_5")
SketchArc_4.result().setName("SketchArc_5")
SketchArc_4.results()[1].setName("SketchArc_5_2")
Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchArc_4.endPoint())
Sketch_1.setRadius(SketchArc_4.results()[1], 30)
Sketch_1.setTangent(SketchArc_1.results()[1], SketchLine_2.result())
Sketch_1.setTangent(SketchLine_2.result(), SketchArc_2.results()[1])
Sketch_1.setTangent(SketchArc_2.results()[1], SketchLine_3.result())
Sketch_1.setTangent(SketchLine_3.result(), SketchArc_3.results()[1])
Sketch_1.setTangent(SketchArc_3.results()[1], SketchLine_4.result())
Sketch_1.setTangent(SketchLine_4.result(), SketchArc_4.results()[1])

### Create SketchArc
SketchArc_5 = Sketch_1.addArc(5.769187715431377, -24.3252229816311, 2.307649253892918, -9.730095319315502, 5.769187715431377, -39.3252229816311, False)
SketchArc_5.setName("SketchArc_6")
SketchArc_5.result().setName("SketchArc_6")
SketchArc_5.results()[1].setName("SketchArc_6_2")

### Create SketchLine
SketchLine_5 = Sketch_1.addLine(5.769187715431377, -39.3252229816311, 35.76918771543138, -39.3252229816311)
SketchLine_5.setName("SketchLine_6")
SketchLine_5.result().setName("SketchLine_6")
Sketch_1.setCoincident(SketchArc_5.endPoint(), SketchLine_5.startPoint())
Sketch_1.setHorizontal(SketchLine_5.result())
Sketch_1.setVerticalDistance(SketchArc_5.startPoint(), SketchArc_1.startPoint(), 10)
Sketch_1.setTangent(SketchArc_5.results()[1], SketchLine_5.result())
Sketch_1.setCoincident(SketchArc_5.center(), SketchArc_1.center())

### Create SketchPoint
SketchPoint_2 = Sketch_1.addPoint(170.7691877154314, -41.8252229816311)
Sketch_1.setHorizontalDistance(SketchArc_2.endPoint(), SketchPoint_2.coordinates(), 55)

### Create SketchProjection
SketchProjection_3 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_3 = SketchProjection_3.createdFeature()

### Create SketchProjection
SketchProjection_4 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_6 = SketchProjection_4.createdFeature()
SketchLine_6.setName("SketchLine_7")
SketchLine_6.result().setName("SketchLine_7")

### Create SketchProjection
SketchProjection_5 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_4 = SketchProjection_5.createdFeature()

### Create SketchProjection
SketchProjection_6 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_7 = SketchProjection_6.createdFeature()
SketchLine_7.setName("SketchLine_8")
SketchLine_7.result().setName("SketchLine_8")

### Create SketchProjection
SketchProjection_7 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchProjection_7.setName("SketchProjection_8")
SketchProjection_7.result().setName("SketchProjection_8")
SketchLine_8 = SketchProjection_7.createdFeature()
SketchLine_8.setName("SketchLine_9")
SketchLine_8.result().setName("SketchLine_9")

### Create SketchProjection
SketchProjection_8 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchProjection_8.setName("SketchProjection_9")
SketchProjection_8.result().setName("SketchProjection_9")
SketchPoint_5 = SketchProjection_8.createdFeature()
SketchPoint_5.setName("SketchPoint_6")
SketchPoint_5.result().setName("SketchPoint_6")

### Create SketchProjection
SketchProjection_9 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchProjection_9.setName("SketchProjection_10")
SketchProjection_9.result().setName("SketchProjection_10")
SketchLine_9 = SketchProjection_9.createdFeature()
SketchLine_9.setName("SketchLine_10")
SketchLine_9.result().setName("SketchLine_10")

### Create SketchProjection
SketchProjection_10 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchProjection_10.setName("SketchProjection_11")
SketchProjection_10.result().setName("SketchProjection_11")
SketchLine_10 = SketchProjection_10.createdFeature()
SketchLine_10.setName("SketchLine_11")
SketchLine_10.result().setName("SketchLine_11")

### Create SketchProjection
SketchProjection_11 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchProjection_11.setName("SketchProjection_12")
SketchProjection_11.result().setName("SketchProjection_12")
SketchLine_11 = SketchProjection_11.createdFeature()
SketchLine_11.setName("SketchLine_12")
SketchLine_11.result().setName("SketchLine_12")

### Create SketchCircle
SketchCircle_1 = Sketch_1.addCircle(170.7691877154314, -41.8252229816311, 5)
Sketch_1.setCoincident(SketchPoint_2.coordinates(), SketchCircle_1.center())
Sketch_1.setRadius(SketchCircle_1.results()[1], 5)

### Create SketchLine
SketchLine_12 = Sketch_1.addLine(115.7691877154314, -29.3252229816311, 315.7691877154314, -29.3252229816311)
SketchLine_12.setName("SketchLine_13")
SketchLine_12.result().setName("SketchLine_13")
Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_12.startPoint())
Sketch_1.setHorizontal(SketchLine_12.result())
Sketch_1.setLength(SketchLine_12.result(), 200)

### Create SketchArc
SketchArc_6 = Sketch_1.addArc(305.7691877154314, -29.3252229816311, 315.7691877154314, -29.3252229816311, 306.8253299980126, -39.26929475726426, True)
SketchArc_6.setName("SketchArc_9")
SketchArc_6.result().setName("SketchArc_9")
SketchArc_6.results()[1].setName("SketchArc_9_2")
Sketch_1.setCoincident(SketchLine_12.endPoint(), SketchArc_6.startPoint())
Sketch_1.setRadius(SketchArc_6.results()[1], 10)
Sketch_1.setCoincident(SketchArc_6.center(), SketchLine_12.result())

### Create SketchLine
SketchLine_13 = Sketch_1.addLine(306.8253299980126, -39.26929475726426, 143.7691877154314, -56.58719935256279)
SketchLine_13.setName("SketchLine_14")
SketchLine_13.result().setName("SketchLine_14")
Sketch_1.setCoincident(SketchArc_6.endPoint(), SketchLine_13.startPoint())

### Create SketchLine
SketchLine_14 = Sketch_1.addLine(129.0936803130578, -51.8252229816311, 120.7691877154314, -44.3252229816311)
SketchLine_14.setName("SketchLine_15")
SketchLine_14.result().setName("SketchLine_15")

### Create SketchLine
SketchLine_15 = Sketch_1.addLine(120.7691877154314, -44.3252229816311, 115.7691877154314, -44.3252229816311)
SketchLine_15.setName("SketchLine_16")
SketchLine_15.result().setName("SketchLine_16")
Sketch_1.setCoincident(SketchLine_14.endPoint(), SketchLine_15.startPoint())
Sketch_1.setCoincident(SketchArc_2.endPoint(), SketchLine_15.endPoint())

### Create SketchArc
SketchArc_7 = Sketch_1.addArc(141.7691877154314, -37.75626730933539, 129.0936803130578, -51.8252229816311, 143.7691877154314, -56.58719935256279, False)
SketchArc_7.setName("SketchArc_10")
SketchArc_7.result().setName("SketchArc_10")
SketchArc_7.results()[1].setName("SketchArc_10_2")
Sketch_1.setCoincident(SketchLine_14.startPoint(), SketchArc_7.startPoint())
Sketch_1.setCoincident(SketchLine_13.endPoint(), SketchArc_7.endPoint())
Sketch_1.setTangent(SketchArc_6.results()[1], SketchLine_13.result())
Sketch_1.setTangent(SketchArc_7.results()[1], SketchLine_13.result())
Sketch_1.setTangent(SketchArc_7.results()[1], SketchLine_14.result())
Sketch_1.setHorizontal(SketchLine_15.result())
Sketch_1.setHorizontalDistance(SketchArc_2.endPoint(), SketchLine_15.startPoint(), 5)
Sketch_1.setVerticalDistance(SketchLine_14.endPoint(), SketchLine_14.startPoint(), 7.5)
Sketch_1.setHorizontalDistance(SketchLine_3.startPoint(), SketchArc_7.endPoint(), 28)
Sketch_1.setHorizontalDistance(SketchArc_7.center(), SketchPoint_2.coordinates(), 29)

### Create SketchLine
SketchLine_16 = Sketch_1.addLine(314.934339105343, -33.3252229816311, 115.7691877154314, -33.3252229816311)
SketchLine_16.setName("SketchLine_17")
SketchLine_16.result().setName("SketchLine_17")
Sketch_1.setHorizontal(SketchLine_16.result())
Sketch_1.setCoincident(SketchLine_16.endPoint(), SketchLine_3.result())
Sketch_1.setCoincident(SketchLine_16.startPoint(), SketchArc_6.results()[1])
Sketch_1.setVerticalDistance(SketchLine_3.endPoint(), SketchLine_16.endPoint(), 4)

### Create SketchPoint
SketchPoint_6 = Sketch_1.addPoint(170.7691877154314, -29.3252229816311)
SketchPoint_6.setName("SketchPoint_8")
SketchPoint_6.result().setName("SketchPoint_8")
Sketch_1.setCoincident(SketchPoint_6.coordinates(), SketchLine_12.result())
Sketch_1.setCoincident(SketchPoint_6.coordinates(), SketchLine_12.result())
Sketch_1.setVerticalDistance(SketchPoint_6.coordinates(), SketchPoint_2.coordinates(), 12.5)
Sketch_1.setHorizontalDistance(SketchPoint_6.coordinates(), SketchLine_3.endPoint(), 55)

### Create SketchLine
SketchLine_17 = Sketch_1.addLine(5.769187715431377, -24.3252229816311, 35.76918771543138, -24.3252229816311)
SketchLine_17.setName("SketchLine_18")
SketchLine_17.result().setName("SketchLine_18")
SketchLine_17.setAuxiliary(True)
Sketch_1.setCoincident(SketchArc_1.center(), SketchLine_17.startPoint())
Sketch_1.setHorizontal(SketchLine_17.result())
Sketch_1.setHorizontalDistance(SketchArc_5.center(), SketchLine_17.endPoint(), 30)

### Create SketchArc
SketchArc_8 = Sketch_1.addArc(35.76918771543138, -24.3252229816311, 35.76918771543138, -39.3252229816311, 39.23072617696985, -9.730095319315502, False)
SketchArc_8.setName("SketchArc_11")
SketchArc_8.result().setName("SketchArc_11")
SketchArc_8.results()[1].setName("SketchArc_11_2")
Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchArc_8.startPoint())

### Create SketchArc
SketchArc_9 = Sketch_1.addArc(20.76918771543136, -87.57077618499868, 39.23072617696985, -9.730095319315502, 2.307649253892918, -9.730095319315502, False)
SketchArc_9.setName("SketchArc_12")
SketchArc_9.result().setName("SketchArc_12")
SketchArc_9.results()[1].setName("SketchArc_12_2")
Sketch_1.setCoincident(SketchArc_8.endPoint(), SketchArc_9.startPoint())
Sketch_1.setCoincident(SketchArc_5.startPoint(), SketchArc_9.endPoint())
Sketch_1.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchArc_1.results()[1])
Sketch_1.setTangent(SketchArc_9.results()[1], SketchArc_5.results()[1])
Sketch_1.setTangent(SketchArc_9.results()[1], SketchArc_8.results()[1])
Sketch_1.setTangent(SketchArc_8.results()[1], SketchLine_5.result())
Sketch_1.setCoincident(SketchLine_17.endPoint(), SketchArc_8.center())

### Create SketchArc
SketchArc_10 = Sketch_1.addArc(20.76918771543136, -87.57077618499868, 0, 0, 41.53838136414088, -1.407200488044206e-06, True)
SketchArc_10.setName("SketchArc_13")
SketchArc_10.result().setName("SketchArc_13")
SketchArc_10.results()[1].setName("SketchArc_13_2")
Sketch_1.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchArc_10.startPoint())

### Create SketchArc
SketchArc_11 = Sketch_1.addArc(16.84611780400845, -104.1119225319161, 41.53838136414088, -1.407200488044206e-06, 80.85123450211243, -18.3661060948322, True)
SketchArc_11.setName("SketchArc_14")
SketchArc_11.result().setName("SketchArc_14")
SketchArc_11.results()[1].setName("SketchArc_14_2")
Sketch_1.setCoincident(SketchArc_10.endPoint(), SketchArc_11.startPoint())
Sketch_1.setCoincident(SketchArc_4.startPoint(), SketchArc_11.endPoint())
Sketch_1.setTangent(SketchArc_1.results()[1], SketchArc_10.results()[1])
Sketch_1.setTangent(SketchArc_10.results()[1], SketchArc_11.results()[1])
Sketch_1.setTangent(SketchArc_11.results()[1], SketchArc_4.results()[1])
Sketch_1.setRadius(SketchArc_11.results()[1], 107)
Sketch_1.setCoincident(SketchArc_9.center(), SketchArc_10.center())
Sketch_1.setDistance(SketchArc_9.startPoint(), SketchArc_10.endPoint(), 10, True)
Sketch_1.setRadius(SketchArc_5.results()[1], 15)
Sketch_1.setRadius(SketchArc_10.results()[1], 90)
model.do()
Sketch_1.changeFacesOrder([[SketchLine_15.result(), SketchLine_14.result(), SketchArc_7.results()[1], SketchLine_13.result(), SketchArc_6.results()[1], SketchLine_16.result(), SketchLine_3.result(), SketchCircle_1.results()[1]],
                           [SketchLine_16.result(), SketchArc_6.results()[1], SketchLine_12.result(), SketchLine_3.result()],
                           [SketchArc_5.results()[1], SketchLine_5.result(), SketchArc_8.results()[1], SketchArc_9.results()[1]],
                           [SketchArc_1.results()[1], SketchLine_2.result(), SketchArc_2.results()[1], SketchLine_3.result(), SketchLine_3.result(), SketchArc_3.results()[1], SketchLine_4.result(), SketchArc_4.results()[1], SketchArc_11.results()[1], SketchArc_10.results()[1], SketchArc_9.results()[1], SketchArc_8.results()[1], SketchLine_5.result(), SketchArc_5.results()[1]],
                           [SketchCircle_1.results()[1]]
                          ])
model.do()

### Create Extrusion
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_16r-SketchLine_15r-SketchArc_10_2f-SketchLine_14r-SketchArc_9_2f-SketchLine_17f-SketchLine_4r-SketchCircle_1_2r"), model.selection("FACE", "Sketch_1/Face-SketchLine_17r-SketchArc_9_2f-SketchLine_13r-SketchLine_4r")], model.selection(), 3, 1)

### Create Extrusion
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchArc_1_2f-SketchLine_3f-SketchArc_2_2f-SketchLine_4f-SketchLine_4f-SketchArc_3_2f-SketchLine_5f-SketchArc_5_2r-SketchArc_14_2f-SketchArc_13_2f-SketchArc_12_2r-SketchArc_11_2r-SketchLine_6r-SketchArc_6_2r")], model.selection(), 6, 8)

### Create Fillet
Fillet_1 = model.addFillet(Part_1_doc, [model.selection("FACE", "Extrusion_2_1/To_Face")], 2, keepSubResults = True)

### Create Fillet
Fillet_2 = model.addFillet(Part_1_doc, [model.selection("FACE", "Extrusion_2_1/From_Face")], 2, keepSubResults = True)

### Create Extrusion
Extrusion_3 = model.addExtrusion(Part_1_doc, [model.selection("WIRE", "Sketch_1/Face-SketchLine_16r-SketchLine_15r-SketchArc_10_2f-SketchLine_14r-SketchArc_9_2f-SketchLine_17f-SketchLine_4r-SketchCircle_1_2r_wire_2")], model.selection(), 4, 6)

### Create Box
Box_1 = model.addBox(Part_1_doc, model.selection("VERTEX", "[Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_14][Extrusion_1_1_1/Generated_Face&Sketch_1/SketchArc_9_2][Extrusion_1_1_1/To_Face]"), model.selection("VERTEX", "[Extrusion_1_1_2/Generated_Face&Sketch_1/SketchArc_9_2][Extrusion_1_1_2/Generated_Face&Sketch_1/SketchLine_13][Extrusion_1_1_2/From_Face]"))

### Create Rotation
Rotation_1 = model.addRotation(Part_1_doc, [model.selection("SOLID", "Box_1_1")], axis = model.selection("EDGE", "PartSet/OX"), angle = -45, keepSubResults = True)

### Create Extrusion
Extrusion_4 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Rotation_1_1/MF:Rotated&Box_1_1/Front")], model.selection(), 300, 0)

### Create Translation
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("COMPOUND", "all-in-Extrusion_4")], vector = [-200, -6, -23], keepSubResults = True)

### Create Cut
Cut_1 = model.addCut(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1_2")], [model.selection("SOLID", "Translation_1_1_1")], keepSubResults = True)

### Create Fuse
Fuse_1_objects_1 = [model.selection("SOLID", "Cut_1_1_1"),
                    model.selection("SOLID", "Cut_1_1_2"),
                    model.selection("SOLID", "Fillet_2_1"),
                    model.selection("SOLID", "Extrusion_3_1")]
Fuse_1 = model.addFuse(Part_1_doc, Fuse_1_objects_1, removeEdges = True, keepSubResults = True)

### Create Symmetry
Symmetry_1 = model.addSymmetry(Part_1_doc, [model.selection("SOLID", "Fuse_1_1")], model.selection("EDGE", "PartSet/OX"), keepOriginal = True, keepSubResults = True)

### Create Sketch
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Symmetry_1_1_2/MF:Symmetried&Extrusion_3_1/From_Face"))

### Create SketchProjection
SketchProjection_12 = Sketch_2.addProjection(model.selection("EDGE", "[(Symmetry_1_1_2/MF:Symmetried&Extrusion_3_1/From_Face)(Symmetry_1_1_2/MF:Symmetried&Extrusion_1_1_2/From_Face&Extrusion_1_1_1/From_Face)][Symmetry_1_1_2/MF:Symmetried&Extrusion_3_1/From_Face]"), True)
SketchProjection_12.setName("SketchProjection_13")
SketchProjection_12.result().setName("SketchProjection_13")
SketchCircle_2 = SketchProjection_12.createdFeature()
model.do()

### Create ExtrusionCut
ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchProjection_13r")], model.selection(), [model.selection("SOLID", "Symmetry_1_1_2")])

### Create Translation
Translation_2 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "ExtrusionCut_1_1_2")], startPoint = model.selection("VERTEX", "[Symmetry_1_1_2/MF:Symmetried&Extrusion_1_1_2/From_Face&Extrusion_1_1_1/From_Face][ExtrusionCut_1_1_2/Generated_Face&Sketch_2/SketchProjection_13]"), endPoint = model.selection("VERTEX", "[(Symmetry_1_1_1/MF:Symmetried&Extrusion_3_1/From_Face)(Symmetry_1_1_1/MF:Symmetried&Extrusion_1_1_2/From_Face&Extrusion_1_1_1/From_Face)][Symmetry_1_1_1/MF:Symmetried&Extrusion_1_1_2/From_Face&Extrusion_1_1_1/From_Face]"), keepSubResults = True)

### Create Axis
Axis_4 = model.addAxis(Part_1_doc, model.selection("FACE", "(Symmetry_1_1_1/MF:Symmetried&Extrusion_1_1_1/To_Face)(Symmetry_1_1_1/MF:Symmetried&Extrusion_3_1/To_Face)"))

### Create Rotation
Rotation_2 = model.addRotation(Part_1_doc, [model.selection("SOLID", "Translation_2_1_1")], axis = model.selection("EDGE", "Axis_1"), angle = 30, keepSubResults = True)

### Create Fuse
Fuse_2 = model.addFuse(Part_1_doc, [model.selection("SOLID", "Rotation_2_1_1"), model.selection("SOLID", "Rotation_2_1_2")], removeEdges = True, keepSubResults = True)

model.testHaveNamingSubshapes(Fuse_2, model, Part_1_doc)

model.end()

#model.generateTests(Fuse_2, "Fuse_2")

from GeomAPI import GeomAPI_Shape

model.testNbResults(Fuse_2, 1)
model.testNbSubResults(Fuse_2, [0])
model.testNbSubShapes(Fuse_2, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Fuse_2, GeomAPI_Shape.FACE, [106])
model.testNbSubShapes(Fuse_2, GeomAPI_Shape.EDGE, [492])
model.testNbSubShapes(Fuse_2, GeomAPI_Shape.VERTEX, [984])
model.testResultsVolumes(Fuse_2, [128590.003649466452770866453647614])

from ModelAPI import *

aFactory = ModelAPI_Session.get().validators()

# Check that the features are not in error
for i in range(Part_1_doc.size("Features")):
  feature = objectToFeature(Part_1_doc.object("Features", i))
  name = feature.name()
  error = feature.error()
  assert(error == ''), "The feature {0} is in error: {1}".format(name, error)
  assert(aFactory.validate(feature)), "The feature {0} is in error: {1}".format(name, error)

assert(Part_1_doc.size("Groups") == 3)

#assert(model.checkPythonDump())
