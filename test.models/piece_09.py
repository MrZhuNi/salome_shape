# Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.addParameter(Part_1_doc, "languette", "20")
model.addParameter(Part_1_doc, "d1", "70")
model.addParameter(Part_1_doc, "d2", "77.5")
model.addParameter(Part_1_doc, "d3", "32.5")
model.addParameter(Part_1_doc, "h1", "10")
model.addParameter(Part_1_doc, "r_trou", "3.125")
model.addParameter(Part_1_doc, "r1", "7.5")
model.addParameter(Part_1_doc, "r_trou2", "5")
model.addParameter(Part_1_doc, "d4", "110")
model.addParameter(Part_1_doc, "d5", "21.25")
model.addParameter(Part_1_doc, "d6", "31.25")
model.addParameter(Part_1_doc, "angle", "30")
model.addParameter(Part_1_doc, "h2", "15")
model.addParameter(Part_1_doc, "h3", "7.5")
model.addParameter(Part_1_doc, "r2", "3.75")
model.addParameter(Part_1_doc, "r3", "1.25")
model.addParameter(Part_1_doc, "epaisseur", "2.5")
model.addParameter(Part_1_doc, "e1", "6.2")
model.addParameter(Part_1_doc, "e2", "15")
model.addParameter(Part_1_doc, "d7", "11.2")
model.addParameter(Part_1_doc, "h4", "40")
model.addParameter(Part_1_doc, "d_diff", "5")
model.addParameter(Part_1_doc, "e3", "35")
model.addParameter(Part_1_doc, "d8", "23.75")
model.addParameter(Part_1_doc, "d9", "6.25")
model.addParameter(Part_1_doc, "d_trou", "17.5")
model.addParameter(Part_1_doc, "coeff", "1")
model.addParameter(Part_1_doc, "epaisseur_plat", "25")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchLine_1 = Sketch_1.addLine(0, 0, 13.23223304703355, -2.874692731756102e-26)
SketchPoint_1 = Sketch_1.addPoint(model.selection("VERTEX", "PartSet/Origin"))
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchPoint_1.result())
SketchLine_2 = Sketch_1.addLine(model.selection("EDGE", "PartSet/OX"))
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.result())
SketchArc_1 = Sketch_1.addArc(13.23223304703355, 5.51776695296637, 13.23223304703355, -2.874692731756102e-26, 18.74999999999991, 5.51776695296637, False)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchArc_1.startPoint())
SketchConstraintTangent_1 = Sketch_1.setTangent(SketchLine_1.result(), SketchArc_1.results()[1])
SketchLine_3 = Sketch_1.addLine(18.74999999999991, 5.51776695296637, 18.74999999999991, 29.48223304703358)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_3.startPoint())
SketchArc_2 = Sketch_1.addArc(21.76776695296633, 29.48223304703358, 18.74999999999991, 29.48223304703358, 21.76776695296638, 32.50000000000001, True)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchArc_2.startPoint())
SketchConstraintTangent_2 = Sketch_1.setTangent(SketchLine_3.result(), SketchArc_2.results()[1])
SketchLine_4 = Sketch_1.addLine(21.76776695296638, 32.50000000000001, 88.23223304703366, 32.50000000000001)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchArc_2.endPoint(), SketchLine_4.startPoint())
SketchArc_3 = Sketch_1.addArc(88.23223304703366, 29.48223304703358, 88.23223304703366, 32.50000000000001, 91.25000000000009, 29.48223304703358, True)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchArc_3.startPoint())
SketchConstraintTangent_3 = Sketch_1.setTangent(SketchLine_4.result(), SketchArc_3.results()[1])
SketchLine_5 = Sketch_1.addLine(91.25000000000009, 29.48223304703358, 91.25000000000009, 5.51776695296637)
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchArc_3.endPoint(), SketchLine_5.startPoint())
SketchArc_4 = Sketch_1.addArc(96.76776695296645, 5.51776695296637, 91.25000000000009, 5.51776695296637, 96.76776695296645, 0, False)
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchArc_4.startPoint())
SketchConstraintTangent_4 = Sketch_1.setTangent(SketchLine_5.result(), SketchArc_4.results()[1])
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchLine_2.result(), SketchArc_4.endPoint())
SketchLine_6 = Sketch_1.addLine(96.76776695296645, 0, 110, 0)
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchArc_4.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_2.result())
SketchLine_7 = Sketch_1.addLine(0, 0, 0, 2.5)
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_7.startPoint())
SketchLine_8 = Sketch_1.addLine(0, 2.5, 14.99999999999991, 2.5)
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchArc_5 = Sketch_1.addArc(14.99999999999991, 3.75, 14.99999999999991, 2.5, 16.24999999999991, 3.75, False)
SketchConstraintCoincidence_15 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchArc_5.startPoint())
SketchConstraintTangent_5 = Sketch_1.setTangent(SketchLine_8.result(), SketchArc_5.results()[1])
SketchLine_9 = Sketch_1.addLine(16.24999999999991, 3.75, 16.24999999999991, 31.24999999999991)
SketchConstraintCoincidence_16 = Sketch_1.setCoincident(SketchArc_5.endPoint(), SketchLine_9.startPoint())
SketchArc_6 = Sketch_1.addArc(19.99999999999991, 31.24999999999991, 16.24999999999991, 31.24999999999991, 19.99999999999996, 35, True)
SketchConstraintCoincidence_17 = Sketch_1.setCoincident(SketchLine_9.endPoint(), SketchArc_6.startPoint())
SketchConstraintTangent_6 = Sketch_1.setTangent(SketchLine_9.result(), SketchArc_6.results()[1])
SketchLine_10 = Sketch_1.addLine(19.99999999999996, 35, 89.99999999999997, 35)
SketchConstraintCoincidence_18 = Sketch_1.setCoincident(SketchArc_6.endPoint(), SketchLine_10.startPoint())
SketchArc_7 = Sketch_1.addArc(90.00000000000009, 31.24999999999977, 89.99999999999997, 35, 93.75000000000009, 31.24999999999977, True)
SketchConstraintCoincidence_19 = Sketch_1.setCoincident(SketchLine_10.endPoint(), SketchArc_7.startPoint())
SketchConstraintTangent_7 = Sketch_1.setTangent(SketchLine_10.result(), SketchArc_7.results()[1])
SketchLine_11 = Sketch_1.addLine(93.75000000000009, 31.24999999999977, 93.75000000000009, 3.75)
SketchConstraintCoincidence_20 = Sketch_1.setCoincident(SketchArc_7.endPoint(), SketchLine_11.startPoint())
SketchArc_8 = Sketch_1.addArc(95.00000000000009, 3.75, 93.75000000000009, 3.75, 95.00000000000009, 2.5, False)
SketchConstraintCoincidence_21 = Sketch_1.setCoincident(SketchLine_11.endPoint(), SketchArc_8.startPoint())
SketchConstraintTangent_8 = Sketch_1.setTangent(SketchLine_11.result(), SketchArc_8.results()[1])
SketchLine_12 = Sketch_1.addLine(95.00000000000009, 2.5, 110, 2.5)
SketchConstraintCoincidence_22 = Sketch_1.setCoincident(SketchArc_8.endPoint(), SketchLine_12.startPoint())
SketchLine_13 = Sketch_1.addLine(110, 2.5, 110, 0)
SketchConstraintCoincidence_23 = Sketch_1.setCoincident(SketchLine_12.endPoint(), SketchLine_13.startPoint())
SketchConstraintCoincidence_24 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_13.endPoint())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_7.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_3.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_9.result())
SketchConstraintVertical_4 = Sketch_1.setVertical(SketchLine_5.result())
SketchConstraintVertical_5 = Sketch_1.setVertical(SketchLine_11.result())
SketchConstraintVertical_6 = Sketch_1.setVertical(SketchLine_13.result())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_10.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_4.result())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_12.result())
SketchConstraintTangent_9 = Sketch_1.setTangent(SketchArc_6.results()[1], SketchLine_10.result())
SketchConstraintTangent_10 = Sketch_1.setTangent(SketchArc_7.results()[1], SketchLine_11.result())
SketchConstraintTangent_11 = Sketch_1.setTangent(SketchArc_8.results()[1], SketchLine_12.result())
SketchConstraintTangent_12 = Sketch_1.setTangent(SketchArc_5.results()[1], SketchLine_9.result())
SketchConstraintTangent_13 = Sketch_1.setTangent(SketchArc_1.results()[1], SketchLine_3.result())
SketchConstraintHorizontal_4 = Sketch_1.setHorizontal(SketchLine_8.result())
SketchConstraintTangent_14 = Sketch_1.setTangent(SketchArc_4.results()[1], SketchLine_6.result())
SketchConstraintTangent_15 = Sketch_1.setTangent(SketchArc_2.results()[1], SketchLine_4.result())
SketchConstraintTangent_16 = Sketch_1.setTangent(SketchArc_3.results()[1], SketchLine_5.result())
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchArc_5.results()[1], SketchArc_8.results()[1])
SketchConstraintEqual_2 = Sketch_1.setEqual(SketchLine_8.result(), SketchLine_12.result())
SketchConstraintEqual_3 = Sketch_1.setEqual(SketchLine_1.result(), SketchLine_6.result())
SketchConstraintEqual_4 = Sketch_1.setEqual(SketchArc_1.results()[1], SketchArc_4.results()[1])
SketchConstraintEqual_5 = Sketch_1.setEqual(SketchLine_3.result(), SketchLine_5.result())
SketchConstraintEqual_6 = Sketch_1.setEqual(SketchLine_9.result(), SketchLine_11.result())
SketchConstraintEqual_7 = Sketch_1.setEqual(SketchLine_7.result(), SketchLine_13.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_7.result(), "epaisseur*coeff")
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchArc_5.endPoint(), SketchLine_3.result(), "epaisseur*coeff")
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchArc_6.endPoint(), SketchLine_4.result(), "epaisseur*coeff")
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_5.results()[1], "r3*coeff")
SketchConstraintRadius_2 = Sketch_1.setRadius(SketchArc_6.results()[1], "r2*coeff")
SketchConstraintDistance_3 = Sketch_1.setDistance(SketchLine_12.endPoint(), SketchLine_7.result(), "d4*coeff")
SketchConstraintDistance_4 = Sketch_1.setDistance(SketchArc_5.center(), SketchArc_1.center(), "epaisseur*coeff")
SketchConstraintDistance_5 = Sketch_1.setDistance(SketchArc_2.center(), SketchArc_6.center(), "epaisseur*coeff")
SketchConstraintDistance_6 = Sketch_1.setDistance(SketchLine_1.result(), SketchArc_6.endPoint(), "(h4-d_diff)*coeff")
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_10.result(), "d1*coeff")
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("WIRE", "Sketch_1/Face-SketchLine_1r-SketchArc_1_2f-SketchLine_3f-SketchArc_2_2r-SketchLine_4f-SketchArc_3_2r-SketchLine_5f-SketchArc_4_2f-SketchLine_6f-SketchLine_13r-SketchLine_12r-SketchArc_8_2r-SketchLine_11r-SketchArc_7_2f-SketchLine_10r-SketchArc_6_2f-SketchLine_9r-SketchArc_5_2r-SketchLine_8r-SketchLine_7r_wire")], model.selection(), "epaisseur_plat*coeff", 0)
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_7"), "d2*coeff", True)
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_1"))
SketchProjection_1 = Sketch_2.addProjection(model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchArc_7_2][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_10]"), False)
SketchLine_14 = SketchProjection_1.createdFeature()
SketchProjection_2 = Sketch_2.addProjection(model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_11][Extrusion_1_1/To_Face]"), False)
SketchLine_15 = SketchProjection_2.createdFeature()
SketchLine_16 = Sketch_2.addLine(-24.99999999999999, -35.00000000000001, -31.24999999999999, -35.00000000000001)
SketchConstraintCoincidence_25 = Sketch_2.setCoincident(SketchAPI_Line(SketchLine_14).endPoint(), SketchLine_16.startPoint())
SketchArc_9 = Sketch_2.addArc(-31.24999999999999, -31.25000000000001, -31.24999999999999, -35.00000000000001, -34.99999999999999, -31.25000000000001, True)
SketchConstraintCoincidence_26 = Sketch_2.setCoincident(SketchLine_16.endPoint(), SketchArc_9.startPoint())
SketchConstraintTangent_17 = Sketch_2.setTangent(SketchLine_16.result(), SketchArc_9.results()[1])
SketchLine_17 = Sketch_2.addLine(-35, -31.24999999999999, -35, 1.250000000000009)
SketchConstraintCoincidence_27 = Sketch_2.setCoincident(SketchArc_9.endPoint(), SketchLine_17.startPoint())
SketchArc_10 = Sketch_2.addArc(-31.25, 1.249999999999998, -35, 1.249999999999998, -31.25, 4.999999999999998, True)
SketchConstraintCoincidence_28 = Sketch_2.setCoincident(SketchLine_17.endPoint(), SketchArc_10.startPoint())
SketchConstraintTangent_18 = Sketch_2.setTangent(SketchLine_17.result(), SketchArc_10.results()[1])
SketchLine_18 = Sketch_2.addLine(-31.25, 4.999999999999999, -11.25, 4.999999999999999)
SketchConstraintCoincidence_29 = Sketch_2.setCoincident(SketchArc_10.endPoint(), SketchLine_18.startPoint())
SketchLine_19 = Sketch_2.addLine(-11.25, 5.000000000000009, -11.25, 2.500000000000009)
SketchConstraintCoincidence_30 = Sketch_2.setCoincident(SketchLine_18.endPoint(), SketchLine_19.startPoint())
SketchLine_20 = Sketch_2.addLine(-11.25, 2.500000000000002, -31.24999999999999, 2.500000000000017)
SketchConstraintCoincidence_31 = Sketch_2.setCoincident(SketchLine_19.endPoint(), SketchLine_20.startPoint())
SketchArc_11 = Sketch_2.addArc(-31.25, 1.25, -31.25, 2.500000000000002, -32.5, 1.250000000000808, False)
SketchConstraintCoincidence_32 = Sketch_2.setCoincident(SketchLine_20.endPoint(), SketchArc_11.startPoint())
SketchConstraintTangent_19 = Sketch_2.setTangent(SketchLine_20.result(), SketchArc_11.results()[1])
SketchLine_21 = Sketch_2.addLine(-32.5, 1.249999999999992, -32.5, -31.24999999999999)
SketchConstraintCoincidence_33 = Sketch_2.setCoincident(SketchArc_11.endPoint(), SketchLine_21.startPoint())
SketchArc_12 = Sketch_2.addArc(-31.25, -31.25, -32.5, -31.25, -31.25, -32.5, False)
SketchConstraintCoincidence_34 = Sketch_2.setCoincident(SketchLine_21.endPoint(), SketchArc_12.startPoint())
SketchConstraintTangent_20 = Sketch_2.setTangent(SketchLine_21.result(), SketchArc_12.results()[1])
SketchLine_22 = Sketch_2.addLine(-31.25, -32.50000000000001, -25.00000000000001, -32.50000000000001)
SketchConstraintCoincidence_35 = Sketch_2.setCoincident(SketchArc_12.endPoint(), SketchLine_22.startPoint())
SketchLine_23 = Sketch_2.addLine(-24.99999999999999, -35.00000000000001, -24.99999999999999, -32.50000000000002)
SketchConstraintCoincidence_36 = Sketch_2.setCoincident(SketchAPI_Line(SketchLine_14).endPoint(), SketchLine_23.startPoint())
SketchConstraintCoincidence_37 = Sketch_2.setCoincident(SketchLine_22.endPoint(), SketchLine_23.endPoint())
SketchConstraintHorizontal_5 = Sketch_2.setHorizontal(SketchLine_16.result())
SketchConstraintHorizontal_6 = Sketch_2.setHorizontal(SketchLine_22.result())
SketchConstraintHorizontal_7 = Sketch_2.setHorizontal(SketchLine_20.result())
SketchConstraintHorizontal_8 = Sketch_2.setHorizontal(SketchLine_18.result())
SketchConstraintVertical_7 = Sketch_2.setVertical(SketchLine_17.result())
SketchConstraintVertical_8 = Sketch_2.setVertical(SketchLine_21.result())
SketchConstraintVertical_9 = Sketch_2.setVertical(SketchLine_23.result())
SketchConstraintVertical_10 = Sketch_2.setVertical(SketchLine_19.result())
SketchConstraintTangent_21 = Sketch_2.setTangent(SketchArc_9.results()[1], SketchLine_17.result())
SketchConstraintTangent_22 = Sketch_2.setTangent(SketchArc_10.results()[1], SketchLine_18.result())
SketchConstraintTangent_23 = Sketch_2.setTangent(SketchArc_11.results()[1], SketchLine_21.result())
SketchConstraintTangent_24 = Sketch_2.setTangent(SketchArc_12.results()[1], SketchLine_22.result())
SketchConstraintDistance_7 = Sketch_2.setDistance(SketchLine_23.startPoint(), SketchLine_18.result(), "h4*coeff")
SketchConstraintDistance_8 = Sketch_2.setDistance(SketchLine_16.startPoint(), SketchLine_22.result(), "epaisseur*coeff")
SketchConstraintDistance_9 = Sketch_2.setDistance(SketchLine_21.endPoint(), SketchLine_17.result(), "epaisseur*coeff")
SketchConstraintDistance_10 = Sketch_2.setDistance(SketchArc_11.startPoint(), SketchLine_18.result(), "epaisseur*coeff")
SketchConstraintCoincidence_39 = Sketch_2.setCoincident(SketchLine_16.startPoint(), SketchAPI_Line(SketchLine_14).endPoint())
SketchPoint_2 = Sketch_2.addPoint(model.selection("VERTEX", "PartSet/Origin"))
SketchConstraintDistance_11 = Sketch_2.setDistance(SketchPoint_2.coordinates(), SketchLine_17.result(), "e3*coeff", False)
SketchConstraintRadius_3 = Sketch_2.setRadius(SketchArc_9.results()[1], "r2*coeff")
SketchConstraintEqual_9 = Sketch_2.setEqual(SketchArc_9.results()[1], SketchArc_10.results()[1])
SketchConstraintEqual_10 = Sketch_2.setEqual(SketchArc_11.results()[1], SketchArc_12.results()[1])
SketchConstraintRadius_4 = Sketch_2.setRadius(SketchArc_12.results()[1], "r3*coeff")
SketchConstraintDistance_13 = Sketch_2.setDistance(SketchArc_10.startPoint(), SketchLine_19.result(), "d8*coeff")
model.do()
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("WIRE", "Sketch_2/Face-SketchLine_23f-SketchLine_22r-SketchArc_12_2r-SketchLine_21r-SketchArc_11_2r-SketchLine_20r-SketchLine_19r-SketchLine_18r-SketchArc_10_2f-SketchLine_17r-SketchArc_9_2f-SketchLine_16r_wire")], model.selection(), "d6*coeff", 0)
Sketch_3 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_2_1/Generated_Face&Sketch_2/SketchLine_17"))
SketchLine_24 = Sketch_3.addLine(46.25, 22.91561885925604, 51.3, 20.00000000000001)
SketchLine_25 = Sketch_3.addLine(model.selection("EDGE", "[Extrusion_2_1/Generated_Face&Sketch_2/SketchLine_17][Extrusion_2_1/To_Face]"))
SketchLine_26 = Sketch_3.addLine(51.3, 20.00000000000001, 62.50000000000001, 20.00000000000001)
SketchConstraintCoincidence_42 = Sketch_3.setCoincident(SketchLine_24.endPoint(), SketchLine_26.startPoint())
SketchLine_27 = Sketch_3.addLine(62.50000000000001, 20.00000000000001, 62.5, -4.999999999999999)
SketchConstraintCoincidence_43 = Sketch_3.setCoincident(SketchLine_26.endPoint(), SketchLine_27.startPoint())
SketchLine_28 = Sketch_3.addLine(62.5, -4.999999999999999, 46.25, -4.999999999999999)
SketchConstraintCoincidence_44 = Sketch_3.setCoincident(SketchLine_27.endPoint(), SketchLine_28.startPoint())
SketchLine_29 = Sketch_3.addLine(46.25, -4.999999999999999, 46.25, 22.91561885925604)
SketchConstraintCoincidence_45 = Sketch_3.setCoincident(SketchLine_28.endPoint(), SketchLine_29.startPoint())
SketchLine_30 = Sketch_3.addLine(46.25, 35.00000000000001, 56.25000000000002, 35.00000000000001)
SketchLine_31 = Sketch_3.addLine(56.25000000000002, 35.00000000000001, 56.25000000000002, 27.50000000000001)
SketchConstraintCoincidence_46 = Sketch_3.setCoincident(SketchLine_30.endPoint(), SketchLine_31.startPoint())
SketchLine_32 = Sketch_3.addLine(56.25000000000002, 27.50000000000001, 46.25, 27.50000000000001)
SketchConstraintCoincidence_47 = Sketch_3.setCoincident(SketchLine_31.endPoint(), SketchLine_32.startPoint())
SketchConstraintCoincidence_48 = Sketch_3.setCoincident(SketchLine_32.endPoint(), SketchLine_25.result())
SketchLine_33 = Sketch_3.addLine(46.25, 27.50000000000001, 46.25, 35.00000000000001)
SketchConstraintCoincidence_49 = Sketch_3.setCoincident(SketchLine_32.endPoint(), SketchLine_33.startPoint())
SketchConstraintCoincidence_50 = Sketch_3.setCoincident(SketchLine_30.startPoint(), SketchLine_33.endPoint())
SketchConstraintHorizontal_9 = Sketch_3.setHorizontal(SketchLine_28.result())
SketchConstraintHorizontal_10 = Sketch_3.setHorizontal(SketchLine_26.result())
SketchConstraintHorizontal_11 = Sketch_3.setHorizontal(SketchLine_32.result())
SketchConstraintHorizontal_12 = Sketch_3.setHorizontal(SketchLine_30.result())
SketchConstraintVertical_11 = Sketch_3.setVertical(SketchLine_29.result())
SketchConstraintVertical_12 = Sketch_3.setVertical(SketchLine_27.result())
SketchConstraintVertical_13 = Sketch_3.setVertical(SketchLine_33.result())
SketchConstraintVertical_14 = Sketch_3.setVertical(SketchLine_31.result())
SketchConstraintAngle_1 = Sketch_3.setAngleComplementary(SketchLine_26.result(), SketchLine_24.result(), "angle")
SketchProjection_3 = Sketch_3.addProjection(model.selection("EDGE", "[Extrusion_2_1/Generated_Face&Sketch_2/SketchLine_18][Extrusion_2_1/Generated_Face&Sketch_2/SketchArc_10_2]"), False)
SketchLine_34 = SketchProjection_3.createdFeature()
SketchProjection_4 = Sketch_3.addProjection(model.selection("EDGE", "[Extrusion_2_1/Generated_Face&Sketch_2/SketchArc_9_2][Extrusion_2_1/Generated_Face&Sketch_2/SketchLine_16]"), False)
SketchLine_35 = SketchProjection_4.createdFeature()
SketchConstraintCoincidence_51 = Sketch_3.setCoincident(SketchLine_28.endPoint(), SketchLine_34.result())
SketchConstraintDistance_15 = Sketch_3.setDistance(SketchLine_31.result(), SketchLine_33.endPoint(), "(d6-d5)*coeff")
SketchConstraintLength_3 = Sketch_3.setLength(SketchLine_33.result(), "h3*coeff")
SketchConstraintCoincidence_52 = Sketch_3.setCoincident(SketchLine_30.endPoint(), SketchLine_35.result())
SketchConstraintDistance_16 = Sketch_3.setDistance(SketchLine_27.result(), SketchLine_28.endPoint(), "(d6-e2)*coeff")
SketchConstraintCoincidence_53 = Sketch_3.setCoincident(SketchLine_28.endPoint(), SketchLine_25.result())
SketchConstraintCoincidence_54 = Sketch_3.setCoincident(SketchLine_29.endPoint(), SketchLine_24.startPoint())
SketchConstraintDistance_17 = Sketch_3.setDistance(SketchLine_26.result(), SketchAPI_Line(SketchLine_35).startPoint(), "h2*coeff")
SketchConstraintLength_4 = Sketch_3.setLength(SketchLine_26.result(), "d7*coeff")
model.do()
ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [model.selection("FACE", "Sketch_3/Face-SketchLine_29r-SketchLine_28r-SketchLine_27r-SketchLine_26r-SketchLine_24r")], model.selection(), model.selection("FACE", "Extrusion_2_1/Generated_Face&Sketch_2/SketchLine_19"), 0, model.selection(), 0, [model.selection("SOLID", "Extrusion_2_1")])
ExtrusionCut_2 = model.addExtrusionCut(Part_1_doc, [model.selection("FACE", "Sketch_3/Face-SketchLine_33r-SketchLine_32r-SketchLine_31r-SketchLine_30r")], model.selection(), model.selection("FACE", "Extrusion_1_1/To_Face"), 0, model.selection(), 0, [model.selection("SOLID", "ExtrusionCut_1_1")])
Plane_5 = model.addPlane(Part_1_doc, model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_7"), "d3*coeff", True)
Sketch_4 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_2"))
SketchProjection_5 = Sketch_4.addProjection(model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_9][Extrusion_1_1/To_Face]"), False)
SketchLine_36 = SketchProjection_5.createdFeature()
SketchProjection_6 = Sketch_4.addProjection(model.selection("EDGE", "[ExtrusionCut_2_1/Modified_Face&Sketch_2/SketchLine_23][ExtrusionCut_2_1/Generated_Face&Sketch_3/SketchLine_31]"), False)
SketchLine_37 = SketchProjection_6.createdFeature()
SketchProjection_7 = Sketch_4.addProjection(model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_10][Extrusion_1_1/Generated_Face&Sketch_1/SketchArc_6_2]"), False)
SketchLine_38 = SketchProjection_7.createdFeature()
SketchLine_39 = Sketch_4.addLine(-25, -35.00000000000001, -31.24999999999999, -35.00000000000001)
SketchConstraintCoincidence_55 = Sketch_4.setCoincident(SketchAPI_Line(SketchLine_37).startPoint(), SketchLine_39.startPoint())
SketchArc_13 = Sketch_4.addArc(-31.25, -31.25000000000001, -31.24999999999999, -35.00000000000001, -34.99999999999999, -31.25000000000001, True)
SketchConstraintCoincidence_56 = Sketch_4.setCoincident(SketchLine_39.endPoint(), SketchArc_13.startPoint())
SketchConstraintTangent_25 = Sketch_4.setTangent(SketchLine_39.result(), SketchArc_13.results()[1])
SketchLine_40 = Sketch_4.addLine(-34.99999999999999, -31.25000000000001, -34.99999999999999, -20)
SketchConstraintCoincidence_57 = Sketch_4.setCoincident(SketchArc_13.endPoint(), SketchLine_40.startPoint())
SketchLine_41 = Sketch_4.addLine(-34.99999999999999, -20, -32.49999999999999, -20)
SketchConstraintCoincidence_58 = Sketch_4.setCoincident(SketchLine_40.endPoint(), SketchLine_41.startPoint())
SketchLine_42 = Sketch_4.addLine(-32.49999999999999, -20, -32.49999999999999, -31.24999999999389)
SketchConstraintCoincidence_59 = Sketch_4.setCoincident(SketchLine_41.endPoint(), SketchLine_42.startPoint())
SketchArc_14 = Sketch_4.addArc(-31.24999999999999, -31.24999999999393, -32.49999999999999, -31.24999999999389, -31.25000000000371, -32.50000000000001, False)
SketchConstraintCoincidence_60 = Sketch_4.setCoincident(SketchLine_42.endPoint(), SketchArc_14.startPoint())
SketchConstraintTangent_26 = Sketch_4.setTangent(SketchLine_42.result(), SketchArc_14.results()[1])
SketchLine_43 = Sketch_4.addLine(-31.25000000000371, -32.50000000000001, -25, -32.50000000000001)
SketchConstraintCoincidence_61 = Sketch_4.setCoincident(SketchArc_14.endPoint(), SketchLine_43.startPoint())
SketchConstraintCoincidence_62 = Sketch_4.setCoincident(SketchAPI_Line(SketchLine_37).endPoint(), SketchLine_43.endPoint())
SketchLine_44 = Sketch_4.addLine(-25, -35.00000000000001, -25, -32.50000000000001)
SketchConstraintCoincidence_63 = Sketch_4.setCoincident(SketchAPI_Line(SketchLine_37).startPoint(), SketchLine_44.startPoint())
SketchConstraintCoincidence_64 = Sketch_4.setCoincident(SketchAPI_Line(SketchLine_37).endPoint(), SketchLine_44.endPoint())
SketchConstraintHorizontal_13 = Sketch_4.setHorizontal(SketchLine_41.result())
SketchConstraintHorizontal_14 = Sketch_4.setHorizontal(SketchLine_43.result())
SketchConstraintHorizontal_15 = Sketch_4.setHorizontal(SketchLine_39.result())
SketchConstraintVertical_15 = Sketch_4.setVertical(SketchLine_42.result())
SketchConstraintVertical_16 = Sketch_4.setVertical(SketchLine_40.result())
SketchConstraintTangent_27 = Sketch_4.setTangent(SketchArc_13.results()[1], SketchLine_40.result())
SketchConstraintTangent_28 = Sketch_4.setTangent(SketchArc_14.results()[1], SketchLine_43.result())
SketchConstraintDistance_18 = Sketch_4.setDistance(SketchLine_40.result(), SketchLine_44.startPoint(), "h1*coeff")
SketchConstraintRadius_5 = Sketch_4.setRadius(SketchArc_13.results()[1], "r2*coeff")
SketchConstraintRadius_6 = Sketch_4.setRadius(SketchArc_14.results()[1], "r3*coeff")
SketchConstraintDistance_19 = Sketch_4.setDistance(SketchLine_41.endPoint(), SketchLine_40.result(), "epaisseur*coeff")
SketchConstraintDistance_20 = Sketch_4.setDistance(SketchLine_41.startPoint(), SketchLine_39.result(), "h2")
model.do()
Extrusion_3 = model.addExtrusion(Part_1_doc, [model.selection("WIRE", "Sketch_4/Face-SketchLine_44f-SketchLine_43r-SketchArc_14_2r-SketchLine_42r-SketchLine_41r-SketchLine_40r-SketchArc_13_2f-SketchLine_39r_wire")], model.selection(), 0, "e1*coeff")
Sketch_5 = model.addSketch(Part_1_doc, model.selection("FACE", "ExtrusionCut_1_1/Modified_Face&Sketch_2/SketchLine_18"))
SketchCircle_1 = Sketch_5.addCircle(70, 17.5, 3.125)
SketchLine_45 = Sketch_5.addLine(model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&Sketch_2/SketchLine_19][ExtrusionCut_1_1/Modified_Face&Sketch_2/SketchLine_18]"))
SketchConstraintDistance_21 = Sketch_5.setDistance(SketchCircle_1.center(), SketchLine_45.result(), "d9*coeff")
SketchProjection_8 = Sketch_5.addProjection(model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_6][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_13]"))
SketchLine_46 = SketchProjection_8.createdFeature()
SketchProjection_9 = Sketch_5.addProjection(model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_1][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_7]"), False)
SketchLine_47 = SketchProjection_9.createdFeature()
SketchConstraintDistance_22 = Sketch_5.setDistance(SketchCircle_1.center(), SketchLine_47.result(), "d1*coeff")
SketchConstraintRadius_7 = Sketch_5.setRadius(SketchCircle_1.results()[1], "r_trou")
model.do()
ExtrusionCut_3 = model.addExtrusionCut(Part_1_doc, [model.selection("FACE", "Sketch_5/Face-SketchCircle_1_2r")], model.selection(), model.selection("FACE", "ExtrusionCut_1_1/Modified_Face&Sketch_2/SketchLine_20"), 0, model.selection(), 0, [model.selection("SOLID", "ExtrusionCut_2_1")])
Sketch_6 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_10"))
SketchCircle_2 = Sketch_6.addCircle(70, -17.5, 5)
SketchLine_48 = Sketch_6.addLine(model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_10][Extrusion_1_1/To_Face]"))
SketchProjection_10 = Sketch_6.addProjection(model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_8][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_7]"), False)
SketchLine_49 = SketchProjection_10.createdFeature()
SketchConstraintDistance_23 = Sketch_6.setDistance(SketchCircle_2.center(), SketchLine_49.result(), "d1*coeff")
SketchConstraintDistance_23.setName("SketchConstraintDistance_24")
SketchConstraintRadius_8 = Sketch_6.setRadius(SketchCircle_2.results()[1], "r_trou2")
SketchLine_50 = Sketch_6.addLine(model.selection("EDGE", "Sketch_1/SketchLine_10"))
SketchConstraintDistance_24 = Sketch_6.setDistance(SketchCircle_2.center(), SketchLine_50.result(), "d_trou")
SketchConstraintDistance_24.setName("SketchConstraintDistance_25")
model.do()
ExtrusionCut_4 = model.addExtrusionCut(Part_1_doc, [model.selection("FACE", "Sketch_6/Face-SketchCircle_2_2r")], model.selection(), model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_4"), 0, model.selection(), 0, [model.selection("SOLID", "Extrusion_1_1")])
Sketch_7 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_6"))
SketchLine_51 = Sketch_7.addLine(110, 21.46600893000899, 110, 7.22841614740048)
SketchLine_51.setAuxiliary(True)
SketchLine_52 = Sketch_7.addLine(model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_6][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_13]"))
SketchConstraintCoincidence_65 = Sketch_7.setCoincident(SketchLine_51.startPoint(), SketchLine_52.result())
SketchConstraintCoincidence_66 = Sketch_7.setCoincident(SketchLine_51.endPoint(), SketchLine_52.result())
SketchLine_53 = Sketch_7.addLine(model.selection("EDGE", "[Extrusion_1_1/From_Face][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_6]"))
SketchArc_15 = Sketch_7.addArc(102.5, 7.22841614740048, 110, 7.22841614740048, 104.5, 0, True)
SketchConstraintCoincidence_67 = Sketch_7.setCoincident(SketchLine_51.endPoint(), SketchArc_15.startPoint())
SketchConstraintTangent_29 = Sketch_7.setTangent(SketchLine_51.result(), SketchArc_15.results()[1])
SketchConstraintCoincidence_68 = Sketch_7.setCoincident(SketchLine_53.result(), SketchArc_15.endPoint())
SketchConstraintRadius_9 = Sketch_7.setRadius(SketchArc_15.results()[1], "r1")
SketchLine_54 = Sketch_7.addLine(104.5, 0, 110, 0)
SketchConstraintCoincidence_69 = Sketch_7.setCoincident(SketchArc_15.endPoint(), SketchLine_54.startPoint())
SketchConstraintCoincidence_70 = Sketch_7.setCoincident(SketchLine_52.startPoint(), SketchLine_54.endPoint())
SketchPoint_3 = Sketch_7.addPoint(0, 0)
SketchLine_55 = Sketch_7.addLine(110, 7.22841614740048, 110, 0)
SketchConstraintCoincidence_71 = Sketch_7.setCoincident(SketchLine_51.endPoint(), SketchLine_55.startPoint())
SketchConstraintCoincidence_72 = Sketch_7.setCoincident(SketchLine_52.startPoint(), SketchLine_55.endPoint())
SketchConstraintLength_5 = Sketch_7.setLength(SketchLine_54.result(), "5.5*coeff")
SketchLine_56 = Sketch_7.addLine(110, 12.5, 0, 12.5)
SketchLine_56.setAuxiliary(True)
SketchConstraintCoincidence_73 = Sketch_7.setCoincident(SketchLine_56.startPoint(), SketchLine_52.result())
SketchLine_57 = Sketch_7.addLine(model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_1][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_7]"))
SketchConstraintCoincidence_74 = Sketch_7.setCoincident(SketchLine_56.endPoint(), SketchLine_57.result())
SketchConstraintMiddle_1 = Sketch_7.setMiddlePoint(SketchLine_56.endPoint(), SketchLine_57.result())
SketchConstraintMiddle_2 = Sketch_7.setMiddlePoint(SketchLine_56.startPoint(), SketchLine_52.result())
SketchProjection_11 = Sketch_7.addProjection(model.selection("EDGE", "[Extrusion_1_1/To_Face][ExtrusionCut_4_1/Modified_Face&Sketch_1/SketchLine_4]"), False)
SketchLine_58 = SketchProjection_11.createdFeature()
SketchLine_59 = Sketch_7.addLine(55.00000000000002, 0, 55.00000000000002, 25)
SketchLine_59.setAuxiliary(True)
SketchLine_60 = Sketch_7.addLine(model.selection("EDGE", "PartSet/OX"))
SketchConstraintCoincidence_75 = Sketch_7.setCoincident(SketchLine_59.startPoint(), SketchLine_60.result())
SketchConstraintCoincidence_76 = Sketch_7.setCoincident(SketchLine_59.endPoint(), SketchLine_58.result())
SketchConstraintMiddle_3 = Sketch_7.setMiddlePoint(SketchLine_59.endPoint(), SketchLine_58.result())
SketchConstraintVertical_17 = Sketch_7.setVertical(SketchLine_59.result())
SketchConstraintMirror_1_objects = [SketchArc_15.results()[1], SketchLine_55.result(), SketchLine_54.result()]
SketchConstraintMirror_1 = Sketch_7.addMirror(SketchLine_56.result(), SketchConstraintMirror_1_objects)
[SketchArc_16, SketchLine_61, SketchLine_62] = SketchConstraintMirror_1.mirrored()
SketchConstraintMirror_2_objects = [SketchArc_15.results()[1], SketchLine_55.result(), SketchLine_54.result(), SketchArc_16.results()[1], SketchLine_62.result(), SketchLine_52.result()]
SketchConstraintMirror_2 = Sketch_7.addMirror(SketchLine_59.result(), SketchConstraintMirror_2_objects)
[SketchArc_17, SketchLine_63, SketchLine_64, SketchArc_18, SketchLine_65, SketchLine_66] = SketchConstraintMirror_2.mirrored()
model.do()
ExtrusionCut_5 = model.addExtrusionCut(Part_1_doc, [model.selection("WIRE", "Sketch_7/Face-SketchLine_61f-SketchLine_62r-SketchArc_16_2r_wire"), model.selection("FACE", "Sketch_7/Face-SketchLine_65f-SketchLine_66r-SketchArc_18_2r"), model.selection("FACE", "Sketch_7/Face-SketchLine_63f-SketchLine_64r-SketchArc_17_2r"), model.selection("WIRE", "Sketch_7/Face-SketchLine_54f-SketchLine_55r-SketchArc_15_2r_wire")], model.selection(), model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_12"), 0, model.selection(), 0, [model.selection("SOLID", "ExtrusionCut_4_1")])
Boolean_1 = model.addFuse(Part_1_doc, [model.selection("SOLID", "ExtrusionCut_5_1")], [model.selection("SOLID", "Extrusion_3_1"), model.selection("SOLID", "ExtrusionCut_3_1")])
model.do()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Part_1, 1)
model.testNbSubResults(Part_1, [0])
model.testNbSubShapes(Part_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Part_1, GeomAPI_Shape.FACE, [57])
model.testNbSubShapes(Part_1, GeomAPI_Shape.EDGE, [314])
model.testNbSubShapes(Part_1, GeomAPI_Shape.VERTEX, [628])
model.testResultsVolumes(Part_1, [13531.037990141923728515394])
model.end()

assert(model.checkPythonDump())
