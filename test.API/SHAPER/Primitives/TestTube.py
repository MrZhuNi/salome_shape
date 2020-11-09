# Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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
Test case for Primitive Tube feature.
Written on High API.
"""
from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

# Tests
Tube_1 = model.addTube(Part_1_doc, 2, 7, 15)
Tube_2 = model.addTube(Part_1_doc, 2, 7, 15, 45, 270)
Tube_3 = model.addTube(Part_1_doc, 2, 7, -15)

model.do()
model.end()

# Checks
from GeomAPI import GeomAPI_Shape

model.testNbResults(Tube_1, 1)
model.testNbSubResults(Tube_1, [0])
model.testNbSubShapes(Tube_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Tube_1, GeomAPI_Shape.FACE, [4])
model.testHaveNamingFaces(Tube_1, model, Part_1_doc)


model.testNbResults(Tube_2, 1)
model.testNbSubResults(Tube_2, [0])
model.testNbSubShapes(Tube_2, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Tube_2, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Tube_2, model, Part_1_doc)

model.testNbResults(Tube_3, 0)
