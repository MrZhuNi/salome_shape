"""
    TestConstraintRadius.py
    Unit test of SketchPlugin_ConstraintRadius class
    
    SketchPlugin_Constraint
        static const std::string MY_CONSTRAINT_VALUE("ConstraintValue");
        static const std::string MY_FLYOUT_VALUE_PNT("ConstraintFlyoutValuePnt");
        static const std::string MY_ENTITY_A("ConstraintEntityA");
        static const std::string MY_ENTITY_B("ConstraintEntityB");
        static const std::string MY_ENTITY_C("ConstraintEntityC");
        static const std::string MY_ENTITY_D("ConstraintEntityD");
        
    SketchPlugin_ConstraintRadius
        static const std::string MY_CONSTRAINT_RADIUS_ID("SketchConstraintRadius");
        data()->addAttribute(SketchPlugin_Constraint::VALUE(),    ModelAPI_AttributeDouble::typeId());
        data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::typeId());
        data()->addAttribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT(), GeomDataAPI_Point2D::typeId());

"""
from GeomDataAPI import *
from ModelAPI import *
import math
#=========================================================================
# Initialization of the test
#=========================================================================

__updated__ = "2014-10-28"

def distancePointPoint(pointA, pointB):
    """
    subroutine to calculate distance between two points
    result of calculated distance is has 10**-5 precision
    """
    xdiff = math.pow((pointA.x() - pointB.x()), 2)
    ydiff = math.pow((pointA.y() - pointB.y()), 2)
    return round(math.sqrt(xdiff + ydiff), 5)


aSession = ModelAPI_Session.get()
aDocument = aSession.moduleDocument()
#=========================================================================
# Creation of a sketch
#=========================================================================
aSession.startOperation()
aSketchCommonFeature = aDocument.addFeature("Sketch")
aSketchFeature = featureToCompositeFeature(aSketchCommonFeature)
origin = geomDataAPI_Point(aSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 0)
dirx = geomDataAPI_Dir(aSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
norm = geomDataAPI_Dir(aSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)
aSession.finishOperation()
#=========================================================================
# Creation of an arc and a circle
#=========================================================================
aSession.startOperation()
aSketchArc = aSketchFeature.addFeature("SketchArc")
anArcCentr = geomDataAPI_Point2D(aSketchArc.attribute("ArcCenter"))
anArcCentr.setValue(10., 10.)
anArcStartPoint = geomDataAPI_Point2D(aSketchArc.attribute("ArcStartPoint"))
anArcStartPoint.setValue(0., 50.)
anArcEndPoint = geomDataAPI_Point2D(aSketchArc.attribute("ArcEndPoint"))
anArcEndPoint.setValue(50., 0.)
aSession.finishOperation()
# Test changing the arc start/end point
aSession.startOperation()
anArcStartPoint.setValue(anArcStartPoint.x(), 40.)
anArcStartPoint.setValue(0., 50.)
assert (math.hypot(anArcStartPoint.x() - 0., anArcStartPoint.y() - 50.) < 1.e-10)
aSession.finishOperation()
aSession.startOperation()
anArcEndPoint.setValue(40., anArcEndPoint.y())
anArcEndPoint.setValue(50., 0.)
assert (math.hypot(anArcEndPoint.x() - 50., anArcEndPoint.y() - 0.) < 1.e-10)
aSession.finishOperation()
# Circle
aSession.startOperation()
aSketchCircle = aSketchFeature.addFeature("SketchCircle")
anCircleCentr = geomDataAPI_Point2D(aSketchCircle.attribute("CircleCenter"))
aCircleRadius = aSketchCircle.real("CircleRadius")
anCircleCentr.setValue(-25., -25)
aCircleRadius.setValue(25.)
aSession.finishOperation()
# Change the radius of the arc
aSession.startOperation()
RADIUS = 40
anArcRadius = aSketchArc.real("ArcRadius")
anArcRadius.setValue(RADIUS)
aSession.finishOperation()
#=========================================================================
# Make a constraint to keep the radius of the arc
#=========================================================================
aSession.startOperation()
aConstraint = aSketchFeature.addFeature("SketchConstraintRadius")
aRadius = aConstraint.real("ConstraintValue")
aRefObject = aConstraint.refattr("ConstraintEntityA")
aResult = aSketchArc.lastResult()
assert (aResult is not None)
aRefObject.setObject(modelAPI_ResultConstruction(aResult))
aConstraint.execute()
aSession.finishOperation()
assert (aRadius.isInitialized())
assert (aRefObject.isInitialized())
#=========================================================================
# Make a constraint to keep the radius of the circle
#=========================================================================
aSession.startOperation()
aConstraint = aSketchFeature.addFeature("SketchConstraintRadius")
aRadius = aConstraint.real("ConstraintValue")
aRefObject = aConstraint.refattr("ConstraintEntityA")
aResult = aSketchCircle.lastResult()
assert (aResult is not None)
aRefObject.setObject(modelAPI_ResultConstruction(aResult))
aConstraint.execute()
aSession.finishOperation()
assert (aRadius.isInitialized())
assert (aRefObject.isInitialized())
#=========================================================================
# Perform some actions and checks:
# 1. Check that constraints does not changes values
# 2. Move one point of the arc
# 3. Check that second point is moved also
#=========================================================================
assert (math.fabs(distancePointPoint(anArcCentr, anArcStartPoint) - RADIUS) < 1.e-10)
assert (math.fabs(distancePointPoint(anArcCentr, anArcEndPoint) - RADIUS) < 1.e-10)
anArcPrevEndPointX = anArcEndPoint.x()
anArcPrevEndPointY = anArcEndPoint.y()
# Move one point of the arc
aSession.startOperation()
anArcStartPoint.setValue(0, 60)
aSession.finishOperation()
assert (anArcEndPoint.x() != anArcPrevEndPointX)
assert (anArcEndPoint.y() != anArcPrevEndPointY)
assert (math.fabs(distancePointPoint(anArcCentr, anArcStartPoint) - RADIUS) < 1.e-10)
assert (math.fabs(distancePointPoint(anArcCentr, anArcEndPoint) - RADIUS) < 1.e-10)
#=========================================================================
# 4. Move the centr or the point of the arc
# 5. Check radius is the same
#=========================================================================
assert (anCircleCentr.x() == -25)
assert (anCircleCentr.y() == -25)
assert (aCircleRadius.value() == 25)
aSession.startOperation()
anCircleCentr.setValue(100., 100.)
aSession.finishOperation()
assert (anCircleCentr.x() == 100)
assert (anCircleCentr.y() == 100)
assert (aCircleRadius.value() == 25)
#=========================================================================
# End of test
#=========================================================================
