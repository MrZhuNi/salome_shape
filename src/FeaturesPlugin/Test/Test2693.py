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

# -*- coding: utf-8 -*-

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
model.addParameter(partSet, "hh", "20")
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-17.23654191510748, 16.36770059625795, -4.760492650097712, 19.4251823602345)
SketchLine_2 = Sketch_1.addLine(20, 2.414080216212369e-15, 20, -31)
SketchLine_3 = Sketch_1.addLine(2.928932188134523, -38.07106781186548, -8.677669529663685, -26.46446609406726)
SketchLine_4 = Sketch_1.addLine(-12.21320343559642, -25, -20.00000000000047, -25)
SketchArc_1 = Sketch_1.addArc(10, -31, 20, -31, 2.928932188134523, -38.07106781186548, True)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchArc_1.startPoint())
SketchConstraintCoincidence_1.setName("SketchConstraintCoincidence_2")
SketchConstraintTangent_1 = Sketch_1.setTangent(SketchLine_2.result(), SketchArc_1.results()[1])
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_2.setName("SketchConstraintCoincidence_3")
SketchConstraintTangent_2 = Sketch_1.setTangent(SketchLine_3.result(), SketchArc_1.results()[1])
SketchArc_2 = Sketch_1.addArc(-12.21320343559642, -30, -8.677669529663685, -26.46446609406726, -12.21320343559642, -25, False)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchArc_2.startPoint(), SketchLine_3.endPoint())
SketchConstraintCoincidence_3.setName("SketchConstraintCoincidence_4")
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchArc_2.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_4.setName("SketchConstraintCoincidence_5")
SketchConstraintTangent_3 = Sketch_1.setTangent(SketchArc_2.results()[1], SketchLine_4.result())
SketchConstraintTangent_4 = Sketch_1.setTangent(SketchArc_2.results()[1], SketchLine_3.result())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_4.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchArc_3 = Sketch_1.addArc(5.671735138196638e-24, 3.298692290329323e-23, -4.760492650097712, 19.4251823602345, 20, 2.414080216212369e-15, True)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchArc_3.startPoint())
SketchConstraintCoincidence_5.setName("SketchConstraintCoincidence_6")
SketchConstraintTangent_5 = Sketch_1.setTangent(SketchLine_1.result(), SketchArc_3.results()[1])
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchArc_3.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_6.setName("SketchConstraintCoincidence_7")
SketchConstraintTangent_6 = Sketch_1.setTangent(SketchArc_3.results()[1], SketchLine_2.result())
SketchArc_4 = Sketch_1.addArc(-12, -5, -22.35294117647118, -24.41176470588231, -17.23654191510748, 16.36770059625795, True)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchArc_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_7.setName("SketchConstraintCoincidence_9")
SketchConstraintTangent_7 = Sketch_1.setTangent(SketchArc_4.results()[1], SketchLine_1.result())
SketchConstraintTangent_7.setName("SketchConstraintTangent_8")
SketchCircle_1 = Sketch_1.addCircle(-22, -5, 7.5)
SketchCircle_2 = Sketch_1.addCircle(5.671735138196638e-24, 3.298692290329323e-23, 10)
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchArc_3.center(), SketchCircle_2.center())
SketchConstraintCoincidence_8.setName("SketchConstraintCoincidence_10")
SketchCircle_3 = Sketch_1.addCircle(7, -25, 7.5)
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_1.results()[1], 10)
SketchConstraintRadius_1.setName("SketchConstraintRadius_2")
SketchConstraintAngle_1 = Sketch_1.setAngle(SketchLine_4.result(), SketchLine_3.result(), 134.9999999999999)
SketchConstraintRadius_2 = Sketch_1.setRadius(SketchArc_2.results()[1], 5)
SketchConstraintRadius_2.setName("SketchConstraintRadius_3")
SketchConstraintRadius_3 = Sketch_1.setRadius(SketchArc_4.results()[1], 22)
SketchConstraintRadius_3.setName("SketchConstraintRadius_4")
SketchConstraintRadius_4 = Sketch_1.setRadius(SketchArc_3.results()[1], 20)
SketchConstraintRadius_4.setName("SketchConstraintRadius_5")
SketchPoint_1 = Sketch_1.addPoint(-20.00000000000047, -25)
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchPoint_1.coordinates(), SketchLine_4.endPoint())
SketchConstraintCoincidence_9.setName("SketchConstraintCoincidence_12")
SketchArc_5 = Sketch_1.addArc(-20.00000000000047, -20, -20.00000000000047, -25, -22.35294117647118, -24.41176470588231, True)
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchArc_5.startPoint())
SketchConstraintCoincidence_10.setName("SketchConstraintCoincidence_11")
SketchConstraintTangent_8 = Sketch_1.setTangent(SketchLine_4.result(), SketchArc_5.results()[1])
SketchConstraintTangent_8.setName("SketchConstraintTangent_9")
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchArc_4.startPoint(), SketchArc_5.endPoint())
SketchConstraintCoincidence_11.setName("SketchConstraintCoincidence_13")
SketchConstraintDistanceHorizontal_1 = Sketch_1.setHorizontalDistance(SketchArc_4.center(), SketchCircle_2.center(), 12)
SketchConstraintDistanceHorizontal_2 = Sketch_1.setHorizontalDistance(SketchCircle_1.center(), SketchArc_3.center(), 22)
SketchConstraintDistanceVertical_1 = Sketch_1.setVerticalDistance(SketchArc_3.center(), SketchArc_4.center(), 5)
SketchLine_5 = Sketch_1.addLine(-22, -5, -12, -5)
SketchLine_5.setAuxiliary(True)
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchCircle_1.center(), SketchLine_5.startPoint())
SketchConstraintCoincidence_12.setName("SketchConstraintCoincidence_14")
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchArc_4.center(), SketchLine_5.endPoint())
SketchConstraintCoincidence_13.setName("SketchConstraintCoincidence_15")
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_5.result())
SketchConstraintRadius_5 = Sketch_1.setRadius(SketchCircle_1.results()[1], 7.5)
SketchConstraintRadius_5.setName("SketchConstraintRadius_6")
SketchConstraintDistanceHorizontal_3 = Sketch_1.setHorizontalDistance(SketchCircle_2.center(), SketchCircle_3.center(), 7)
SketchConstraintDistanceVertical_2 = Sketch_1.setVerticalDistance(SketchCircle_2.center(), SketchCircle_3.center(), 25)
SketchConstraintDistanceVertical_3 = Sketch_1.setVerticalDistance(SketchArc_3.center(), SketchArc_1.center(), 31)
SketchConstraintRadius_6 = Sketch_1.setRadius(SketchCircle_3.results()[1], 7.5)
SketchConstraintRadius_6.setName("SketchConstraintRadius_7")
SketchConstraintRadius_7 = Sketch_1.setRadius(SketchCircle_2.results()[1], 10)
SketchConstraintRadius_7.setName("SketchConstraintRadius_8")
SketchConstraintRadius_8 = Sketch_1.setRadius(SketchArc_5.results()[1], 5)
SketchConstraintRadius_8.setName("SketchConstraintRadius_9")
SketchConstraintTangent_9 = Sketch_1.setTangent(SketchArc_4.results()[1], SketchArc_5.results()[1])
SketchConstraintTangent_9.setName("SketchConstraintTangent_10")
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_2 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchArc_3.center(), SketchAPI_Point(SketchPoint_2).coordinates())
SketchConstraintCoincidence_14.setName("SketchConstraintCoincidence_16")
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchLine_5.startPoint(), SketchLine_4.result(), 20, True)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchArc_4_2f-SketchArc_5_2f-SketchLine_4r-SketchArc_2_2r-SketchLine_3r-SketchArc_1_2f-SketchLine_2r-SketchArc_3_2f-SketchLine_1r-SketchCircle_1_2r-SketchCircle_2_2r-SketchCircle_3_2r")], model.selection(), 6.5, 0)
ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [], model.selection(), 0, 100, [model.selection("SOLID", "Extrusion_1_1")])
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_2"))
SketchLine_6 = Sketch_2.addLine(12.25, 6.5, -12.25, 6.5)
SketchProjection_2 = Sketch_2.addProjection(model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_2][Extrusion_1_1/To_Face]"), False)
SketchLine_7 = SketchProjection_2.createdFeature()
SketchConstraintCoincidence_15 = Sketch_2.setCoincident(SketchLine_6.endPoint(), SketchLine_7.result())
SketchConstraintCoincidence_15.setName("SketchConstraintCoincidence_17")
SketchLine_8 = Sketch_2.addLine(-12.25, 6.5, -12.25, 4.5)
SketchLine_9 = Sketch_2.addLine(-12.25, 4.5, 12.25, 4.5)
SketchLine_10 = Sketch_2.addLine(12.25, 4.5, 12.25, 6.5)
SketchConstraintCoincidence_16 = Sketch_2.setCoincident(SketchLine_10.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_16.setName("SketchConstraintCoincidence_18")
SketchConstraintCoincidence_17 = Sketch_2.setCoincident(SketchLine_6.endPoint(), SketchLine_8.startPoint())
SketchConstraintCoincidence_17.setName("SketchConstraintCoincidence_19")
SketchConstraintCoincidence_18 = Sketch_2.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchConstraintCoincidence_18.setName("SketchConstraintCoincidence_20")
SketchConstraintCoincidence_19 = Sketch_2.setCoincident(SketchLine_9.endPoint(), SketchLine_10.startPoint())
SketchConstraintCoincidence_19.setName("SketchConstraintCoincidence_21")
SketchConstraintHorizontal_3 = Sketch_2.setHorizontal(SketchLine_6.result())
SketchConstraintVertical_2 = Sketch_2.setVertical(SketchLine_8.result())
SketchConstraintHorizontal_4 = Sketch_2.setHorizontal(SketchLine_9.result())
SketchConstraintVertical_3 = Sketch_2.setVertical(SketchLine_10.result())
SketchConstraintLength_1 = Sketch_2.setLength(SketchLine_9.result(), 24.5)
SketchConstraintLength_2 = Sketch_2.setLength(SketchLine_8.result(), 2)
SketchPoint_3 = Sketch_2.addPoint(2.442490654175344e-15, 6.499999999999999)
SketchConstraintCoincidence_20 = Sketch_2.setCoincident(SketchPoint_3.coordinates(), SketchLine_6.result())
SketchConstraintCoincidence_20.setName("SketchConstraintCoincidence_22")
SketchConstraintMiddle_1 = Sketch_2.setMiddlePoint(SketchPoint_3.coordinates(), SketchLine_6.result())
SketchProjection_3 = Sketch_2.addProjection(model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_2][Extrusion_1_1/Generated_Face&Sketch_1/SketchArc_3_2]"), False)
SketchLine_11 = SketchProjection_3.createdFeature()
SketchConstraintCoincidence_21 = Sketch_2.setCoincident(SketchPoint_3.coordinates(), SketchLine_11.result())
SketchConstraintCoincidence_21.setName("SketchConstraintCoincidence_23")
ExtrusionCut_1.setNestedSketch(Sketch_2)
model.do()
model.testHaveNamingSubshapes(ExtrusionCut_1, model, Part_1_doc)
model.end()

assert(model.checkPythonDump())