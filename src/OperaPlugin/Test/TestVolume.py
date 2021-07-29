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

#=========================================================================
# Create a Box, a Torus and a Cylinder
#=========================================================================
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Torus_1 = model.addTorus(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 15, 3)
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 5, 10)

#=========================================================================
# Create one Volume
#=========================================================================

Volume_1 = model.addVolume(Part_1_doc, "Test_medium", [model.selection("SOLID", "Box_1")])

#=========================================================================
# Check results
#=========================================================================

from GeomAPI import GeomAPI_Shape

print("TOTO")
print(len(Volume_1.results()))
#model.testNbResults(Volume_1, 1)
#model.testNbSubResults(Volume_1, [0])
#model.testNbSubShapes(Volume_1, GeomAPI_Shape.SOLID, [1])
#model.testNbSubShapes(Volume_1, GeomAPI_Shape.FACE, [6])
#model.testHaveNamingFaces(Volume_1, model, Part_1_doc)

#=========================================================================
# Create two volumes at once
#=========================================================================

Volume_2 = model.addVolume(Part_1_doc, "Test_medium_2", [model.selection("SOLID", "Torus_1"),
                                                         model.selection("SOLID", "Cylinder_1")])

#=========================================================================
# Check results
#=========================================================================

print(len(Volume_2.results()))
#model.testNbResults(Volume_2, 2)
# model.testNbSubResults(Volume_2, [0])
# model.testNbSubShapes(Volume_2, GeomAPI_Shape.SOLID, [1])
# model.testNbSubShapes(Volume_2, GeomAPI_Shape.FACE, [6])
# model.testHaveNamingFaces(Volume_2, model, Part_1_doc)

#=========================================================================
# End of test
#=========================================================================

from salome.shaper import model
#assert(model.checkPythonDump())
