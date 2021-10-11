# Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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
import os

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

data_dir = os.path.join(os.path.dirname(sys.argv[0]), "Test_data")

nameFile = "PythonAPI_test_parametres1.txt"

aDir = os.path.join(data_dir, nameFile)

aFile = open(nameFile, 'w', encoding = "utf_8")

aFile.write("Longueur  36. # \"Longueur de la pièce\"\n")
aFile.write("Largeur  24.  # Largeur de la pièce\n")
aFile.write("Hauteur  Longueur*Largeur\n")
aFile.write("\n")
aFile.write(" \n")
aFile.write("Largeur2\n")
aFile.write("Largeur3 #Comment\n")
aFile.write("A12  5. * 5.\n")
aFile.write("# Comment\n")
aFile.write("Name#Comment\n")
aFile.write(" # Comm\n")
aFile.write("Longueur2 36. #\"Comment\" #Comm  #Comm\n")

aFile.close()

aListOfParameters = model.importParameters(Part_1_doc, nameFile)
Box_1 = model.addBox(Part_1_doc, "Longueur", "Largeur", "Hauteur")

assert(len(Box_1.feature().error()) == 0)
assert(len(aListOfParameters) > 0)
assert(len(aListOfParameters) == 8)
