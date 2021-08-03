# Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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
      TestVolume.py
      Test case of OperaPlugin_Volume
"""
#=========================================================================
# Initialization of the test
#=========================================================================
from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

### Create two box (one for error case)
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Box_2 = model.addBox(Part_1_doc, 10, 10, 10)

### Create a cylinder
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 5, 10)

### Create a sphere
Sphere_1 = model.addSphere(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), 10)

### Create a volume from the box
Volume_1 = model.addVolume(Part_1_doc, "Air", [model.selection("SOLID", "Box_1_1")])

### Create a volume from the cylinder and the sphere
Volume_2 = model.addVolume(Part_1_doc, "Eau", [model.selection("SOLID", "Cylinder_1_1"), model.selection("SOLID", "Sphere_1_1")])

#Checks
from GeomAPI import GeomAPI_Shape

model.testNbResults(Volume_1, 1)
model.testNbSubResults(Volume_1, [0])
model.testNbSubShapes(Volume_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Volume_1, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Volume_1, model, Part_1_doc)

model.testNbResults(Volume_2, 2)
model.testNbSubResults(Volume_2, [0, 0])
model.testNbSubShapes(Volume_2, GeomAPI_Shape.SOLID, [1, 1])
model.testNbSubShapes(Volume_2, GeomAPI_Shape.FACE, [3, 1])

### Create a volume with no medium
Volume_3 = model.addVolume(Part_1_doc, "", [model.selection("SOLID", "Box_2_1")])
model.testNbResults(Volume_3, 0)
assert(Volume_3.feature().error() == "Error: Medium cannot be empty.")

### Create a volume with no shapes
Volume_4 = model.addVolume(Part_1_doc, "Error", [])
model.testNbResults(Volume_4, 0)
assert(Volume_4.feature().error() == "Attribute \"volume_list\" is not initialized.")

### Create a volume with bad shapes
Volume_5 = model.addVolume(Part_1_doc, "Bad Shape", [model.selection("VERTEX", "PartSet/Origin")])
model.testNbResults(Volume_5, 0)
assert(Volume_5.feature().error() == "It does not contain element with acceptable shape type. The type should be one of the next: solid")

#=========================================================================
# End of test
#=========================================================================
