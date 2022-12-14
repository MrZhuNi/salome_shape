# Copyright (C) 2018-2022  CEA/DEN, EDF R&D
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

"""
    Test2134.py
    Test case for issue #2134 "Conflicting constraint not shown in the viewer"
"""

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOZ"))
SketchLine_1 = Sketch_1.addLine(6.188, 9.025, 6.188, -3.45)
SketchLine_2 = Sketch_1.addLine(6.188, -3.45, 0, -3.45)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(0, -3.45, 0, -6.45)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(0, -6.45, 7.782000000000002, -6.45)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(7.781999999999998, -6.45, 7.782, -3.45)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchLine_6 = Sketch_1.addLine(7.782000000000002, -3.45, 6.538000000000001, -3.45)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchLine_7 = Sketch_1.addLine(6.538000000000001, -3.45, 6.538, -2.4)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchLine_8 = Sketch_1.addLine(6.538000000000002, -2.4, 6.375999999999999, -2.25)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchLine_9 = Sketch_1.addLine(6.375999999999999, -2.25, 6.375999999999999, 9.025)
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_9.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_1.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_5.result())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_6.result())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_4.result())
SketchConstraintVertical_4 = Sketch_1.setVertical(SketchLine_7.result())
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_7.startPoint(), SketchLine_2.result())
SketchLine_10 = Sketch_1.addLine(model.selection("EDGE", "OZ"))
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchLine_3.startPoint(), SketchLine_10.result())
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_10.result())
SketchLine_11 = Sketch_1.addLine(6.187999999999999, 9.025, 6.375999999999999, 9.025)
SketchLine_11.setAuxiliary(True)
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_11.startPoint())
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchLine_9.endPoint(), SketchLine_11.endPoint())
SketchConstraintHorizontal_4 = Sketch_1.setHorizontal(SketchLine_11.result())
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchLine_10.startPoint(), SketchLine_2.result(), 3.45)
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchLine_3.startPoint(), SketchLine_1.result(), 6.188)
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_11.result(), 0.188)
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_3.result(), 3)
SketchLine_12 = Sketch_1.addLine(7.032000000000001, -4.050000000000001, 5.532000000000001, -4.050000000000001)
SketchLine_13 = Sketch_1.addLine(5.532000000000001, -4.050000000000001, 5.532000000000001, -5.850000000000001)
SketchLine_14 = Sketch_1.addLine(5.532000000000001, -5.850000000000001, 7.032000000000001, -5.850000000000001)
SketchLine_15 = Sketch_1.addLine(7.032000000000001, -5.850000000000001, 7.032000000000001, -4.050000000000001)
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchLine_15.endPoint(), SketchLine_12.startPoint())
SketchConstraintCoincidence_15 = Sketch_1.setCoincident(SketchLine_12.endPoint(), SketchLine_13.startPoint())
SketchConstraintCoincidence_16 = Sketch_1.setCoincident(SketchLine_13.endPoint(), SketchLine_14.startPoint())
SketchConstraintCoincidence_17 = Sketch_1.setCoincident(SketchLine_14.endPoint(), SketchLine_15.startPoint())
SketchConstraintHorizontal_5 = Sketch_1.setHorizontal(SketchLine_12.result())
SketchConstraintVertical_5 = Sketch_1.setVertical(SketchLine_13.result())
SketchConstraintHorizontal_6 = Sketch_1.setHorizontal(SketchLine_14.result())
SketchConstraintVertical_6 = Sketch_1.setVertical(SketchLine_15.result())
SketchConstraintLength_3 = Sketch_1.setLength(SketchLine_14.result(), 1.5)
SketchConstraintLength_4 = Sketch_1.setLength(SketchLine_13.result(), 1.8)
SketchLine_16 = Sketch_1.addLine(6.282000000000001, 9.025000000000011, 6.282000000000001, -4.05)
SketchLine_16.setAuxiliary(True)
SketchConstraintCoincidence_18 = Sketch_1.setCoincident(SketchLine_16.startPoint(), SketchLine_11.result())
SketchConstraintCoincidence_19 = Sketch_1.setCoincident(SketchLine_16.endPoint(), SketchLine_12.result())
SketchConstraintVertical_7 = Sketch_1.setVertical(SketchLine_16.result())
SketchConstraintMiddle_1 = Sketch_1.setMiddlePoint(SketchLine_12.result(), SketchLine_16.endPoint())
SketchConstraintMiddle_2 = Sketch_1.setMiddlePoint(SketchLine_16.startPoint(), SketchLine_11.result())
SketchConstraintDistance_3 = Sketch_1.setDistance(SketchLine_16.endPoint(), SketchLine_5.result(), 1.5)
SketchConstraintDistance_4 = Sketch_1.setDistance(SketchLine_16.endPoint(), SketchLine_7.result(), 0.256)
SketchLine_17 = Sketch_1.addLine(model.selection("EDGE", "OX"))
SketchConstraintDistance_5 = Sketch_1.setDistance(SketchLine_1.startPoint(), SketchLine_17.result(), 9.025)
SketchConstraintDistance_6 = Sketch_1.setDistance(SketchLine_8.endPoint(), SketchLine_17.result(), 2.25)
SketchConstraintDistance_7 = Sketch_1.setDistance(SketchLine_7.endPoint(), SketchLine_17.result(), 2.4)
SketchConstraintDistance_8 = Sketch_1.setDistance(SketchLine_2.startPoint(), SketchLine_12.result(), 0.6)
SketchLine_18 = Sketch_1.addLine(6.375999999999999, 9.025, 6.537999999999997, 9.186999999999998)
SketchConstraintCoincidence_20 = Sketch_1.setCoincident(SketchLine_9.endPoint(), SketchLine_18.startPoint())
SketchLine_19 = Sketch_1.addLine(6.538000000000008, 9.187000000000008, 6.537999999999997, 10.256)
SketchConstraintCoincidence_21 = Sketch_1.setCoincident(SketchLine_18.endPoint(), SketchLine_19.startPoint())
SketchLine_20 = Sketch_1.addLine(6.537999999999997, 10.256, 6.187999999999997, 10.256)
SketchConstraintCoincidence_22 = Sketch_1.setCoincident(SketchLine_19.endPoint(), SketchLine_20.startPoint())
SketchLine_21 = Sketch_1.addLine(6.187999999999997, 10.256, 4.93800000000044, 10.97786688777776)
SketchConstraintCoincidence_23 = Sketch_1.setCoincident(SketchLine_20.endPoint(), SketchLine_21.startPoint())
SketchConstraintVertical_8 = Sketch_1.setVertical(SketchLine_19.result())
SketchConstraintHorizontal_7 = Sketch_1.setHorizontal(SketchLine_20.result())
SketchConstraintLength_5 = Sketch_1.setLength(SketchLine_20.result(), 0.35)
SketchConstraintDistance_9 = Sketch_1.setDistance(SketchLine_16.startPoint(), SketchLine_19.result(), 0.256)
SketchConstraintAngle_1 = Sketch_1.setAngleComplementary(SketchLine_18.result(), SketchLine_11.result(), 45)
SketchConstraintDistance_10 = Sketch_1.setDistance(SketchLine_19.endPoint(), SketchLine_17.result(), 10.256)
SketchLine_22 = Sketch_1.addLine(6.188, 9.025, 6.081999999999999, 9.532)
SketchConstraintCoincidence_24 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_22.startPoint())
SketchArc_1 = Sketch_1.addArc(0, 2.425, 4.93800000000044, 10.97786688777776, 0, 12.301, False)
SketchConstraintCoincidence_25 = Sketch_1.setCoincident(SketchLine_10.result(), SketchArc_1.center())
SketchConstraintCoincidence_26 = Sketch_1.setCoincident(SketchLine_21.endPoint(), SketchArc_1.startPoint())
SketchConstraintCoincidence_27 = Sketch_1.setCoincident(SketchLine_10.result(), SketchArc_1.endPoint())
SketchArc_2 = Sketch_1.addArc(-1.129352735403899e-028, 2.425, 4.856000000000012, 10.83583872155449, 0, 12.137, False)
SketchConstraintCoincidence_28 = Sketch_1.setCoincident(SketchLine_10.result(), SketchArc_2.center())
SketchConstraintCoincidence_29 = Sketch_1.setCoincident(SketchLine_10.result(), SketchArc_2.endPoint())
SketchConstraintCoincidence_30 = Sketch_1.setCoincident(SketchArc_1.center(), SketchArc_2.center())
SketchLine_23 = Sketch_1.addLine(0, 12.301, 0, 12.137)
SketchConstraintCoincidence_31 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_23.startPoint())
SketchConstraintCoincidence_32 = Sketch_1.setCoincident(SketchArc_2.endPoint(), SketchLine_23.endPoint())
SketchConstraintDistance_11 = Sketch_1.setDistance(SketchArc_2.center(), SketchLine_17.result(), 2.425)
SketchArc_3 = Sketch_1.addArc(1.409050931477502e-030, 2.425, -1.654361225106055e-024, 12.219, 5.756768760952482, 10.34851244290824, True)
SketchArc_3.setAuxiliary(True)
SketchConstraintCoincidence_33 = Sketch_1.setCoincident(SketchArc_1.center(), SketchArc_3.center())
SketchConstraintCoincidence_34 = Sketch_1.setCoincident(SketchLine_23.result(), SketchArc_3.startPoint())
SketchConstraintMiddle_3 = Sketch_1.setMiddlePoint(SketchLine_23.result(), SketchArc_3.startPoint())
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_3.results()[1], 9.794000000000001)
SketchLine_24 = Sketch_1.addLine(4.856000000000012, 10.83583872155449, 5.1433963786475, 10.65615482129429)
SketchConstraintCoincidence_35 = Sketch_1.setCoincident(SketchArc_2.startPoint(), SketchLine_24.startPoint())
SketchLine_25 = Sketch_1.addLine(5.1433963786475, 10.65615482129429, 5.417460848824597, 10.45671600291325)
SketchConstraintCoincidence_36 = Sketch_1.setCoincident(SketchLine_24.endPoint(), SketchLine_25.startPoint())
SketchLine_26 = Sketch_1.addLine(5.417460848824597, 10.45671600291325, 5.676945960870162, 10.23864578403355)
SketchConstraintCoincidence_37 = Sketch_1.setCoincident(SketchLine_25.endPoint(), SketchLine_26.startPoint())
SketchArc_4 = Sketch_1.addArc(4.79378612024245, 9.263, 6.082, 9.532, 5.676945960870163, 10.23864578403355, False)
SketchConstraintCoincidence_38 = Sketch_1.setCoincident(SketchLine_22.endPoint(), SketchArc_4.startPoint())
SketchConstraintCoincidence_39 = Sketch_1.setCoincident(SketchArc_4.endPoint(), SketchLine_26.endPoint())
SketchConstraintDistance_12 = Sketch_1.setDistance(SketchArc_4.startPoint(), SketchLine_17.result(), 9.532)
SketchConstraintDistance_13 = Sketch_1.setDistance(SketchArc_4.startPoint(), SketchLine_1.result(), 0.106)
SketchConstraintRadius_2 = Sketch_1.setRadius(SketchArc_4.results()[1], 1.316)
SketchConstraintDistance_14 = Sketch_1.setDistance(SketchArc_4.center(), SketchLine_17.result(), 9.263)
SketchLine_27 = Sketch_1.addLine(-1.129352735403899e-028, 2.425, 4.897000000000013, 10.90685280466482)
SketchLine_27.setAuxiliary(True)
SketchConstraintCoincidence_40 = Sketch_1.setCoincident(SketchArc_1.center(), SketchLine_27.startPoint())
SketchConstraintCoincidence_41 = Sketch_1.setCoincident(SketchLine_27.endPoint(), SketchArc_3.results()[1])
SketchConstraintCoincidence_42 = Sketch_1.setCoincident(SketchArc_2.startPoint(), SketchLine_27.result())
SketchLine_28 = Sketch_1.addLine(-2.420652665866155e-034, 2.425, 5.190029273900023, 10.73078305375606)
SketchLine_28.setAuxiliary(True)
SketchConstraintCoincidence_43 = Sketch_1.setCoincident(SketchArc_1.center(), SketchLine_28.startPoint())
SketchConstraintCoincidence_44 = Sketch_1.setCoincident(SketchLine_28.endPoint(), SketchArc_3.results()[1])
SketchConstraintCoincidence_45 = Sketch_1.setCoincident(SketchLine_28.result(), SketchLine_24.endPoint())
SketchLine_29 = Sketch_1.addLine(0, 2.425, 5.476735296592496, 10.54459398560408)
SketchLine_29.setAuxiliary(True)
SketchConstraintCoincidence_46 = Sketch_1.setCoincident(SketchArc_1.center(), SketchLine_29.startPoint())
SketchConstraintCoincidence_47 = Sketch_1.setCoincident(SketchLine_29.endPoint(), SketchArc_3.results()[1])
SketchConstraintCoincidence_48 = Sketch_1.setCoincident(SketchLine_25.endPoint(), SketchLine_29.result())
SketchLine_30 = Sketch_1.addLine(-1.129352735403899e-028, 2.425, 5.756768760952482, 10.34851244290824)
SketchLine_30.setAuxiliary(True)
SketchConstraintCoincidence_49 = Sketch_1.setCoincident(SketchArc_1.center(), SketchLine_30.startPoint())
SketchConstraintCoincidence_50 = Sketch_1.setCoincident(SketchArc_3.endPoint(), SketchLine_30.endPoint())
SketchConstraintCoincidence_51 = Sketch_1.setCoincident(SketchArc_4.endPoint(), SketchLine_30.result())
SketchConstraintDistance_15 = Sketch_1.setDistance(SketchArc_3.startPoint(), SketchLine_23.endPoint(), 0.082)
SketchConstraintDistance_16 = Sketch_1.setDistance(SketchLine_24.endPoint(), SketchLine_28.endPoint(), 0.088)
SketchConstraintDistance_17 = Sketch_1.setDistance(SketchLine_25.endPoint(), SketchLine_29.endPoint(), 0.106)
SketchConstraintAngle_2 = Sketch_1.setAngle(SketchLine_30.result(), SketchLine_10.result(), 36)
SketchConstraintAngle_3 = Sketch_1.setAngle(SketchLine_29.result(), SketchLine_10.result(), 34)
SketchConstraintAngle_4 = Sketch_1.setAngle(SketchLine_28.result(), SketchLine_10.result(), 32)
SketchConstraintAngle_5 = Sketch_1.setAngle(SketchLine_27.result(), SketchLine_10.result(), 30)
SketchConstraintCoincidence_52 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_27.result())
model.do()

# check sketch is correct
assert(Sketch_1.solverError().value() == "")

# add coincidence between center of arc and a point from the first sketch
SketchConstraintDistance_18 = Sketch_1.setDistance(SketchLine_28.endPoint(), SketchLine_21.result(), 0.088, True)
model.do()
assert(Sketch_1.solverError().value() != "")

# remove constraint, check sketch is correct now
partSet.removeFeature(SketchConstraintDistance_18.feature())
model.do()
assert(Sketch_1.solverError().value() == "")

model.end()

assert(model.checkPythonDump())