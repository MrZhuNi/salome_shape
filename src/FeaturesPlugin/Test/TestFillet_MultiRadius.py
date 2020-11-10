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
      TestFillet.py
      Unit test of ...
"""
#=========================================================================
# Initialization of the test
#=========================================================================

import salome

import os
import math
from tempfile import TemporaryDirectory


from ModelAPI import *
from salome.shaper import model

from GeomAPI import GeomAPI_Shape
from GeomAlgoAPI import *

__updated__ = "2020-10-10"

#=========================================================================
# Help functions
#=========================================================================
def removeFile(theFileName):
    try: os.remove(theFileName)
    except OSError: pass
    assert not os.path.exists(theFileName), \
            "Can not remove file {0}".format(theFileName)


#=========================================================================
# test Fillet on an edge with multiple radii identified by points
#=========================================================================
def testFillet_multiradii_bypoints():
    model.begin()
    partSet = model.moduleDocument()

    ### Create Part
    Part_1 = model.addPart(partSet)
    Part_1_doc = Part_1.document()

    ### Create Box
    Box_1 = model.addBox(Part_1_doc, 10, 10, 10)

    ### Create 2 Points
    Point_2 = model.addPoint(Part_1_doc, model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Top]"), 0.4, True, False)
    Point_3 = model.addPoint(Part_1_doc, model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Top]"), 0.8, True, False)

    ### Create Fillet on an edge with 4 radii identified by points
    Fillet_1 = model.addFilletMultiRadius(Part_1_doc, model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Top]"), [model.selection("VERTEX", "Point_1"), model.selection("VERTEX", "Point_2")], [0.5, 2, 1, 2])

    model.end()  
    model.testNbResults(Fillet_1, 1)
    model.testResultsVolumes(Fillet_1, [995.3927])

#=========================================================================
# test Fillet on an edge(s) or/and face(s) with multiple radii identified
# by curvilinear abscissa
#=========================================================================
def testFillet_multiradii_bycurvabs():
    model.begin()
    partSet = model.moduleDocument()

    ### Create Part
    Part_1 = model.addPart(partSet)
    Part_1_doc = Part_1.document()

    ### Create Box
    Box_1 = model.addBox(Part_1_doc, 10, 10, 10)

    ### Create Fillet
    Fillet_1 = model.addFilletMultiRadius(Part_1_doc, [model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Top]")], [0, 0.4, 0.7, 1],[1, 2, 0.5, 2])

    ### Create Fillet
    Fillet_2 = model.addFilletMultiRadius(Part_1_doc, [model.selection("FACE", "Box_1_1/Right")], [0, 0.5, 1],[1, 0.5, 1])
    model.end()
    model.testNbResults(Fillet_2, 1)
    model.testResultsVolumes(Fillet_2, [990.974897])
    
    
if __name__ == '__main__':
    with TemporaryDirectory() as tmp_dir:
      
        testFillet_multiradii_bypoints()
        testFillet_multiradii_bycurvabs()
        #=========================================================================
        # End of test
        #=========================================================================
