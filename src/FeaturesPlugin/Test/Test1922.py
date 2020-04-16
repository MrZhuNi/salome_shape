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

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-717.0963364993213, -45.06817090071282, -254.8675173043772, -45.06817090071281)
SketchLine_1.setAuxiliary(True)
SketchLine_2 = Sketch_1.addLine(-254.8675173043772, -45.06817090071281, -254.8675173043772, 193.0685819822336)
SketchLine_2.setAuxiliary(True)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(-254.8675173043772, 193.0685819822336, -102.0809741405142, 193.0685819822336)
SketchLine_3.setAuxiliary(True)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(-102.0809741405142, 193.0685819822336, -102.0809741405142, -180.9380775505473)
SketchLine_4.setAuxiliary(True)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(-102.0809741405142, -180.9380775505473, -717.0963364993213, -180.9380775505473)
SketchLine_5.setAuxiliary(True)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchLine_6 = Sketch_1.addLine(-717.0963364993213, -180.9380775505473, -717.0963364993213, -45.06817090071282)
SketchLine_6.setAuxiliary(True)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_6.endPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_5.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_4.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_6.result())
SketchPoint_1 = Sketch_1.addPoint(-684.5318860244231, -45.06817090071282)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchPoint_1.coordinates(), SketchLine_1.result())
SketchPoint_2 = Sketch_1.addPoint(-599.0502035278153, -45.06817090071282)
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchPoint_2.coordinates(), SketchLine_1.result())
SketchPoint_3 = Sketch_1.addPoint(-509.4979647218452, -45.06817090071282)
SketchPoint_3.setName("SketchPoint_4")
SketchPoint_3.result().setName("SketchPoint_4")
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchPoint_3.coordinates(), SketchLine_1.result())
SketchPoint_4 = Sketch_1.addPoint(-429.4436906377204, -45.06817090071282)
SketchPoint_4.setName("SketchPoint_5")
SketchPoint_4.result().setName("SketchPoint_5")
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchPoint_4.coordinates(), SketchLine_1.result())
SketchPoint_5 = Sketch_1.addPoint(-356.1736770691994, -45.06817090071282)
SketchPoint_5.setName("SketchPoint_6")
SketchPoint_5.result().setName("SketchPoint_6")
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchPoint_5.coordinates(), SketchLine_1.result())
SketchPoint_6 = Sketch_1.addPoint(-254.8675173043772, 25.10176390773411)
SketchPoint_6.setName("SketchPoint_7")
SketchPoint_6.result().setName("SketchPoint_7")
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchPoint_6.coordinates(), SketchLine_2.result())
SketchPoint_7 = Sketch_1.addPoint(-254.8675173043772, 103.7991858887381)
SketchPoint_7.setName("SketchPoint_8")
SketchPoint_7.result().setName("SketchPoint_8")
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchPoint_7.coordinates(), SketchLine_2.result())
SketchPoint_8 = Sketch_1.addPoint(-196.0651289009498, 193.0685819822336)
SketchPoint_8.setName("SketchPoint_9")
SketchPoint_8.result().setName("SketchPoint_9")
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchPoint_8.coordinates(), SketchLine_3.result())
SketchPoint_9 = Sketch_1.addPoint(-102.0809741405142, 116.0108548168249)
SketchPoint_9.setName("SketchPoint_10")
SketchPoint_9.result().setName("SketchPoint_10")
SketchConstraintCoincidence_15 = Sketch_1.setCoincident(SketchPoint_9.coordinates(), SketchLine_4.result())
SketchPoint_10 = Sketch_1.addPoint(-102.0809741405142, -118.7245590230665)
SketchPoint_10.setName("SketchPoint_11")
SketchPoint_10.result().setName("SketchPoint_11")
SketchConstraintCoincidence_16 = Sketch_1.setCoincident(SketchPoint_10.coordinates(), SketchLine_4.result())
SketchPoint_11 = Sketch_1.addPoint(-342.6051560379917, -180.9380775505472)
SketchPoint_11.setName("SketchPoint_13")
SketchPoint_11.result().setName("SketchPoint_13")
SketchConstraintCoincidence_17 = Sketch_1.setCoincident(SketchPoint_11.coordinates(), SketchLine_5.result())
SketchPoint_12 = Sketch_1.addPoint(-584.1248303934869, -180.9380775505472)
SketchPoint_12.setName("SketchPoint_14")
SketchPoint_12.result().setName("SketchPoint_14")
SketchConstraintCoincidence_18 = Sketch_1.setCoincident(SketchPoint_12.coordinates(), SketchLine_5.result())
model.do()
Vertex_1_objects = [model.selection("VERTEX", "Sketch_1/SketchLine_5_EndVertex"), model.selection("VERTEX", "Sketch_1/SketchPoint_7"), model.selection("VERTEX", "Sketch_1/SketchLine_4_EndVertex"), model.selection("VERTEX", "Sketch_1/SketchPoint_11"), model.selection("VERTEX", "Sketch_1/SketchLine_2_EndVertex"), model.selection("VERTEX", "Sketch_1/SketchPoint_10"), model.selection("VERTEX", "Sketch_1/SketchPoint_8"), model.selection("VERTEX", "Sketch_1/SketchPoint_9"), model.selection("VERTEX", "Sketch_1/SketchLine_1_EndVertex"), model.selection("VERTEX", "Sketch_1/SketchLine_3_EndVertex"), model.selection("VERTEX", "Sketch_1/SketchPoint_13"), model.selection("VERTEX", "Sketch_1/SketchLine_1_StartVertex"), model.selection("VERTEX", "Sketch_1/SketchPoint_14"), model.selection("VERTEX", "Sketch_1/SketchPoint_2"), model.selection("VERTEX", "Sketch_1/SketchPoint_6"), model.selection("VERTEX", "Sketch_1/SketchPoint_1"), model.selection("VERTEX", "Sketch_1/SketchPoint_4"), model.selection("VERTEX", "Sketch_1/SketchPoint_5")]
Vertex_1 = model.addVertex(Part_1_doc, Vertex_1_objects)
Extrusion_1_objects = [model.selection("VERTEX", "Vertex_1_1"), model.selection("VERTEX", "Vertex_1_2"), model.selection("VERTEX", "Vertex_1_3"), model.selection("VERTEX", "Vertex_1_4"), model.selection("VERTEX", "Vertex_1_5"), model.selection("VERTEX", "Vertex_1_6"), model.selection("VERTEX", "Vertex_1_7"), model.selection("VERTEX", "Vertex_1_8"), model.selection("VERTEX", "Vertex_1_9"), model.selection("VERTEX", "Vertex_1_10"), model.selection("VERTEX", "Vertex_1_11"), model.selection("VERTEX", "Vertex_1_12"), model.selection("VERTEX", "Vertex_1_13"), model.selection("VERTEX", "Vertex_1_14"), model.selection("VERTEX", "Vertex_1_15"), model.selection("VERTEX", "Vertex_1_16"), model.selection("VERTEX", "Vertex_1_17"), model.selection("VERTEX", "Vertex_1_18")]
Extrusion_1 = model.addExtrusion(Part_1_doc, Extrusion_1_objects, model.selection("EDGE", "PartSet/OZ"), 100, 0)
Plane_4 = model.addPlane(Part_1_doc, model.selection("VERTEX", "Extrusion_1_12/To_Vertex"), model.selection("VERTEX", "Extrusion_1_13/To_Vertex"), model.selection("VERTEX", "Extrusion_1_3/To_Vertex"))
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_1"))
SketchLine_7 = Sketch_2.addLine(-717.0963364993213, -45.06817090071282, -717.0963364993213, -180.9380775505473)
SketchPoint_13 = Sketch_2.addPoint(model.selection("VERTEX", "Extrusion_1_12/To_Vertex"))
SketchPoint_13.setName("SketchPoint_15")
SketchPoint_13.result().setName("SketchPoint_15")
SketchConstraintCoincidence_19 = Sketch_2.setCoincident(SketchLine_7.startPoint(), SketchPoint_13.result())
SketchPoint_14 = Sketch_2.addPoint(model.selection("VERTEX", "Extrusion_1_1/To_Vertex"))
SketchPoint_14.setName("SketchPoint_16")
SketchPoint_14.result().setName("SketchPoint_16")
SketchConstraintCoincidence_20 = Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchPoint_14.result())
SketchLine_8 = Sketch_2.addLine(-717.0963364993213, -180.9380775505473, -102.0809741405142, -180.9380775505473)
SketchConstraintCoincidence_21 = Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchPoint_15 = Sketch_2.addPoint(model.selection("VERTEX", "Extrusion_1_3/To_Vertex"))
SketchPoint_15.setName("SketchPoint_17")
SketchPoint_15.result().setName("SketchPoint_17")
SketchConstraintCoincidence_22 = Sketch_2.setCoincident(SketchLine_8.endPoint(), SketchPoint_15.result())
SketchLine_9 = Sketch_2.addLine(-102.0809741405142, -180.9380775505473, -102.0809741405142, 193.0685819822336)
SketchConstraintCoincidence_23 = Sketch_2.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchPoint_16 = Sketch_2.addPoint(model.selection("VERTEX", "Extrusion_1_10/To_Vertex"))
SketchPoint_16.setName("SketchPoint_18")
SketchPoint_16.result().setName("SketchPoint_18")
SketchConstraintCoincidence_24 = Sketch_2.setCoincident(SketchLine_9.endPoint(), SketchPoint_16.result())
SketchLine_10 = Sketch_2.addLine(-102.0809741405142, 193.0685819822336, -254.8675173043772, 193.0685819822336)
SketchConstraintCoincidence_25 = Sketch_2.setCoincident(SketchLine_9.endPoint(), SketchLine_10.startPoint())
SketchPoint_17 = Sketch_2.addPoint(model.selection("VERTEX", "Extrusion_1_5/To_Vertex"))
SketchPoint_17.setName("SketchPoint_19")
SketchPoint_17.result().setName("SketchPoint_19")
SketchConstraintCoincidence_26 = Sketch_2.setCoincident(SketchLine_10.endPoint(), SketchPoint_17.result())
SketchLine_11 = Sketch_2.addLine(-254.8675173043772, 193.0685819822336, -254.8675173043772, -45.06817090071281)
SketchConstraintCoincidence_27 = Sketch_2.setCoincident(SketchLine_10.endPoint(), SketchLine_11.startPoint())
SketchPoint_18 = Sketch_2.addPoint(model.selection("VERTEX", "Extrusion_1_9/To_Vertex"))
SketchPoint_18.setName("SketchPoint_20")
SketchPoint_18.result().setName("SketchPoint_20")
SketchConstraintCoincidence_28 = Sketch_2.setCoincident(SketchLine_11.endPoint(), SketchPoint_18.result())
SketchLine_12 = Sketch_2.addLine(-254.8675173043772, -45.06817090071281, -717.0963364993213, -45.06817090071282)
SketchConstraintCoincidence_29 = Sketch_2.setCoincident(SketchLine_11.endPoint(), SketchLine_12.startPoint())
SketchConstraintCoincidence_30 = Sketch_2.setCoincident(SketchLine_7.startPoint(), SketchLine_12.endPoint())
SketchConstraintCoincidence_31 = Sketch_2.setCoincident(SketchPoint_13.coordinates(), SketchLine_12.endPoint())
SketchLine_13 = Sketch_2.addLine(-599.2405881202537, -45.06817090071282, -584.1248303934869, -180.9380775505472)
SketchConstraintCoincidence_32 = Sketch_2.setCoincident(SketchLine_13.startPoint(), SketchLine_12.result())
SketchPoint_19 = Sketch_2.addPoint(model.selection("VERTEX", "Extrusion_1_13/To_Vertex"))
SketchPoint_19.setName("SketchPoint_21")
SketchPoint_19.result().setName("SketchPoint_21")
SketchConstraintCoincidence_33 = Sketch_2.setCoincident(SketchLine_13.endPoint(), SketchPoint_19.result())
SketchLine_14 = Sketch_2.addLine(-356.1736770691994, -45.06817090071282, -342.6051560379917, -180.9380775505472)
SketchPoint_20 = Sketch_2.addPoint(model.selection("VERTEX", "Extrusion_1_15/To_Vertex"))
SketchPoint_20.setName("SketchPoint_22")
SketchPoint_20.result().setName("SketchPoint_22")
SketchConstraintCoincidence_34 = Sketch_2.setCoincident(SketchLine_14.startPoint(), SketchPoint_20.result())
SketchPoint_21 = Sketch_2.addPoint(model.selection("VERTEX", "Extrusion_1_11/To_Vertex"))
SketchPoint_21.setName("SketchPoint_23")
SketchPoint_21.result().setName("SketchPoint_23")
SketchConstraintCoincidence_35 = Sketch_2.setCoincident(SketchLine_14.endPoint(), SketchPoint_21.result())
SketchLine_15 = Sketch_2.addLine(-254.8675173043772, -45.06817090071281, -102.0809741405142, -49.89915168208545)
SketchConstraintCoincidence_36 = Sketch_2.setCoincident(SketchLine_11.endPoint(), SketchLine_15.startPoint())
SketchConstraintCoincidence_37 = Sketch_2.setCoincident(SketchPoint_18.coordinates(), SketchLine_15.startPoint())
SketchConstraintCoincidence_38 = Sketch_2.setCoincident(SketchLine_12.startPoint(), SketchLine_15.startPoint())
SketchConstraintCoincidence_39 = Sketch_2.setCoincident(SketchLine_15.endPoint(), SketchLine_9.result())
model.do()
Face_1 = model.addFace(Part_1_doc, [model.selection("WIRE", "Sketch_2/Face-SketchLine_7r-SketchLine_8f-SketchLine_13r-SketchLine_12f_wire")])
Face_2 = model.addFace(Part_1_doc, [model.selection("WIRE", "Sketch_2/Face-SketchLine_8f-SketchLine_14r-SketchLine_12f-SketchLine_13f_wire")])
Face_3 = model.addFace(Part_1_doc, [model.selection("WIRE", "Sketch_2/Face-SketchLine_8f-SketchLine_9f-SketchLine_15r-SketchLine_12f-SketchLine_14f_wire")])
Face_4 = model.addFace(Part_1_doc, [model.selection("WIRE", "Sketch_2/Face-SketchLine_9f-SketchLine_10f-SketchLine_11f-SketchLine_15f_wire")])
Partition_1_objects = [model.selection("EDGE", "Extrusion_1_1"), model.selection("EDGE", "Extrusion_1_2"), model.selection("EDGE", "Extrusion_1_3"), model.selection("EDGE", "Extrusion_1_4"), model.selection("EDGE", "Extrusion_1_5"), model.selection("EDGE", "Extrusion_1_6"), model.selection("EDGE", "Extrusion_1_7"), model.selection("EDGE", "Extrusion_1_8"), model.selection("EDGE", "Extrusion_1_9"), model.selection("EDGE", "Extrusion_1_10"), model.selection("EDGE", "Extrusion_1_11"), model.selection("EDGE", "Extrusion_1_12"), model.selection("EDGE", "Extrusion_1_13"), model.selection("EDGE", "Extrusion_1_14"), model.selection("EDGE", "Extrusion_1_15"), model.selection("EDGE", "Extrusion_1_16"), model.selection("EDGE", "Extrusion_1_17"), model.selection("EDGE", "Extrusion_1_18"), model.selection("FACE", "Face_1_1"), model.selection("FACE", "Face_2_1"), model.selection("FACE", "Face_3_1"), model.selection("FACE", "Face_4_1")]
Partition_1 = model.addPartition(Part_1_doc, Partition_1_objects)
model.do()
model.end()

from ModelAPI import *

PartitionFeature = Partition_1.feature()
assert(len(PartitionFeature.results()) == 1)
PartitionResult = modelAPI_ResultBody(PartitionFeature.firstResult())
assert(PartitionResult.numberOfSubs() == 22)

assert(model.checkPythonDump())

