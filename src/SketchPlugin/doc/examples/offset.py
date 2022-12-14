from salome.shaper import model

model.begin()
partSet = model.moduleDocument()

Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOY"))

SketchLine_1 = Sketch_1.addLine(0, 0, 0, 100)
SketchLine_2 = Sketch_1.addLine(0, 100, 100, 100)
SketchLine_3 = Sketch_1.addLine(100, 100, 100, 0)
SketchLine_4 = Sketch_1.addLine(0, 0, 100, 0)

Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.endPoint())
Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_4.startPoint())

SketchOffset_1_objects = [SketchLine_1.result(), SketchLine_2.result(), SketchLine_3.result(), SketchLine_4.result()]

### KeepDistance (default) mode, not reversed (outside)
SketchOffset_1 = Sketch_1.addOffset(SketchOffset_1_objects, 10.0, False)

### KeepDistance mode, reversed (inside)
SketchOffset_2 = Sketch_1.addOffset(SketchOffset_1_objects, 25.0, True, "KeepDistance")

### Arcs mode, reversed (inside)
SketchOffset_3 = Sketch_1.addOffset(SketchOffset_1_objects, 15.0, True, "Arcs")

### Lines mode, not reversed (outside)
SketchOffset_4 = Sketch_1.addOffset(SketchOffset_1_objects, 20.0, False, "Lines")

model.do()
model.end()
