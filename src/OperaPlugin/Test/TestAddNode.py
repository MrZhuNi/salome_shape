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
from GeomAPI import GeomAPI_Shape
from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

### Create Box
Box_1 = model.addBox(Part_1_doc, 0, 0, 0, 20, 20, 20)
Box_2 = model.addBox(Part_1_doc, 10, 0, -10, 5, 5, 5)
Box_3 = model.addBox(Part_1_doc, 0, 0, 20, 10, 10, 10)
Box_4 = model.addBox(Part_1_doc, 0, 0, 0, 2, 2, 2)
Box_5 = model.addBox(Part_1_doc, -3, -3, -3, 3, 3, 3)
Box_6 = model.addBox(Part_1_doc, -10, 13, -10, 7, 7, 7)
Box_7 = model.addBox(Part_1_doc, 35, 35, 35, 10, 10, 10) #ERROR : Box is outside main Object
Box_8 = model.addBox(Part_1_doc, 0, 0, 0, 10, 10, 10)    #ERROR : For test with no Main Object
Box_9 = model.addBox(Part_1_doc, 0, 0, 0, 10, 10, 10)    #ERROR : For test with no Main Object

### Create addNodes
AddNode_1 = model.addAddNode(Part_1_doc, model.selection("SOLID", "Box_1_1"), model.selection("SOLID", "Box_2_1"))
AddNode_2 = model.addAddNode(Part_1_doc, model.selection("SOLID", "AddNode_1_1"), model.selection("SOLID", "Box_3_1"))
AddNode_3 = model.addAddNode(Part_1_doc, model.selection("SOLID", "AddNode_2_1"), model.selection("SOLID", "Box_4_1"))
AddNode_4 = model.addAddNode(Part_1_doc, model.selection("SOLID", "AddNode_3_1"), model.selection("SOLID", "Box_5_1"))
AddNode_5 = model.addAddNode(Part_1_doc, model.selection("SOLID", "AddNode_4_1"), model.selection("SOLID", "Box_6_1"))

# # #Checks
model.testNbResults(AddNode_5, 1)
model.testNbSubResults(AddNode_5, [6])
model.testNbSubShapes(AddNode_5, GeomAPI_Shape.SOLID, [6])
model.testHaveNamingFaces(AddNode_5, model, Part_1_doc)

# ### Create a AddNode with tools outside the main object
AddNode_6 = model.addAddNode(Part_1_doc, model.selection("SOLID", "AddNode_5_1"), model.selection("SOLID", "Box_7_1"))
model.testNbResults(AddNode_6, 0)
assert(AddNode_6.feature().error() == "Error: All tools must intersect the main object")

### Create a AddNode with no main object
AddNode_7 = model.addAddNode(Part_1_doc, model.selection("SOLID", "None"), model.selection("SOLID", "Box_8_1"))
model.testNbResults(AddNode_7, 0)
assert(AddNode_7.feature().error() == "Attribute \"main_object\" is not initialized.")

### Create a AddNode with no tools
AddNode_8 = model.addAddNode(Part_1_doc, model.selection("SOLID", "Box_9_1"), model.selection("SOLID", "None"))
model.testNbResults(AddNode_8, 0)
assert(AddNode_8.feature().error() == "Attribute \"tool_object\" is not initialized.")

#=========================================================================
# End of test
#=========================================================================
