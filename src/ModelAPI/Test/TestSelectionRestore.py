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

# Test that invalid selection attributes become valid after the parameters changes and
# coming the topology back in a valid state.
# Based on the CEA report mail 04.12.2018, page 6

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.addParameter(Part_1_doc, "ep_base", "0.65")
EpVoiles = model.addParameter(Part_1_doc, "ep_voiles", "0.1")
model.addParameter(Part_1_doc, "ep_poteau", "0.15")
model.addParameter(Part_1_doc, "h_ouverture", "0.95")
model.addParameter(Part_1_doc, "h_apres_ouverture", "0.6")
model.addParameter(Part_1_doc, "ep_dalles", "0.12")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(0.275, 3.325, -0.375, 3.325)
SketchLine_2 = Sketch_1.addLine(-0.375, 3.325, -0.375, -0.325)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(-0.375, -0.325, 4.275, -0.325)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(4.275, -0.325, 4.275, 1.325)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(4.275, 1.325, 3.625, 1.325)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchLine_6 = Sketch_1.addLine(3.625, 1.325, 3.625, 0.325)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchLine_7 = Sketch_1.addLine(3.625, 0.325, 0.275, 0.3250000000000001)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchLine_8 = Sketch_1.addLine(0.275, 0.3250000000000001, 0.275, 3.325)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_8.endPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_5.result())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintHorizontal_4 = Sketch_1.setHorizontal(SketchLine_7.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_8.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_6.result())
SketchConstraintVertical_4 = Sketch_1.setVertical(SketchLine_4.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_1.result(), "ep_base")
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_2.result(), 3.65)
SketchConstraintLength_3 = Sketch_1.setLength(SketchLine_3.result(), 4.65)
SketchConstraintLength_4 = Sketch_1.setLength(SketchLine_4.result(), 1.65)
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchLine_7.startPoint(), SketchLine_3.result(), "ep_base", False)
SketchLine_9 = Sketch_1.addLine(model.selection("EDGE", "PartSet/OX"))
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchLine_3.endPoint(), SketchLine_9.result(), "0.275+ep_voiles/2", False)
SketchLine_10 = Sketch_1.addLine(model.selection("EDGE", "PartSet/OY"))
SketchConstraintDistance_3 = Sketch_1.setDistance(SketchLine_2.startPoint(), SketchLine_10.result(), "0.325+ep_voiles/2", False)
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchLine_5.result(), SketchLine_1.result())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_2f-SketchLine_3f-SketchLine_4f-SketchLine_5f-SketchLine_6f-SketchLine_7f-SketchLine_8f")], model.selection(), 0, 0.3)
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/From_Face"))
SketchLine_11 = Sketch_2.addLine(2.26, -2.31, 1.64, -2.31)
SketchLine_12 = Sketch_2.addLine(1.64, -2.31, 1.64, -1.69)
SketchLine_13 = Sketch_2.addLine(1.64, -1.69, 2.26, -1.69)
SketchLine_14 = Sketch_2.addLine(2.26, -1.69, 2.26, -2.31)
SketchConstraintCoincidence_9 = Sketch_2.setCoincident(SketchLine_14.endPoint(), SketchLine_11.startPoint())
SketchConstraintCoincidence_10 = Sketch_2.setCoincident(SketchLine_11.endPoint(), SketchLine_12.startPoint())
SketchConstraintCoincidence_11 = Sketch_2.setCoincident(SketchLine_12.endPoint(), SketchLine_13.startPoint())
SketchConstraintCoincidence_12 = Sketch_2.setCoincident(SketchLine_13.endPoint(), SketchLine_14.startPoint())
SketchConstraintHorizontal_5 = Sketch_2.setHorizontal(SketchLine_11.result())
SketchConstraintVertical_5 = Sketch_2.setVertical(SketchLine_12.result())
SketchConstraintHorizontal_6 = Sketch_2.setHorizontal(SketchLine_13.result())
SketchConstraintVertical_6 = Sketch_2.setVertical(SketchLine_14.result())
SketchPoint_1 = Sketch_2.addPoint(1.925, -1.875)
SketchConstraintLength_5 = Sketch_2.setLength(SketchLine_11.result(), 0.62)
SketchConstraintLength_5.setName("SketchConstraintLength_10")
SketchConstraintEqual_2 = Sketch_2.setEqual(SketchLine_11.result(), SketchLine_12.result())
SketchConstraintDistance_4 = Sketch_2.setDistance(SketchPoint_1.coordinates(), SketchLine_13.result(), "0.11+ep_poteau/2", False)
SketchConstraintDistance_4.setName("SketchConstraintDistance_6")
SketchConstraintDistance_5 = Sketch_2.setDistance(SketchPoint_1.coordinates(), SketchLine_12.result(), "0.21+ep_poteau/2", False)
SketchConstraintDistance_5.setName("SketchConstraintDistance_7")
SketchProjection_1 = Sketch_2.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_15 = SketchProjection_1.createdFeature()
SketchProjection_2 = Sketch_2.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_16 = SketchProjection_2.createdFeature()
SketchConstraintDistance_6 = Sketch_2.setDistance(SketchPoint_1.coordinates(), SketchLine_15.result(), "1.75+ep_poteau/2+ep_voiles/2", False)
SketchConstraintDistance_6.setName("SketchConstraintDistance_8")
SketchConstraintDistance_7 = Sketch_2.setDistance(SketchPoint_1.coordinates(), SketchLine_16.result(), "1.8+ep_poteau/2+ep_voiles/2", False)
SketchConstraintDistance_7.setName("SketchConstraintDistance_9")
model.do()
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchLine_14r-SketchLine_13r-SketchLine_12r-SketchLine_11r")], model.selection(), 0, 0.02)
Sketch_3 = model.addSketch(Part_1_doc, model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_2f-SketchLine_3f-SketchLine_4f-SketchLine_5f-SketchLine_6f-SketchLine_7f-SketchLine_8f"))
SketchLine_17 = Sketch_3.addLine(0, 2.950000000000001, 0, 1.700000000000001)
SketchLine_17.setName("SketchLine_15")
SketchLine_17.result().setName("SketchLine_15")
SketchLine_18 = Sketch_3.addLine(model.selection("EDGE", "PartSet/OY"))
SketchLine_18.setName("SketchLine_16")
SketchLine_18.result().setName("SketchLine_16")
SketchConstraintCoincidence_13 = Sketch_3.setCoincident(SketchLine_17.startPoint(), SketchLine_18.result())
SketchConstraintCoincidence_14 = Sketch_3.setCoincident(SketchLine_17.endPoint(), SketchLine_18.result())
SketchLine_19 = Sketch_3.addLine(0, 1.2, 0, 0)
SketchLine_19.setName("SketchLine_17")
SketchLine_19.result().setName("SketchLine_17")
SketchConstraintCoincidence_15 = Sketch_3.setCoincident(SketchLine_19.startPoint(), SketchLine_18.result())
SketchConstraintCoincidence_16 = Sketch_3.setCoincident(SketchLine_19.endPoint(), SketchLine_18.result())
SketchLine_20 = Sketch_3.addLine(0, 0, 0.95, 0)
SketchLine_20.setName("SketchLine_18")
SketchLine_20.result().setName("SketchLine_18")
SketchLine_21 = Sketch_3.addLine(model.selection("EDGE", "PartSet/OX"))
SketchLine_21.setName("SketchLine_19")
SketchLine_21.result().setName("SketchLine_19")
SketchLine_22 = Sketch_3.addLine(2.525000000000001, 0, 3.975000000000001, 0)
SketchLine_22.setName("SketchLine_20")
SketchLine_22.result().setName("SketchLine_20")
SketchConstraintCoincidence_17 = Sketch_3.setCoincident(SketchLine_22.startPoint(), SketchLine_21.result())
SketchConstraintCoincidence_18 = Sketch_3.setCoincident(SketchLine_22.endPoint(), SketchLine_21.result())
SketchLine_23 = Sketch_3.addLine(3.975000000000001, 0, 3.975000000000001, 0.95)
SketchLine_23.setName("SketchLine_21")
SketchLine_23.result().setName("SketchLine_21")
SketchConstraintCoincidence_19 = Sketch_3.setCoincident(SketchLine_22.endPoint(), SketchLine_23.startPoint())
SketchLine_24 = Sketch_3.addLine(model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_5][Extrusion_1_1/To_Face]"))
SketchLine_24.setName("SketchLine_22")
SketchLine_24.result().setName("SketchLine_22")
SketchConstraintLength_6 = Sketch_3.setLength(SketchLine_17.result(), 1.25)
SketchConstraintLength_6.setName("SketchConstraintLength_5")
SketchConstraintCoincidence_20 = Sketch_3.setCoincident(SketchLine_19.endPoint(), SketchLine_18.startPoint())
SketchConstraintCoincidence_21 = Sketch_3.setCoincident(SketchLine_20.startPoint(), SketchLine_19.endPoint())
SketchConstraintLength_7 = Sketch_3.setLength(SketchLine_19.result(), "1.25-ep_voiles/2")
SketchConstraintLength_7.setName("SketchConstraintLength_6")
SketchConstraintLength_8 = Sketch_3.setLength(SketchLine_20.result(), "1-ep_voiles/2")
SketchConstraintLength_8.setName("SketchConstraintLength_7")
SketchConstraintLength_9 = Sketch_3.setLength(SketchLine_22.result(), "1.5-ep_voiles/2")
SketchConstraintLength_9.setName("SketchConstraintLength_8")
SketchConstraintVertical_7 = Sketch_3.setVertical(SketchLine_23.result())
SketchLine_25 = Sketch_3.addLine(model.selection("EDGE", "Sketch_1/SketchLine_4"))
SketchLine_25.setName("SketchLine_23")
SketchLine_25.result().setName("SketchLine_23")
SketchConstraintDistance_8 = Sketch_3.setDistance(SketchLine_22.endPoint(), SketchLine_25.result(), "0.25+ep_voiles/2", False)
SketchConstraintDistance_8.setName("SketchConstraintDistance_5")
SketchConstraintLength_10 = Sketch_3.setLength(SketchLine_23.result(), "1-ep_voiles/2")
SketchConstraintLength_10.setName("SketchConstraintLength_9")
SketchLine_26 = Sketch_3.addLine(model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_1][Extrusion_1_1/To_Face]"))
SketchLine_26.setName("SketchLine_35")
SketchLine_26.result().setName("SketchLine_35")
SketchConstraintDistance_9 = Sketch_3.setDistance(SketchLine_17.startPoint(), SketchLine_26.result(), 0.375, False)
SketchConstraintDistance_9.setName("SketchConstraintDistance_12")
model.do()
Edge_1_objects = [model.selection("EDGE", "Sketch_3/SketchLine_15"), model.selection("EDGE", "Sketch_3/SketchLine_17"), model.selection("EDGE", "Sketch_3/SketchLine_18"), model.selection("EDGE", "Sketch_3/SketchLine_20"), model.selection("EDGE", "Sketch_3/SketchLine_21")]
Edge_1 = model.addEdge(Part_1_doc, Edge_1_objects)
Extrusion_3_objects = [model.selection("EDGE", "Edge_1_1"), model.selection("EDGE", "Edge_1_2"), model.selection("EDGE", "Edge_1_3"), model.selection("EDGE", "Edge_1_4"), model.selection("EDGE", "Edge_1_5")]
Extrusion_3 = model.addExtrusion(Part_1_doc, Extrusion_3_objects, model.selection("EDGE", "PartSet/OZ"), "h_ouverture", 0)
Plane_4 = model.addPlane(Part_1_doc, model.selection("VERTEX", "[Extrusion_3_1/Generated_Edge&Sketch_3/SketchLine_15_StartVertex]e[Extrusion_3_1/To_Edge]e"), model.selection("VERTEX", "[Extrusion_3_3/Generated_Edge&Sketch_3/SketchLine_18_StartVertex]e[Extrusion_3_3/To_Edge]e"), model.selection("VERTEX", "[Extrusion_3_4/Generated_Edge&Sketch_3/SketchLine_20_EndVertex]e[Extrusion_3_4/To_Edge]e"))
Sketch_4 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_1"))
SketchLine_27 = Sketch_4.addLine(0, 2.950000000000001, 0, 0)
SketchLine_27.setName("SketchLine_26")
SketchLine_27.result().setName("SketchLine_26")
SketchPoint_2 = Sketch_4.addPoint(model.selection("VERTEX", "[Extrusion_3_1/Generated_Edge&Sketch_3/SketchLine_15_StartVertex]e[Extrusion_3_1/To_Edge]e"))
SketchConstraintCoincidence_22 = Sketch_4.setCoincident(SketchLine_27.startPoint(), SketchPoint_2.result())
SketchPoint_3 = Sketch_4.addPoint(model.selection("VERTEX", "[Extrusion_3_3/Generated_Edge&Sketch_3/SketchLine_18_StartVertex]e[Extrusion_3_3/To_Edge]e"))
SketchConstraintCoincidence_23 = Sketch_4.setCoincident(SketchLine_27.endPoint(), SketchPoint_3.result())
SketchLine_28 = Sketch_4.addLine(0, 0, 3.975000000000001, 0)
SketchLine_28.setName("SketchLine_27")
SketchLine_28.result().setName("SketchLine_27")
SketchConstraintCoincidence_24 = Sketch_4.setCoincident(SketchLine_27.endPoint(), SketchLine_28.startPoint())
SketchPoint_4 = Sketch_4.addPoint(model.selection("VERTEX", "[Extrusion_3_4/Generated_Edge&Sketch_3/SketchLine_20_EndVertex]e[Extrusion_3_4/To_Edge]e"))
SketchConstraintCoincidence_25 = Sketch_4.setCoincident(SketchLine_28.endPoint(), SketchPoint_4.result())
SketchLine_29 = Sketch_4.addLine(3.975000000000001, 0, 3.975000000000001, 0.95)
SketchLine_29.setName("SketchLine_28")
SketchLine_29.result().setName("SketchLine_28")
SketchConstraintCoincidence_26 = Sketch_4.setCoincident(SketchLine_28.endPoint(), SketchLine_29.startPoint())
SketchPoint_5 = Sketch_4.addPoint(model.selection("VERTEX", "[Extrusion_3_5/Generated_Edge&Sketch_3/SketchLine_21_EndVertex]e[Extrusion_3_5/To_Edge]e"))
SketchConstraintCoincidence_27 = Sketch_4.setCoincident(SketchLine_29.endPoint(), SketchPoint_5.result())
model.do()
Edge_2_objects = [model.selection("EDGE", "Sketch_4/SketchLine_26"), model.selection("EDGE", "Sketch_4/SketchLine_27"), model.selection("EDGE", "Sketch_4/SketchLine_28")]
Edge_2 = model.addEdge(Part_1_doc, Edge_2_objects)
Extrusion_4_objects = [model.selection("EDGE", "Edge_2_1"), model.selection("EDGE", "Edge_2_2"), model.selection("EDGE", "Edge_2_3")]
Extrusion_4 = model.addExtrusion(Part_1_doc, Extrusion_4_objects, model.selection("EDGE", "PartSet/OZ"), "h_apres_ouverture-ep_dalles/2", 0)
Plane_5 = model.addPlane(Part_1_doc, model.selection("VERTEX", "[Extrusion_4_1/Generated_Edge&Sketch_4/SketchLine_26_StartVertex]e[Extrusion_4_1/To_Edge]e"), model.selection("VERTEX", "[Extrusion_4_1/Generated_Edge&Sketch_4/SketchLine_26_EndVertex]e[Extrusion_4_1/To_Edge]e"), model.selection("VERTEX", "[Extrusion_4_2/Generated_Edge&Sketch_4/SketchLine_27_EndVertex]e[Extrusion_4_2/To_Edge]e"))
Sketch_5 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_2"))
SketchLine_30 = Sketch_5.addLine(0, 2.950000000000001, 0, 0)
SketchLine_30.setName("SketchLine_29")
SketchLine_30.result().setName("SketchLine_29")
SketchPoint_6 = Sketch_5.addPoint(model.selection("VERTEX", "[Extrusion_4_1/Generated_Edge&Sketch_4/SketchLine_26_StartVertex]e[Extrusion_4_1/To_Edge]e"))
SketchConstraintCoincidence_28 = Sketch_5.setCoincident(SketchLine_30.startPoint(), SketchPoint_6.result())
SketchPoint_7 = Sketch_5.addPoint(model.selection("VERTEX", "[Extrusion_4_1/Generated_Edge&Sketch_4/SketchLine_26_EndVertex]e[Extrusion_4_1/To_Edge]e"))
SketchConstraintCoincidence_29 = Sketch_5.setCoincident(SketchLine_30.endPoint(), SketchPoint_7.result())
SketchLine_31 = Sketch_5.addLine(0, 0, 3.975000000000001, 0)
SketchLine_31.setName("SketchLine_30")
SketchLine_31.result().setName("SketchLine_30")
SketchConstraintCoincidence_30 = Sketch_5.setCoincident(SketchLine_30.endPoint(), SketchLine_31.startPoint())
SketchPoint_8 = Sketch_5.addPoint(model.selection("VERTEX", "[Extrusion_4_2/Generated_Edge&Sketch_4/SketchLine_27_EndVertex]e[Extrusion_4_2/To_Edge]e"))
SketchConstraintCoincidence_31 = Sketch_5.setCoincident(SketchLine_31.endPoint(), SketchPoint_8.result())
SketchLine_32 = Sketch_5.addLine(3.975000000000001, 0, 3.975000000000001, 0.95)
SketchLine_32.setName("SketchLine_31")
SketchLine_32.result().setName("SketchLine_31")
SketchConstraintCoincidence_32 = Sketch_5.setCoincident(SketchLine_31.endPoint(), SketchLine_32.startPoint())
SketchPoint_9 = Sketch_5.addPoint(model.selection("VERTEX", "[Extrusion_4_3/Generated_Edge&Sketch_4/SketchLine_28_EndVertex]e[Extrusion_4_3/To_Edge]e"))
SketchConstraintCoincidence_33 = Sketch_5.setCoincident(SketchLine_32.endPoint(), SketchPoint_9.result())
SketchLine_33 = Sketch_5.addLine(3.975000000000001, 0.95, 0, 2.950000000000001)
SketchLine_33.setName("SketchLine_32")
SketchLine_33.result().setName("SketchLine_32")
SketchConstraintCoincidence_34 = Sketch_5.setCoincident(SketchLine_32.endPoint(), SketchLine_33.startPoint())
SketchConstraintCoincidence_35 = Sketch_5.setCoincident(SketchLine_30.startPoint(), SketchLine_33.endPoint())
SketchConstraintCoincidence_36 = Sketch_5.setCoincident(SketchPoint_6.coordinates(), SketchLine_33.endPoint())
SketchLine_34 = Sketch_5.addLine(1.85, 2.019182389937107, 1.85, 0)
SketchLine_34.setName("SketchLine_33")
SketchLine_34.result().setName("SketchLine_33")
SketchConstraintCoincidence_37 = Sketch_5.setCoincident(SketchLine_34.startPoint(), SketchLine_33.result())
SketchLine_35 = Sketch_5.addLine(2.075000000000001, 1.905974842767296, 2.075000000000001, 0)
SketchLine_35.setName("SketchLine_34")
SketchLine_35.result().setName("SketchLine_34")
SketchConstraintCoincidence_38 = Sketch_5.setCoincident(SketchLine_35.startPoint(), SketchLine_33.result())
SketchConstraintCoincidence_39 = Sketch_5.setCoincident(SketchLine_35.endPoint(), SketchLine_31.result())
SketchConstraintCoincidence_40 = Sketch_5.setCoincident(SketchLine_34.endPoint(), SketchLine_31.result())
SketchConstraintVertical_8 = Sketch_5.setVertical(SketchLine_34.result())
SketchConstraintVertical_9 = Sketch_5.setVertical(SketchLine_35.result())
SketchConstraintDistance_10 = Sketch_5.setDistance(SketchLine_34.startPoint(), SketchLine_30.result(), "1.8+ep_voiles/2", False)
SketchConstraintDistance_10.setName("SketchConstraintDistance_10")
SketchConstraintDistance_11 = Sketch_5.setDistance(SketchLine_35.startPoint(), SketchLine_32.result(), "1.85+ep_voiles/2", False)
SketchConstraintDistance_11.setName("SketchConstraintDistance_11")
model.do()
Face_1 = model.addFace(Part_1_doc, [model.selection("WIRE", "Sketch_5/Face-SketchLine_29r-SketchLine_30f-SketchLine_33r-SketchLine_32f_wire")])
Face_2 = model.addFace(Part_1_doc, [model.selection("WIRE", "Sketch_5/Face-SketchLine_30f-SketchLine_31f-SketchLine_32f-SketchLine_34f_wire")])
Face_3 = model.addFace(Part_1_doc, [model.selection("WIRE", "Sketch_5/Face-SketchLine_30f-SketchLine_34r-SketchLine_32f-SketchLine_33f_wire")])
Translation_1_objects = [model.selection("FACE", "Extrusion_4_3"), model.selection("FACE", "Extrusion_3_3"), model.selection("FACE", "Extrusion_4_2"), model.selection("FACE", "Extrusion_3_5"), model.selection("FACE", "Extrusion_3_4"), model.selection("FACE", "Extrusion_3_2"), model.selection("FACE", "Extrusion_4_1"), model.selection("FACE", "Extrusion_3_1"), model.selection("FACE", "Face_1_1"), model.selection("FACE", "Face_3_1"), model.selection("FACE", "Face_2_1")]
Translation_1 = model.addTranslation(Part_1_doc, Translation_1_objects, model.selection("EDGE", "PartSet/OZ"), "h_ouverture+h_apres_ouverture-ep_dalles/2")
Recover_1_objects = [Face_2.result(), Face_3.result(), Face_1.result(), Extrusion_3.result(), Extrusion_4.result(), Extrusion_3.results()[1], Extrusion_3.results()[3], Extrusion_3.results()[4], Extrusion_4.results()[1], Extrusion_3.results()[2], Extrusion_4.results()[2]]
Recover_1 = model.addRecover(Part_1_doc, Translation_1, Recover_1_objects)
Translation_2_objects = [model.selection("FACE", "Recover_1_2"), model.selection("FACE", "Recover_1_5"), model.selection("FACE", "Recover_1_3"), model.selection("FACE", "Recover_1_4"), model.selection("FACE", "Recover_1_1"), model.selection("FACE", "Recover_1_6"), model.selection("FACE", "Recover_1_8"), model.selection("FACE", "Recover_1_7"), model.selection("FACE", "Recover_1_11"), model.selection("FACE", "Recover_1_10"), model.selection("FACE", "Recover_1_9")]
Translation_2 = model.addTranslation(Part_1_doc, Translation_2_objects, model.selection("EDGE", "PartSet/OZ"), "2*(h_ouverture+h_apres_ouverture-ep_dalles/2)")
Recover_2_objects = [Recover_1.results()[8], Recover_1.results()[9], Recover_1.results()[10], Recover_1.results()[6], Recover_1.results()[7], Recover_1.results()[5], Recover_1.result(), Recover_1.results()[3], Recover_1.results()[2], Recover_1.results()[4], Recover_1.results()[1]]
Recover_2 = model.addRecover(Part_1_doc, Translation_2, Recover_2_objects)
Vertex_1 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "Sketch_2/SketchPoint_1")])
Extrusion_5 = model.addExtrusion(Part_1_doc, [model.selection("VERTEX", "Vertex_1_1")], model.selection("EDGE", "PartSet/OZ"), model.selection("FACE", "Translation_2_11"), 0, model.selection(), 0)
Partition_1_objects = [model.selection("SOLID", "Extrusion_1_1"), model.selection("SOLID", "Extrusion_2_1"), model.selection("FACE", "Translation_1_1"), model.selection("FACE", "Translation_1_2"), model.selection("FACE", "Translation_1_3"), model.selection("FACE", "Translation_1_4"), model.selection("FACE", "Translation_1_5"), model.selection("FACE", "Translation_1_6"), model.selection("FACE", "Translation_1_7"), model.selection("FACE", "Translation_1_8"), model.selection("FACE", "Translation_1_9"), model.selection("FACE", "Translation_1_10"), model.selection("FACE", "Translation_1_11"), model.selection("FACE", "Translation_2_1"), model.selection("FACE", "Translation_2_2"), model.selection("FACE", "Translation_2_3"), model.selection("FACE", "Translation_2_4"), model.selection("FACE", "Translation_2_5"), model.selection("FACE", "Translation_2_6"), model.selection("FACE", "Translation_2_7"), model.selection("FACE", "Translation_2_8"), model.selection("FACE", "Translation_2_9"), model.selection("FACE", "Translation_2_10"), model.selection("FACE", "Translation_2_11"), model.selection("FACE", "Recover_2_1"), model.selection("FACE", "Recover_2_2"), model.selection("FACE", "Recover_2_3"), model.selection("FACE", "Recover_2_4"), model.selection("FACE", "Recover_2_5"), model.selection("FACE", "Recover_2_6"), model.selection("FACE", "Recover_2_7"), model.selection("FACE", "Recover_2_8"), model.selection("FACE", "Recover_2_9"), model.selection("FACE", "Recover_2_10"), model.selection("FACE", "Recover_2_11"), model.selection("EDGE", "Extrusion_5_1")]
Partition_1 = model.addPartition(Part_1_doc, Partition_1_objects)
# create groups
model.testHaveNamingSubshapes(Partition_1, model, Part_1_doc)
model.do()
# make parameter to de-touch some vertical edges and gorizontal faces =>
# two partition results are created instead of one => groups become invalid
EpVoiles.setValue(0.18)
model.do()

from ModelAPI import *
aFactory = ModelAPI_Session.get().validators()
for i in range(Part_1_doc.size("Groups")):
  GroupFeature = Part_1_doc.feature(objectToResult(Part_1_doc.object("Groups", i)))
  assert(aFactory.validate(GroupFeature) == False)


# move edges back, so, intersection re-appeared, so, groups should become valid
EpVoiles.setValue(0.17)

model.end()

for i in range(Part_1_doc.size("Groups")):
  GroupFeature = Part_1_doc.feature(objectToResult(Part_1_doc.object("Groups", i)))
  assert(aFactory.validate(GroupFeature))
