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
      TestFillet_MultiRadius.py
      Unit test of ...
"""
#=========================================================================
# Initialization of the test
#=========================================================================

import salome

import os
import math
from tempfile import TemporaryDirectory

import GEOM
from ModelAPI import *
from salome.shaper import model
from salome.geom import geomBuilder
from GeomAPI import GeomAPI_Shape
from GeomAlgoAPI import *

#import SALOMEDS


__updated__ = "2020-10-10"

salome.salome_init(1)

#=========================================================================
# Help functions
#=========================================================================
def removeFile(theFileName):
    try: os.remove(theFileName)
    except OSError: pass
    assert not os.path.exists(theFileName), \
            "Can not remove file {0}".format(theFileName)

#=========================================================================
# test Fillet on a solid with a constant radius
#=========================================================================
def testFillet_constant_radius_onsolids():

    model.begin()
    partSet = model.moduleDocument()
    ### Create Part
    Part_1 = model.addPart(partSet)
    Part_1_doc = Part_1.document()
    
    ### Create Box
    Box_1 = model.addBox(Part_1_doc, 20, 30, 10)

    ### Create Fillet
    Fillet_1 = model.addFillet(Part_1_doc, [model.selection("SOLID", "Box_1_1")], 2)

    ### Create Export
    Export_1 = model.exportToXAO(Part_1_doc, '/tmp/shaper_exp_cglb.xao', model.selection("SOLID", "Fillet_1_1"), 'XAO')

    model.end()

    # Check results
    ###
    ### GEOM component
    ###

    geompy = geomBuilder.New()

    O = geompy.MakeVertex(0, 0, 0)
    OX = geompy.MakeVectorDXDYDZ(1, 0, 0)
    OY = geompy.MakeVectorDXDYDZ(0, 1, 0)
    OZ = geompy.MakeVectorDXDYDZ(0, 0, 1)
    
    # where we import the fillet from SHAPER and get its basic properties
    (imported, Fillet_1_1, [], [], []) = geompy.ImportXAO("/tmp/shaper_exp_cglb.xao")
    geompy.addToStudy( O, 'O' )
    geompy.addToStudy( OX, 'OX' )
    geompy.addToStudy( OY, 'OY' )
    geompy.addToStudy( OZ, 'OZ' )
    geompy.addToStudy( Fillet_1_1, 'Fillet_1_1' )

    myDelta = 1e-6
    Props_shaper = geompy.BasicProperties(Fillet_1_1)
    print("\nBasic Properties:")
    print(" Wires length: ", Props_shaper[0])
    print(" Surface area: ", Props_shaper[1])
    print(" Volume      : ", Props_shaper[2])      

    # where we build the same fillet in GEOM for comparison
    Box_1 = geompy.MakeBoxDXDYDZ(20, 30, 10)
    Fillet_1 = geompy.MakeFilletAll(Box_1, 2)
    geompy.addToStudy( Box_1, 'Box_1' )
    geompy.addToStudy( Fillet_1, 'Fillet_1' )
    
    Props_geom = geompy.BasicProperties(Fillet_1)
    print("\nBasic Properties:")
    print(" Wires length: ", Props_geom[0])
    print(" Surface area: ", Props_geom[1])
    print(" Volume      : ", Props_geom[2]) 

    aRefSurface = Props_geom[1]
    aResSurface = Props_shaper[1]
    assert (math.fabs(aResSurface - aRefSurface) < myDelta), "The surface is wrong: expected = {0}, real = {1}".format(aRefSurface, aResSurface)

    aRefVolume = Props_geom[2]
    aResVolume = Props_shaper[2]
    assert (math.fabs(aResVolume - aRefVolume) < myDelta), "The volume is wrong: expected = {0}, real = {1}".format(aRefVolume, aResVolume)
    
    
if __name__ == '__main__':
    with TemporaryDirectory() as tmp_dir:
      
        testFillet_constant_radius_onsolids()
        #=========================================================================
        # End of test
        #=========================================================================
