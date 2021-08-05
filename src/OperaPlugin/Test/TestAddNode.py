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
      TestAddNode.py
      Test case of OperaPlugin_AddNode
"""
#=========================================================================
# Initialization of the test
#=========================================================================
from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

### Create two box (one inside the other)
Box_1 = model.addBox(Part_1_doc, 0, 0, 0, 20, 20, 20)
Box_2 = model.addBox(Part_1_doc, 0, 0, 0, 10, 10, 10)

### Create more boxes for error tests
Box_3= model.addBox(Part_1_doc, 0, 0, 0, 10, 10, 10)
Box_4 = model.addBox(Part_1_doc, 0, 0, 0, 20, 20, 20)
Box_5 = model.addBox(Part_1_doc, 0, 0, 0, 20, 20, 20)
Box_6 = model.addBox(Part_1_doc, 50, 50, 50, 10, 10, 10)
Box_7 = model.addBox(Part_1_doc, 0, 0, 0, 20, 20, 20)
Box_8 = model.addBox(Part_1_doc, 20, 0, 0, 10, 10, 10)
Box_9 = model.addBox(Part_1_doc, 0, 0, 0, 20, 20, 20)
Box_10 = model.addBox(Part_1_doc, 0, 0, 0, 10, 10, 10)
Box_11 = model.addBox(Part_1_doc, 5, 5, 5, 10, 10, 10)
Box_12 = model.addBox(Part_1_doc, 0, 0, 0, 20, 20, 20)
Box_13 = model.addBox(Part_1_doc, 0, 0, 0, 10, 10, 10)
Box_14 = model.addBox(Part_1_doc, 0, 0, 0, 5, 5, 5)


### Create a volume from the cylinder and the sphere
AddNode_1 = model.addAddNode(Part_1_doc, model.selection("SOLID", "Box_1_1"), [model.selection("SOLID", "Box_2_1")])

#Checks
from GeomAPI import GeomAPI_Shape

model.testNbResults(AddNode_1, 1)
model.testNbSubResults(AddNode_1, [2])
model.testNbSubShapes(AddNode_1, GeomAPI_Shape.SOLID, [2])
model.testNbSubShapes(AddNode_1, GeomAPI_Shape.FACE, [18])
model.testHaveNamingFaces(AddNode_1, model, Part_1_doc)

### Create a AddNode with no main object
AddNode_2 = model.addAddNode(Part_1_doc, model.selection("SOLID", "None"), [model.selection("SOLID", "Box_3_1")])
model.testNbResults(AddNode_2, 0)
assert(AddNode_2.feature().error() == "Attribute \"main_object\" is not initialized.")

### Create a AddNode with no tools
AddNode_3 = model.addAddNode(Part_1_doc, model.selection("SOLID", "Box_4_1"), [])
model.testNbResults(AddNode_3, 0)
assert(AddNode_3.feature().error() == "Attribute \"tools_list\" is not initialized.")

### Create a AddNode with tools outside the main object
AddNode_4 = model.addAddNode(Part_1_doc, model.selection("SOLID", "Box_5_1"), [model.selection("SOLID", "Box_6_1")])
model.testNbResults(AddNode_4, 0)
assert(AddNode_4.feature().error() == "Error: All tools must be fully inside the main object")

### Create a AddNode with tools intersecting the main object
AddNode_5 = model.addAddNode(Part_1_doc, model.selection("SOLID", "Box_7_1"), [model.selection("SOLID", "Box_8_1")])
model.testNbResults(AddNode_5, 0)
assert(AddNode_5.feature().error() == "Error: All tools must be fully inside the main object")

### Create a AddNode with intersecting tools
AddNode_6 = model.addAddNode(Part_1_doc, model.selection("SOLID", "Box_9_1"), [model.selection("SOLID", "Box_10_1"), model.selection("SOLID", "Box_11_1")])
model.testNbResults(AddNode_6, 0)
assert(AddNode_6.feature().error() == "Error: Tools must not intersect each others")

### Create a AddNode with a tool inside another
AddNode_7 = model.addAddNode(Part_1_doc, model.selection("SOLID", "Box_12_1"), [model.selection("SOLID", "Box_13_1"), model.selection("SOLID", "Box_14_1")])
model.testNbResults(AddNode_7, 0)
assert(AddNode_7.feature().error() == "Error: Tools must not intersect each others")


# TODO : Test with out of bounds tools
# TODO : Test with intersected tools

#=========================================================================
# End of test
#=========================================================================
