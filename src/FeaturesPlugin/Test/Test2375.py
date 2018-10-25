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
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.addParameter(Part_1_doc, "angle", "27.5")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchLine_1 = Sketch_1.addLine(0, 4, 6.2, 4)
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OZ"), False)
SketchLine_2 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_2.result())
SketchLine_3 = Sketch_1.addLine(6.2, 4, 6.2, 2.5)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(6.2, 2.5, 3.5, 2)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(3.5, 2, 3.5, 0)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_6 = SketchProjection_2.createdFeature()
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.result())
SketchLine_7 = Sketch_1.addLine(3.5, 0, 0, 0)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchAPI_Line(SketchLine_2).startPoint(), SketchLine_7.endPoint())
SketchLine_8 = Sketch_1.addLine(0, 4, 0, 0)
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_8.startPoint())
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchAPI_Line(SketchLine_2).startPoint(), SketchLine_8.endPoint())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_5.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_3.result())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_1.result(), 6.2)
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_7.result(), 3.5)
SketchConstraintLength_3 = Sketch_1.setLength(SketchLine_5.result(), 2)
SketchConstraintLength_4 = Sketch_1.setLength(SketchLine_3.result(), 1.5)
SketchConstraintLength_5 = Sketch_1.setLength(SketchLine_8.result(), 4)
model.do()
Revolution_1 = model.addRevolution(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_8f-SketchLine_7r-SketchLine_5r-SketchLine_4r-SketchLine_3r-SketchLine_1r")], model.selection("EDGE", "PartSet/OZ"), 360, 0)
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Revolution_1_1/Generated_Face&Sketch_1/SketchLine_1"))
SketchProjection_3 = Sketch_2.addProjection(model.selection("VERTEX", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_3][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_1]__cc"), False)
SketchPoint_1 = SketchProjection_3.createdFeature()
SketchCircle_1 = Sketch_2.addCircle(0, 0, 4.75)
SketchCircle_1.setAuxiliary(True)
SketchConstraintCoincidence_10 = Sketch_2.setCoincident(SketchPoint_1.result(), SketchCircle_1.center())
SketchCircle_2 = Sketch_2.addCircle(4.113620667976081, 2.375000000000003, 1.1)
SketchConstraintCoincidence_11 = Sketch_2.setCoincident(SketchCircle_1.results()[1], SketchCircle_2.center())
SketchCircle_3 = Sketch_2.addCircle(-4.113620667974014, 2.375000000003604, 1.1)
SketchConstraintCoincidence_12 = Sketch_2.setCoincident(SketchCircle_1.results()[1], SketchCircle_3.center())
SketchCircle_4 = Sketch_2.addCircle(4.163806340121522e-12, -4.75000000000001, 1.1)
SketchConstraintCoincidence_13 = Sketch_2.setCoincident(SketchCircle_1.results()[1], SketchCircle_4.center())
SketchConstraintEqual_1 = Sketch_2.setEqual(SketchCircle_2.results()[1], SketchCircle_3.results()[1])
SketchConstraintEqual_2 = Sketch_2.setEqual(SketchCircle_3.results()[1], SketchCircle_4.results()[1])
SketchLine_9 = Sketch_2.addLine(0, 0, 4.113620667976081, 2.375000000000003)
SketchLine_9.setAuxiliary(True)
SketchConstraintCoincidence_14 = Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_9.startPoint())
SketchConstraintCoincidence_15 = Sketch_2.setCoincident(SketchCircle_2.center(), SketchLine_9.endPoint())
SketchLine_10 = Sketch_2.addLine(-4.113620667974014, 2.375000000003604, 0, 0)
SketchLine_10.setAuxiliary(True)
SketchConstraintCoincidence_16 = Sketch_2.setCoincident(SketchCircle_3.center(), SketchLine_10.startPoint())
SketchConstraintCoincidence_17 = Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_10.endPoint())
SketchLine_11 = Sketch_2.addLine(4.163806340121522e-12, -4.75000000000001, 0, 0)
SketchLine_11.setAuxiliary(True)
SketchConstraintCoincidence_18 = Sketch_2.setCoincident(SketchCircle_4.center(), SketchLine_11.startPoint())
SketchConstraintCoincidence_19 = Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_11.endPoint())
SketchConstraintRadius_1 = Sketch_2.setRadius(SketchCircle_2.results()[1], 1.1)
SketchProjection_4 = Sketch_2.addProjection(model.selection("EDGE", "Sketch_1/SketchLine_1"), False)
SketchLine_12 = SketchProjection_4.createdFeature()
SketchConstraintAngle_1 = Sketch_2.setAngleBackward(SketchLine_9.result(), SketchLine_12.result(), 30.00000000000006)
SketchConstraintAngle_2 = Sketch_2.setAngle(SketchLine_9.result(), SketchLine_10.result(), 119.9999999999498)
SketchConstraintAngle_3 = Sketch_2.setAngle(SketchLine_10.result(), SketchLine_11.result(), 120.0000000001004)
SketchConstraintLength_6 = Sketch_2.setLength(SketchLine_9.result(), 4.75)
SketchCircle_5 = Sketch_2.addCircle(2.552173139647412, 4.006109367611208, 0.65)
SketchConstraintCoincidence_20 = Sketch_2.setCoincident(SketchCircle_1.results()[1], SketchCircle_5.center())
SketchCircle_6 = Sketch_2.addCircle(2.552173139647412, 4.006109367611208, 0.8)
SketchConstraintCoincidence_21 = Sketch_2.setCoincident(SketchCircle_1.results()[1], SketchCircle_6.center())
SketchConstraintCoincidence_22 = Sketch_2.setCoincident(SketchCircle_5.center(), SketchCircle_6.center())
SketchCircle_7 = Sketch_2.addCircle(4.745479052513824, 0.20719208998535, 0.65)
SketchConstraintCoincidence_23 = Sketch_2.setCoincident(SketchCircle_1.results()[1], SketchCircle_7.center())
SketchCircle_8 = Sketch_2.addCircle(4.745479052513824, 0.20719208998535, 0.8)
SketchConstraintCoincidence_24 = Sketch_2.setCoincident(SketchCircle_1.results()[1], SketchCircle_8.center())
SketchConstraintCoincidence_25 = Sketch_2.setCoincident(SketchCircle_7.center(), SketchCircle_8.center())
SketchLine_13 = Sketch_2.addLine(0, 0, 2.552173139647412, 4.006109367611208)
SketchLine_13.setAuxiliary(True)
SketchConstraintCoincidence_26 = Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_13.startPoint())
SketchConstraintCoincidence_27 = Sketch_2.setCoincident(SketchCircle_5.center(), SketchLine_13.endPoint())
SketchLine_14 = Sketch_2.addLine(0, 0, 4.745479052513824, 0.20719208998535)
SketchLine_14.setAuxiliary(True)
SketchConstraintCoincidence_28 = Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_14.startPoint())
SketchConstraintCoincidence_29 = Sketch_2.setCoincident(SketchCircle_7.center(), SketchLine_14.endPoint())
SketchConstraintEqual_3 = Sketch_2.setEqual(SketchCircle_7.results()[1], SketchCircle_5.results()[1])
SketchConstraintEqual_4 = Sketch_2.setEqual(SketchCircle_8.results()[1], SketchCircle_6.results()[1])
SketchConstraintRadius_2 = Sketch_2.setRadius(SketchCircle_7.results()[1], 0.65)
SketchConstraintRadius_3 = Sketch_2.setRadius(SketchCircle_8.results()[1], 0.8)
SketchCircle_9 = Sketch_2.addCircle(-2.552173139482203, 4.006109367717388, 0.65)
SketchConstraintCoincidence_30 = Sketch_2.setCoincident(SketchCircle_1.results()[1], SketchCircle_9.center())
SketchCircle_10 = Sketch_2.addCircle(-4.745479052513825, 0.2071920899853395, 0.65)
SketchConstraintCoincidence_31 = Sketch_2.setCoincident(SketchCircle_1.results()[1], SketchCircle_10.center())
SketchCircle_11 = Sketch_2.addCircle(-2.552173139482203, 4.006109367717388, 0.8)
SketchConstraintCoincidence_32 = Sketch_2.setCoincident(SketchCircle_1.results()[1], SketchCircle_11.center())
SketchCircle_12 = Sketch_2.addCircle(-4.745479052513825, 0.2071920899853395, 0.8)
SketchConstraintCoincidence_33 = Sketch_2.setCoincident(SketchCircle_10.center(), SketchCircle_12.center())
SketchConstraintCoincidence_34 = Sketch_2.setCoincident(SketchCircle_9.center(), SketchCircle_11.center())
SketchConstraintEqual_5 = Sketch_2.setEqual(SketchCircle_6.results()[1], SketchCircle_11.results()[1])
SketchConstraintEqual_6 = Sketch_2.setEqual(SketchCircle_5.results()[1], SketchCircle_9.results()[1])
SketchConstraintEqual_7 = Sketch_2.setEqual(SketchCircle_9.results()[1], SketchCircle_10.results()[1])
SketchConstraintEqual_8 = Sketch_2.setEqual(SketchCircle_11.results()[1], SketchCircle_12.results()[1])
SketchLine_15 = Sketch_2.addLine(-2.552173139482203, 4.006109367717388, 0, 0)
SketchLine_15.setAuxiliary(True)
SketchConstraintCoincidence_35 = Sketch_2.setCoincident(SketchCircle_9.center(), SketchLine_15.startPoint())
SketchConstraintCoincidence_36 = Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_15.endPoint())
SketchLine_16 = Sketch_2.addLine(-4.745479052513825, 0.2071920899853395, 0, 0)
SketchLine_16.setAuxiliary(True)
SketchConstraintCoincidence_37 = Sketch_2.setCoincident(SketchCircle_10.center(), SketchLine_16.startPoint())
SketchConstraintCoincidence_38 = Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_16.endPoint())
SketchCircle_13 = Sketch_2.addCircle(-2.193305912870557, -4.213301457594897, 0.8)
SketchConstraintCoincidence_39 = Sketch_2.setCoincident(SketchCircle_1.results()[1], SketchCircle_13.center())
SketchCircle_14 = Sketch_2.addCircle(2.193305912973027, -4.213301457541542, 0.8)
SketchConstraintCoincidence_40 = Sketch_2.setCoincident(SketchCircle_1.results()[1], SketchCircle_14.center())
SketchConstraintEqual_9 = Sketch_2.setEqual(SketchCircle_14.results()[1], SketchCircle_13.results()[1])
SketchConstraintEqual_10 = Sketch_2.setEqual(SketchCircle_12.results()[1], SketchCircle_13.results()[1])
SketchLine_17 = Sketch_2.addLine(0, 0, -2.193305912870557, -4.213301457594897)
SketchLine_17.setAuxiliary(True)
SketchConstraintCoincidence_41 = Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_17.startPoint())
SketchConstraintCoincidence_42 = Sketch_2.setCoincident(SketchCircle_13.center(), SketchLine_17.endPoint())
SketchLine_18 = Sketch_2.addLine(0, 0, 2.193305912973027, -4.213301457541542)
SketchLine_18.setAuxiliary(True)
SketchConstraintCoincidence_43 = Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_18.startPoint())
SketchConstraintCoincidence_44 = Sketch_2.setCoincident(SketchCircle_14.center(), SketchLine_18.endPoint())
SketchConstraintAngle_4 = Sketch_2.setAngleBackward(SketchLine_9.result(), SketchLine_14.result(), "angle")
SketchConstraintAngle_5 = Sketch_2.setAngle(SketchLine_9.result(), SketchLine_13.result(), "angle")
SketchConstraintAngle_6 = Sketch_2.setAngleBackward(SketchLine_10.result(), SketchLine_15.result(), "angle")
SketchConstraintAngle_7 = Sketch_2.setAngleBackward(SketchLine_16.result(), SketchLine_10.result(), "angle")
SketchConstraintAngle_8 = Sketch_2.setAngle(SketchLine_17.result(), SketchLine_11.result(), "angle")
SketchConstraintAngle_9 = Sketch_2.setAngle(SketchLine_11.result(), SketchLine_18.result(), "angle")
SketchProjection_5 = Sketch_2.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_19 = SketchProjection_5.createdFeature()
SketchCircle_15 = Sketch_2.addCircle(0, 5, 0.45)
SketchConstraintCoincidence_45 = Sketch_2.setCoincident(SketchLine_19.result(), SketchCircle_15.center())
SketchConstraintRadius_4 = Sketch_2.setRadius(SketchCircle_15.results()[1], 0.45)
SketchLine_20 = Sketch_2.addLine(0, 0, 4.385635518227006, -2.793957963403483)
SketchLine_20.setAuxiliary(True)
SketchConstraintCoincidence_46 = Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_20.startPoint())
SketchCircle_16 = Sketch_2.addCircle(3.373565783251674, -2.149198433387089, 0.3)
SketchConstraintCoincidence_47 = Sketch_2.setCoincident(SketchLine_20.result(), SketchCircle_16.center())
SketchCircle_17 = Sketch_2.addCircle(4.385635518227006, -2.793957963403483, 0.5)
SketchConstraintCoincidence_48 = Sketch_2.setCoincident(SketchLine_20.endPoint(), SketchCircle_17.center())
SketchConstraintRadius_5 = Sketch_2.setRadius(SketchCircle_16.results()[1], 0.3)
SketchConstraintRadius_6 = Sketch_2.setRadius(SketchCircle_17.results()[1], 0.5)
SketchConstraintAngle_10 = Sketch_2.setAngleBackward(SketchLine_14.result(), SketchLine_20.result(), 35.00000000000006)
SketchConstraintDistance_1 = Sketch_2.setDistance(SketchCircle_15.center(), SketchLine_12.result(), 5, True)
SketchConstraintDistance_2 = Sketch_2.setDistance(SketchCircle_16.center(), SketchAPI_Line(SketchLine_19).startPoint(), 4, True)
SketchConstraintDistance_3 = Sketch_2.setDistance(SketchAPI_Line(SketchLine_19).startPoint(), SketchCircle_17.center(), 5.2, True)
model.do()
ExtrusionCut_1_objects_1 = [model.selection("FACE", "Sketch_2/Face-SketchCircle_9_2f"), model.selection("FACE", "Sketch_2/Face-SketchCircle_10_2f"), model.selection("FACE", "Sketch_2/Face-SketchCircle_5_2f"), model.selection("FACE", "Sketch_2/Face-SketchCircle_2_2f"), model.selection("FACE", "Sketch_2/Face-SketchCircle_7_2f"), model.selection("FACE", "Sketch_2/Face-SketchCircle_17_2f"), model.selection("FACE", "Sketch_2/Face-SketchCircle_16_2f"), model.selection("FACE", "Sketch_2/Face-SketchCircle_4_2f"), model.selection("FACE", "Sketch_2/Face-SketchCircle_3_2f")]
ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, ExtrusionCut_1_objects_1, model.selection(), model.selection("FACE", "Revolution_1_1/Generated_Face&Sketch_1/SketchLine_7"), 0, model.selection(), 0, [model.selection("SOLID", "Revolution_1_1")])
ExtrusionCut_2_objects_1 = [model.selection("WIRE", "[Sketch_2/SketchCircle_11_2]"), model.selection("WIRE", "[Sketch_2/SketchCircle_12_2]"), model.selection("WIRE", "[Sketch_2/SketchCircle_6_2]"), model.selection("WIRE", "[Sketch_2/SketchCircle_8_2]")]
ExtrusionCut_2 = model.addExtrusionCut(Part_1_doc, ExtrusionCut_2_objects_1, model.selection(), 0, 0.75, [model.selection("SOLID", "ExtrusionCut_1_1")])
Axis_4 = model.addAxis(Part_1_doc, 0, -1, -10)
ExtrusionCut_3 = model.addExtrusionCut(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchCircle_15_2f")], model.selection("EDGE", "Axis_1"), model.selection("FACE", "Revolution_1_1/Generated_Face&Sketch_1/SketchLine_7"), 0, model.selection(), 0, [model.selection("SOLID", "ExtrusionCut_2_1")])
ExtrusionCut_3.result().setDeflection(1e-05)
Sketch_3 = model.addSketch(Part_1_doc, model.standardPlane("XOZ"))
SketchLine_21 = Sketch_3.addLine(3.5, 2.508501119175777, 3.5, 4.934853156880259)
SketchLine_22 = Sketch_3.addLine(3.5, 4.934853156880259, 6.2, 4.934853156880259)
SketchConstraintCoincidence_49 = Sketch_3.setCoincident(SketchLine_21.endPoint(), SketchLine_22.startPoint())
SketchLine_23 = Sketch_3.addLine(6.2, 4.934853156880259, 6.2, 3.008501119175777)
SketchConstraintCoincidence_50 = Sketch_3.setCoincident(SketchLine_22.endPoint(), SketchLine_23.startPoint())
SketchProjection_6 = Sketch_3.addProjection(model.selection("EDGE", "Sketch_1/SketchLine_3"), False)
SketchLine_24 = SketchProjection_6.createdFeature()
SketchConstraintCoincidence_51 = Sketch_3.setCoincident(SketchLine_23.endPoint(), SketchLine_24.result())
SketchLine_25 = Sketch_3.addLine(6.2, 3.008501119175777, 3.5, 2.508501119175777)
SketchConstraintCoincidence_52 = Sketch_3.setCoincident(SketchLine_23.endPoint(), SketchLine_25.startPoint())
SketchConstraintCoincidence_53 = Sketch_3.setCoincident(SketchLine_21.startPoint(), SketchLine_25.endPoint())
SketchProjection_7 = Sketch_3.addProjection(model.selection("EDGE", "Sketch_1/SketchLine_5"), False)
SketchLine_26 = SketchProjection_7.createdFeature()
SketchConstraintCoincidence_54 = Sketch_3.setCoincident(SketchLine_21.startPoint(), SketchLine_26.result())
SketchConstraintCoincidence_55 = Sketch_3.setCoincident(SketchLine_21.endPoint(), SketchLine_26.result())
SketchConstraintHorizontal_2 = Sketch_3.setHorizontal(SketchLine_22.result())
SketchConstraintCoincidence_56 = Sketch_3.setCoincident(SketchLine_22.endPoint(), SketchLine_24.result())
SketchProjection_8 = Sketch_3.addProjection(model.selection("EDGE", "Sketch_1/SketchLine_4"), False)
SketchLine_27 = SketchProjection_8.createdFeature()
SketchConstraintParallel_1 = Sketch_3.setParallel(SketchLine_25.result(), SketchLine_27.result())
SketchProjection_9 = Sketch_3.addProjection(model.selection("EDGE", "Sketch_1/SketchLine_4"), False)
SketchLine_28 = SketchProjection_9.createdFeature()
SketchConstraintDistance_4 = Sketch_3.setDistance(SketchLine_21.startPoint(), SketchLine_28.result(), 0.5, True)
model.do()
Revolution_2 = model.addRevolution(Part_1_doc, [model.selection("COMPOUND", "Sketch_3")], model.selection("EDGE", "PartSet/OZ"), 360, 0)
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchCircle_13_2f"), model.selection("FACE", "Sketch_2/Face-SketchCircle_14_2f")], model.selection(), model.selection("FACE", "Revolution_1_1/Generated_Face&Sketch_1/SketchLine_7"), 0, model.selection(), 0)
Common_1 = model.addCommon(Part_1_doc, [model.selection("SOLID", "Extrusion_1_2"), model.selection("SOLID", "Extrusion_1_1")], [model.selection("SOLID", "Revolution_2_1")])
Cut_1 = model.addCut(Part_1_doc, [model.selection("SOLID", "ExtrusionCut_3_1")], [model.selection("SOLID", "Common_1_1"), model.selection("SOLID", "Common_1_2")])
Group_1_objects = [model.selection("VERTEX", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_7][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_5]"), model.selection("VERTEX", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_5][ExtrusionCut_3_1/Modified_Face&Sketch_1/SketchLine_4]"), model.selection("VERTEX", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_3][ExtrusionCut_3_1/Modified_Face&Sketch_1/SketchLine_4]"), model.selection("VERTEX", "[ExtrusionCut_1_1/Modified_Face&Sketch_2/SketchCircle_4_2][ExtrusionCut_3_1/Modified_Face&Sketch_1/SketchLine_4]"), model.selection("VERTEX", "[ExtrusionCut_2_1/Modified_Face&Sketch_2/SketchCircle_5_2][ExtrusionCut_3_1/Modified_Face&Sketch_1/SketchLine_4]"), model.selection("VERTEX", "[ExtrusionCut_3_1/Modified_Face&Sketch_2/SketchCircle_15_2][ExtrusionCut_3_1/Modified_Face&Sketch_1/SketchLine_4]"), model.selection("VERTEX", "[ExtrusionCut_2_1/Modified_Face&Sketch_2/SketchCircle_9_2][ExtrusionCut_3_1/Modified_Face&Sketch_1/SketchLine_4]"), model.selection("VERTEX", "[ExtrusionCut_2_1/Modified_Face&Sketch_2/SketchCircle_10_2][ExtrusionCut_3_1/Modified_Face&Sketch_1/SketchLine_4]"), model.selection("VERTEX", "[ExtrusionCut_1_1/Modified_Face&Sketch_2/SketchCircle_3_2][ExtrusionCut_3_1/Modified_Face&Sketch_1/SketchLine_4]"), model.selection("VERTEX", "[ExtrusionCut_1_1/Modified_Face&Sketch_2/SketchCircle_16_2][ExtrusionCut_3_1/Modified_Face&Sketch_1/SketchLine_4]"), model.selection("VERTEX", "[ExtrusionCut_1_1/Modified_Face&Sketch_2/SketchCircle_17_2][ExtrusionCut_3_1/Modified_Face&Sketch_1/SketchLine_4]"), model.selection("VERTEX", "[ExtrusionCut_1_1/Modified_Face&Sketch_2/SketchCircle_2_2][ExtrusionCut_3_1/Modified_Face&Sketch_1/SketchLine_4]"), model.selection("VERTEX", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_3][Cut_1_1/Modified_Face&Sketch_1/SketchLine_1]"), model.selection("VERTEX", "[ExtrusionCut_2_1/Modified_Face&ExtrusionCut_2_1/From_Face_4][ExtrusionCut_2_1/Modified_Face&Sketch_2/SketchCircle_7_2]"), model.selection("VERTEX", "[ExtrusionCut_1_1/Modified_Face&Sketch_2/SketchCircle_4_2][Cut_1_1/Modified_Face&Sketch_1/SketchLine_1]"), model.selection("VERTEX", "[ExtrusionCut_2_1/Modified_Face&Sketch_2/SketchCircle_5_2][ExtrusionCut_2_1/Modified_Face&ExtrusionCut_2_1/From_Face_3]"), model.selection("VERTEX", "[ExtrusionCut_3_1/Modified_Face&Sketch_2/SketchCircle_15_2][Cut_1_1/Modified_Face&Sketch_1/SketchLine_1]"), model.selection("VERTEX", "[ExtrusionCut_2_1/Modified_Face&Sketch_2/SketchCircle_9_2][ExtrusionCut_2_1/Modified_Face&ExtrusionCut_2_1/From_Face_1]"), model.selection("VERTEX", "[ExtrusionCut_2_1/Modified_Face&Sketch_2/SketchCircle_10_2][ExtrusionCut_2_1/Modified_Face&ExtrusionCut_2_1/From_Face_2]"), model.selection("VERTEX", "[Cut_1_1/Modified_Face&Sketch_1/SketchLine_1][ExtrusionCut_1_1/Modified_Face&Sketch_2/SketchCircle_3_2]"), model.selection("VERTEX", "[ExtrusionCut_1_1/Modified_Face&Sketch_2/SketchCircle_16_2][Cut_1_1/Modified_Face&Sketch_1/SketchLine_1]"), model.selection("VERTEX", "[ExtrusionCut_1_1/Modified_Face&Sketch_2/SketchCircle_17_2][Cut_1_1/Modified_Face&Sketch_1/SketchLine_1]"), model.selection("VERTEX", "[ExtrusionCut_1_1/Modified_Face&Sketch_2/SketchCircle_2_2][Cut_1_1/Modified_Face&Sketch_1/SketchLine_1]"), model.selection("VERTEX", "[Cut_1_1/Modified_Face&Sketch_1/SketchLine_1][ExtrusionCut_2_1/Generated_Face&Sketch_2/SketchCircle_6_2]"), model.selection("VERTEX", "[ExtrusionCut_2_1/Generated_Face&Sketch_2/SketchCircle_11_2][Cut_1_1/Modified_Face&Sketch_1/SketchLine_1]"), model.selection("VERTEX", "[Cut_1_1/Modified_Face&Sketch_1/SketchLine_1][ExtrusionCut_2_1/Generated_Face&Sketch_2/SketchCircle_12_2]"), model.selection("VERTEX", "[Cut_1_1/Modified_Face&Sketch_1/SketchLine_1][Common_1_2/Modified_Face&Sketch_2/SketchCircle_13_2]"), model.selection("VERTEX", "[Cut_1_1/Modified_Face&Sketch_1/SketchLine_1][Common_1_1/Modified_Face&Sketch_2/SketchCircle_14_2]"), model.selection("VERTEX", "[ExtrusionCut_2_1/Generated_Face&Sketch_2/SketchCircle_8_2][Cut_1_1/Modified_Face&Sketch_1/SketchLine_1]"), model.selection("VERTEX", "[ExtrusionCut_2_1/Modified_Face&ExtrusionCut_2_1/From_Face_4][ExtrusionCut_2_1/Generated_Face&Sketch_2/SketchCircle_8_2]"), model.selection("VERTEX", "[ExtrusionCut_2_1/Generated_Face&Sketch_2/SketchCircle_6_2][ExtrusionCut_2_1/Modified_Face&ExtrusionCut_2_1/From_Face_3]"), model.selection("VERTEX", "[ExtrusionCut_2_1/Modified_Face&ExtrusionCut_2_1/From_Face_1][ExtrusionCut_2_1/Generated_Face&Sketch_2/SketchCircle_11_2]"), model.selection("VERTEX", "[ExtrusionCut_2_1/Modified_Face&ExtrusionCut_2_1/From_Face_2][ExtrusionCut_2_1/Generated_Face&Sketch_2/SketchCircle_12_2]"), model.selection("VERTEX", "Common_1_2/Generated_Vertex&Sketch_3/SketchLine_25"), model.selection("VERTEX", "Common_1_1/Generated_Vertex&Sketch_3/SketchLine_25")]
Group_1 = model.addGroup(Part_1_doc, Group_1_objects)
Group_2_objects = [model.selection("EDGE", "[ExtrusionCut_2_1/Modified_Face&Sketch_2/SketchCircle_10_2][ExtrusionCut_3_1/Modified_Face&Sketch_1/SketchLine_4]"), model.selection("EDGE", "([ExtrusionCut_2_1/Modified_Face&Sketch_2/SketchCircle_10_2][ExtrusionCut_2_1/Modified_Face&ExtrusionCut_2_1/From_Face_2])([ExtrusionCut_2_1/Modified_Face&Sketch_2/SketchCircle_10_2][ExtrusionCut_3_1/Modified_Face&Sketch_1/SketchLine_4])"), model.selection("EDGE", "[ExtrusionCut_2_1/Generated_Face&Sketch_2/SketchCircle_11_2][Cut_1_1/Modified_Face&Sketch_1/SketchLine_1]"), model.selection("EDGE", "[ExtrusionCut_2_1/Modified_Face&Sketch_2/SketchCircle_10_2][ExtrusionCut_2_1/Modified_Face&ExtrusionCut_2_1/From_Face_2]"), model.selection("EDGE", "([ExtrusionCut_2_1/Modified_Face&ExtrusionCut_2_1/From_Face_2][ExtrusionCut_2_1/Generated_Face&Sketch_2/SketchCircle_12_2])([Cut_1_1/Modified_Face&Sketch_1/SketchLine_1][ExtrusionCut_2_1/Generated_Face&Sketch_2/SketchCircle_12_2])"), model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&Sketch_2/SketchCircle_3_2][ExtrusionCut_3_1/Modified_Face&Sketch_1/SketchLine_4]"), model.selection("EDGE", "[Cut_1_1/Modified_Face&Sketch_1/SketchLine_1][ExtrusionCut_1_1/Modified_Face&Sketch_2/SketchCircle_3_2]"), model.selection("EDGE", "Common_1_2/Generated_Edge&Sketch_3/SketchLine_25&Sketch_2/SketchCircle_13_2"), model.selection("EDGE", "[Cut_1_1/Modified_Face&Sketch_1/SketchLine_1][Common_1_2/Modified_Face&Sketch_2/SketchCircle_13_2]"), model.selection("EDGE", "(Common_1_2/Generated_Edge&Sketch_3/SketchLine_25&Sketch_2/SketchCircle_13_2)([Cut_1_1/Modified_Face&Sketch_1/SketchLine_1][Common_1_2/Modified_Face&Sketch_2/SketchCircle_13_2])"), model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&Sketch_2/SketchCircle_4_2][ExtrusionCut_3_1/Modified_Face&Sketch_1/SketchLine_4]"), model.selection("EDGE", "([Cut_1_1/Modified_Face&Sketch_1/SketchLine_1][ExtrusionCut_1_1/Modified_Face&Sketch_2/SketchCircle_3_2])([ExtrusionCut_1_1/Modified_Face&Sketch_2/SketchCircle_3_2][ExtrusionCut_3_1/Modified_Face&Sketch_1/SketchLine_4])"), model.selection("EDGE", "([ExtrusionCut_2_1/Modified_Face&Sketch_2/SketchCircle_9_2][ExtrusionCut_3_1/Modified_Face&Sketch_1/SketchLine_4])([ExtrusionCut_2_1/Modified_Face&Sketch_2/SketchCircle_9_2][ExtrusionCut_2_1/Modified_Face&ExtrusionCut_2_1/From_Face_1])"), model.selection("EDGE", "[ExtrusionCut_2_1/Modified_Face&Sketch_2/SketchCircle_9_2][ExtrusionCut_3_1/Modified_Face&Sketch_1/SketchLine_4]"), model.selection("EDGE", "[ExtrusionCut_2_1/Modified_Face&Sketch_2/SketchCircle_9_2][ExtrusionCut_2_1/Modified_Face&ExtrusionCut_2_1/From_Face_1]"), model.selection("EDGE", "[ExtrusionCut_2_1/Modified_Face&ExtrusionCut_2_1/From_Face_1][ExtrusionCut_2_1/Generated_Face&Sketch_2/SketchCircle_11_2]"), model.selection("EDGE", "[ExtrusionCut_2_1/Generated_Face&Sketch_2/SketchCircle_11_2][Cut_1_1/Modified_Face&Sketch_1/SketchLine_1]"), model.selection("EDGE", "([ExtrusionCut_2_1/Generated_Face&Sketch_2/SketchCircle_11_2][Cut_1_1/Modified_Face&Sketch_1/SketchLine_1])([ExtrusionCut_2_1/Modified_Face&ExtrusionCut_2_1/From_Face_1][ExtrusionCut_2_1/Generated_Face&Sketch_2/SketchCircle_11_2])"), model.selection("EDGE", "[ExtrusionCut_3_1/Modified_Face&Sketch_2/SketchCircle_15_2][ExtrusionCut_3_1/Modified_Face&Sketch_1/SketchLine_4]"), model.selection("EDGE", "(Common_1_1/Generated_Edge&Sketch_3/SketchLine_25&Sketch_2/SketchCircle_14_2)([Cut_1_1/Modified_Face&Sketch_1/SketchLine_1][Common_1_1/Modified_Face&Sketch_2/SketchCircle_14_2])"), model.selection("EDGE", "Common_1_1/Generated_Edge&Sketch_3/SketchLine_25&Sketch_2/SketchCircle_14_2"), model.selection("EDGE", "[ExtrusionCut_2_1/Generated_Face&Sketch_2/SketchCircle_11_2][Cut_1_1/Modified_Face&Sketch_1/SketchLine_1]"), model.selection("EDGE", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_3][ExtrusionCut_3_1/Modified_Face&Sketch_1/SketchLine_4]"), model.selection("EDGE", "[Cut_1_1/Modified_Face&Sketch_1/SketchLine_1][Common_1_1/Modified_Face&Sketch_2/SketchCircle_14_2]"), model.selection("EDGE", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_5][ExtrusionCut_3_1/Modified_Face&Sketch_1/SketchLine_4]"), model.selection("EDGE", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_7][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_5]"), model.selection("EDGE", "[ExtrusionCut_2_1/Modified_Face&Sketch_2/SketchCircle_5_2][ExtrusionCut_3_1/Modified_Face&Sketch_1/SketchLine_4]"), model.selection("EDGE", "[ExtrusionCut_2_1/Modified_Face&Sketch_2/SketchCircle_5_2][ExtrusionCut_2_1/Modified_Face&ExtrusionCut_2_1/From_Face_3]"), model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&Sketch_2/SketchCircle_4_2][Cut_1_1/Modified_Face&Sketch_1/SketchLine_1]"), model.selection("EDGE", "([ExtrusionCut_1_1/Modified_Face&Sketch_2/SketchCircle_4_2][ExtrusionCut_3_1/Modified_Face&Sketch_1/SketchLine_4])([ExtrusionCut_1_1/Modified_Face&Sketch_2/SketchCircle_4_2][Cut_1_1/Modified_Face&Sketch_1/SketchLine_1])"), model.selection("EDGE", "([ExtrusionCut_3_1/Modified_Face&Sketch_2/SketchCircle_15_2][ExtrusionCut_3_1/Modified_Face&Sketch_1/SketchLine_4])([ExtrusionCut_3_1/Modified_Face&Sketch_2/SketchCircle_15_2][Cut_1_1/Modified_Face&Sketch_1/SketchLine_1])"), model.selection("EDGE", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_3][Cut_1_1/Modified_Face&Sketch_1/SketchLine_1]"), model.selection("EDGE", "[ExtrusionCut_3_1/Modified_Face&Sketch_2/SketchCircle_15_2][Cut_1_1/Modified_Face&Sketch_1/SketchLine_1]"), model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&Sketch_2/SketchCircle_16_2][ExtrusionCut_3_1/Modified_Face&Sketch_1/SketchLine_4]"), model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&Sketch_2/SketchCircle_17_2][ExtrusionCut_3_1/Modified_Face&Sketch_1/SketchLine_4]"), model.selection("EDGE", "([ExtrusionCut_1_1/Modified_Face&Sketch_2/SketchCircle_17_2][Cut_1_1/Modified_Face&Sketch_1/SketchLine_1])([ExtrusionCut_1_1/Modified_Face&Sketch_2/SketchCircle_17_2][ExtrusionCut_3_1/Modified_Face&Sketch_1/SketchLine_4])"), model.selection("EDGE", "([ExtrusionCut_1_1/Modified_Face&Sketch_2/SketchCircle_16_2][ExtrusionCut_3_1/Modified_Face&Sketch_1/SketchLine_4])([ExtrusionCut_1_1/Modified_Face&Sketch_2/SketchCircle_16_2][Cut_1_1/Modified_Face&Sketch_1/SketchLine_1])"), model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&Sketch_2/SketchCircle_16_2][Cut_1_1/Modified_Face&Sketch_1/SketchLine_1]"), model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&Sketch_2/SketchCircle_17_2][Cut_1_1/Modified_Face&Sketch_1/SketchLine_1]"), model.selection("EDGE", "([Revolution_1_1/Generated_Face&Sketch_1/SketchLine_5][ExtrusionCut_3_1/Modified_Face&Sketch_1/SketchLine_4])([Revolution_1_1/Generated_Face&Sketch_1/SketchLine_7][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_5])"), model.selection("EDGE", "[ExtrusionCut_2_1/Generated_Face&Sketch_2/SketchCircle_11_2][Cut_1_1/Modified_Face&Sketch_1/SketchLine_1]"), model.selection("EDGE", "[ExtrusionCut_2_1/Modified_Face&ExtrusionCut_2_1/From_Face_4][ExtrusionCut_2_1/Modified_Face&Sketch_2/SketchCircle_7_2]"), model.selection("EDGE", "([ExtrusionCut_2_1/Modified_Face&ExtrusionCut_2_1/From_Face_4][ExtrusionCut_2_1/Modified_Face&Sketch_2/SketchCircle_7_2])"), model.selection("EDGE", "([Revolution_1_1/Generated_Face&Sketch_1/SketchLine_3][ExtrusionCut_3_1/Modified_Face&Sketch_1/SketchLine_4])([Revolution_1_1/Generated_Face&Sketch_1/SketchLine_3][Cut_1_1/Modified_Face&Sketch_1/SketchLine_1])"), model.selection("EDGE", "([ExtrusionCut_2_1/Generated_Face&Sketch_2/SketchCircle_8_2][Cut_1_1/Modified_Face&Sketch_1/SketchLine_1])([ExtrusionCut_2_1/Modified_Face&ExtrusionCut_2_1/From_Face_4][ExtrusionCut_2_1/Generated_Face&Sketch_2/SketchCircle_8_2])"), model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&Sketch_2/SketchCircle_2_2][ExtrusionCut_3_1/Modified_Face&Sketch_1/SketchLine_4]"), model.selection("EDGE", "([ExtrusionCut_1_1/Modified_Face&Sketch_2/SketchCircle_2_2][ExtrusionCut_3_1/Modified_Face&Sketch_1/SketchLine_4])([ExtrusionCut_1_1/Modified_Face&Sketch_2/SketchCircle_2_2][Cut_1_1/Modified_Face&Sketch_1/SketchLine_1])"), model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&Sketch_2/SketchCircle_2_2][Cut_1_1/Modified_Face&Sketch_1/SketchLine_1]"), model.selection("EDGE", "([ExtrusionCut_2_1/Modified_Face&Sketch_2/SketchCircle_5_2][ExtrusionCut_2_1/Modified_Face&ExtrusionCut_2_1/From_Face_3])([ExtrusionCut_2_1/Modified_Face&Sketch_2/SketchCircle_5_2][ExtrusionCut_3_1/Modified_Face&Sketch_1/SketchLine_4])"), model.selection("EDGE", "([ExtrusionCut_2_1/Generated_Face&Sketch_2/SketchCircle_6_2][ExtrusionCut_2_1/Modified_Face&ExtrusionCut_2_1/From_Face_3])([Cut_1_1/Modified_Face&Sketch_1/SketchLine_1][ExtrusionCut_2_1/Generated_Face&Sketch_2/SketchCircle_6_2])")]
Group_2 = model.addGroup(Part_1_doc, Group_2_objects)
Group_3_objects = [model.selection("FACE", "ExtrusionCut_2_1/Modified_Face&Sketch_2/SketchCircle_10_2"), model.selection("FACE", "ExtrusionCut_2_1/Generated_Face&Sketch_2/SketchCircle_11_2"), model.selection("FACE", "ExtrusionCut_2_1/Modified_Face&ExtrusionCut_2_1/From_Face_2"), model.selection("FACE", "ExtrusionCut_1_1/Modified_Face&Sketch_2/SketchCircle_3_2"), model.selection("FACE", "Common_1_2/Modified_Face&Sketch_3/SketchLine_25"), model.selection("FACE", "ExtrusionCut_1_1/Modified_Face&Sketch_2/SketchCircle_4_2"), model.selection("FACE", "ExtrusionCut_2_1/Modified_Face&Sketch_2/SketchCircle_9_2"), model.selection("FACE", "ExtrusionCut_2_1/Modified_Face&ExtrusionCut_2_1/From_Face_1"), model.selection("FACE", "Common_1_1/Modified_Face&Sketch_3/SketchLine_25"), model.selection("FACE", "Revolution_1_1/Generated_Face&Sketch_1/SketchLine_7"), model.selection("FACE", "ExtrusionCut_2_1/Generated_Face&Sketch_2/SketchCircle_11_2"), model.selection("FACE", "Cut_1_1/Modified_Face&Sketch_1/SketchLine_1"), model.selection("FACE", "ExtrusionCut_2_1/Modified_Face&Sketch_2/SketchCircle_5_2"), model.selection("FACE", "ExtrusionCut_2_1/Modified_Face&ExtrusionCut_2_1/From_Face_3"), model.selection("FACE", "Common_1_1/Modified_Face&Sketch_2/SketchCircle_14_2"), model.selection("FACE", "ExtrusionCut_3_1/Modified_Face&Sketch_1/SketchLine_4"), model.selection("FACE", "Revolution_1_1/Generated_Face&Sketch_1/SketchLine_5"), model.selection("FACE", "ExtrusionCut_2_1/Generated_Face&Sketch_2/SketchCircle_11_2"), model.selection("FACE", "Common_1_2/Modified_Face&Sketch_2/SketchCircle_13_2"), model.selection("FACE", "Revolution_1_1/Generated_Face&Sketch_1/SketchLine_3"), model.selection("FACE", "ExtrusionCut_3_1/Modified_Face&Sketch_2/SketchCircle_15_2"), model.selection("FACE", "ExtrusionCut_1_1/Modified_Face&Sketch_2/SketchCircle_16_2"), model.selection("FACE", "ExtrusionCut_1_1/Modified_Face&Sketch_2/SketchCircle_17_2"), model.selection("FACE", "ExtrusionCut_2_1/Modified_Face&Sketch_2/SketchCircle_7_2"), model.selection("FACE", "ExtrusionCut_2_1/Generated_Face&Sketch_2/SketchCircle_11_2"), model.selection("FACE", "ExtrusionCut_2_1/Modified_Face&ExtrusionCut_2_1/From_Face_4"), model.selection("FACE", "ExtrusionCut_1_1/Modified_Face&Sketch_2/SketchCircle_2_2")]
Group_3 = model.addGroup(Part_1_doc, Group_3_objects)
model.do()
model.end()

assert(model.checkPythonDump())
