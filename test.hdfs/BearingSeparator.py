# Copyright (C) 2021-2022  CEA/DEN, EDF R&D
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

if __name__ == "__main__":
  aPartFeature = locals()["Part_1"]
  model.testNbResults(aPartFeature, 1)
  model.testNbSubResults(aPartFeature, [0])
  model.testNbSubShapes(aPartFeature, GeomAPI_Shape.SOLID, [1])
  model.testNbSubShapes(aPartFeature, GeomAPI_Shape.FACE, [115])
  model.testNbSubShapes(aPartFeature, GeomAPI_Shape.EDGE, [890])
  model.testNbSubShapes(aPartFeature, GeomAPI_Shape.VERTEX, [1780])
  model.testResultsVolumes(aPartFeature, [1311.87963636394])
  model.testResultsAreas(aPartFeature, [3765.24411189])

  assert(model.checkPythonDump())
