"""
      TestExtrusionFuse.py
      Unit test of FeaturesPlugin_ExtrusionFuse class
      
      class FeaturesPlugin_ExtrusionFuse : public FeaturesPlugin_ExtrusionBoolean
		static const std::string MY_EXTRUSION_ID("ExtrusionFuse");
        static const std::string MY_SKETCH_OBJECT_ID("sketch");
        static const std::string METHOD_ATTR("CreationMethod");
        static const std::string MY_TO_SIZE_ID("to_size");
        static const std::string MY_FROM_SIZE_ID("from_size");
        static const std::string MY_TO_OBJECT_ID("to_object");
        static const std::string MY_TO_OFFSET_ID("to_offset");
        static const std::string MY_FROM_OBJECT_ID("from_object");
        static const std::string MY_FROM_OFFSET_ID("from_offset");
        static const std::string MY_BOOLEAN_OBJECTS_ID("boolean_objects");

        data()->addAttribute(SKETCH_OBJECT_ID(), ModelAPI_AttributeReference::typeId());
        data()->addAttribute(CREATION_METHOD(), ModelAPI_AttributeString::typeId());
        data()->addAttribute(TO_SIZE_ID(), ModelAPI_AttributeDouble::typeId());
        data()->addAttribute(FROM_SIZE_ID(), ModelAPI_AttributeDouble::typeId());
        data()->addAttribute(TO_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
        data()->addAttribute(TO_OFFSET_ID(), ModelAPI_AttributeDouble::typeId());
        data()->addAttribute(FROM_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
        data()->addAttribute(FROM_OFFSET_ID(), ModelAPI_AttributeSelection::typeId());
        data()->addAttribute(BOOLEAN_OBJECTS_ID(), ModelAPI_AttributeSelectionList::typeId());
"""
#=========================================================================
# Initialization of the test
#=========================================================================
from ModelAPI import *
from GeomDataAPI import *
from GeomAlgoAPI import *
from GeomAPI import *
import math

__updated__ = "2014-12-16"

aSession = ModelAPI_Session.get()
aDocument = aSession.moduleDocument()
# Create a part for extrusion
aSession.startOperation()
aPartFeature = aDocument.addFeature("Part")
aSession.finishOperation()
assert (len(aPartFeature.results()) == 1)
# Another way is:
# aPart = aSession.activeDocument()
aPartResult = modelAPI_ResultPart(aPartFeature.firstResult())
aPart = aPartResult.partDoc()
#=========================================================================
# Create a sketch circle to extrude
#=========================================================================
aSession.startOperation()
aCircleSketchFeature = featureToCompositeFeature(aPart.addFeature("Sketch"))
origin = geomDataAPI_Point(aCircleSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 0)
dirx = geomDataAPI_Dir(aCircleSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
norm = geomDataAPI_Dir(aCircleSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)
# Create circle
aSketchCircle = aCircleSketchFeature.addFeature("SketchCircle")
anCircleCentr = geomDataAPI_Point2D(aSketchCircle.attribute("CircleCenter"))
aCircleRadius = aSketchCircle.real("CircleRadius")
anCircleCentr.setValue(0, 0)
aCircleRadius.setValue(20)
aSession.finishOperation()
#=========================================================================
# Make extrusion on circle
#=========================================================================
# Build shape from sketcher results
aCircleSketchResult = aCircleSketchFeature.firstResult()
aCircleSketchEdges = modelAPI_ResultConstruction(aCircleSketchResult).shape()
origin = geomDataAPI_Point(aCircleSketchFeature.attribute("Origin")).pnt()
dirX = geomDataAPI_Dir(aCircleSketchFeature.attribute("DirX")).dir()
norm = geomDataAPI_Dir(aCircleSketchFeature.attribute("Norm")).dir()
aCircleSketchFaces = ShapeList()
GeomAlgoAPI_SketchBuilder.createFaces(
    origin, dirX, norm, aCircleSketchEdges, aCircleSketchFaces)
assert (len(aCircleSketchFaces) > 0)
assert (aCircleSketchFaces[0] is not None)
# Create extrusion
aSession.startOperation()
anExtrusionFt = aPart.addFeature("Extrusion")
assert (anExtrusionFt.getKind() == "Extrusion")
# selection type FACE=4
anExtrusionFt.selectionList("base").append(
    aCircleSketchResult, aCircleSketchFaces[0])
anExtrusionFt.string("CreationMethod").setValue("BySizes")
anExtrusionFt.real("to_size").setValue(50)
anExtrusionFt.real("from_size").setValue(0)
anExtrusionFt.real("to_offset").setValue(0) #TODO: remove
anExtrusionFt.real("from_offset").setValue(0) #TODO: remove
anExtrusionFt.execute()
aSession.finishOperation()
assert (anExtrusionFt.real("to_size").value() == 50.0)

# Check extrusion results
assert (len(anExtrusionFt.results()) > 0)
anExtrusionResult = modelAPI_ResultBody(anExtrusionFt.firstResult())
assert (anExtrusionResult is not None)

#=========================================================================
# Make extrusion cut
#=========================================================================
aSession.startOperation()
anExtrusionFuseFt = featureToCompositeFeature(aPart.addFeature("ExtrusionFuse"))
assert (anExtrusionFuseFt.getKind() == "ExtrusionFuse")
# selection type FACE=4
aSession.startOperation()
aCircleSketchFeature = featureToCompositeFeature(anExtrusionFuseFt.addFeature("Sketch"))
origin = geomDataAPI_Point(aCircleSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 50)
dirx = geomDataAPI_Dir(aCircleSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
norm = geomDataAPI_Dir(aCircleSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)
aCircleSketchFeature.selection("External").selectSubShape("face", "Extrusion_1/TopFace_1")
aSketchCircle = aCircleSketchFeature.addFeature("SketchCircle")
anCircleCentr = geomDataAPI_Point2D(aSketchCircle.attribute("CircleCenter"))
aCircleRadius = aSketchCircle.real("CircleRadius")
anCircleCentr.setValue(0, 0)
aCircleRadius.setValue(10)
aSession.finishOperation()
anExtrusionFuseFt.string("CreationMethod").setValue("BySizes")
anExtrusionFuseFt.real("to_size").setValue(10)
anExtrusionFuseFt.real("from_size").setValue(10)
anExtrusionFuseFt.real("to_offset").setValue(0) #TODO: remove
anExtrusionFuseFt.real("from_offset").setValue(0) #TODO: remove
anExtrusionFuseFt.selectionList("boolean_objects").append(anExtrusionResult, anExtrusionResult.shape())
anExtrusionFuseFt.execute()
aSession.finishOperation()

#=========================================================================
# Test results
#=========================================================================
aFactory = ModelAPI_Session.get().validators()
assert (aFactory.validate(anExtrusionFuseFt))
assert (len(anExtrusionFuseFt.results()) > 0)
aCurrentResult = modelAPI_ResultBody(anExtrusionFuseFt.firstResult())
assert (aCurrentResult is not None)
aSession.finishOperation()