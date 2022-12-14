# Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

from GeomAPI import GeomAPI_Pnt
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
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_7.result(), 3.5)
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_1.result(), 6.2)
SketchConstraintLength_3 = Sketch_1.setLength(SketchLine_3.result(), 1.5)
SketchConstraintLength_4 = Sketch_1.setLength(SketchLine_8.result(), 4)
SketchConstraintLength_5 = Sketch_1.setLength(SketchLine_5.result(), 2)
model.do()
Revolution_1 = model.addRevolution(Part_1_doc, [model.selection("COMPOUND", GeomAPI_Pnt(3.1, 0, 2))], model.selection("EDGE", "PartSet/OZ"), 360, 0)
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", GeomAPI_Pnt(0, 0, 4)))
SketchCircle_1 = Sketch_2.addCircle(4.027018127601872, 2.325000000002457, 1.1)
SketchCircle_2 = Sketch_2.addCircle(-4.027018127597639, 2.325000000000002, 1.1)
SketchCircle_3 = Sketch_2.addCircle(7.188370358619843e-12, -4.650000000090492, 1.1)
SketchProjection_3 = Sketch_2.addProjection(model.selection("VERTEX", GeomAPI_Pnt(0, 0, 4)), False)
SketchPoint_1 = SketchProjection_3.createdFeature()
SketchCircle_4 = Sketch_2.addCircle(0, 0, 4.65)
SketchCircle_4.setAuxiliary(True)
SketchConstraintCoincidence_10 = Sketch_2.setCoincident(SketchPoint_1.result(), SketchCircle_4.center())
SketchProjection_4 = Sketch_2.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_9 = SketchProjection_4.createdFeature()
SketchLine_10 = Sketch_2.addLine(0, 0, 4.027018127601872, 2.325000000002457)
SketchLine_10.setAuxiliary(True)
SketchConstraintCoincidence_11 = Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_10.startPoint())
SketchConstraintCoincidence_12 = Sketch_2.setCoincident(SketchCircle_1.center(), SketchLine_10.endPoint())
SketchLine_11 = Sketch_2.addLine(0, 0, -4.027018127597639, 2.325000000000002)
SketchLine_11.setAuxiliary(True)
SketchConstraintCoincidence_13 = Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_11.startPoint())
SketchConstraintCoincidence_14 = Sketch_2.setCoincident(SketchCircle_2.center(), SketchLine_11.endPoint())
SketchLine_12 = Sketch_2.addLine(0, 0, 7.188370358619843e-12, -4.650000000090492)
SketchLine_12.setAuxiliary(True)
SketchConstraintCoincidence_15 = Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_12.startPoint())
SketchConstraintCoincidence_16 = Sketch_2.setCoincident(SketchCircle_3.center(), SketchLine_12.endPoint())
SketchConstraintCoincidence_17 = Sketch_2.setCoincident(SketchCircle_2.center(), SketchCircle_4.results()[1])
SketchConstraintCoincidence_18 = Sketch_2.setCoincident(SketchCircle_1.center(), SketchCircle_4.results()[1])
SketchConstraintCoincidence_19 = Sketch_2.setCoincident(SketchLine_12.endPoint(), SketchCircle_4.results()[1])
SketchConstraintRadius_1 = Sketch_2.setRadius(SketchCircle_4.results()[1], 4.65)
SketchConstraintRadius_2 = Sketch_2.setRadius(SketchCircle_1.results()[1], 1.1)
SketchConstraintEqual_1 = Sketch_2.setEqual(SketchCircle_1.results()[1], SketchCircle_2.results()[1])
SketchConstraintEqual_2 = Sketch_2.setEqual(SketchCircle_2.results()[1], SketchCircle_3.results()[1])
SketchProjection_5 = Sketch_2.addProjection(model.selection("EDGE", GeomAPI_Pnt(3.1, 0, 4)), False)
SketchLine_13 = SketchProjection_5.createdFeature()
SketchConstraintAngle_1 = Sketch_2.setAngle(SketchLine_13.result(), SketchLine_10.result(), 30)
SketchConstraintAngle_2 = Sketch_2.setAngle(SketchLine_10.result(), SketchLine_11.result(), 120)
SketchConstraintAngle_3 = Sketch_2.setAngle(SketchLine_11.result(), SketchLine_12.result(), 120)
SketchCircle_5 = Sketch_2.addCircle(4.645574230355638, 0.2028301512488116, 0.8)
SketchConstraintCoincidence_20 = Sketch_2.setCoincident(SketchCircle_4.results()[1], SketchCircle_5.center())
SketchCircle_6 = Sketch_2.addCircle(4.645574230355638, 0.2028301512488116, 0.65)
SketchConstraintCoincidence_21 = Sketch_2.setCoincident(SketchCircle_5.center(), SketchCircle_6.center())
SketchConstraintRadius_3 = Sketch_2.setRadius(SketchCircle_6.results()[1], 0.65)
SketchConstraintRadius_4 = Sketch_2.setRadius(SketchCircle_5.results()[1], 0.8)
SketchLine_14 = Sketch_2.addLine(0, 0, 4.645574230355638, 0.2028301512488116)
SketchLine_14.setAuxiliary(True)
SketchConstraintCoincidence_22 = Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_14.startPoint())
SketchConstraintCoincidence_23 = Sketch_2.setCoincident(SketchCircle_5.center(), SketchLine_14.endPoint())
SketchConstraintAngle_4 = Sketch_2.setAngle(SketchLine_14.result(), SketchLine_10.result(), "angle")
SketchCircle_7 = Sketch_2.addCircle(2.498443178812738, 3.921770223029927, 0.65)
SketchConstraintCoincidence_24 = Sketch_2.setCoincident(SketchCircle_4.results()[1], SketchCircle_7.center())
SketchCircle_8 = Sketch_2.addCircle(2.498443178812738, 3.921770223029927, 0.8)
SketchConstraintCoincidence_25 = Sketch_2.setCoincident(SketchCircle_4.results()[1], SketchCircle_8.center())
SketchConstraintCoincidence_26 = Sketch_2.setCoincident(SketchCircle_7.center(), SketchCircle_8.center())
SketchConstraintEqual_3 = Sketch_2.setEqual(SketchCircle_6.results()[1], SketchCircle_7.results()[1])
SketchConstraintEqual_4 = Sketch_2.setEqual(SketchCircle_8.results()[1], SketchCircle_5.results()[1])
SketchLine_15 = Sketch_2.addLine(0, 0, 2.498443178812738, 3.921770223029927)
SketchLine_15.setAuxiliary(True)
SketchConstraintCoincidence_27 = Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_15.startPoint())
SketchConstraintCoincidence_28 = Sketch_2.setCoincident(SketchCircle_7.center(), SketchLine_15.endPoint())
SketchConstraintAngle_5 = Sketch_2.setAngle(SketchLine_10.result(), SketchLine_15.result(), "angle")
SketchMultiRotation_1 = Sketch_2.addRotation([SketchCircle_8.results()[1], SketchCircle_5.results()[1]], SketchCircle_4.center(), 360, 3, True)
[SketchCircle_9, SketchCircle_10, SketchCircle_11, SketchCircle_12] = SketchMultiRotation_1.rotated()
SketchCircle_12.setName("SketchCircle_13")
SketchCircle_12.result().setName("SketchCircle_13")
SketchCircle_12.results()[1].setName("SketchCircle_13_2")
SketchCircle_11.setName("SketchCircle_12")
SketchCircle_11.result().setName("SketchCircle_12")
SketchCircle_11.results()[1].setName("SketchCircle_12_2")
SketchMultiRotation_2 = Sketch_2.addRotation([SketchCircle_6.results()[1], SketchCircle_7.results()[1]], SketchAPI_Line(SketchLine_9).startPoint(), 120, 2)
[SketchCircle_13, SketchCircle_14] = SketchMultiRotation_2.rotated()
SketchCircle_14.setName("SketchCircle_17")
SketchCircle_14.result().setName("SketchCircle_17")
SketchCircle_14.results()[1].setName("SketchCircle_17_2")
SketchCircle_13.setName("SketchCircle_14")
SketchCircle_13.result().setName("SketchCircle_14")
SketchCircle_13.results()[1].setName("SketchCircle_14_2")
SketchCircle_15 = Sketch_2.addCircle(0, 5, 0.45)
SketchCircle_15.setName("SketchCircle_15")
SketchCircle_15.result().setName("SketchCircle_15")
SketchCircle_15.results()[1].setName("SketchCircle_15_2")
SketchLine_16 = Sketch_2.addLine(0, 0, 0, 5)
SketchLine_16.setAuxiliary(True)
SketchConstraintCoincidence_29 = Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_16.startPoint())
SketchConstraintCoincidence_30 = Sketch_2.setCoincident(SketchCircle_15.center(), SketchLine_16.endPoint())
SketchConstraintLength_6 = Sketch_2.setLength(SketchLine_16.result(), 5)
SketchProjection_6 = Sketch_2.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_17 = SketchProjection_6.createdFeature()
SketchConstraintCoincidence_31 = Sketch_2.setCoincident(SketchCircle_15.center(), SketchLine_17.result())
SketchConstraintRadius_5 = Sketch_2.setRadius(SketchCircle_15.results()[1], 0.45)
SketchProjection_7 = Sketch_2.addProjection(model.selection("EDGE", GeomAPI_Pnt(-6.2, 7.59281015471359e-16, 4)), False)
SketchCircle_16 = SketchProjection_7.createdFeature()
SketchCircle_16.setName("SketchCircle_16")
SketchCircle_16.result().setName("SketchCircle_16")
SketchCircle_16.results()[1].setName("SketchCircle_16_2")
SketchCircle_17 = Sketch_2.addCircle(4.259590630302756, -2.982597469025441, 0.5)
SketchCircle_17.setName("SketchCircle_18")
SketchCircle_17.result().setName("SketchCircle_18")
SketchCircle_17.results()[1].setName("SketchCircle_18_2")
SketchCircle_18 = Sketch_2.addCircle(3.276608177155966, -2.294305745404186, 0.3)
SketchCircle_18.setName("SketchCircle_19")
SketchCircle_18.result().setName("SketchCircle_19")
SketchCircle_18.results()[1].setName("SketchCircle_19_2")
SketchLine_18 = Sketch_2.addLine(0, 0, 4.259590630302756, -2.982597469025441)
SketchLine_18.setAuxiliary(True)
SketchConstraintCoincidence_32 = Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_18.startPoint())
SketchConstraintCoincidence_33 = Sketch_2.setCoincident(SketchCircle_17.center(), SketchLine_18.endPoint())
SketchConstraintCoincidence_34 = Sketch_2.setCoincident(SketchCircle_18.center(), SketchLine_18.result())
SketchProjection_8 = Sketch_2.addProjection(model.selection("EDGE", GeomAPI_Pnt(3.1, 0, 4)), False)
SketchLine_19 = SketchProjection_8.createdFeature()
SketchConstraintAngle_6 = Sketch_2.setAngleBackward(SketchLine_19.result(), SketchLine_18.result(), 35)
SketchConstraintRadius_6 = Sketch_2.setRadius(SketchCircle_17.results()[1], 0.5)
SketchConstraintRadius_7 = Sketch_2.setRadius(SketchCircle_18.results()[1], 0.3)
SketchConstraintDistance_1 = Sketch_2.setDistance(SketchCircle_4.center(), SketchCircle_18.center(), 4, True)
SketchConstraintDistance_2 = Sketch_2.setDistance(SketchCircle_4.center(), SketchLine_18.endPoint(), 5.2, True)
model.do()
ExtrusionCut_1_objects_1 = [model.selection("WIRE", GeomAPI_Pnt(2.927018127601872, 2.325000000002457, 4)), model.selection("WIRE", GeomAPI_Pnt(1.848443178812738, 3.921770223029927, 4)), model.selection("WIRE", GeomAPI_Pnt(3.995574230355639, 0.2028301512488117, 4)), model.selection("WIRE_2", GeomAPI_Pnt(-3.148443178812729, 3.921770223029919, 4)), model.selection("WIRE", GeomAPI_Pnt(-5.127018127597639, 2.325000000000002, 4)), model.selection("WIRE_2", GeomAPI_Pnt(-5.29557423035565, 0.2028301512488154, 4)), model.selection("FACE", GeomAPI_Pnt(7.188472039842964e-12, -4.650000000090492, 4)), model.selection("FACE", GeomAPI_Pnt(4.259590630302756, -2.982597469025441, 4)), model.selection("FACE", GeomAPI_Pnt(3.276608177155966, -2.294305745404186, 4))]
ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, ExtrusionCut_1_objects_1, model.selection(), model.selection("FACE", GeomAPI_Pnt(0, 0, 0)), 0, model.selection(), 0, [model.selection("SOLID", GeomAPI_Pnt(0, 0, 2.542710267975591))])
ExtrusionCut_2_objects_1 = [model.selection("WIRE", GeomAPI_Pnt(-5.44557423035565, 0.2028301512488154, 4)), model.selection("WIRE", GeomAPI_Pnt(-3.298443178812729, 3.921770223029919, 4)), model.selection("WIRE", GeomAPI_Pnt(1.698443178812738, 3.921770223029927, 4)), model.selection("WIRE", GeomAPI_Pnt(3.845574230355639, 0.2028301512488117, 4))]
ExtrusionCut_2 = model.addExtrusionCut(Part_1_doc, ExtrusionCut_2_objects_1, model.selection(), 0, 0.75, [model.selection("SOLID", GeomAPI_Pnt(-0.02796175964867624, -0.05417366434185267, 2.475075117094407))])
Axis_4 = model.addAxis(Part_1_doc, 0, -1, -10)
ExtrusionCut_3 = model.addExtrusionCut(Part_1_doc, [model.selection("WIRE", GeomAPI_Pnt(-0.45, 5, 4))], model.selection("EDGE", GeomAPI_Pnt(0, -0.5, -5)), 10, 10, [model.selection("SOLID", GeomAPI_Pnt(-0.02818028507968423, -0.07071295235452894, 2.466088994028068))])
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", GeomAPI_Pnt(-2.14713105154291, -4.124600374278729, 4)), model.selection("FACE", GeomAPI_Pnt(2.14713105154291, -4.124600374278742, 4))], model.selection(), model.selection("FACE", GeomAPI_Pnt(0, 0, 0)), 0, model.selection(), 0)
Sketch_3 = model.addSketch(Part_1_doc, model.standardPlane("XOZ"))
SketchLine_20 = Sketch_3.addLine(3.5, 2.508501119175777, 3.5, 4)
SketchLine_21 = Sketch_3.addLine(3.5, 4, 6.2, 4)
SketchConstraintCoincidence_35 = Sketch_3.setCoincident(SketchLine_20.endPoint(), SketchLine_21.startPoint())
SketchLine_22 = Sketch_3.addLine(6.2, 4, 6.2, 3.008501119175777)
SketchConstraintCoincidence_36 = Sketch_3.setCoincident(SketchLine_21.endPoint(), SketchLine_22.startPoint())
SketchLine_23 = Sketch_3.addLine(6.2, 3.008501119175777, 3.5, 2.508501119175777)
SketchConstraintCoincidence_37 = Sketch_3.setCoincident(SketchLine_22.endPoint(), SketchLine_23.startPoint())
SketchConstraintCoincidence_38 = Sketch_3.setCoincident(SketchLine_23.endPoint(), SketchLine_20.startPoint())
SketchConstraintVertical_3 = Sketch_3.setVertical(SketchLine_20.result())
SketchProjection_9 = Sketch_3.addProjection(model.selection("EDGE_9", GeomAPI_Pnt(3.5, 0, 1)), False)
SketchLine_24 = SketchProjection_9.createdFeature()
SketchConstraintCoincidence_39 = Sketch_3.setCoincident(SketchLine_20.startPoint(), SketchLine_24.result())
SketchProjection_10 = Sketch_3.addProjection(model.selection("EDGE_3", GeomAPI_Pnt(4.85, 0, 2.25)), False)
SketchLine_25 = SketchProjection_10.createdFeature()
SketchConstraintParallel_1 = Sketch_3.setParallel(SketchLine_23.result(), SketchLine_25.result())
SketchConstraintVertical_4 = Sketch_3.setVertical(SketchLine_22.result())
SketchProjection_11 = Sketch_3.addProjection(model.selection("EDGE_9", GeomAPI_Pnt(6.2, 0, 3.25)), False)
SketchLine_26 = SketchProjection_11.createdFeature()
SketchConstraintCoincidence_40 = Sketch_3.setCoincident(SketchLine_23.startPoint(), SketchLine_26.result())
SketchConstraintHorizontal_2 = Sketch_3.setHorizontal(SketchLine_21.result())
SketchConstraintDistance_3 = Sketch_3.setDistance(SketchLine_20.startPoint(), SketchLine_25.result(), 0.5, True)
SketchProjection_12 = Sketch_3.addProjection(model.selection("EDGE_5", GeomAPI_Pnt(3.1, 0, 4)), False)
SketchLine_27 = SketchProjection_12.createdFeature()
SketchConstraintCoincidence_41 = Sketch_3.setCoincident(SketchLine_20.endPoint(), SketchLine_27.result())
model.do()
Revolution_2 = model.addRevolution(Part_1_doc, [model.selection("COMPOUND", GeomAPI_Pnt(4.850000000000001, 0, 3.254250559587889))], model.selection("EDGE", "PartSet/OZ"), 360, 0)
Common_1 = model.addCommon(Part_1_doc, [model.selection("SOLID", GeomAPI_Pnt(-2.14713105154291, -4.124600374278729, 2)), model.selection("SOLID", GeomAPI_Pnt(2.14713105154291, -4.124600374278742, 2))], [model.selection("SOLID", GeomAPI_Pnt(-1.278142456973851e-15, 2.734224632526547e-16, 3.382519172194574))])
Cut_1 = model.addCut(Part_1_doc, [model.selection("SOLID", GeomAPI_Pnt(-0.02856815579589599, -0.09246122079397179, 2.463292892680745))], [model.selection("SOLID", GeomAPI_Pnt(-2.136430444927541, -4.104043624269158, 3.360182492861197)), model.selection("SOLID", GeomAPI_Pnt(2.136429748085782, -4.104043260947467, 3.360182494729003))])
model.do()
model.testHaveNamingSubshapes(Cut_1, model, Part_1_doc)
model.end()

assert(model.checkPythonDump())
