## Copyright (C) 2014-2017  CEA/DEN, EDF R&D
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##
## See http:##www.salome-platform.org/ or
## email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
##

"""
      TestBoolean.py
      Unit test of FeaturesPlugin_Boolean class

      class FeaturesPlugin_Boolean
        static const std::string MY_ID("Boolean");
        static const std::string MY_OBJECT_ID("main_object");
        static const std::string MY_TOOL_ID("tool_object");
        static const std::string MY_TYPE_ID("bool_type");

        data()->addAttribute(FeaturesPlugin_Boolean::OBJECT_ID(), ModelAPI_AttributeReference::typeId());
        data()->addAttribute(FeaturesPlugin_Boolean::TOOL_ID(), ModelAPI_AttributeReference::typeId());
        data()->addAttribute(FeaturesPlugin_Boolean::TYPE_ID(), ModelAPI_AttributeInteger::typeId());
"""
#=========================================================================
# Initialization of the test
#=========================================================================
from ModelAPI import *
from GeomDataAPI import *
from GeomAlgoAPI import *
from GeomAPI import *

__updated__ = "2014-12-16"

aSession = ModelAPI_Session.get()
# Create a part for extrusions & boolean
aSession.startOperation()
aPartFeature = aSession.moduleDocument().addFeature("Part")
aSession.finishOperation()
aPart = aSession.activeDocument()
#=========================================================================
# Create a sketch with circle to extrude
#=========================================================================
aSession.startOperation()
aCircleSketchFeature = featureToCompositeFeature(aPart.addFeature("Sketch"))
origin = geomDataAPI_Point(aCircleSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 0)
dirx = geomDataAPI_Dir(aCircleSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
norm = geomDataAPI_Dir(aCircleSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)
aSketchCircle = aCircleSketchFeature.addFeature("SketchCircle")
anCircleCentr = geomDataAPI_Point2D(aSketchCircle.attribute("circle_center"))
aCircleRadius = aSketchCircle.real("circle_radius")
anCircleCentr.setValue(10., 10.)
aCircleRadius.setValue(50.)
aSession.finishOperation()
#=========================================================================
# Create a sketch with triangle to extrude
#=========================================================================
aSession.startOperation()
aTriangleSketchFeature = featureToCompositeFeature(aPart.addFeature("Sketch"))
origin = geomDataAPI_Point(aTriangleSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 0)
dirx = geomDataAPI_Dir(aTriangleSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
norm = geomDataAPI_Dir(aTriangleSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)
aSketchLineA = aTriangleSketchFeature.addFeature("SketchLine")
aSketchLineB = aTriangleSketchFeature.addFeature("SketchLine")
aSketchLineC = aTriangleSketchFeature.addFeature("SketchLine")
aLineAStartPoint = geomDataAPI_Point2D(aSketchLineA.attribute("StartPoint"))
aLineAEndPoint = geomDataAPI_Point2D(aSketchLineA.attribute("EndPoint"))
aLineBStartPoint = geomDataAPI_Point2D(aSketchLineB.attribute("StartPoint"))
aLineBEndPoint = geomDataAPI_Point2D(aSketchLineB.attribute("EndPoint"))
aLineCStartPoint = geomDataAPI_Point2D(aSketchLineC.attribute("StartPoint"))
aLineCEndPoint = geomDataAPI_Point2D(aSketchLineC.attribute("EndPoint"))
aLineAStartPoint.setValue(25., 25.)
aLineAEndPoint.setValue(100., 25.)
aLineBStartPoint.setValue(100., 25.)
aLineBEndPoint.setValue(60., 75.)
aLineCStartPoint.setValue(60., 75.)
aLineCEndPoint.setValue(25., 25.)
aSession.finishOperation()
#=========================================================================
# Make extrusion on circle (cylinder) and triangle (prism)
#=========================================================================
# Build shape from sketcher results
aSession.startOperation()
extrudedObjects = []
for eachSketchFeature in [aCircleSketchFeature, aTriangleSketchFeature]:
    # Build sketch faces
    aSketchResult = eachSketchFeature.firstResult()
    aSketchEdges = modelAPI_ResultConstruction(aSketchResult).shape()
    origin = geomDataAPI_Point(eachSketchFeature.attribute("Origin")).pnt()
    dirX = geomDataAPI_Dir(eachSketchFeature.attribute("DirX")).dir()
    norm = geomDataAPI_Dir(eachSketchFeature.attribute("Norm")).dir()
    aSketchFaces = ShapeList()
    GeomAlgoAPI_SketchBuilder.createFaces(
        origin, dirX, norm, aSketchEdges, aSketchFaces)
    # Create extrusion on them
    anExtrusionFt = aPart.addFeature("Extrusion")
    anExtrusionFt.selectionList("base").append(
        aSketchResult, aSketchFaces[0])
    anExtrusionFt.string("CreationMethod").setValue("BySizes")
    anExtrusionFt.real("from_size").setValue(0)
    anExtrusionFt.real("to_size").setValue(50)
    anExtrusionFt.real("to_offset").setValue(0) #TODO: remove
    anExtrusionFt.real("from_offset").setValue(0) #TODO: remove
    anExtrusionFt.execute()
    extrudedObjects.append(modelAPI_ResultBody(anExtrusionFt.firstResult()))
aSession.finishOperation()
#=========================================================================
# Create a pacman as boolean cut of the prism from the cylinder
#=========================================================================
aSession.startOperation()
aBooleanFt = aPart.addFeature("Cut")
aBooleanFt.selectionList("main_objects").append(extrudedObjects[0], extrudedObjects[0].shape())
aBooleanFt.selectionList("tool_objects").append(extrudedObjects[1], extrudedObjects[1].shape())
aBooleanFt.execute()
aSession.finishOperation()

assert (len(aBooleanFt.results()) > 0)
aBooleanResult = modelAPI_ResultBody(aBooleanFt.firstResult())
assert (aBooleanResult is not None)
#=========================================================================
# End of test
#=========================================================================

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(-353.344768439108, 0.857632933105, 175.14200032067)
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_2 = Sketch_2.addCircle(-126.929674099485, -2.572898799314, 176.971885556791)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2f"), model.selection("FACE", "Sketch_2/Face-SketchCircle_2_2f")], model.selection(), 10, 0)
Common_1 = model.addCommon(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1")], [model.selection("SOLID", "Extrusion_1_2")])
model.end()
assert(len(Common_1.results()) > 0)

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(-353.344768439108, 0.857632933105, 175.14200032067)
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_2 = Sketch_2.addCircle(-126.929674099485, -2.572898799314, 176.971885556791)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2f"), model.selection("FACE", "Sketch_2/Face-SketchCircle_2_2f")], model.selection(), 10, 0)
Smash_1 = model.addSmash(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1")], [model.selection("SOLID", "Extrusion_1_2")])
model.end()
assert(len(Smash_1.results()) > 0)

assert(model.checkPythonDump())
