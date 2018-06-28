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

"""
Test case for Construction Point feature by lines intersection.
"""

from salome.shaper import model
from GeomAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchLine_1 = Sketch_1.addLine(-50, 50, 50, 50)
model.do()
Box_1 = model.addBox(Part_1_doc, 25, 100, 100)
Point_2 = model.addPoint(Part_1_doc, model.selection("EDGE", "Box_1_1/Back&Box_1_1/Bottom"), model.selection("EDGE", "Sketch_1/Edge-SketchLine_1"))
Point_3 = model.addPoint(Part_1_doc, model.selection("EDGE", "Sketch_1/Edge-SketchLine_1"), model.selection("EDGE", "Box_1_1/Front&Box_1_1/Bottom"))
model.do()
model.end()

assert (len(Point_2.results()) > 0)
rightPosition = GeomAPI_Vertex(0, 50, 0)
assert(rightPosition.isEqual(Point_2.results()[0].resultSubShapePair()[0].shape()))

assert (len(Point_3.results()) > 0)
rightPosition = GeomAPI_Vertex(25, 50, 0)
assert(rightPosition.isEqual(Point_3.results()[0].resultSubShapePair()[0].shape()))

assert(model.checkPythonDump())
