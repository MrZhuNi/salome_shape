# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchLine_1 = Sketch_1.addLine(72.2418524871355, 29.96679215537161, -69.82505995666611, 29.96679215537161)
SketchLine_2 = Sketch_1.addLine(-69.82505995666611, 29.96679215537161, -69.82505995666611, -40.02291624943114)
SketchLine_3 = Sketch_1.addLine(-69.82505995666611, -40.02291624943114, 72.2418524871355, -40.02291624943114)
SketchLine_4 = Sketch_1.addLine(72.2418524871355, -40.02291624943114, 72.2418524871355, 29.96679215537161)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
model.do()
Point_2 = model.addPoint(Part_1_doc, 0, 100, 40)
Point_3 = model.addPoint(Part_1_doc, 0, 260, 60)
Interpolation_1_objects = [model.selection("VERTEX", "PartSet/Origin"), model.selection("VERTEX", "Point_1"), model.selection("VERTEX", "Point_2")]
Interpolation_1 = model.addInterpolation(Part_1_doc, Interpolation_1_objects, False, False)
Pipe_1 = model.addPipe(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_2f-SketchLine_3f-SketchLine_4f")], model.selection("EDGE", "Interpolation_1_1"), model.selection("EDGE", "PartSet/OX"))
model.do()
model.end()
