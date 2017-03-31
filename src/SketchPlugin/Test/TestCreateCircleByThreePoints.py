"""
    TestCreateCircleByThreePoints.py

    Test creation methods of a circle built by three points
"""

#=========================================================================
# Initialization of the test
#=========================================================================
from GeomDataAPI import *
from GeomAPI import *
from ModelAPI import *
from SketchAPI import SketchAPI_Sketch
from salome.shaper import model
import math

__updated__ = "2017-03-22"


#=========================================================================
# Auxiliary functions
#=========================================================================
TOLERANCE = 1.e-7

def verifyLastCircle(theSketch, theX, theY, theR):
    """
    subroutine to verify position of last circle in the sketch
    """
    aLastCircle = model.lastSubFeature(theSketch, "SketchCircle")
    aCenter = geomDataAPI_Point2D(aLastCircle.attribute("circle_center"))
    verifyPointCoordinates(aCenter, theX, theY)
    aRadius = aLastCircle.real("circle_radius")
    assert aRadius.value() == theR, "Wrong radius {0}, expected {1}".format(aRadius.value(), theR)

def verifyPointCoordinates(thePoint, theX, theY):
    assert thePoint.x() == theX and thePoint.y() == theY, "Wrong '{0}' point ({1}, {2}), expected ({3}, {4})".format(thePoint.attributeType(), thePoint.x(), thePoint.y(), theX, theY)

def verifyPointOnCircle(thePoint, theCircle):
    aCircleCenter = geomDataAPI_Point2D(theCircle.attribute("circle_center"))
    aDistCP = model.distancePointPoint(aCircleCenter, thePoint)
    aCircleRadius = theCircle.real("circle_radius").value()
    assert math.fabs(aDistCP - aCircleRadius) < TOLERANCE, "Point is not on circle, distance: {0}, radius of circle: {1}".format(aDistCP, aCircleRadius)

def verifyTangentCircles(theCircle1, theCircle2):
    aCenter1 = geomDataAPI_Point2D(theCircle1.attribute("circle_center"))
    aCenter2 = geomDataAPI_Point2D(theCircle2.attribute("circle_center"))
    aDistCC = model.distancePointPoint(aCenter1, aCenter2)
    aRadius1 = theCircle1.real("circle_radius").value()
    aRadius2 = theCircle2.real("circle_radius").value()
    verifyTangentCircular(aDistCC, aRadius1, aRadius2)

def verifyTangentCircleArc(theCircle, theArc):
    aCircleCenter = geomDataAPI_Point2D(theCircle.attribute("circle_center"))
    anArcCenter = geomDataAPI_Point2D(theArc.attribute("center_point"))
    anArcStart = geomDataAPI_Point2D(theArc.attribute("start_point"))
    aDistCC = model.distancePointPoint(aCircleCenter, anArcCenter)
    aCircleRadius = theCircle.real("circle_radius").value()
    anArcRadius = model.distancePointPoint(anArcCenter, anArcStart)
    verifyTangentCircular(aDistCC, aCircleRadius, anArcRadius)

def verifyTangentCircular(theDistBetweenCenters, theRadius1, theRadius2):
    aRSum = theRadius1 + theRadius2
    aRDiff = math.fabs(theRadius1 - theRadius2)
    assert math.fabs(aRSum - theDistBetweenCenters) < TOLERANCE or math.fabs(aRDiff - theDistBetweenCenters) < TOLERANCE, "Two circulars are not tangent"

def verifyTangentCircleLine(theCircle, theLine):
    aCenter = geomDataAPI_Point2D(theCircle.attribute("circle_center"))
    aRadius = theCircle.real("circle_radius").value()
    aDistCL = distancePointLine(aCenter, theLine)
    assert math.fabs(aDistCL - aRadius) < TOLERANCE, "Circle and line are not tangent"

def distancePointLine(thePoint, theLine):
    aLineStart = geomDataAPI_Point2D(theLine.attribute("StartPoint")).pnt().xy()
    aLineEnd = geomDataAPI_Point2D(theLine.attribute("EndPoint")).pnt().xy()
    aLineDir = aLineEnd.decreased(aLineStart)
    aLineLen = aLineEnd.distance(aLineStart)
    aPntDir = thePoint.pnt().xy().decreased(aLineStart)
    return math.fabs(aPntDir.cross(aLineDir) / aLineLen)


#=========================================================================
# Start of test
#=========================================================================

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
aSketch = SketchAPI_Sketch(aSketchFeature)

#=========================================================================
# Test 1. Create a circle by three points
#=========================================================================
expectedCenter = [0., 0.]
expectedRadius = 10.
aSession.startOperation()
aCircle = aSketchFeature.addFeature("SketchMacroCircle")
assert (aCircle.getKind() == "SketchMacroCircle")
aCirclePnt1 = geomDataAPI_Point2D(aCircle.attribute("first_point"))
aCirclePnt2 = geomDataAPI_Point2D(aCircle.attribute("second_point"))
aCirclePnt3 = geomDataAPI_Point2D(aCircle.attribute("third_point"))
assert (not aCirclePnt1.isInitialized())
assert (not aCirclePnt2.isInitialized())
assert (not aCirclePnt3.isInitialized())
aCircleType = aCircle.string("circle_type")
assert (not aCircleType.isInitialized())
aCircleType.setValue("circle_type_by_three_points")
aCirclePnt1.setValue(expectedCenter[0] - expectedRadius, expectedCenter[1])
aCirclePnt2.setValue(expectedCenter[0] + expectedRadius, expectedCenter[1])
aCirclePnt3.setValue(expectedCenter[0], expectedCenter[1] + expectedRadius)
aSession.finishOperation()
assert (aSketchFeature.numberOfSubs() == 1)
verifyLastCircle(aSketchFeature, expectedCenter[0], expectedCenter[1], expectedRadius)

#=========================================================================
# Test 2. Create a circle by three points coincident to other points
#=========================================================================
# get previous circle
aPrevCircle = model.lastSubFeature(aSketchFeature, "SketchCircle")
aPrevCenter = geomDataAPI_Point2D(aPrevCircle.attribute("circle_center"))
aPrevCenterXY = [aPrevCenter.x(), aPrevCenter.y()]
aPrevCircleRadius = aPrevCircle.real("circle_radius").value()
# create additional point and line
aPointCoodinates = [5., 20.]
aLineStart = [10., 0.]
aLineEnd = [10., 50.]
aSession.startOperation()
aPoint = aSketchFeature.addFeature("SketchPoint")
aPointCoord = geomDataAPI_Point2D(aPoint.attribute("PointCoordinates"))
aPointCoord.setValue(aPointCoodinates[0], aPointCoodinates[1])
aLine = aSketchFeature.addFeature("SketchLine")
aStartPnt = geomDataAPI_Point2D(aLine.attribute("StartPoint"))
aStartPnt.setValue(aLineStart[0], aLineStart[1])
aEndPnt = geomDataAPI_Point2D(aLine.attribute("EndPoint"))
aEndPnt.setValue(aLineEnd[0], aLineEnd[1])
aSession.finishOperation()
# create new circle
aSession.startOperation()
aCircle = aSketchFeature.addFeature("SketchMacroCircle")
aCirclePnt1 = geomDataAPI_Point2D(aCircle.attribute("first_point"))
aCirclePnt2 = geomDataAPI_Point2D(aCircle.attribute("second_point"))
aCirclePnt3 = geomDataAPI_Point2D(aCircle.attribute("third_point"))
aCirclePnt1Ref = aCircle.refattr("first_point_ref")
aCirclePnt2Ref = aCircle.refattr("second_point_ref")
aCirclePnt3Ref = aCircle.refattr("third_point_ref")
aCircleType = aCircle.string("circle_type")
# initialize attributes
aCircleType.setValue("circle_type_by_three_points")
aCirclePnt1Ref.setAttr(aPrevCenter)
aCirclePnt1.setValue(aPrevCenter.pnt())
aCirclePnt2Ref.setObject(aPoint.lastResult())
aCirclePnt2.setValue(aPointCoord.pnt())
aCirclePnt3Ref.setAttr(aStartPnt)
aCirclePnt3.setValue(aLineStart[0], aLineStart[1])
aSession.finishOperation()
assert (aSketchFeature.numberOfSubs() == 7)
# check the points do not change their positions
verifyPointCoordinates(aPrevCenter, aPrevCenterXY[0], aPrevCenterXY[1])
verifyPointCoordinates(aPointCoord, aPointCoodinates[0], aPointCoodinates[1])
verifyPointCoordinates(aStartPnt, aLineStart[0], aLineStart[1])
# check newly created circle passes through the points
aCircle = model.lastSubFeature(aSketchFeature, "SketchCircle")
verifyPointOnCircle(aPrevCenter, aCircle)
verifyPointOnCircle(aPointCoord, aCircle)
verifyPointOnCircle(aStartPnt, aCircle)
model.testNbSubFeatures(aSketch, "SketchConstraintCoincidence", 3)
model.testNbSubFeatures(aSketch, "SketchConstraintTangent", 0)

#=========================================================================
# Test 3. Create a circle by three points and tangent to line, circle and arc
#=========================================================================
# create additional arc
anArcRadius = 5.
anArcCenter = [-10., 10.]
anArcStart = [anArcCenter[0], anArcCenter[1] - anArcRadius]
anArcEnd = [anArcCenter[0], anArcCenter[1] + anArcRadius]
aSession.startOperation()
anArc = aSketchFeature.addFeature("SketchArc")
anArcCenterPnt = geomDataAPI_Point2D(anArc.attribute("center_point"))
anArcCenterPnt.setValue(anArcCenter[0], anArcCenter[1])
anArcStartPnt = geomDataAPI_Point2D(anArc.attribute("start_point"))
anArcStartPnt.setValue(anArcStart[0], anArcStart[1])
anArcEndPnt = geomDataAPI_Point2D(anArc.attribute("end_point"))
anArcEndPnt.setValue(anArcEnd[0], anArcEnd[1])
aSession.finishOperation()
# create new circle
aSession.startOperation()
aCircle = aSketchFeature.addFeature("SketchMacroCircle")
aCirclePnt1 = geomDataAPI_Point2D(aCircle.attribute("first_point"))
aCirclePnt2 = geomDataAPI_Point2D(aCircle.attribute("second_point"))
aCirclePnt3 = geomDataAPI_Point2D(aCircle.attribute("third_point"))
aCirclePnt1Ref = aCircle.refattr("first_point_ref")
aCirclePnt2Ref = aCircle.refattr("second_point_ref")
aCirclePnt3Ref = aCircle.refattr("third_point_ref")
aCircleType = aCircle.string("circle_type")
# initialize attributes
aCircleType.setValue("circle_type_by_three_points")
aCirclePnt1Ref.setObject(aPrevCircle.lastResult())
aCirclePnt1.setValue(aPrevCenter.x(), aPrevCenter.y() + aPrevCircleRadius)
aCirclePnt2Ref.setObject(aLine.lastResult())
aCirclePnt2.setValue(aLineEnd[0], aLineEnd[1])
aCirclePnt3Ref.setObject(anArc.lastResult())
aCirclePnt3.setValue(anArcCenter[0] + anArcRadius, anArcCenter[1])
aSession.finishOperation()
assert (aSketchFeature.numberOfSubs() == 12)
# check the tangent entities do not change their positions
verifyPointCoordinates(aPrevCenter, aPrevCenterXY[0], aPrevCenterXY[1])
assert (aPrevCircle.real("circle_radius").value() == aPrevCircleRadius)
verifyPointCoordinates(aStartPnt, aLineStart[0], aLineStart[1])
verifyPointCoordinates(aEndPnt, aLineEnd[0], aLineEnd[1])
verifyPointCoordinates(anArcCenterPnt, anArcCenter[0], anArcCenter[1])
verifyPointCoordinates(anArcStartPnt, anArcStart[0], anArcStart[1])
verifyPointCoordinates(anArcEndPnt, anArcEnd[0], anArcEnd[1])
# check newly created circle passes through the points
aCircle = model.lastSubFeature(aSketchFeature, "SketchCircle")
verifyTangentCircles(aCircle, aPrevCircle)
verifyTangentCircleArc(aCircle, anArc)
verifyTangentCircleLine(aCircle, aLine)
model.testNbSubFeatures(aSketch, "SketchConstraintCoincidence", 3)
model.testNbSubFeatures(aSketch, "SketchConstraintTangent", 3)

#=========================================================================
# Test 4. Create a circle by three points:
#         a. first two points are coincident to extremities of the line
#         b. check that this line is not selectable by third point
#=========================================================================
aSession.startOperation()
aCircle = aSketchFeature.addFeature("SketchMacroCircle")
aCirclePnt1 = geomDataAPI_Point2D(aCircle.attribute("first_point"))
aCirclePnt2 = geomDataAPI_Point2D(aCircle.attribute("second_point"))
aCirclePnt3 = geomDataAPI_Point2D(aCircle.attribute("third_point"))
aCirclePnt1Ref = aCircle.refattr("first_point_ref")
aCirclePnt2Ref = aCircle.refattr("second_point_ref")
aCirclePnt3Ref = aCircle.refattr("third_point_ref")
aCircleType = aCircle.string("circle_type")
# initialize attributes
aCircleType.setValue("circle_type_by_three_points")
aCirclePnt1Ref.setAttr(aStartPnt)
aCirclePnt1.setValue(aStartPnt.pnt())
aCirclePnt2Ref.setAttr(aEndPnt)
aCirclePnt2.setValue(aEndPnt.pnt())
aCirclePnt3Ref.setObject(aLine.lastResult())
aCirclePnt3.setValue(aLineEnd[0], aLineEnd[1])
aSession.finishOperation()
aLastFeature = aSketchFeature.subFeature(aSketchFeature.numberOfSubs() - 1)
assert aLastFeature.getKind() == "SketchMacroCircle", "ERROR: SketchMacroCircle has NOT expected to be valid"
aDocument.removeFeature(aCircle)
assert (aSketchFeature.numberOfSubs() == 12)

#=========================================================================
# Test 5. Create a circle by three points:
#         a. first two points are placed on both sides from line
#         b. check that this line is not selectable by third point
#=========================================================================
aDistanceFromLine = 20.
aSession.startOperation()
aCircle = aSketchFeature.addFeature("SketchMacroCircle")
aCirclePnt1 = geomDataAPI_Point2D(aCircle.attribute("first_point"))
aCirclePnt2 = geomDataAPI_Point2D(aCircle.attribute("second_point"))
aCirclePnt3 = geomDataAPI_Point2D(aCircle.attribute("third_point"))
aCirclePnt1Ref = aCircle.refattr("first_point_ref")
aCirclePnt2Ref = aCircle.refattr("second_point_ref")
aCirclePnt3Ref = aCircle.refattr("third_point_ref")
aCircleType = aCircle.string("circle_type")
# initialize attributes
aCircleType.setValue("circle_type_by_three_points")
aCirclePnt1.setValue(aLineStart[0] + aDistanceFromLine, aLineStart[1])
aCirclePnt2.setValue(aLineStart[0] - aDistanceFromLine, aLineStart[1])
aCirclePnt3Ref.setObject(aLine.lastResult())
aCirclePnt3.setValue(aLineEnd[0], aLineEnd[1])
aSession.finishOperation()
aLastFeature = aSketchFeature.subFeature(aSketchFeature.numberOfSubs() - 1)
assert aLastFeature.getKind() == "SketchMacroCircle", "ERROR: SketchMacroCircle has NOT expected to be valid"
aDocument.removeFeature(aCircle)
assert (aSketchFeature.numberOfSubs() == 12)

#=========================================================================
# End of test
#=========================================================================

from salome.shaper import model
assert(model.checkPythonDump())
