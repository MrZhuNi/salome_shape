# Copyright (C) 2018-2022  CEA/DEN, EDF R&D
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
model.do()

Part_2 = model.addPart(partSet)
Part_2_doc = Part_2.document()
Box_1 = model.addBox(Part_2_doc, 10, 10, 10)
model.do()

Placement_1 = model.addPlacement(partSet, [model.selection("COMPOUND", "Part_2/")], model.selection("COMPOUND", "all-in-Part_1"), model.selection("FACE", "Part_2/Box_1_1/Top"), False, False)
assert(Placement_1.feature().error() != "")
Part_1_doc.removeFeature(Placement_1.feature())

model.end()
