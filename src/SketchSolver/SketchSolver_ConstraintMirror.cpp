#include <SketchSolver_ConstraintMirror.h>
#include <SketchSolver_Group.h>
#include <SketchSolver_Error.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_ResultConstruction.h>

#include <GeomAPI_Dir2d.h>
#include <GeomAPI_XY.h>

#include <cmath>

void SketchSolver_ConstraintMirror::getAttributes(
    Slvs_Entity& theMirrorLine,
    std::vector<Slvs_Entity>& theBaseEntities,
    std::vector<Slvs_Entity>& theMirrorEntities)
{
  DataPtr aData = myBaseConstraint->data();
  AttributeRefAttrPtr aMirLineAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
      myBaseConstraint->attribute(SketchPlugin_Constraint::ENTITY_A()));
  if (!aMirLineAttr || !aMirLineAttr->isInitialized() || !aMirLineAttr->isObject()) {
    myErrorMsg = SketchSolver_Error::NOT_INITIALIZED();
    return;
  }
  int aType = SLVS_E_UNKNOWN; // type of created entity
  Slvs_hEntity anEntity = myGroup->getAttributeId(aMirLineAttr);
  if (anEntity == SLVS_E_UNKNOWN)
    anEntity = changeEntity(aMirLineAttr, aType);
  theMirrorLine = myStorage->getEntity(anEntity);

  // Create SolveSpace entity for all features
  AttributeRefListPtr aBaseRefList = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
      myBaseConstraint->attribute(SketchPlugin_Constraint::ENTITY_B()));
  AttributeRefListPtr aMirroredRefList = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
      myBaseConstraint->attribute(SketchPlugin_Constraint::ENTITY_C()));
  myNumberOfObjects = aMirroredRefList->size();
  if (!aBaseRefList || !aMirroredRefList) {
    myErrorMsg = SketchSolver_Error::INCORRECT_MIRROR_ATTRIBUTE();
    return;
  }

  std::list<ObjectPtr> aBaseList = aBaseRefList->list();
  std::list<ObjectPtr> aMirroredList = aMirroredRefList->list();

  FeaturePtr aFeature;
  ResultConstructionPtr aRC;
  for (int i = 0; i < 2; i++) {
    std::list<ObjectPtr>::iterator anIter = i == 0 ? aBaseList.begin() : aMirroredList.begin();
    std::list<ObjectPtr>::iterator aEndIter = i == 0 ? aBaseList.end() : aMirroredList.end();
    std::vector<Slvs_Entity>* aList = i == 0 ? &theBaseEntities : & theMirrorEntities;
    for ( ; anIter != aEndIter; anIter++) {
      aRC = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(*anIter);
      aFeature = aRC ? aRC->document()->feature(aRC) :
        std::dynamic_pointer_cast<ModelAPI_Feature>(*anIter);
      if (!aFeature)
        continue;

      anEntity = changeEntity(aFeature, aType);
      // Sort entities by their type
      std::vector<Slvs_Entity>::iterator anIt = aList->begin();
      for (; anIt != aList->end(); anIt++)
        if (aType < anIt->type)
          break;
//      aList->push_back(myStorage->getEntity(anEntity));
      aList->insert(anIt, myStorage->getEntity(anEntity));
    }
  }

  if (theBaseEntities.size() > theMirrorEntities.size())
    myErrorMsg = SketchSolver_Error::NOT_INITIALIZED();
}

void SketchSolver_ConstraintMirror::process()
{
  cleanErrorMsg();
  if (!myBaseConstraint || !myStorage || myGroup == 0) {
    /// TODO: Put error message here
    return;
  }
  if (!mySlvsConstraints.empty()) // some data is changed, update constraint
    update(myBaseConstraint);

  Slvs_Entity aMirrorLine;
  std::vector<Slvs_Entity> aBaseList;
  std::vector<Slvs_Entity> aMirrorList;
  getAttributes(aMirrorLine, aBaseList, aMirrorList);
  if (!myErrorMsg.empty())
    return;

  if (aBaseList.size() != aMirrorList.size()) {
    myErrorMsg = SketchSolver_Error::INCORRECT_MIRROR_ATTRIBUTE();
    return;
  }

  Slvs_Constraint aConstraint;
  // Get coordinates of mirror line points for speed up
  double aStartEnd[4];
  for (int i = 0; i < 2; i++) {
    Slvs_Entity aPoint = myStorage->getEntity(aMirrorLine.point[i]);
    for (int j = 0; j < 2; j++)
      aStartEnd[2*i+j] = myStorage->getParameter(aPoint.param[j]).val;
  }

  std::vector<Slvs_Entity>::iterator aBaseIter = aBaseList.begin();
  std::vector<Slvs_Entity>::iterator aMirrorIter = aMirrorList.begin();
  for (; aBaseIter != aBaseList.end(); aBaseIter++, aMirrorIter++) {
    // Make aMirrorEnt parameters to be symmetric with aBaseEnt
    makeMirrorEntity(*aBaseIter, *aMirrorIter, aStartEnd);

    if (aBaseIter->type == SLVS_E_POINT_IN_2D) {
      aConstraint = Slvs_MakeConstraint(
          SLVS_E_UNKNOWN, myGroup->getId(), getType(), myGroup->getWorkplaneId(),
          0.0, aBaseIter->h, aMirrorIter->h, aMirrorLine.h, SLVS_E_UNKNOWN);
      aConstraint.h = myStorage->addConstraint(aConstraint);
      mySlvsConstraints.push_back(aConstraint.h);
    } else if (aBaseIter->type == SLVS_E_LINE_SEGMENT) {
      for (int i = 0; i < 2; i++) {
        aConstraint = Slvs_MakeConstraint(
            SLVS_E_UNKNOWN, myGroup->getId(), getType(), myGroup->getWorkplaneId(), 0.0,
            aBaseIter->point[i], aMirrorIter->point[i], aMirrorLine.h, SLVS_E_UNKNOWN);
        aConstraint.h = myStorage->addConstraint(aConstraint);
        mySlvsConstraints.push_back(aConstraint.h);
      }
    } else if (aBaseIter->type == SLVS_E_CIRCLE) {
      aConstraint = Slvs_MakeConstraint(
          SLVS_E_UNKNOWN, myGroup->getId(), getType(), myGroup->getWorkplaneId(), 0.0,
          aBaseIter->point[0], aMirrorIter->point[0], aMirrorLine.h, SLVS_E_UNKNOWN);
      aConstraint.h = myStorage->addConstraint(aConstraint);
      mySlvsConstraints.push_back(aConstraint.h);
      // Additional constraint for equal radii
      Slvs_Constraint anEqRadConstr = Slvs_MakeConstraint(
          SLVS_E_UNKNOWN, myGroup->getId(), SLVS_C_EQUAL_RADIUS, myGroup->getWorkplaneId(),
          0.0, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN, aBaseIter->h, aMirrorIter->h);
      anEqRadConstr.h = myStorage->addConstraint(anEqRadConstr);
      mySlvsConstraints.push_back(anEqRadConstr.h);
    } else if (aBaseIter->type == SLVS_E_ARC_OF_CIRCLE) {
      // Workaround to avoid problems in SolveSpace.
      // The symmetry of two arcs will be done using symmetry of three points on these arcs:
      // start point, end point, and any other point on the arc
      Slvs_hEntity aBaseArcPoints[3] = {
          aBaseIter->point[1],
          aBaseIter->point[2],
          SLVS_E_UNKNOWN};
      Slvs_hEntity aMirrorArcPoints[3] = { // indices of points of arc, center corresponds center, first point corresponds last point
          aMirrorIter->point[2],
          aMirrorIter->point[1],
          SLVS_E_UNKNOWN};

      Slvs_Entity aBothArcs[2] = {*aBaseIter, *aMirrorIter};
      Slvs_hEntity aBothMiddlePoints[2];
      for (int i = 0; i < 2; i++) {
        double x, y;
        calculateMiddlePoint(aBothArcs[i], 0.5, x, y);
        Slvs_Param aParamX = Slvs_MakeParam(SLVS_E_UNKNOWN, myGroup->getId(), x);
        Slvs_Param aParamY = Slvs_MakeParam(SLVS_E_UNKNOWN, myGroup->getId(), y);
        aParamX.h = myStorage->addParameter(aParamX);
        aParamY.h = myStorage->addParameter(aParamY);
        Slvs_Entity aPoint = Slvs_MakePoint2d(SLVS_E_UNKNOWN, myGroup->getId(),
            myGroup->getWorkplaneId(), aParamX.h, aParamY.h);
        aBothMiddlePoints[i] = myStorage->addEntity(aPoint);
        // additional constraint point-on-curve
        Slvs_Constraint aPonCircConstr = Slvs_MakeConstraint(
            SLVS_E_UNKNOWN, myGroup->getId(), SLVS_C_PT_ON_CIRCLE, myGroup->getWorkplaneId(),
            0.0, aBothMiddlePoints[i], SLVS_E_UNKNOWN, aBothArcs[i].h, SLVS_E_UNKNOWN);
        aPonCircConstr.h = myStorage->addConstraint(aPonCircConstr);
        mySlvsConstraints.push_back(aPonCircConstr.h);
        if (i == 0) {
          // additional constraint for the point to be in the middle of a base arc
          Slvs_Entity aLine1 = Slvs_MakeLineSegment(SLVS_E_UNKNOWN, myGroup->getId(),
              myGroup->getWorkplaneId(), aBothArcs[i].point[1], aBothMiddlePoints[i]);
          aLine1.h = myStorage->addEntity(aLine1);
          Slvs_Entity aLine2 = Slvs_MakeLineSegment(SLVS_E_UNKNOWN, myGroup->getId(),
              myGroup->getWorkplaneId(), aBothArcs[i].point[2], aBothMiddlePoints[i]);
          aLine2.h = myStorage->addEntity(aLine2);
          Slvs_Constraint aMiddleConstr = Slvs_MakeConstraint(SLVS_E_UNKNOWN, myGroup->getId(),
              SLVS_C_EQUAL_LENGTH_LINES, myGroup->getWorkplaneId(),
              0.0, SLVS_E_UNKNOWN, SLVS_E_UNKNOWN, aLine1.h, aLine2.h);
          aMiddleConstr.h = myStorage->addConstraint(aMiddleConstr);
          mySlvsConstraints.push_back(aMiddleConstr.h);
        }
      }

      aBaseArcPoints[2] = aBothMiddlePoints[0];
      aMirrorArcPoints[2] = aBothMiddlePoints[1];
      for (int ind = 0; ind < 3; ind++) {
        Slvs_Constraint aConstraint = Slvs_MakeConstraint(
            SLVS_E_UNKNOWN, myGroup->getId(), getType(), myGroup->getWorkplaneId(), 0.0,
            aBaseArcPoints[ind], aMirrorArcPoints[ind], aMirrorLine.h, SLVS_E_UNKNOWN);
        aConstraint.h = myStorage->addConstraint(aConstraint);
        mySlvsConstraints.push_back(aConstraint.h);
      }
    }
  }
}


void SketchSolver_ConstraintMirror::update(ConstraintPtr theConstraint)
{
  cleanErrorMsg();
  if (!theConstraint || theConstraint == myBaseConstraint) {
    AttributeRefListPtr aMirroredRefList = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(
        myBaseConstraint->attribute(SketchPlugin_Constraint::ENTITY_C()));
    if (aMirroredRefList->size() != myNumberOfObjects) {
      remove(myBaseConstraint);
      process();
      return;
    }
  }
  SketchSolver_Constraint::update();
}

bool SketchSolver_ConstraintMirror::remove(ConstraintPtr theConstraint)
{
  cleanErrorMsg();
  if (theConstraint && theConstraint != myBaseConstraint)
    return false;
  bool isFullyRemoved = true;
  std::vector<Slvs_hEntity>::iterator aCIter = mySlvsConstraints.begin();
  for (; aCIter != mySlvsConstraints.end(); aCIter++)
   isFullyRemoved = myStorage->removeConstraint(*aCIter) && isFullyRemoved;
  mySlvsConstraints.clear();

  std::map<FeaturePtr, Slvs_hEntity>::iterator aFeatIt = myFeatureMap.begin();
  for (; aFeatIt != myFeatureMap.end(); aFeatIt++)
    myStorage->removeEntity(aFeatIt->second);

  if (isFullyRemoved) {
    myFeatureMap.clear();
    myAttributeMap.clear();
    myValueMap.clear();
  } else
    cleanRemovedEntities();
  return true;
}

bool SketchSolver_ConstraintMirror::checkAttributesChanged(ConstraintPtr theConstraint)
{
  // First of all, check the mirror line is changed.
  // It may be changed to one of mirrored lines, which is already in this constraint
  // (this case is not marked as attribute changing)
  ConstraintPtr aConstraint = theConstraint ? theConstraint : myBaseConstraint;
  AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
      aConstraint->attribute(SketchPlugin_Constraint::ENTITY_A()));
  if (!aRefAttr || !aRefAttr->isObject() || !aRefAttr->object())
    return true;
  FeaturePtr aMirrorLine = ModelAPI_Feature::feature(aRefAttr->object());
  if (!aMirrorLine)
    return true;

  std::map<FeaturePtr, Slvs_hEntity>::iterator aMirrorIter = myFeatureMap.find(aMirrorLine);
  if (aMirrorIter == myFeatureMap.end())
    return true;

  // Check the entity is not used as mirror line
  std::vector<Slvs_hConstraint>::iterator aCIter = mySlvsConstraints.begin();
  for (; aCIter != mySlvsConstraints.end(); aCIter++) {
    Slvs_Constraint aMirrorConstr = myStorage->getConstraint(*aCIter);
    if (aMirrorConstr.type != SLVS_C_SYMMETRIC_LINE)
      continue;
    if (aMirrorConstr.entityA != aMirrorIter->second)
      return true;
    else break; // check just one symmetric constraint
  }

  // Base verification
  return SketchSolver_Constraint::checkAttributesChanged(theConstraint);
}

void SketchSolver_ConstraintMirror::makeMirrorEntity(
    const Slvs_Entity& theBase,
    const Slvs_Entity& theMirror,
    const double theMirrorLine[]) const
{
  Slvs_hEntity aBasePoint[4];
  Slvs_hEntity aMirrorPoint[4];
  for (int i = 0; i < 4; i++) {
    aBasePoint[i] = theBase.point[i];
    aMirrorPoint[i] = theMirror.point[i];
  }
  if (theBase.type == SLVS_E_ARC_OF_CIRCLE) {
    Slvs_hEntity aTmp = aMirrorPoint[2];
    aMirrorPoint[2] = aMirrorPoint[1];
    aMirrorPoint[1] = aTmp;
    adjustArcPoints(theBase);
  }
  if (theBase.type == SLVS_E_POINT_IN_2D || theBase.type == SLVS_E_POINT_IN_3D) {
    aBasePoint[0] = theBase.h;
    aMirrorPoint[0] = theMirror.h;
  }

  // Mirror line parameters
  std::shared_ptr<GeomAPI_XY> aLinePoints[2];
  for (int i = 0; i < 2; i++)
    aLinePoints[i] = std::shared_ptr<GeomAPI_XY>(
        new GeomAPI_XY(theMirrorLine[2*i], theMirrorLine[2*i+1]));
  // direction of a mirror line
  std::shared_ptr<GeomAPI_Dir2d> aDir = std::shared_ptr<GeomAPI_Dir2d>(
    new GeomAPI_Dir2d(aLinePoints[1]->added(aLinePoints[0]->multiplied(-1.0))));
  // orthogonal direction
  aDir = std::shared_ptr<GeomAPI_Dir2d>(new GeomAPI_Dir2d(aDir->y(), -aDir->x()));

  Slvs_hConstraint aFixed; // transient variable
  for (int i = 0; i < 4; i++) {
    if (aBasePoint[i] == SLVS_E_UNKNOWN || aMirrorPoint[i] == SLVS_E_UNKNOWN)
      continue;
    // check the mirror point is not fixed
    if (myStorage->isPointFixed(aMirrorPoint[i], aFixed, true))
      continue;

    Slvs_Entity aPointEnt = myStorage->getEntity(aBasePoint[i]);
    double aBaseX = myStorage->getParameter(aPointEnt.param[0]).val;
    double aBaseY = myStorage->getParameter(aPointEnt.param[1]).val;
    std::shared_ptr<GeomAPI_XY> aPoint = std::shared_ptr<GeomAPI_XY>(new GeomAPI_XY(aBaseX, aBaseY));

    std::shared_ptr<GeomAPI_XY> aVec = std::shared_ptr<GeomAPI_XY>(
        new GeomAPI_XY(aPoint->x() - aLinePoints[0]->x(), aPoint->y() - aLinePoints[0]->y()));
    double aDist = aVec->dot(aDir->xy());
    aPoint = aPoint->added(aDir->xy()->multiplied(-2.0 * aDist));

    Slvs_Entity aMirrorEnt = myStorage->getEntity(aMirrorPoint[i]);
    Slvs_Param aParam = Slvs_MakeParam(aMirrorEnt.param[0], myGroup->getId(), aPoint->x());
    myStorage->updateParameter(aParam);
    aParam = Slvs_MakeParam(aMirrorEnt.param[1], myGroup->getId(), aPoint->y());
    myStorage->updateParameter(aParam);
  }
}

void SketchSolver_ConstraintMirror::adjustArcPoints(const Slvs_Entity& theArc) const
{
  Slvs_Param aParam;
  Slvs_Entity aPoint;
  double anArcParams[3][2];
  for (int i = 0; i < 3; i++) {
    aPoint = myStorage->getEntity(theArc.point[i]);
    for (int j = 0; j < 2; j++) {
      aParam = myStorage->getParameter(aPoint.param[j]);
      anArcParams[i][j] = aParam.val;
      if (i > 0)
        anArcParams[i][j] -= anArcParams[0][j];
    }
  }
  double aRad2 = anArcParams[1][0] * anArcParams[1][0] + anArcParams[1][1] * anArcParams[1][1];
  double aDist2 = anArcParams[2][0] * anArcParams[2][0] + anArcParams[2][1] * anArcParams[2][1];
  if (std::fabs(aRad2 - aDist2) < tolerance)
    return; // nothing to update (last point already on the arc)
  if (aDist2 < tolerance)
    return; // unable to update
  double aCoeff = std::sqrt(aRad2 / aDist2);
  anArcParams[2][0] *= aCoeff;
  anArcParams[2][1] *= aCoeff;

  // Update last point
  aPoint = myStorage->getEntity(theArc.point[2]);
  for (int i = 0; i < 2; i++) {
    aParam = Slvs_MakeParam(aPoint.param[i], myGroup->getId(),
        anArcParams[0][i] + anArcParams[2][i]);
    myStorage->updateParameter(aParam);
  }
}

void SketchSolver_ConstraintMirror::adjustConstraint()
{
  AttributeRefAttrPtr aMirLineAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
      myBaseConstraint->attribute(SketchPlugin_Constraint::ENTITY_A()));
  if (!aMirLineAttr || !aMirLineAttr->isInitialized() || !aMirLineAttr->isObject()) {
    myErrorMsg = SketchSolver_Error::NOT_INITIALIZED();
    return;
  }
  ResultConstructionPtr aRC =
      std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aMirLineAttr->object());
  FeaturePtr aFeature = aRC ? aRC->document()->feature(aRC) :
    std::dynamic_pointer_cast<ModelAPI_Feature>(aMirLineAttr->object());
  std::map<FeaturePtr, Slvs_hEntity>::iterator aMirLineIter = myFeatureMap.find(aFeature);
  if (aMirLineIter == myFeatureMap.end())
    return;
  Slvs_Entity aMirrorLine = myStorage->getEntity(aMirLineIter->second);

  Slvs_Constraint aMirror;
  double aStartEnd[4];
  for (int i = 0; i < 2; i++) {
    Slvs_Entity aPoint = myStorage->getEntity(aMirrorLine.point[i]);
    for (int j = 0; j < 2; j++)
      aStartEnd[2*i+j] = myStorage->getParameter(aPoint.param[j]).val;
  }

  // Search mirror between middle points on the arcs and recompute their coordinates
  std::map<Slvs_hEntity, Slvs_hEntity> aPointsOnCircles;
  std::list<Slvs_Constraint> aMirrorPonCirc;
  std::list<Slvs_Constraint> aPonCirc = myStorage->getConstraintsByType(SLVS_C_PT_ON_CIRCLE);
  std::vector<Slvs_hConstraint>::iterator aConstrIter = mySlvsConstraints.begin();
  for (; aConstrIter != mySlvsConstraints.end(); aConstrIter++) {
    aMirror = myStorage->getConstraint(*aConstrIter);
    if (aMirror.type != SLVS_C_SYMMETRIC_LINE)
      continue;
    if (aMirror.entityA != aMirrorLine.h)
      continue; // don't update another Mirror constraints
    Slvs_Constraint aPonCircA, aPonCircB;
    aPonCircA.h = SLVS_E_UNKNOWN;
    aPonCircB.h = SLVS_E_UNKNOWN;
    std::list<Slvs_Constraint>::iterator aPtIter = aPonCirc.begin();
    for (; aPtIter != aPonCirc.end(); aPtIter++) {
      if (aMirror.ptA == aPtIter->ptA)
        aPonCircA = *aPtIter;
      if (aMirror.ptB == aPtIter->ptA)
        aPonCircB = *aPtIter;
    }
    if (aPonCircA.h == SLVS_E_UNKNOWN || aPonCircB.h == SLVS_E_UNKNOWN)
      continue;
    aMirrorPonCirc.push_back(aMirror);
    // Store point IDs to avoid their recalculation twice
    aPointsOnCircles[aPonCircA.ptA] = aPonCircA.entityA;
    aPointsOnCircles[aPonCircB.ptA] = aPonCircB.entityA;
  }

  // Recalculate positions of mirroring points
  std::list<Slvs_Constraint> aMirrorList = myStorage->getConstraintsByType(SLVS_C_SYMMETRIC_LINE);
  std::list<Slvs_Constraint>::iterator aMirIter = aMirrorList.begin();
  for (; aMirIter != aMirrorList.end(); aMirIter++) {
    if (aMirIter->entityA != aMirrorLine.h)
      continue; // don't update another Mirror constraints
    if (aPointsOnCircles.find(aMirIter->ptA) != aPointsOnCircles.end())
      continue; // Avoid mirroring points on circles
    Slvs_Entity aBase = myStorage->getEntity(aMirIter->ptA);
    Slvs_Entity aMirror = myStorage->getEntity(aMirIter->ptB);
    makeMirrorEntity(aBase, aMirror, aStartEnd);
  }

  bool aNeedToResolve = myStorage->isNeedToResolve();
  for (aMirIter = aMirrorPonCirc.begin(); aMirIter != aMirrorPonCirc.end(); aMirIter++) {
    // Calculate middle point for base arc and mirrored point on mirror arc
    Slvs_Entity aBaseArc = myStorage->getEntity(aPointsOnCircles[aMirIter->ptA]);
    Slvs_Entity aBasePoint = myStorage->getEntity(aMirIter->ptA);
    Slvs_Param aParamX = myStorage->getParameter(aBasePoint.param[0]);
    Slvs_Param aParamY = myStorage->getParameter(aBasePoint.param[1]);
    calculateMiddlePoint(aBaseArc, 0.5, aParamX.val, aParamY.val);
    myStorage->updateParameter(aParamX);
    myStorage->updateParameter(aParamY);
    Slvs_Entity aMirrorArc = myStorage->getEntity(aPointsOnCircles[aMirIter->ptB]);
    Slvs_Entity aMirrorPoint = myStorage->getEntity(aMirIter->ptB);
    aParamX = myStorage->getParameter(aMirrorPoint.param[0]);
    aParamY = myStorage->getParameter(aMirrorPoint.param[1]);
    calculateMiddlePoint(aMirrorArc, 0.5, aParamX.val, aParamY.val);
    myStorage->updateParameter(aParamX);
    myStorage->updateParameter(aParamY);
    // make centers of arcs symmetric
    aBasePoint = myStorage->getEntity(aBaseArc.point[0]);
    aMirrorPoint = myStorage->getEntity(aMirrorArc.point[0]);
    makeMirrorEntity(aBasePoint, aMirrorPoint, aStartEnd);
  }
  // Restore previous value to avoid looped recalculations of sketch
  myStorage->setNeedToResolve(aNeedToResolve);
}
