# Copyright (C) 2020-2022  CEA/DEN, EDF R&D
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

from GeomAPI import *
from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.addParameter(Part_1_doc, "cir2", "22.225")
model.addParameter(Part_1_doc, "ext1", "3")
model.addParameter(Part_1_doc, "rib1", "13")
model.addParameter(Part_1_doc, "rib2", "13")
model.addParameter(Part_1_doc, "rib3", "13")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(193.5527547665121, 217.4086484957226, 25.4)
SketchCircle_2 = Sketch_1.addCircle(193.5527547665121, 217.4086484957226, 38)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchCircle_1.center(), SketchCircle_2.center())
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_2.results()[1], 38)
SketchConstraintRadius_2 = Sketch_1.setRadius(SketchCircle_1.results()[1], 25.4)
SketchLine_1 = Sketch_1.addLine(231.5527547665122, 217.4085749004042, 231.5527547665122, 96.40864849572259)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchCircle_2.results()[1])
SketchLine_2 = Sketch_1.addLine(212.5527547665122, 77.40864849572259, 79.55275476651215, 77.40864849572259)
SketchLine_3 = Sketch_1.addLine(79.55275476651215, 77.40864849572259, -35.01044899266839, 119.578215062531)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchArc_1 = Sketch_1.addArc(-28.44724523348785, 137.4086484957226, -35.01044899266839, 119.578215062531, -28.44724523348786, 156.4086484957226, True)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchArc_1.startPoint())
SketchLine_4 = Sketch_1.addLine(-28.44724523348786, 156.4086484957226, 96.55275476651215, 156.4086484957226)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(193.5527547665121, 217.4086484957226, 193.5527547665121, 96.40864849572259)
SketchLine_5.setAuxiliary(True)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchCircle_1.center(), SketchLine_5.startPoint())
SketchLine_6 = Sketch_1.addLine(193.5527547665121, 96.40864849572259, -186.9071792634899, 96.40864849572259)
SketchLine_6.setAuxiliary(True)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_5.result())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_6.result())
SketchConstraintTangent_1 = Sketch_1.setTangent(SketchLine_4.result(), SketchArc_1.results()[1])
SketchConstraintTangent_2 = Sketch_1.setTangent(SketchLine_3.result(), SketchArc_1.results()[1])
SketchCircle_3 = Sketch_1.addCircle(-28.44724523348785, 137.4086484957226, 7)
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchArc_1.center(), SketchCircle_3.center())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_2.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_1.result())
SketchArc_2 = Sketch_1.addArc(212.5527547665122, 96.40864849572259, 212.5527547665122, 77.40864849572259, 231.5527547665122, 96.40864849572259, False)
SketchArc_2.setName("SketchArc_3")
SketchArc_2.result().setName("SketchArc_3")
SketchArc_2.results()[1].setName("SketchArc_3_2")
SketchPoint_1 = Sketch_1.addPoint(231.5527547665122, 77.4086484957226)
SketchPoint_1.setAuxiliary(True)
SketchConstraintDistanceVertical_1 = Sketch_1.setVerticalDistance(SketchLine_5.startPoint(), SketchPoint_1.coordinates(), 140)
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchPoint_1.coordinates(), SketchLine_1.result())
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchPoint_1.coordinates(), SketchLine_2.result())
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchArc_2.startPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchArc_2.endPoint(), SketchLine_1.endPoint())
SketchConstraintTangent_3 = Sketch_1.setTangent(SketchArc_2.results()[1], SketchLine_1.result())
SketchConstraintTangent_4 = Sketch_1.setTangent(SketchArc_2.results()[1], SketchLine_2.result())
SketchConstraintRadius_3 = Sketch_1.setRadius(SketchArc_2.results()[1], 19)
SketchCircle_4 = Sketch_1.addCircle(212.5527547665122, 96.40864849572259, 7)
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchArc_2.center(), SketchCircle_4.center())
SketchCircle_5 = Sketch_1.addCircle(149.5527547665122, 137.4086484957226, 7)
SketchLine_7 = Sketch_1.addLine(149.5527547665122, 137.4086484957226, -36.17794869484125, 137.4086484957226)
SketchLine_7.setAuxiliary(True)
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchCircle_5.center(), SketchLine_7.startPoint())
SketchConstraintParallel_1 = Sketch_1.setParallel(SketchLine_7.result(), SketchLine_6.result())
SketchConstraintCoincidence_15 = Sketch_1.setCoincident(SketchArc_1.center(), SketchLine_7.result())
SketchCircle_6 = Sketch_1.addCircle(79.55275476651215, 96.4086484957226, 7)
SketchConstraintCoincidence_16 = Sketch_1.setCoincident(SketchLine_6.result(), SketchCircle_6.center())
SketchCircle_7 = Sketch_1.addCircle(-3.447245233487854, 124.4086484957226, 5)
SketchCircle_8 = Sketch_1.addCircle(174.5527547665122, 96.4086484957226, 5)
SketchConstraintCoincidence_17 = Sketch_1.setCoincident(SketchLine_6.result(), SketchCircle_8.center())
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchCircle_3.results()[1], SketchCircle_6.results()[1])
SketchConstraintEqual_2 = Sketch_1.setEqual(SketchCircle_6.results()[1], SketchCircle_5.results()[1])
SketchConstraintEqual_3 = Sketch_1.setEqual(SketchCircle_5.results()[1], SketchCircle_4.results()[1])
SketchConstraintDistanceHorizontal_1 = Sketch_1.setHorizontalDistance(SketchArc_1.center(), SketchCircle_4.center(), 241)
SketchConstraintRadius_4 = Sketch_1.setRadius(SketchCircle_3.results()[1], 7)
SketchConstraintEqual_4 = Sketch_1.setEqual(SketchCircle_8.results()[1], SketchCircle_7.results()[1])
SketchConstraintRadius_5 = Sketch_1.setRadius(SketchCircle_7.results()[1], 5)
SketchConstraintDistanceHorizontal_2 = Sketch_1.setHorizontalDistance(SketchLine_3.startPoint(), SketchCircle_3.center(), 108)
SketchConstraintDistanceHorizontal_3 = Sketch_1.setHorizontalDistance(SketchLine_3.startPoint(), SketchLine_7.startPoint(), 70)
SketchConstraintDistanceHorizontal_4 = Sketch_1.setHorizontalDistance(SketchCircle_8.center(), SketchLine_7.startPoint(), 25)
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchLine_3.startPoint(), SketchLine_6.result(), 19, True)
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchLine_7.result(), SketchCircle_6.center(), 41, True)
SketchLine_8 = Sketch_1.addLine(79.55275476651215, 77.40864849572259, 79.55275476651215, 115.4086484957226)
SketchLine_8.setAuxiliary(True)
SketchConstraintCoincidence_18 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_8.startPoint())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_8.result())
SketchConstraintMiddle_1 = Sketch_1.setMiddlePoint(SketchLine_8.result(), SketchCircle_6.center())
SketchConstraintDistance_3 = Sketch_1.setDistance(SketchCircle_7.center(), SketchLine_7.result(), 13, True)
SketchConstraintDistanceHorizontal_5 = Sketch_1.setHorizontalDistance(SketchArc_1.center(), SketchCircle_7.center(), 25)
SketchConstraintRadius_6 = Sketch_1.setRadius(SketchArc_1.results()[1], 19)
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_4.result())
SketchConstraintTangent_5 = Sketch_1.setTangent(SketchLine_1.result(), SketchCircle_2.results()[1])
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_4.result(), 125)
SketchArc_3 = Sketch_1.addArc(96.55275476651215, 215.5823348515451, 96.55275476651215, 156.4086484957226, 155.6041596876128, 219.3845360273737, False)
SketchArc_3.setName("SketchArc_5")
SketchArc_3.result().setName("SketchArc_5")
SketchArc_3.results()[1].setName("SketchArc_5_2")
SketchConstraintCoincidence_19 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchArc_3.startPoint())
SketchConstraintTangent_6 = Sketch_1.setTangent(SketchLine_4.result(), SketchArc_3.results()[1])
SketchConstraintCoincidence_20 = Sketch_1.setCoincident(SketchCircle_2.results()[1], SketchArc_3.endPoint())
model.do()
Sketch_1.changeFacesOrder([[SketchCircle_1.results()[1]],
                           [SketchCircle_2.results()[1], SketchCircle_2.results()[1], SketchArc_3.results()[1], SketchCircle_2.results()[1], SketchCircle_1.results()[1]],
                           [SketchCircle_4.results()[1]],
                           [SketchCircle_5.results()[1]],
                           [SketchCircle_6.results()[1]],
                           [SketchCircle_7.results()[1]],
                           [SketchCircle_8.results()[1]],
                           [SketchCircle_2.results()[1], SketchArc_3.results()[1]],
                           [SketchCircle_3.results()[1]],
                           [SketchArc_3.results()[1], SketchLine_4.result(), SketchArc_1.results()[1], SketchLine_3.result(), SketchLine_2.result(), SketchArc_2.results()[1], SketchLine_1.result(), SketchCircle_2.results()[1], SketchCircle_3.results()[1], SketchCircle_4.results()[1], SketchCircle_5.results()[1], SketchCircle_6.results()[1], SketchCircle_7.results()[1], SketchCircle_8.results()[1]]
                          ])
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchArc_5_2r-SketchLine_4r-SketchArc_1_2f-SketchLine_3r-SketchLine_2r-SketchArc_3_2f-SketchLine_1r-SketchCircle_2_2r-SketchCircle_3_2r-SketchCircle_4_2r-SketchCircle_5_2r-SketchCircle_6_2r-SketchCircle_7_2r-SketchCircle_8_2r"), model.selection("FACE", "Sketch_1/Face-SketchCircle_2_2f-SketchCircle_2_2f-SketchArc_5_2r-SketchCircle_2_2f-SketchCircle_1_2r")], model.selection(), 19, 0)
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_4"))
SketchLine_9 = Sketch_2.addLine(41.55275476651212, -117.4829288507397, 41.55275476651212, -28.90149327102993)
SketchLine_9.setAuxiliary(True)
SketchConstraintVertical_4 = Sketch_2.setVertical(SketchLine_9.result())
SketchLine_10 = Sketch_2.addLine(96.55275476651215, -19, 75.65570703399533, -109.515058097161)
SketchLine_11 = Sketch_2.addLine(7.449802499028871, -109.515058097161, -13.44724523348792, -19)
SketchLine_12 = Sketch_2.addLine(96.55275476651215, -19, -13.44724523348792, -19)
SketchConstraintCoincidence_21 = Sketch_2.setCoincident(SketchLine_10.startPoint(), SketchLine_12.startPoint())
SketchConstraintCoincidence_22 = Sketch_2.setCoincident(SketchLine_11.endPoint(), SketchLine_12.endPoint())
SketchArc_4 = Sketch_2.addArc(41.5527547665121, -101.6417711951257, 75.65570703399533, -109.515058097161, 7.449802499028871, -109.515058097161, True)
SketchArc_4.setName("SketchArc_4")
SketchArc_4.result().setName("SketchArc_4")
SketchArc_4.results()[1].setName("SketchArc_4_2")
SketchConstraintCoincidence_23 = Sketch_2.setCoincident(SketchLine_10.endPoint(), SketchArc_4.startPoint())
SketchConstraintCoincidence_24 = Sketch_2.setCoincident(SketchLine_11.startPoint(), SketchArc_4.endPoint())
SketchConstraintTangent_7 = Sketch_2.setTangent(SketchArc_4.results()[1], SketchLine_10.result())
SketchConstraintTangent_8 = Sketch_2.setTangent(SketchArc_4.results()[1], SketchLine_11.result())
SketchConstraintEqual_5 = Sketch_2.setEqual(SketchLine_10.result(), SketchLine_11.result())
SketchConstraintHorizontal_4 = Sketch_2.setHorizontal(SketchLine_12.result())
SketchConstraintAngle_1 = Sketch_2.setAngle(SketchLine_10.result(), SketchLine_12.result(), 283, type = "Backward")
SketchConstraintRadius_7 = Sketch_2.setRadius(SketchArc_4.results()[1], 35)
SketchProjection_1 = Sketch_2.addProjection(model.selection("EDGE", "Sketch_1/SketchLine_4"), True)
SketchLine_13 = SketchProjection_1.createdFeature()
SketchProjection_2 = Sketch_2.addProjection(model.selection("VERTEX", "Sketch_1/SketchLine_5_StartVertex"), True)
SketchPoint_2 = SketchProjection_2.createdFeature()
SketchPoint_3 = Sketch_2.addPoint(41.55275476651212, -19)
SketchConstraintCoincidence_25 = Sketch_2.setCoincident(SketchPoint_3.coordinates(), SketchLine_9.result())
SketchConstraintCoincidence_26 = Sketch_2.setCoincident(SketchLine_12.result(), SketchPoint_3.coordinates())
SketchProjection_3 = Sketch_2.addProjection(model.selection("VERTEX", "[Extrusion_1_1_1/Generated_Face&Sketch_1/SketchArc_5_2][Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_4][Extrusion_1_1_1/To_Face]"), False)
SketchPoint_4 = SketchProjection_3.createdFeature()
SketchConstraintCoincidence_27 = Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_4).coordinates(), SketchLine_12.startPoint())
SketchConstraintDistanceHorizontal_6 = Sketch_2.setHorizontalDistance(SketchAPI_Point(SketchPoint_2).coordinates(), SketchPoint_3.coordinates(), 152)
SketchConstraintMiddle_2 = Sketch_2.setMiddlePoint(SketchLine_12.result(), SketchPoint_3.coordinates())
SketchCircle_9 = Sketch_2.addCircle(41.5527547665121, -101.6417711951257, 22.225)
SketchCircle_9.setName("SketchCircle_12")
SketchCircle_9.result().setName("SketchCircle_12")
SketchCircle_9.results()[1].setName("SketchCircle_12_2")
SketchConstraintCoincidence_28 = Sketch_2.setCoincident(SketchArc_4.center(), SketchCircle_9.center())
SketchConstraintRadius_8 = Sketch_2.setRadius(SketchCircle_9.results()[1], 22.225)
model.do()
ExtrusionFuse_1 = model.addExtrusionFuse(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchLine_12f-SketchLine_11r-SketchArc_4_2f-SketchLine_10r-SketchCircle_12_2r")], model.selection(), 0, 19, [model.selection("SOLID", "Extrusion_1_1_1")])
Sketch_3 = model.addSketch(Part_1_doc, model.selection("FACE", "ExtrusionFuse_1_1_2/From_Face"))
SketchCircle_10 = Sketch_3.addCircle(26.55177397544429, 51.01394267771718, 23.65265081716653)
SketchCircle_10.setName("SketchCircle_10")
SketchCircle_10.result().setName("SketchCircle_10")
SketchCircle_10.results()[1].setName("SketchCircle_10_2")
model.do()
ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [model.selection("WIRE", "Sketch_3/Face-SketchCircle_10_2r_wire")], model.selection(), model.selection("FACE", "ExtrusionFuse_1_1_2/Modified_Face&ExtrusionFuse_1_1_1/To_Face&ExtrusionFuse_1_1_2/To_Face&ExtrusionFuse_1_1_3/To_Face"), 0, model.selection(), 0, [model.selection("COMPSOLID", "ExtrusionFuse_1_1")])

model.end()

model.testNbResults(ExtrusionCut_1, 1)
model.testNbSubResults(ExtrusionCut_1, [3])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.SOLID, [3])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.FACE, [32])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.EDGE, [158])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.VERTEX, [316])
model.testResultsVolumes(ExtrusionCut_1, [567425.6058])

assert(model.checkPythonDump())
