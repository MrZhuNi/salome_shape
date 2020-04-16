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
model.addParameter(Part_1_doc, "R1", "50")
model.addParameter(Part_1_doc, "L1", "100")
model.addParameter(Part_1_doc, "R2", "40")
model.addParameter(Part_1_doc, "L2", "50")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("YOZ"))
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchCircle_1 = Sketch_1.addCircle(0, 0, 50)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchPoint_1.result(), SketchCircle_1.center())
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_1.results()[1], "R1")
SketchLine_1 = Sketch_1.addLine(0, 49.99999999999999, 0, 31.81980515339405)
SketchLine_2 = Sketch_1.addLine(-1.202451477893311e-021, -31.81980515339534, 0, -50)
SketchLine_2.setName("SketchLine_3")
SketchLine_2.result().setName("SketchLine_3")
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchCircle_1.results()[1])
SketchConstraintCoincidence_2.setName("SketchConstraintCoincidence_4")
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_1.result())
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OZ"), False)
SketchLine_3 = SketchProjection_2.createdFeature()
SketchLine_3.setName("SketchLine_4")
SketchLine_3.result().setName("SketchLine_4")
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.result(), SketchLine_1.startPoint())
SketchConstraintCollinear_1 = Sketch_1.setCollinear(SketchLine_1.result(), SketchLine_2.result())
SketchLine_4 = Sketch_1.addLine(0, 31.81980515339405, 31.81980515339462, -6.650124210799604e-013)
SketchLine_4.setName("SketchLine_5")
SketchLine_4.result().setName("SketchLine_5")
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_4.setName("SketchConstraintCoincidence_5")
SketchLine_5 = Sketch_1.addLine(31.81980515339462, -6.650124210799604e-013, -1.202451477893311e-021, -31.81980515339534)
SketchLine_5.setName("SketchLine_6")
SketchLine_5.result().setName("SketchLine_6")
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_5.setName("SketchConstraintCoincidence_6")
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_2.startPoint(), SketchLine_5.endPoint())
SketchConstraintCoincidence_6.setName("SketchConstraintCoincidence_7")
SketchLine_6 = Sketch_1.addLine(-1.202451477893311e-021, -31.81980515339534, -31.8198051533955, -6.650124202329274e-013)
SketchLine_6.setName("SketchLine_7")
SketchLine_6.result().setName("SketchLine_7")
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_2.startPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_7.setName("SketchConstraintCoincidence_8")
SketchLine_7 = Sketch_1.addLine(-31.8198051533955, -6.650124202329274e-013, -8.789247626282909e-013, 31.81980515339405)
SketchLine_7.setName("SketchLine_8")
SketchLine_7.result().setName("SketchLine_8")
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_8.setName("SketchConstraintCoincidence_9")
SketchConstraintPerpendicular_1 = Sketch_1.setPerpendicular(SketchLine_7.result(), SketchLine_4.result())
SketchConstraintParallel_1 = Sketch_1.setParallel(SketchLine_7.result(), SketchLine_5.result())
SketchConstraintParallel_2 = Sketch_1.setParallel(SketchLine_4.result(), SketchLine_6.result())
SketchLine_8 = Sketch_1.addLine(-31.8198051533955, -6.650124202329274e-013, -50, -8.919190713816664e-015)
SketchLine_8.setName("SketchLine_9")
SketchLine_8.result().setName("SketchLine_9")
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_8.startPoint())
SketchConstraintCoincidence_10.setName("SketchConstraintCoincidence_11")
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchCircle_1.results()[1])
SketchConstraintCoincidence_11.setName("SketchConstraintCoincidence_12")
SketchLine_9 = Sketch_1.addLine(31.81980515339462, -6.650124210799604e-013, 50, -8.919190733668999e-015)
SketchLine_9.setName("SketchLine_10")
SketchLine_9.result().setName("SketchLine_10")
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_9.startPoint())
SketchConstraintCoincidence_12.setName("SketchConstraintCoincidence_13")
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchLine_9.endPoint(), SketchCircle_1.results()[1])
SketchConstraintCoincidence_13.setName("SketchConstraintCoincidence_14")
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchLine_4.result(), SketchLine_6.result())
SketchConstraintEqual_2 = Sketch_1.setEqual(SketchLine_7.result(), SketchLine_5.result())
SketchConstraintCollinear_2 = Sketch_1.setCollinear(SketchLine_8.result(), SketchLine_9.result())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_8.result())
SketchConstraintEqual_3 = Sketch_1.setEqual(SketchLine_8.result(), SketchLine_1.result())
SketchConstraintEqual_4 = Sketch_1.setEqual(SketchLine_2.result(), SketchLine_1.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_7.result(), "R1*0.9")
SketchConstraintLength_1.setName("SketchConstraintLength_2")
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), "L1", "L1")
Extrusion_1.result().setName("tuyau_principal")
Extrusion_1.result().subResult(0).setName("Extrusion_1_1_1")
Extrusion_1.result().subResult(1).setName("Extrusion_1_1_2")
Extrusion_1.result().subResult(2).setName("Extrusion_1_1_3")
Extrusion_1.result().subResult(3).setName("Extrusion_1_1_4")
Extrusion_1.result().subResult(4).setName("Extrusion_1_1_5")
Rotation_1 = model.addRotation(Part_1_doc, [model.selection("COMPSOLID", "tuyau_principal")], model.selection("EDGE", "PartSet/OX"), 45)
Sketch_2 = model.addSketch(Part_1_doc, model.standardPlane("XOZ"))
SketchProjection_3 = Sketch_2.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_2 = SketchProjection_3.createdFeature()
SketchCircle_2 = Sketch_2.addCircle(0, 0, 40)
SketchConstraintCoincidence_14 = Sketch_2.setCoincident(SketchPoint_2.result(), SketchCircle_2.center())
SketchConstraintCoincidence_14.setName("SketchConstraintCoincidence_15")
SketchConstraintRadius_2 = Sketch_2.setRadius(SketchCircle_2.results()[1], "R2")
model.do()
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchCircle_2_2f")], model.selection(), 0, "R1+L2")
Extrusion_2.result().setName("piquage")
Smash_1 = model.addSmash(Part_1_doc, [model.selection("SOLID", "piquage")], [model.selection("COMPSOLID", "tuyau_principal")])
Smash_1.result().setName("té")
Smash_1.result().setColor(153, 76, 153)
Smash_1.result().subResult(0).setName("Smash_1_1_1")
Smash_1.result().subResult(0).setColor(51, 102, 51)
Smash_1.result().subResult(1).setName("Smash_1_1_2")
Smash_1.result().subResult(1).setColor(0, 204, 0)
Smash_1.result().subResult(2).setName("Smash_1_1_3")
Smash_1.result().subResult(2).setColor(204, 102, 204)
Smash_1.result().subResult(3).setName("Smash_1_1_4")
Smash_1.result().subResult(3).setColor(204, 102, 204)
Smash_1.result().subResult(4).setName("Smash_1_1_5")
Smash_1.result().subResult(4).setColor(204, 0, 204)
Smash_1.result().subResult(5).setName("Smash_1_1_6")
Smash_1.result().subResult(5).setColor(76, 76, 153)
Recover_1 = model.addRecover(Part_1_doc, Smash_1, [Extrusion_2.result(), Rotation_1.result()])
Recover_1.result().setName("tuyau_principal")
Recover_1.result().subResult(0).setName("Recover_1_1_1")
Recover_1.result().subResult(1).setName("Recover_1_1_2")
Recover_1.result().subResult(2).setName("Recover_1_1_3")
Recover_1.result().subResult(3).setName("Recover_1_1_4")
Recover_1.result().subResult(4).setName("Recover_1_1_5")
Recover_1.results()[1].setName("piquage")
Intersection_1 = model.addIntersection(Part_1_doc, [model.selection("COMPSOLID", "tuyau_principal"), model.selection("SOLID", "piquage")])
Intersection_1.result().setName("intersection")
Intersection_1.result().subResult(0).setName("Intersection_1_1_1")
Intersection_1.result().subResult(1).setName("Intersection_1_1_2")
Intersection_1.result().subResult(2).setName("Intersection_1_1_3")
Intersection_1.result().subResult(3).setName("Intersection_1_1_4")
Intersection_1.result().subResult(4).setName("Intersection_1_1_5")
Intersection_1.result().subResult(5).setName("Intersection_1_1_6")
Intersection_1.result().subResult(6).setName("Intersection_1_1_7")
Intersection_1.result().subResult(7).setName("Intersection_1_1_8")
Intersection_1.result().subResult(8).setName("Intersection_1_1_9")
Intersection_1.result().subResult(9).setName("Intersection_1_1_10")
Intersection_1.result().subResult(10).setName("Intersection_1_1_11")
Intersection_1.result().subResult(11).setName("Intersection_1_1_12")
Intersection_1.result().subResult(12).setName("Intersection_1_1_13")
Intersection_1.result().subResult(13).setName("Intersection_1_1_14")
Intersection_1.result().subResult(14).setName("Intersection_1_1_15")
Intersection_1.result().subResult(15).setName("Intersection_1_1_16")
Intersection_1.result().subResult(16).setName("Intersection_1_1_17")
Edge_1_objects = [model.selection("EDGE", "Intersection_1_1_3"), model.selection("EDGE", "Intersection_1_1_12"), model.selection("EDGE", "Intersection_1_1_14"), model.selection("EDGE", "Intersection_1_1_5"), model.selection("EDGE", "Intersection_1_1_1")]
Edge_1 = model.addEdge(Part_1_doc, Edge_1_objects)
Edge_1.results()[1].setName("intersection_1")
Edge_1.results()[2].setName("intersection_2")
Edge_1.results()[3].setName("intersection_3")
Edge_1.results()[4].setName("intersection_4")
model.do()
model.testHaveNamingSubshapes(Edge_1, model, Part_1_doc)
model.end()
