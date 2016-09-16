# Copyright (C) 2014-2016 CEA/DEN, EDF R&D

# File:        UnitTestSphere.py
# Created:     31 Mar 2016
# Author:      CEA (delegation to Alyotech)

"""
    UnitTestSphere.py
    Unit Test of PrimitivesPlugin_Sphere class

class PrimitivesPlugin_Sphere : public ModelAPI_Feature
    static const std::string MY_SPHERE_ID("Sphere");
    static const std::string METHOD_ATTR("CreationMethod");
    static const std::string MY_CENTER("center");
    static const std::string MY_RADIUS("radius");


    data()->addAttribute(METHOD(), ModelAPI_AttributeString::typeId());
    data()->addAttribute(CENTER(), ModelAPI_AttributeSelection::typeId());
    data()->addAttribute(RADIUS(), ModelAPI_AttributeDouble::typeId());


"""

#=========================================================================
# Initialization of the test
#=========================================================================
from ModelAPI import *
from GeomDataAPI import *
from GeomAlgoAPI import *
from GeomAPI import *
import math

__updated__ = "2016-01-04"

aSession = ModelAPI_Session.get()
aDocument = aSession.moduleDocument()
# Create a part for creation of a sphere
aSession.startOperation()
aPartFeature = aDocument.addFeature("Part")
aSession.finishOperation()
assert (len(aPartFeature.results()) == 1)

aPartResult = modelAPI_ResultPart(aPartFeature.firstResult())
aPart = aPartResult.partDoc()

#=========================================================================
# Creation of a Sphere by a point and a radius
#=========================================================================

aSession.startOperation()

#Create the center
aCenter = aPart.addFeature("Point")
assert(aCenter.getKind() == "Point")
aCenter.real("x").setValue(10.0)
aCenter.real("y").setValue(10.0)
aCenter.real("z").setValue(10.0)
aCenter.execute()
aCenterResult = aCenter.firstResult()
aCenterVertex = aCenterResult.shape()

aSphereByCenter = aPart.addFeature("Sphere")
assert (aSphereByCenter.getKind() == "Sphere")
aSphereByCenter.selection("center").setValue(aCenterResult, aCenterVertex)
aSphereByCenter.real("radius").setValue(5.)
aSphereByCenter.execute()

# Check sphere results
assert (len(aSphereByCenter.results()) > 0)
aSphereCenterResult = modelAPI_ResultBody(aSphereByCenter.firstResult())
assert (aSphereCenterResult is not None)

# Check sphere volume
aRefVolumeSphereCenter = (4. * math.pi * 5. ** 3 ) / 3.
aResVolumeSphereCenter = GeomAlgoAPI_ShapeTools_volume(aSphereCenterResult.shape())
assert (math.fabs(aResVolumeSphereCenter - aRefVolumeSphereCenter) < 10. ** -5)

aSession.finishOperation()

#=========================================================================
# End of test
#=========================================================================
