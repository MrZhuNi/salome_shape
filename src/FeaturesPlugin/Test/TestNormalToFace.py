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

"""
      Unit test of ...
"""
#=========================================================================
# Initialization of the test
#=========================================================================


import os
import math

from ModelAPI import *
from salome.shaper import model


__updated__ = "2020-11-12"


#=========================================================================
# test creating normal to face  
#=========================================================================
def test_Normal_to_face():

    model.begin()
    file_path = os.path.join(os.getenv("DATA_DIR"),"Shapes","Brep","box1.brep")
    partSet = model.moduleDocument()
    Part_1 = model.addPart(partSet)
    Part_1_doc = Part_1.document()
    Import_1 = model.addImport(Part_1_doc,file_path)
    model.do()
    ### Create Normal
    Normal_1 = model.getNormal(Part_1_doc, model.selection("FACE", "box1_1/Shape_6"))
    model.end()
    
    assert (len(Normal_1.results()) > 0)
    assert(Normal_1.feature().error() == "")
    anAxisResult = modelAPI_ResultConstruction(Normal_1.feature().firstResult())
    assert (anAxisResult is not None)

    
if __name__ == '__main__':

    test_Normal_to_face()
        
    #=========================================================================
    # End of test
    #=========================================================================
