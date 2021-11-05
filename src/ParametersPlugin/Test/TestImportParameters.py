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

from salome.shaper import model
import inspect, os

from PyQt5.Qt import QApplication

import salome
salome.salome_init_without_session()
salome.salome_init(1)
if QApplication.instance() is None:
  app = QApplication([])

data_dir = os.path.join(os.path.dirname(inspect.getfile(lambda: None)), "data")

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

nameFile = "parameters.txt"

aDir = os.path.join(data_dir, nameFile)

aListOfParameters = model.importParameters(Part_1_doc, aDir)

Box_1 = model.addBox(Part_1_doc, "Longueur", "Largeur", "Hauteur")

assert(len(Box_1.feature().error()) == 0)
assert(len(aListOfParameters) == 5)
assert(model.checkPythonDump())
