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
      Unit test of Check Duplicated faces
"""
#=========================================================================
# Initialization of the test
#=========================================================================


import os
import math

from ModelAPI import *
from GeomAPI import *
from salome.shaper import model


__updated__ = "2020-11-12"


if __name__ == '__main__':

    model.begin()
    partSet = model.moduleDocument()
    Part_1 = model.addPart(partSet)
    Part_1_doc = Part_1.document()
    ### Create Box
    Box_1 = model.addBox(Part_1_doc, 100, 50, 100)
    ### Create Point
    Point_1 = model.addPoint(Part_1_doc, 100, 0, 0)
    ### Create Point
    Point_2 = model.addPoint(Part_1_doc, 250, 50, 100)
    ### Create Box
    Box_2 = model.addBox(Part_1_doc, model.selection("VERTEX", "all-in-Point_1"), model.selection("VERTEX", "all-in-Point_2"))
    ### Create Point
    Point_3 = model.addPoint(Part_1_doc, 100, 50, 250)
    ### Create Box
    Box_3 = model.addBox(Part_1_doc, model.selection("VERTEX", "[Box_1_1/Back][Box_1_1/Left][Box_1_1/Top]"), model.selection("VERTEX", "Point_3"))
    ### Create Compound
    Compound_1_objects = [model.selection("SOLID", "Box_1_1"), 
                          model.selection("SOLID", "Box_2_1"), 
                          model.selection("SOLID", "Box_3_1")]
    Compound_1 = model.addCompound(Part_1_doc, Compound_1_objects)
    ### Create Duplicated_faces
    Duplicated_faces_1 = model.getDuplicatedFaces(Part_1_doc, model.selection("COMPOUND", "Compound_1_1"), 0.001, "mygroup")

    model.do()
     # Check results
    Duplicated_faces_1_Feature = Duplicated_faces_1.feature()
    assert Duplicated_faces_1_Feature.error() == ''
    assert Duplicated_faces_1_Feature.name() == "Duplicated_faces_1"

    aSelectionList = Duplicated_faces_1_Feature.selectionList("group_list")
    assert aSelectionList.size() == 2

    assert(Part_1_doc.size("Groups") == 1)

    #assert Part_1_doc.object("Groups", 0).name() == "mygroup"
    resShape = modelAPI_Result(Part_1_doc.object("Groups", 0)).shape()
    assert(not resShape.isNull())

    # the group result is a face, check that this is one face
    aShapeExplorer = GeomAPI_ShapeExplorer(resShape, GeomAPI_Shape.FACE)
    assert(aShapeExplorer.more())
    assert(aShapeExplorer.current().isFace())
    aShapeExplorer.next()
    aShapeExplorer.next()
    assert(not aShapeExplorer.more())
    
    model.end()

    #=========================================================================
    # End of test
    #=========================================================================
