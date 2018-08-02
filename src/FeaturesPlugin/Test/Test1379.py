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
Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOZ"))
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "OZ"), False)
SketchLine_1 = SketchProjection_1.createdFeature()
SketchLine_1.setName("SketchLine_5")
SketchLine_1.result().setName("SketchLine_5")
SketchLine_2 = Sketch_1.addLine(0.008, 0.0005000000000000006, 0.0075, -1.258295422426014e-035)
SketchLine_2.setName("SketchLine_9")
SketchLine_2.result().setName("SketchLine_9")
SketchLine_2.result().setColor(0, 150, 0)
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "OX"), False)
SketchLine_3 = SketchProjection_2.createdFeature()
SketchLine_3.setName("SketchLine_10")
SketchLine_3.result().setName("SketchLine_10")
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.result())
SketchConstraintCoincidence_1.setName("SketchConstraintCoincidence_12")
SketchLine_4 = Sketch_1.addLine(0.0075, -1.258295422426014e-035, 0.005, 0)
SketchLine_4.setName("SketchLine_11")
SketchLine_4.result().setName("SketchLine_11")
SketchLine_4.result().setColor(0, 150, 0)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_2.setName("SketchConstraintCoincidence_13")
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_3.result())
SketchConstraintCoincidence_3.setName("SketchConstraintCoincidence_14")
SketchLine_5 = Sketch_1.addLine(0.005, 0, 0.005, 0.02)
SketchLine_5.setName("SketchLine_12")
SketchLine_5.result().setName("SketchLine_12")
SketchLine_5.result().setColor(0, 150, 0)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_4.setName("SketchConstraintCoincidence_15")
SketchLine_6 = Sketch_1.addLine(0.005, 0.02, 0, 0.02)
SketchLine_6.setName("SketchLine_13")
SketchLine_6.result().setName("SketchLine_13")
SketchLine_6.result().setColor(0, 150, 0)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_5.setName("SketchConstraintCoincidence_16")
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_5.result())
SketchConstraintVertical_1.setName("SketchConstraintVertical_3")
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_6.result())
SketchConstraintHorizontal_1.setName("SketchConstraintHorizontal_3")
SketchConstraintAngle_1 = Sketch_1.setAngle(SketchLine_3.result(), SketchLine_2.result(), 45)
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_5.result(), 0.02)
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_6.result(), 0.005)
SketchConstraintLength_2.setName("SketchConstraintLength_4")
SketchLine_7 = Sketch_1.addLine(0.008, 0.0005000000000000006, 0.008, 0.03)
SketchLine_7.setName("SketchLine_7")
SketchLine_7.result().setName("SketchLine_7")
SketchLine_7.result().setColor(0, 150, 0)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_2.startPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_6.setName("SketchConstraintCoincidence_6")
SketchLine_8 = Sketch_1.addLine(0.008, 0.03, 0.007500000000000001, 0.03)
SketchLine_8.setName("SketchLine_8")
SketchLine_8.result().setName("SketchLine_8")
SketchLine_8.result().setColor(0, 150, 0)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintCoincidence_7.setName("SketchConstraintCoincidence_7")
SketchArc_1 = Sketch_1.addArc(0.007500000000000001, 0.031, 0.007500000000000001, 0.03, 0.006500000000000001, 0.031, True)
SketchArc_1.result().setColor(0, 150, 0)
SketchArc_1.results()[1].setColor(0, 150, 0)
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchArc_1.startPoint())
SketchConstraintCoincidence_8.setName("SketchConstraintCoincidence_8")
SketchLine_9 = Sketch_1.addLine(0.006500000000000001, 0.031, 0.006500000000000001, 0.032)
SketchLine_9.setName("SketchLine_14")
SketchLine_9.result().setName("SketchLine_14")
SketchLine_9.result().setColor(0, 150, 0)
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_9.startPoint())
SketchConstraintCoincidence_9.setName("SketchConstraintCoincidence_9")
SketchArc_2 = Sketch_1.addArc(0.007500000000000001, 0.032, 0.006500000000000001, 0.032, 0.007500000000000001, 0.033, True)
SketchArc_2.result().setColor(0, 150, 0)
SketchArc_2.results()[1].setColor(0, 150, 0)
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchLine_9.endPoint(), SketchArc_2.startPoint())
SketchConstraintCoincidence_10.setName("SketchConstraintCoincidence_10")
SketchLine_10 = Sketch_1.addLine(0.007500000000000001, 0.033, 0.008999999999999999, 0.033)
SketchLine_10.setName("SketchLine_15")
SketchLine_10.result().setName("SketchLine_15")
SketchLine_10.result().setColor(0, 150, 0)
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchArc_2.endPoint(), SketchLine_10.startPoint())
SketchConstraintCoincidence_11.setName("SketchConstraintCoincidence_11")
SketchLine_11 = Sketch_1.addLine(0.008999999999999999, 0.033, 0.008999999999999999, 0.042)
SketchLine_11.setName("SketchLine_16")
SketchLine_11.result().setName("SketchLine_16")
SketchLine_11.result().setColor(0, 150, 0)
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchLine_10.endPoint(), SketchLine_11.startPoint())
SketchConstraintCoincidence_12.setName("SketchConstraintCoincidence_17")
SketchLine_12 = Sketch_1.addLine(0.008999999999999999, 0.042, -5.446629538236431e-036, 0.042)
SketchLine_12.setName("SketchLine_17")
SketchLine_12.result().setName("SketchLine_17")
SketchLine_12.result().setColor(0, 150, 0)
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchLine_11.endPoint(), SketchLine_12.startPoint())
SketchConstraintCoincidence_13.setName("SketchConstraintCoincidence_18")
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchLine_12.endPoint(), SketchLine_1.result())
SketchConstraintCoincidence_14.setName("SketchConstraintCoincidence_19")
SketchLine_13 = Sketch_1.addLine(-5.446629538236431e-036, 0.042, 0, 0.02)
SketchLine_13.setName("SketchLine_18")
SketchLine_13.result().setName("SketchLine_18")
SketchLine_13.result().setColor(0, 150, 0)
SketchConstraintCoincidence_15 = Sketch_1.setCoincident(SketchLine_12.endPoint(), SketchLine_13.startPoint())
SketchConstraintCoincidence_15.setName("SketchConstraintCoincidence_20")
SketchConstraintCoincidence_16 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_13.endPoint())
SketchConstraintCoincidence_16.setName("SketchConstraintCoincidence_21")
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_12.result())
SketchConstraintHorizontal_2.setName("SketchConstraintHorizontal_2")
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_10.result())
SketchConstraintHorizontal_3.setName("SketchConstraintHorizontal_4")
SketchConstraintHorizontal_4 = Sketch_1.setHorizontal(SketchLine_8.result())
SketchConstraintHorizontal_4.setName("SketchConstraintHorizontal_5")
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_11.result())
SketchConstraintVertical_2.setName("SketchConstraintVertical_2")
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_7.result())
SketchConstraintVertical_3.setName("SketchConstraintVertical_4")
SketchConstraintVertical_4 = Sketch_1.setVertical(SketchLine_9.result())
SketchConstraintVertical_4.setName("SketchConstraintVertical_5")
SketchConstraintTangent_1 = Sketch_1.setTangent(SketchArc_2.results()[1], SketchLine_9.result())
SketchConstraintTangent_2 = Sketch_1.setTangent(SketchArc_1.results()[1], SketchLine_9.result())
SketchConstraintTangent_3 = Sketch_1.setTangent(SketchLine_10.result(), SketchArc_2.results()[1])
SketchConstraintTangent_4 = Sketch_1.setTangent(SketchArc_1.results()[1], SketchLine_8.result())
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchLine_2.startPoint(), SketchLine_1.result(), 0.008, True)
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchLine_4.startPoint(), SketchLine_7.result(), 0.0005, True)
SketchConstraintDistance_3 = Sketch_1.setDistance(SketchLine_9.endPoint(), SketchLine_13.result(), 0.0065, True)
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_1.results()[1], 0.001)
SketchConstraintRadius_2 = Sketch_1.setRadius(SketchArc_2.results()[1], 0.001)
SketchConstraintDistance_4 = Sketch_1.setDistance(SketchArc_2.endPoint(), SketchArc_1.startPoint(), 0.003, True)
SketchConstraintLength_3 = Sketch_1.setLength(SketchLine_11.result(), 0.008999999999999999)
SketchConstraintLength_3.setName("SketchConstraintLength_3")
SketchConstraintDistance_5 = Sketch_1.setDistance(SketchLine_13.startPoint(), SketchLine_3.result(), 0.042, True)
SketchConstraintLength_4 = Sketch_1.setLength(SketchLine_12.result(), 0.008999999999999999)
SketchConstraintLength_4.setName("SketchConstraintLength_5")
SketchConstraintCoincidence_17 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_1.result())
SketchConstraintCoincidence_17.setName("SketchConstraintCoincidence_22")
model.do()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Revolution_1 = model.addRevolution(Part_1_doc, [model.selection("FACE", "PartSet/Sketch_1/Face-SketchLine_9r-SketchLine_11r-SketchLine_12r-SketchLine_13r-SketchLine_7f-SketchLine_8f-SketchArc_1_2r-SketchLine_14f-SketchArc_2_2r-SketchLine_15f-SketchLine_16f-SketchLine_17f-SketchLine_18f")], model.selection("EDGE", "PartSet/OZ"), 360, 0)
Sketch_2 = model.addSketch(Part_1_doc, model.standardPlane("XOZ"))
SketchLine_14 = Sketch_2.addLine(-5.446629538236431e-036, 0.042, 5.172728347136208e-022, 0.03963932022500211)
SketchLine_14.result().setColor(0, 150, 0)
SketchProjection_3 = Sketch_2.addProjection(model.selection("VERTEX", "PartSet/Sketch_1/Vertex-SketchLine_18s-SketchLine_17e"), False)
SketchPoint_1 = SketchProjection_3.createdFeature()
SketchConstraintCoincidence_18 = Sketch_2.setCoincident(SketchLine_14.startPoint(), SketchPoint_1.result())
SketchProjection_4 = Sketch_2.addProjection(model.selection("EDGE", "PartSet/Sketch_1/Edge-SketchLine_18"), False)
SketchLine_15 = SketchProjection_4.createdFeature()
SketchConstraintCoincidence_19 = Sketch_2.setCoincident(SketchLine_14.endPoint(), SketchLine_15.result())
SketchArc_3 = Sketch_2.addArc(0.02, 0.062, 5.172728347136208e-022, 0.03963932022500211, 0.02, 0.032, False)
SketchArc_3.result().setColor(0, 150, 0)
SketchArc_3.results()[1].setColor(0, 150, 0)
SketchConstraintCoincidence_20 = Sketch_2.setCoincident(SketchLine_14.endPoint(), SketchArc_3.startPoint())
SketchLine_16 = Sketch_2.addLine(0.02, 0.032, 0.02, 0.042)
SketchLine_16.result().setColor(0, 150, 0)
SketchConstraintCoincidence_21 = Sketch_2.setCoincident(SketchArc_3.endPoint(), SketchLine_16.startPoint())
SketchLine_17 = Sketch_2.addLine(0.02, 0.042, 0, 0.042)
SketchLine_17.result().setColor(0, 150, 0)
SketchConstraintCoincidence_22 = Sketch_2.setCoincident(SketchLine_16.endPoint(), SketchLine_17.startPoint())
SketchConstraintCoincidence_23 = Sketch_2.setCoincident(SketchLine_14.startPoint(), SketchLine_17.endPoint())
SketchConstraintHorizontal_5 = Sketch_2.setHorizontal(SketchLine_17.result())
SketchConstraintVertical_5 = Sketch_2.setVertical(SketchLine_16.result())
SketchConstraintRadius_3 = Sketch_2.setRadius(SketchArc_3.results()[1], 0.03)
SketchConstraintLength_5 = Sketch_2.setLength(SketchLine_17.result(), 0.02)
SketchConstraintDistance_6 = Sketch_2.setDistance(SketchLine_14.result(), SketchArc_3.center(), 0.02, True)
SketchProjection_5 = Sketch_2.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_18 = SketchProjection_5.createdFeature()
SketchConstraintDistance_7 = Sketch_2.setDistance(SketchLine_18.result(), SketchArc_3.center(), 0.062, True)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1f-SketchArc_1_2f-SketchLine_3f-SketchLine_4f")], model.selection(), 0.008999999999999999, 0.008999999999999999)
Fuse_1 = model.addFuse(Part_1_doc, [model.selection("SOLID", "Revolution_1_1")], [model.selection("SOLID", "Extrusion_1_1")])
Axis_4 = model.addAxis(Part_1_doc, model.selection("FACE", "Revolution_1_1/Generated_Face_13"))
RevolutionCut_1 = model.addRevolutionCut(Part_1_doc, [], model.selection("EDGE", "PartSet/OZ"), 10, 28, [model.selection("SOLID", "Fuse_1_1")])
Sketch_3 = model.addSketch(Part_1_doc, model.standardPlane("XOZ"))
SketchLine_19 = Sketch_3.addLine(0.029, 0.042, 0.029, 0.03267384683690003)
SketchLine_19.setName("SketchLine_7")
SketchLine_19.result().setName("SketchLine_7")
SketchLine_19.result().setColor(0, 150, 0)
SketchLine_20 = Sketch_3.addLine(0.029, 0.03267384683690003, 0.008999999999999999, 0.042)
SketchLine_20.setName("SketchLine_8")
SketchLine_20.result().setName("SketchLine_8")
SketchLine_20.result().setColor(0, 150, 0)
SketchConstraintCoincidence_24 = Sketch_3.setCoincident(SketchLine_19.endPoint(), SketchLine_20.startPoint())
SketchConstraintCoincidence_24.setName("SketchConstraintCoincidence_8")
SketchConstraintVertical_6 = Sketch_3.setVertical(SketchLine_19.result())
SketchLine_21 = Sketch_3.addLine(0.008999999999999999, 0.042, 0.029, 0.042)
SketchLine_21.setName("SketchLine_9")
SketchLine_21.result().setName("SketchLine_9")
SketchLine_21.result().setColor(0, 150, 0)
SketchConstraintCoincidence_26 = Sketch_3.setCoincident(SketchLine_19.startPoint(), SketchLine_21.endPoint())
SketchConstraintCoincidence_26.setName("SketchConstraintCoincidence_10")
SketchConstraintCoincidence_27 = Sketch_3.setCoincident(SketchLine_21.startPoint(), SketchLine_20.endPoint())
SketchConstraintCoincidence_27.setName("SketchConstraintCoincidence_11")
SketchConstraintHorizontal_6 = Sketch_3.setHorizontal(SketchLine_21.result())
SketchConstraintAngle_2 = Sketch_3.setAngle(SketchLine_20.result(), SketchLine_21.result(), 25)
SketchProjection_7 = Sketch_3.addProjection(model.selection("VERTEX", "PartSet/Sketch_1/Vertex-SketchLine_17s-SketchLine_16e"), False)
SketchPoint_2 = SketchProjection_7.createdFeature()
SketchConstraintCoincidence_28 = Sketch_3.setCoincident(SketchAPI_Point(SketchPoint_2).coordinates(), SketchLine_20.endPoint())
SketchConstraintCoincidence_28.setName("SketchConstraintCoincidence_12")
SketchConstraintLength_6 = Sketch_3.setLength(SketchLine_21.result(), 0.02)
RevolutionCut_1.setNestedSketch(Sketch_3)
model.end()

# check that result was created
assert(RevolutionCut_1.feature().results().size() == 1)

assert(model.checkPythonDump())
