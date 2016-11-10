// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    PlaneGCSSolver_Builder.cpp
// Created: 14 Dec 2015
// Author:  Artem ZHIDKOV

#include <PlaneGCSSolver_Builder.h>
#include <PlaneGCSSolver_Solver.h>
#include <PlaneGCSSolver_Storage.h>
#include <PlaneGCSSolver_ParameterWrapper.h>
#include <PlaneGCSSolver_AngleWrapper.h>
#include <PlaneGCSSolver_EntityWrapper.h>
#include <PlaneGCSSolver_PointWrapper.h>
#include <PlaneGCSSolver_ScalarWrapper.h>
#include <PlaneGCSSolver_ConstraintWrapper.h>

#include <SketchSolver_Manager.h>

#include <GeomAPI_Dir2d.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_XY.h>
#include <GeomDataAPI_Dir.h>
#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Point2D.h>
#include <ModelAPI_Attribute.h>
#include <ModelAPI_AttributeRefAttr.h>

#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_IntersectionPoint.h>
#include <SketchPlugin_ConstraintAngle.h>

#include <math.h>


#define GCS_ENTITY_WRAPPER(x) std::dynamic_pointer_cast<PlaneGCSSolver_EntityWrapper>(x)
#define GCS_POINT_WRAPPER(x) std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(x)
#define GCS_PARAMETER_WRAPPER(x) std::dynamic_pointer_cast<PlaneGCSSolver_ParameterWrapper>(x)


/// \brief Converts a value to SolveSpace parameter
/// \param theGroup [in]  group to store parameter
/// \param theValue [in]  value of parameter
/// \param theExpr  [in]  shows the parameter is given by expression
/// \return Created parameter's wrapper
static ParameterWrapperPtr createParameter(const GroupID& theGroup,
                                           const double theValue = 0.0,
                                           const bool theExpr = false);

static ParameterWrapperPtr createParamAngle(const GroupID& theGroup,
                                            const double& theValue = 0.0);

static std::shared_ptr<PlaneGCSSolver_ScalarWrapper>
    createScalar(const GroupID& theGroupID,
                 AttributeDoublePtr theDoubleAttr = AttributeDoublePtr());

static EntityWrapperPtr createLine(FeaturePtr theFeature,
                                   const std::list<EntityWrapperPtr>& theAttributes,
                                   const GroupID& theGroupID);
static EntityWrapperPtr createCircle(FeaturePtr theFeature,
                                     const std::list<EntityWrapperPtr>& theAttributes,
                                     const GroupID& theGroupID);
static EntityWrapperPtr createArc(FeaturePtr theFeature,
                                  const std::list<EntityWrapperPtr>& theAttributes,
                                  const GroupID& theGroupID);


static ConstraintWrapperPtr
  createConstraintCoincidence(ConstraintPtr theConstraint,
                              const GroupID& theGroupID,
                              std::shared_ptr<PlaneGCSSolver_PointWrapper> thePoint1,
                              std::shared_ptr<PlaneGCSSolver_PointWrapper> thePoint2);
static ConstraintWrapperPtr
  createConstraintPointOnEntity(ConstraintPtr theConstraint,
                                const GroupID& theGroupID,
                                const SketchSolver_ConstraintType& theType,
                                std::shared_ptr<PlaneGCSSolver_PointWrapper> thePoint,
                                std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity);
static ConstraintWrapperPtr
  createConstraintDistancePointPoint(ConstraintPtr theConstraint,
                                     const GroupID& theGroupID,
                                     std::shared_ptr<PlaneGCSSolver_ParameterWrapper> theValue,
                                     std::shared_ptr<PlaneGCSSolver_PointWrapper> thePoint1,
                                     std::shared_ptr<PlaneGCSSolver_PointWrapper> thePoint2);
static ConstraintWrapperPtr
  createConstraintDistancePointLine(ConstraintPtr theConstraint,
                                    const GroupID& theGroupID,
                                    std::shared_ptr<PlaneGCSSolver_ParameterWrapper> theValue,
                                    std::shared_ptr<PlaneGCSSolver_PointWrapper> thePoint,
                                    std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity);
static ConstraintWrapperPtr
  createConstraintRadius(ConstraintPtr theConstraint,
                         const GroupID& theGroupID,
                         std::shared_ptr<PlaneGCSSolver_ParameterWrapper> theValue,
                         std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity);
static ConstraintWrapperPtr
  createConstraintAngle(ConstraintPtr theConstraint,
                        const GroupID& theGroupID,
                        std::shared_ptr<PlaneGCSSolver_ParameterWrapper> theValue,
                        std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity1,
                        std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity2);
static ConstraintWrapperPtr
  createConstraintHorizVert(ConstraintPtr theConstraint,
                            const GroupID& theGroupID,
                            const SketchSolver_ConstraintType& theType,
                            std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity);
static ConstraintWrapperPtr
  createConstraintParallel(ConstraintPtr theConstraint,
                           const GroupID& theGroupID,
                           std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity1,
                           std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity2);
static ConstraintWrapperPtr
  createConstraintPerpendicular(ConstraintPtr theConstraint,
                                const GroupID& theGroupID,
                                std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity1,
                                std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity2);
static ConstraintWrapperPtr
  createConstraintEqual(ConstraintPtr theConstraint,
                        const GroupID& theGroupID,
                        const SketchSolver_ConstraintType& theType,
                        std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity1,
                        std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity2,
                        std::shared_ptr<PlaneGCSSolver_ParameterWrapper> theIntermed);
static ConstraintWrapperPtr
  createConstraintTangent(ConstraintPtr theConstraint,
                          const GroupID& theGroupID,
                          const SketchSolver_ConstraintType& theType,
                          std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity1,
                          std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity2);
static ConstraintWrapperPtr
  createConstraintCollinear(ConstraintPtr theConstraint,
                           const GroupID& theGroupID,
                           std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity1,
                           std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity2);
static ConstraintWrapperPtr
  createConstraintMiddlePoint(ConstraintPtr theConstraint,
                              const GroupID& theGroupID,
                              std::shared_ptr<PlaneGCSSolver_PointWrapper> thePoint,
                              std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity);



/// \brief Update mirror points
static void adjustMirror(ConstraintWrapperPtr theConstraint);
/// \brief Update a sign of the point-line distance constraint
static void adjustPtLineDistance(ConstraintWrapperPtr theConstraint);

/// \brief Transform points to be symmetric regarding to the mirror line
static void makeMirrorPoints(EntityWrapperPtr theOriginal,
                             EntityWrapperPtr theMirrored,
                             EntityWrapperPtr theMirrorLine);



// Initialization of constraint builder self pointer
BuilderPtr PlaneGCSSolver_Builder::mySelf = PlaneGCSSolver_Builder::getInstance();

BuilderPtr PlaneGCSSolver_Builder::getInstance()
{
  if (!mySelf) {
    mySelf = BuilderPtr(new PlaneGCSSolver_Builder);
    SketchSolver_Manager::instance()->setBuilder(mySelf);
  }
  return mySelf;
}

StoragePtr PlaneGCSSolver_Builder::createStorage(const GroupID& theGroup) const
{
  return StoragePtr(new PlaneGCSSolver_Storage(theGroup));
}

SolverPtr PlaneGCSSolver_Builder::createSolver() const
{
  return SolverPtr(new PlaneGCSSolver_Solver);
}


std::list<ConstraintWrapperPtr> PlaneGCSSolver_Builder::createConstraint(
    ConstraintPtr theConstraint,
    const GroupID& theGroupID,
    const EntityID& theSketchID,
    const SketchSolver_ConstraintType& theType,
    const double& theValue,
    const EntityWrapperPtr& thePoint1,
    const EntityWrapperPtr& thePoint2,
    const EntityWrapperPtr& theEntity1,
    const EntityWrapperPtr& theEntity2) const
{
  ConstraintWrapperPtr aResult;
  ParameterWrapperPtr anIntermediate;

  std::shared_ptr<PlaneGCSSolver_PointWrapper> aPoint1 = GCS_POINT_WRAPPER(thePoint1);
  if (!aPoint1 && thePoint1 && thePoint1->type() == ENTITY_POINT)
    aPoint1 = GCS_POINT_WRAPPER( GCS_ENTITY_WRAPPER(thePoint1)->subEntities().front() );
  std::shared_ptr<PlaneGCSSolver_PointWrapper> aPoint2 = GCS_POINT_WRAPPER(thePoint2);
  if (!aPoint2 && thePoint2 && thePoint2->type() == ENTITY_POINT)
    aPoint2 = GCS_POINT_WRAPPER( GCS_ENTITY_WRAPPER(thePoint2)->subEntities().front() );

  switch (theType) {
  case CONSTRAINT_PT_PT_COINCIDENT:
    aResult = createConstraintCoincidence(theConstraint, theGroupID, aPoint1, aPoint2);
    break;
  case CONSTRAINT_PT_ON_LINE:
  case CONSTRAINT_PT_ON_CIRCLE:
    aResult = createConstraintPointOnEntity(theConstraint, theGroupID, theType,
                  aPoint1, GCS_ENTITY_WRAPPER(theEntity1));
    break;
  case CONSTRAINT_MIDDLE_POINT:
    aResult = createConstraintMiddlePoint(theConstraint, theGroupID,
                  aPoint1, GCS_ENTITY_WRAPPER(theEntity1));
    break;
  case CONSTRAINT_PT_PT_DISTANCE:
    aResult = createConstraintDistancePointPoint(theConstraint, theGroupID,
                  GCS_PARAMETER_WRAPPER(createParameter(GID_OUTOFGROUP, theValue)),
                  aPoint1, aPoint2);
    break;
  case CONSTRAINT_PT_LINE_DISTANCE:
    aResult = createConstraintDistancePointLine(theConstraint, theGroupID,
                  GCS_PARAMETER_WRAPPER(createParameter(GID_OUTOFGROUP, theValue)),
                  aPoint1, GCS_ENTITY_WRAPPER(theEntity1));
    break;
  case CONSTRAINT_RADIUS:
    aResult = createConstraintRadius(theConstraint, theGroupID,
                  GCS_PARAMETER_WRAPPER(createParameter(GID_OUTOFGROUP, theValue)),
                  GCS_ENTITY_WRAPPER(theEntity1));
    break;
  case CONSTRAINT_ANGLE:
    aResult = createConstraintAngle(theConstraint, theGroupID,
                  GCS_PARAMETER_WRAPPER(createParamAngle(GID_OUTOFGROUP, theValue)),
                  GCS_ENTITY_WRAPPER(theEntity1), GCS_ENTITY_WRAPPER(theEntity2));
    break;
  case CONSTRAINT_FIXED:
    break;
  case CONSTRAINT_HORIZONTAL:
  case CONSTRAINT_VERTICAL:
    aResult = createConstraintHorizVert(theConstraint, theGroupID, theType,
                  GCS_ENTITY_WRAPPER(theEntity1));
    break;
  case CONSTRAINT_PARALLEL:
    aResult = createConstraintParallel(theConstraint, theGroupID,
                  GCS_ENTITY_WRAPPER(theEntity1), GCS_ENTITY_WRAPPER(theEntity2));
    break;
  case CONSTRAINT_PERPENDICULAR:
    aResult = createConstraintPerpendicular(theConstraint, theGroupID,
                  GCS_ENTITY_WRAPPER(theEntity1), GCS_ENTITY_WRAPPER(theEntity2));
    break;
  case CONSTRAINT_EQUAL_LINES:
    anIntermediate = createParameter(theGroupID);
  case CONSTRAINT_EQUAL_LINE_ARC:
  case CONSTRAINT_EQUAL_RADIUS:
    aResult = createConstraintEqual(theConstraint, theGroupID, theType,
                  GCS_ENTITY_WRAPPER(theEntity1), GCS_ENTITY_WRAPPER(theEntity2),
                  GCS_PARAMETER_WRAPPER(anIntermediate));
    break;
  case CONSTRAINT_TANGENT_ARC_LINE:
  case CONSTRAINT_TANGENT_CIRCLE_LINE:
  case CONSTRAINT_TANGENT_ARC_ARC:
    aResult = createConstraintTangent(theConstraint, theGroupID, theType,
                  GCS_ENTITY_WRAPPER(theEntity1), GCS_ENTITY_WRAPPER(theEntity2));
    break;
  case CONSTRAINT_COLLINEAR:
    aResult = createConstraintCollinear(theConstraint, theGroupID,
                  GCS_ENTITY_WRAPPER(theEntity1), GCS_ENTITY_WRAPPER(theEntity2));
    break;
  case CONSTRAINT_MULTI_TRANSLATION:
  case CONSTRAINT_MULTI_ROTATION:
    break;
  case CONSTRAINT_SYMMETRIC:
    return createMirror(theConstraint, theGroupID, theSketchID,
                        thePoint1, thePoint2, theEntity1);
  default:
    break;
  }

  if (!aResult)
    return std::list<ConstraintWrapperPtr>();
  adjustConstraint(aResult);
  return std::list<ConstraintWrapperPtr>(1, aResult);
}

std::list<ConstraintWrapperPtr> PlaneGCSSolver_Builder::createConstraint(
    ConstraintPtr theConstraint,
    const GroupID& theGroupID,
    const EntityID& theSketchID,
    const SketchSolver_ConstraintType& theType,
    const double& theValue,
    const bool theFullValue,
    const EntityWrapperPtr& thePoint1,
    const EntityWrapperPtr& thePoint2,
    const std::list<EntityWrapperPtr>& theTrsfEnt) const
{
  ParameterWrapperPtr anAngleParam;
  if (theType == CONSTRAINT_MULTI_ROTATION)
    anAngleParam = createParamAngle(theGroupID, theValue);
  else if (theType != CONSTRAINT_MULTI_TRANSLATION)
    return std::list<ConstraintWrapperPtr>();

  std::list<EntityWrapperPtr> aConstrAttrList = theTrsfEnt;
  if (thePoint2)
    aConstrAttrList.push_front(thePoint2);
  aConstrAttrList.push_front(thePoint1);

  std::shared_ptr<PlaneGCSSolver_ConstraintWrapper> aResult(
      new PlaneGCSSolver_ConstraintWrapper(theConstraint, std::list<GCSConstraintPtr>(), theType));
  aResult->setGroup(theGroupID);
  aResult->setEntities(aConstrAttrList);
  if (anAngleParam)
    aResult->setValueParameter(anAngleParam);
  aResult->setIsFullValue(theFullValue);
  return std::list<ConstraintWrapperPtr>(1, aResult);
}


std::list<ConstraintWrapperPtr> PlaneGCSSolver_Builder::createMirror(
    ConstraintPtr theConstraint,
    const GroupID& theGroupID,
    const EntityID& theSketchID,
    const EntityWrapperPtr& theEntity1,
    const EntityWrapperPtr& theEntity2,
    const EntityWrapperPtr& theMirrorLine) const
{
  std::list<ConstraintWrapperPtr> aResult;
  std::list<EntityWrapperPtr> aConstrAttrList;
  if (theEntity1->type() == ENTITY_POINT) {
    if (theEntity2->group() == theGroupID) // theEntity2 is not fixed
      makeMirrorPoints(theEntity1, theEntity2, theMirrorLine);

    std::shared_ptr<PlaneGCSSolver_PointWrapper> aPoint1 = GCS_POINT_WRAPPER(theEntity1);
    if (!aPoint1 && theEntity1->type() == ENTITY_POINT)
      aPoint1 = GCS_POINT_WRAPPER( GCS_ENTITY_WRAPPER(theEntity1)->subEntities().front() );
    std::shared_ptr<PlaneGCSSolver_PointWrapper> aPoint2 = GCS_POINT_WRAPPER(theEntity2);
    if (!aPoint2 && theEntity2->type() == ENTITY_POINT)
      aPoint2 = GCS_POINT_WRAPPER( GCS_ENTITY_WRAPPER(theEntity2)->subEntities().front() );

    std::shared_ptr<PlaneGCSSolver_EntityWrapper> aMirrorLine =
        std::dynamic_pointer_cast<PlaneGCSSolver_EntityWrapper>(theMirrorLine);
    std::shared_ptr<GCS::Line> aLine =
        std::dynamic_pointer_cast<GCS::Line>(aMirrorLine->entity());

    std::list<GCSConstraintPtr> aConstrList;
    aConstrList.push_back(GCSConstraintPtr(new GCS::ConstraintMidpointOnLine(
        *(aPoint1->point()), *(aPoint2->point()), aLine->p1, aLine->p2)));
    aConstrList.push_back(GCSConstraintPtr(new GCS::ConstraintPerpendicular(
        *(aPoint1->point()), *(aPoint2->point()), aLine->p1, aLine->p2)));

    ConstraintWrapperPtr aSubResult(new PlaneGCSSolver_ConstraintWrapper(
        theConstraint, aConstrList, CONSTRAINT_SYMMETRIC));
    aSubResult->setGroup(theGroupID);
    std::list<EntityWrapperPtr> aSubs(1, theEntity1);
    aSubs.push_back(theEntity2);
    aSubs.push_back(theMirrorLine);
    aSubResult->setEntities(aSubs);
    aResult.push_back(aSubResult);
  }
  else if (theEntity1->type() == ENTITY_LINE) {
    const std::list<EntityWrapperPtr>& aPoints1 = theEntity1->subEntities();
    const std::list<EntityWrapperPtr>& aPoints2 = theEntity2->subEntities();
    std::list<EntityWrapperPtr>::const_iterator anIt1 = aPoints1.begin();
    std::list<EntityWrapperPtr>::const_iterator anIt2 = aPoints2.begin();
    for (; anIt1 != aPoints1.end() && anIt2 != aPoints2.end(); ++anIt1, ++anIt2) {
      std::list<ConstraintWrapperPtr> aMrrList =
          createMirror(theConstraint, theGroupID, theSketchID, *anIt1, *anIt2, theMirrorLine);
      aResult.insert(aResult.end(), aMrrList.begin(), aMrrList.end());
    }
  }
  else if (theEntity1->type() == ENTITY_CIRCLE) {
    const std::list<EntityWrapperPtr>& aPoints1 = theEntity1->subEntities();
    std::list<EntityWrapperPtr>::const_iterator anIt1 = aPoints1.begin();
    for (; anIt1 != aPoints1.end(); ++anIt1)
      if ((*anIt1)->type() == ENTITY_POINT)
        break;
    const std::list<EntityWrapperPtr>& aPoints2 = theEntity2->subEntities();
    std::list<EntityWrapperPtr>::const_iterator anIt2 = aPoints2.begin();
    for (; anIt2 != aPoints2.end(); ++anIt2)
      if ((*anIt2)->type() == ENTITY_POINT)
        break;

    std::list<ConstraintWrapperPtr> aMrrList =
        createMirror(theConstraint, theGroupID, theSketchID, *anIt1, *anIt2, theMirrorLine);
    aResult.insert(aResult.end(), aMrrList.begin(), aMrrList.end());

    // Additional constraint for equal radii
    aMrrList = createConstraint(theConstraint, theGroupID, theSketchID, CONSTRAINT_EQUAL_RADIUS,
        0.0, EntityWrapperPtr(), EntityWrapperPtr(), theEntity1, theEntity2);
    aResult.insert(aResult.end(), aMrrList.begin(), aMrrList.end());
  }
  else if (theEntity1->type() == ENTITY_ARC) {
    // Do not allow mirrored arc recalculate its position until
    // coordinated of all points recalculated
    FeaturePtr aMirrArc = theEntity2->baseFeature();
    aMirrArc->data()->blockSendAttributeUpdated(true);

    // Make mirror for center and start point of original arc
    std::list<ConstraintWrapperPtr> aMrrList;
    std::list<EntityWrapperPtr>::const_iterator anIt1 = theEntity1->subEntities().begin();
    std::list<EntityWrapperPtr>::const_iterator anIt2 = theEntity2->subEntities().begin();
    aMrrList = createMirror(theConstraint, theGroupID, theSketchID, *anIt1, *anIt2, theMirrorLine);
    aResult.insert(aResult.end(), aMrrList.begin(), aMrrList.end());

    ++anIt1;
    ++anIt2; ++anIt2;
    aMrrList = createMirror(theConstraint, theGroupID, theSketchID, *anIt1, *anIt2, theMirrorLine);
    aResult.insert(aResult.end(), aMrrList.begin(), aMrrList.end());

    // make symmetric last point of original arc and first point of
    // mirrored arc without additional constraint
    ++anIt1;
    --anIt2;
    makeMirrorPoints(*anIt1, *anIt2, theMirrorLine);

    // Additionally, make equal radii...
    aMrrList = createConstraint(theConstraint, theGroupID, theSketchID, CONSTRAINT_EQUAL_RADIUS,
        0.0, EntityWrapperPtr(), EntityWrapperPtr(), theEntity1, theEntity2);
    aResult.insert(aResult.end(), aMrrList.begin(), aMrrList.end());
    // ... and make parametric length of arcs the same
    std::shared_ptr<PlaneGCSSolver_EntityWrapper> anArcEnt1 =
        std::dynamic_pointer_cast<PlaneGCSSolver_EntityWrapper>(theEntity1);
    std::shared_ptr<PlaneGCSSolver_EntityWrapper> anArcEnt2 =
        std::dynamic_pointer_cast<PlaneGCSSolver_EntityWrapper>(theEntity2);
    std::shared_ptr<GCS::Arc> anArc1 = std::dynamic_pointer_cast<GCS::Arc>(anArcEnt1->entity());
    std::shared_ptr<GCS::Arc> anArc2 = std::dynamic_pointer_cast<GCS::Arc>(anArcEnt2->entity());
    std::shared_ptr<PlaneGCSSolver_ParameterWrapper> anIntermed =
        std::dynamic_pointer_cast<PlaneGCSSolver_ParameterWrapper>(
        createParameter(theGroupID, *(anArc1->endAngle) - *(anArc1->startAngle)));
    // By the way, recalculate start and end angles of mirrored arc
    std::shared_ptr<GeomAPI_Dir2d> anOX(new GeomAPI_Dir2d(1.0, 0.0));
    std::shared_ptr<GeomAPI_Dir2d> aStartDir(new GeomAPI_Dir2d(
        *(anArc2->start.x) - *(anArc2->center.x), *(anArc2->start.y) - *(anArc2->center.y)));
    std::shared_ptr<GeomAPI_Dir2d> aEndDir(new GeomAPI_Dir2d(
        *(anArc2->end.x) - *(anArc2->center.x), *(anArc2->end.y) - *(anArc2->center.y)));
    *anArc2->startAngle = anOX->angle(aStartDir);
    *anArc2->endAngle = anOX->angle(aEndDir);

    std::list<GCSConstraintPtr> aConstrList;
    aConstrList.push_back(GCSConstraintPtr(new GCS::ConstraintDifference(
        anArc1->endAngle, anArc1->startAngle, anIntermed->parameter())));
    aConstrList.push_back(GCSConstraintPtr(new GCS::ConstraintDifference(
        anArc2->endAngle, anArc2->startAngle, anIntermed->parameter())));

    std::shared_ptr<PlaneGCSSolver_ConstraintWrapper> aSubResult(
        new PlaneGCSSolver_ConstraintWrapper(theConstraint, aConstrList, CONSTRAINT_SYMMETRIC));
    aSubResult->setGroup(theGroupID);
    std::list<EntityWrapperPtr> aSubs(1, theEntity1);
    aSubs.push_back(theEntity2);
    aSubs.push_back(theMirrorLine);
    aSubResult->setEntities(aSubs);
    aSubResult->setValueParameter(anIntermed);
    aResult.push_back(aSubResult);

    // Restore event sending
    aMirrArc->data()->blockSendAttributeUpdated(false);
  }
  return aResult;
}

void PlaneGCSSolver_Builder::adjustConstraint(ConstraintWrapperPtr theConstraint) const
{
  SketchSolver_ConstraintType aType = theConstraint->type();
  // Update flags and parameters in constraints
  if (aType == CONSTRAINT_PT_LINE_DISTANCE)
    adjustPtLineDistance(theConstraint);
  else if (aType == CONSTRAINT_SYMMETRIC)
    adjustMirror(theConstraint);
}

EntityWrapperPtr PlaneGCSSolver_Builder::createFeature(
    FeaturePtr theFeature,
    const std::list<EntityWrapperPtr>& theAttributes,
    const GroupID& theGroupID,
    const EntityID& /*theSketchID*/) const
{
  static EntityWrapperPtr aDummy;
  if (!theFeature->data()->isValid())
    return aDummy;

  // Sketch
  CompositeFeaturePtr aSketch = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(theFeature);
  if (aSketch)
    return createSketchEntity(aSketch, theGroupID);

  // SketchPlugin features
  std::shared_ptr<SketchPlugin_Feature> aFeature =
      std::dynamic_pointer_cast<SketchPlugin_Feature>(theFeature);
  if (!aFeature)
    return aDummy;

  // Verify the feature by its kind
  const std::string& aFeatureKind = aFeature->getKind();
  // Line
  if (aFeatureKind == SketchPlugin_Line::ID())
    return createLine(theFeature, theAttributes, theGroupID);
  // Circle
  else if (aFeatureKind == SketchPlugin_Circle::ID())
    return createCircle(theFeature, theAttributes, theGroupID);
  // Arc
  else if (aFeatureKind == SketchPlugin_Arc::ID())
    return createArc(theFeature, theAttributes, theGroupID);
  // Point (it has low probability to be an attribute of constraint, so it is checked at the end)
  else if (aFeatureKind == SketchPlugin_Point::ID() ||
           aFeatureKind == SketchPlugin_IntersectionPoint::ID()) {
    EntityWrapperPtr aSub;
    if (theAttributes.size() == 1)
      aSub = theAttributes.front();
    else {
      AttributePtr aPoint = theFeature->attribute(SketchPlugin_Point::COORD_ID());
      if (aPoint->isInitialized())
        aSub = createAttribute(aPoint, theGroupID);
    }
    if (!aSub)
      return aDummy;

    GCSPointPtr aSubEnt =
        std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(aSub)->point();
    EntityWrapperPtr aNewEntity(new PlaneGCSSolver_EntityWrapper(theFeature));
    aNewEntity->setSubEntities(std::list<EntityWrapperPtr>(1, aSub));
    return aNewEntity;
  }

  // wrong entity
  return aDummy;
}

EntityWrapperPtr PlaneGCSSolver_Builder::createAttribute(
    AttributePtr theAttribute,
    const GroupID& theGroupID,
    const EntityID& theSketchID) const
{
  AttributePtr anAttribute = theAttribute;
  AttributeRefAttrPtr aRefAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(anAttribute);
  if (aRefAttr) {
    if (aRefAttr->isObject()) {
      // do not create features here
      return EntityWrapperPtr();
    } else
      anAttribute = aRefAttr->attr();
  }

  std::list<ParameterWrapperPtr> aParameters;
  EntityWrapperPtr aResult;

  // Point in 2D
  std::shared_ptr<GeomDataAPI_Point2D> aPoint2D =
    std::dynamic_pointer_cast<GeomDataAPI_Point2D>(theAttribute);
  if (aPoint2D) {
    aParameters.push_back(::createParameter(theGroupID, aPoint2D->x(), !aPoint2D->textX().empty()));
    aParameters.push_back(::createParameter(theGroupID, aPoint2D->y(), !aPoint2D->textY().empty()));
    GCSPointPtr aGCSPoint(new GCS::Point);
    aGCSPoint->x = std::dynamic_pointer_cast<
      PlaneGCSSolver_ParameterWrapper>(aParameters.front())->parameter();
    aGCSPoint->y = std::dynamic_pointer_cast<
      PlaneGCSSolver_ParameterWrapper>(aParameters.back())->parameter();
    // Create entity (parameters are not filled)
    aResult = EntityWrapperPtr(new PlaneGCSSolver_PointWrapper(theAttribute, aGCSPoint));
  } else {
    // Scalar value (used for the distance entities)
    AttributeDoublePtr aScalar =
      std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(theAttribute);
    if (aScalar)
      return createScalar(theGroupID, aScalar);
  }

  if (!aResult) {
    // unknown attribute type
    return EntityWrapperPtr();
  }

  aResult->setGroup(theGroupID);
  aResult->setParameters(aParameters);
  return aResult;
}

ParameterWrapperPtr PlaneGCSSolver_Builder::createParameter(
    const GroupID& theGroupID, double theValue) const
{
  return ::createParameter(theGroupID, theValue);
}


EntityWrapperPtr PlaneGCSSolver_Builder::createSketchEntity(
    CompositeFeaturePtr theSketch,
    const GroupID& theGroupID) const
{
  DataPtr aSketchData = theSketch->data();
  if (!aSketchData || !aSketchData->isValid())
    return EntityWrapperPtr(); // the sketch is incorrect

  // Create dummy wrapper representing workplane
  std::shared_ptr<PlaneGCSSolver_EntityWrapper> aSketchEnt(
      new PlaneGCSSolver_EntityWrapper(FeaturePtr(theSketch)));
  aSketchEnt->setGroup(theGroupID);
  aSketchEnt->setId(EID_SKETCH);
  return aSketchEnt;
}





// ================   Auxiliary functions   ==========================
ParameterWrapperPtr createParameter(
    const GroupID& theGroup, const double theValue, const bool theExpr)
{
  double* aParam = new double(theValue);
  ParameterWrapperPtr aWrapper(new PlaneGCSSolver_ParameterWrapper(aParam));
  aWrapper->setGroup(theGroup);
  aWrapper->setIsParametric(theExpr);
  return aWrapper;
}

ParameterWrapperPtr createParamAngle(const GroupID& theGroup, const double& theValue)
{
  double* aParam = new double(theValue);
  ParameterWrapperPtr aWrapper(new PlaneGCSSolver_AngleWrapper(aParam));
  aWrapper->setGroup(theGroup);
  return aWrapper;
}

std::shared_ptr<PlaneGCSSolver_ScalarWrapper> createScalar(
    const GroupID& theGroupID,
    AttributeDoublePtr theDoubleAttr)
{
  ParameterWrapperPtr aParam =
    createParameter(theGroupID, theDoubleAttr ? theDoubleAttr->value() : 0.0);
  return std::shared_ptr<PlaneGCSSolver_ScalarWrapper>(
      new PlaneGCSSolver_ScalarWrapper(theDoubleAttr, aParam));
}

EntityWrapperPtr createLine(FeaturePtr theFeature,
                            const std::list<EntityWrapperPtr>& theAttributes,
                            const GroupID& theGroupID)
{
  EntityWrapperPtr aNewEntity;
  std::list<EntityWrapperPtr> aSubs;

  AttributePtr aStart = theFeature->attribute(SketchPlugin_Line::START_ID());
  AttributePtr aEnd = theFeature->attribute(SketchPlugin_Line::END_ID());
  if (!aStart->isInitialized() || !aEnd->isInitialized())
    return aNewEntity;

  std::shared_ptr<PlaneGCSSolver_PointWrapper> aStartEnt, aEndEnt;
  std::list<EntityWrapperPtr>::const_iterator anIt = theAttributes.begin();
  for (; anIt != theAttributes.end(); ++anIt) {
    std::shared_ptr<PlaneGCSSolver_PointWrapper> aWrapper =
        std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(*anIt);
    if (!aWrapper)
      continue;
    if (aWrapper->isBase(aStart))
      aStartEnt = aWrapper;
    else if (aWrapper->isBase(aEnd))
      aEndEnt = aWrapper;
  }
  if (!aStartEnt || !aEndEnt)
    return aNewEntity;

  aSubs.push_back(aStartEnt);
  aSubs.push_back(aEndEnt);

  std::shared_ptr<GCS::Line> aLine(new GCS::Line);
  aLine->p1 = *(aStartEnt->point());
  aLine->p2 = *(aEndEnt->point());

  aNewEntity = EntityWrapperPtr(new PlaneGCSSolver_EntityWrapper(theFeature, aLine));
  // sub-entities should not change their groups, therefore they are added later
  aNewEntity->setGroup(theGroupID);
  aNewEntity->setSubEntities(aSubs);
  return aNewEntity;
}

EntityWrapperPtr createCircle(FeaturePtr theFeature,
                              const std::list<EntityWrapperPtr>& theAttributes,
                              const GroupID& theGroupID)
{
  EntityWrapperPtr aNewEntity;
  std::list<EntityWrapperPtr> aSubs;

  AttributePtr aCenter = theFeature->attribute(SketchPlugin_Circle::CENTER_ID());
  AttributePtr aRadius = theFeature->attribute(SketchPlugin_Circle::RADIUS_ID());
  if (!aCenter->isInitialized() || !aRadius->isInitialized())
    return aNewEntity;

  std::shared_ptr<PlaneGCSSolver_PointWrapper> aCenterEnt;
  std::shared_ptr<PlaneGCSSolver_ScalarWrapper> aRadiusEnt;
  std::list<EntityWrapperPtr>::const_iterator anIt = theAttributes.begin();
  for (; anIt != theAttributes.end(); ++anIt) {
    if ((*anIt)->isBase(aCenter))
      aCenterEnt = std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(*anIt);
    else if ((*anIt)->isBase(aRadius))
      aRadiusEnt = std::dynamic_pointer_cast<PlaneGCSSolver_ScalarWrapper>(*anIt);
  }
  if (!aCenterEnt || !aRadiusEnt)
    return aNewEntity;

  aSubs.push_back(aCenterEnt);
  aSubs.push_back(aRadiusEnt);

  std::shared_ptr<GCS::Circle> aCircle(new GCS::Circle);
  aCircle->center = *(aCenterEnt->point());
  aCircle->rad = aRadiusEnt->scalar();

  aNewEntity = EntityWrapperPtr(new PlaneGCSSolver_EntityWrapper(theFeature, aCircle));
  // sub-entities should not change their groups, therefore they are added later
  aNewEntity->setGroup(theGroupID);
  aNewEntity->setSubEntities(aSubs);
  return aNewEntity;
}

EntityWrapperPtr createArc(FeaturePtr theFeature,
                           const std::list<EntityWrapperPtr>& theAttributes,
                           const GroupID& theGroupID)
{
  EntityWrapperPtr aNewEntity;
  std::list<EntityWrapperPtr> aSubs;

  AttributePtr aCenter = theFeature->attribute(SketchPlugin_Arc::CENTER_ID());
  AttributePtr aStart = theFeature->attribute(SketchPlugin_Arc::START_ID());
  AttributePtr aEnd = theFeature->attribute(SketchPlugin_Arc::END_ID());
  if (!aCenter->isInitialized() || !aStart->isInitialized() || !aEnd->isInitialized())
    return aNewEntity;

  std::shared_ptr<PlaneGCSSolver_PointWrapper> aCenterEnt, aStartEnt, aEndEnt;
  std::list<EntityWrapperPtr>::const_iterator anIt = theAttributes.begin();
  for (; anIt != theAttributes.end(); ++anIt) {
    std::shared_ptr<PlaneGCSSolver_PointWrapper> aWrapper =
        std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(*anIt);
    if (!aWrapper)
      continue;
    if (aWrapper->isBase(aCenter))
      aCenterEnt = aWrapper;
    else if (aWrapper->isBase(aStart))
      aStartEnt = aWrapper;
    else if (aWrapper->isBase(aEnd))
      aEndEnt = aWrapper;
  }
  if (!aCenterEnt || !aStartEnt || !aEndEnt)
    return aNewEntity;

  std::shared_ptr<PlaneGCSSolver_ScalarWrapper> aStartAng, aEndAng, aRadius;
  aStartAng = createScalar(theGroupID);
  aEndAng   = createScalar(theGroupID);
  aRadius   = createScalar(theGroupID);

  aSubs.push_back(aCenterEnt);
  aSubs.push_back(aStartEnt);
  aSubs.push_back(aEndEnt);
  aSubs.push_back(aStartAng);
  aSubs.push_back(aEndAng);
  aSubs.push_back(aRadius);

  std::shared_ptr<GCS::Arc> anArc(new GCS::Arc);
  anArc->center     = *(aCenterEnt->point());
  anArc->start      = *(aStartEnt->point());
  anArc->end        = *(aEndEnt->point());
  anArc->startAngle = aStartAng->scalar();
  anArc->endAngle   = aEndAng->scalar();
  anArc->rad        = aRadius->scalar();

  aNewEntity = EntityWrapperPtr(new PlaneGCSSolver_EntityWrapper(theFeature, anArc));
  // sub-entities should not change their groups, therefore they are added later
  aNewEntity->setGroup(theGroupID);
  aNewEntity->setSubEntities(aSubs);
  return aNewEntity;
}



ConstraintWrapperPtr createConstraintCoincidence(
    ConstraintPtr theConstraint,
    const GroupID& theGroupID,
    std::shared_ptr<PlaneGCSSolver_PointWrapper> thePoint1,
    std::shared_ptr<PlaneGCSSolver_PointWrapper> thePoint2)
{
  // Create equality constraint for corresponding attributes of the points
  std::list<GCSConstraintPtr> aConstrList;
  std::list<ParameterWrapperPtr>::const_iterator anIt1 = thePoint1->parameters().begin();
  std::list<ParameterWrapperPtr>::const_iterator anIt2 = thePoint2->parameters().begin();
  for (; anIt1 != thePoint1->parameters().end(); ++anIt1, ++anIt2) {
    if (*anIt1 == *anIt2)
      continue; // points use same parameters, no need additional constraints
    std::shared_ptr<PlaneGCSSolver_ParameterWrapper> aParam1 =
        std::dynamic_pointer_cast<PlaneGCSSolver_ParameterWrapper>(*anIt1);
    std::shared_ptr<PlaneGCSSolver_ParameterWrapper> aParam2 =
        std::dynamic_pointer_cast<PlaneGCSSolver_ParameterWrapper>(*anIt2);
    aConstrList.push_back(
        GCSConstraintPtr(new GCS::ConstraintEqual(aParam1->parameter(), aParam2->parameter())));
  }

  ConstraintWrapperPtr aResult(new PlaneGCSSolver_ConstraintWrapper(
      theConstraint, aConstrList, CONSTRAINT_PT_PT_COINCIDENT));
  aResult->setGroup(theGroupID);
  std::list<EntityWrapperPtr> aSubs(1, thePoint1);
  aSubs.push_back(thePoint2);
  aResult->setEntities(aSubs);
  return aResult;
}

ConstraintWrapperPtr createConstraintPointOnEntity(
    ConstraintPtr theConstraint,
    const GroupID& theGroupID,
    const SketchSolver_ConstraintType& theType,
    std::shared_ptr<PlaneGCSSolver_PointWrapper> thePoint,
    std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity)
{
  GCSConstraintPtr aNewConstr;

  switch (theEntity->type()) {
  case ENTITY_LINE: {
    std::shared_ptr<GCS::Line> aLine = std::dynamic_pointer_cast<GCS::Line>(theEntity->entity());
    aNewConstr = GCSConstraintPtr(new GCS::ConstraintPointOnLine(*(thePoint->point()), *aLine));
    break;
    }
  case ENTITY_ARC:
  case ENTITY_CIRCLE: {
    std::shared_ptr<GCS::Circle> aCirc =
      std::dynamic_pointer_cast<GCS::Circle>(theEntity->entity());
    aNewConstr = GCSConstraintPtr(
        new GCS::ConstraintP2PDistance(*(thePoint->point()), aCirc->center, aCirc->rad));
    break;
    }
  default:
    return ConstraintWrapperPtr();
  }

  ConstraintWrapperPtr aResult(new PlaneGCSSolver_ConstraintWrapper(
      theConstraint, aNewConstr, theType));
  aResult->setGroup(theGroupID);
  std::list<EntityWrapperPtr> aSubs(1, thePoint);
  aSubs.push_back(theEntity);
  aResult->setEntities(aSubs);
  return aResult;
}

// calculate length of the line
static inline double lineLength(const GCS::Line& theLine)
{
  double aDir[2] = {*(theLine.p2.x) - *(theLine.p1.x), *(theLine.p2.y) - *(theLine.p1.y)};
  return sqrt(aDir[0] * aDir[0] + aDir[1] * aDir[1]);
}

// check the point is on the line
static inline bool isPointOnLine(const GCS::Point& thePoint, const GCS::Line& theLine)
{
  double aDir[2] = {*(theLine.p2.x) - *(theLine.p1.x), *(theLine.p2.y) - *(theLine.p1.y)};
  double aVec[2] = {*(thePoint.x) - *(theLine.p1.x), *(thePoint.y) - *(theLine.p1.y)};
  double aCross = aVec[0] * aDir[1] - aVec[1] * aDir[0];
  return fabs(aCross) < tolerance;
}

ConstraintWrapperPtr createConstraintMiddlePoint(
    ConstraintPtr theConstraint,
    const GroupID& theGroupID,
    std::shared_ptr<PlaneGCSSolver_PointWrapper> thePoint,
    std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity)
{
  GCSPointPtr aPoint = thePoint->point();
  std::shared_ptr<GCS::Line> aLine = std::dynamic_pointer_cast<GCS::Line>(theEntity->entity());
  if (!aLine)
    return ConstraintWrapperPtr();

  std::list<GCSConstraintPtr> aConstrList;
  aConstrList.push_back(GCSConstraintPtr(new GCS::ConstraintPointOnLine(*aPoint, *aLine)));
  double aDist = lineLength(*aLine);
  std::shared_ptr<PlaneGCSSolver_ParameterWrapper> aDistance =
      std::dynamic_pointer_cast<PlaneGCSSolver_ParameterWrapper>(
      createParameter(theGroupID, aDist * 0.5));
  aConstrList.push_back(GCSConstraintPtr(
      new GCS::ConstraintP2PDistance(*aPoint, aLine->p1, aDistance->parameter())));
  aConstrList.push_back(GCSConstraintPtr(
      new GCS::ConstraintP2PDistance(*aPoint, aLine->p2, aDistance->parameter())));

  // Workaround to avoid conflicting constraints when the point is already placed on line
  if (thePoint->group() != GID_UNKNOWN && isPointOnLine(*aPoint, *aLine)) {
    std::shared_ptr<GeomDataAPI_Point2D> aCoord =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(thePoint->baseAttribute());
    if (aCoord) {
      *(aPoint->x) = (*(aLine->p1.x) + *(aLine->p2.x)) * 0.5;
      *(aPoint->y) = (*(aLine->p1.y) + *(aLine->p2.y)) * 0.5;
      aCoord->setValue(*(aPoint->x), *(aPoint->y));
    }
  }

  std::shared_ptr<PlaneGCSSolver_ConstraintWrapper> aResult(new PlaneGCSSolver_ConstraintWrapper(
      theConstraint, aConstrList, CONSTRAINT_MIDDLE_POINT));
  aResult->setGroup(theGroupID);
  std::list<EntityWrapperPtr> aSubs(1, thePoint);
  aSubs.push_back(theEntity);
  aResult->setEntities(aSubs);
  aResult->setValueParameter(aDistance);
  return aResult;
}


ConstraintWrapperPtr createConstraintDistancePointPoint(
    ConstraintPtr theConstraint,
    const GroupID& theGroupID,
    std::shared_ptr<PlaneGCSSolver_ParameterWrapper> theValue,
    std::shared_ptr<PlaneGCSSolver_PointWrapper> thePoint1,
    std::shared_ptr<PlaneGCSSolver_PointWrapper> thePoint2)
{
  GCSConstraintPtr aNewConstr(new GCS::ConstraintP2PDistance(
      *(thePoint1->point()), *(thePoint2->point()), theValue->parameter()));

  std::shared_ptr<PlaneGCSSolver_ConstraintWrapper> aResult(
      new PlaneGCSSolver_ConstraintWrapper(theConstraint, aNewConstr, CONSTRAINT_PT_PT_DISTANCE));
  aResult->setGroup(theGroupID);
  std::list<EntityWrapperPtr> aSubs(1, thePoint1);
  aSubs.push_back(thePoint2);
  aResult->setEntities(aSubs);
  aResult->setValueParameter(theValue);
  return aResult;
}

ConstraintWrapperPtr createConstraintDistancePointLine(
    ConstraintPtr theConstraint,
    const GroupID& theGroupID,
    std::shared_ptr<PlaneGCSSolver_ParameterWrapper> theValue,
    std::shared_ptr<PlaneGCSSolver_PointWrapper> thePoint,
    std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity)
{
  std::shared_ptr<GCS::Line> aLine = std::dynamic_pointer_cast<GCS::Line>(theEntity->entity());
  GCSConstraintPtr aNewConstr(new GCS::ConstraintP2LDistance(
      *(thePoint->point()), *(aLine), theValue->parameter()));

  std::shared_ptr<PlaneGCSSolver_ConstraintWrapper> aResult(
      new PlaneGCSSolver_ConstraintWrapper(theConstraint, aNewConstr, CONSTRAINT_PT_LINE_DISTANCE));
  aResult->setGroup(theGroupID);
  std::list<EntityWrapperPtr> aSubs(1, thePoint);
  aSubs.push_back(theEntity);
  aResult->setEntities(aSubs);
  aResult->setValueParameter(theValue);
  return aResult;
}

ConstraintWrapperPtr createConstraintRadius(
    ConstraintPtr theConstraint,
    const GroupID& theGroupID,
    std::shared_ptr<PlaneGCSSolver_ParameterWrapper> theValue,
    std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity)
{
  std::shared_ptr<GCS::Circle> aCircle =
    std::dynamic_pointer_cast<GCS::Circle>(theEntity->entity());
  GCSConstraintPtr aNewConstr(new GCS::ConstraintEqual(aCircle->rad, theValue->parameter()));

  std::shared_ptr<PlaneGCSSolver_ConstraintWrapper> aResult(
      new PlaneGCSSolver_ConstraintWrapper(theConstraint, aNewConstr, CONSTRAINT_RADIUS));
  aResult->setGroup(theGroupID);
  std::list<EntityWrapperPtr> aSubs(1, theEntity);
  aResult->setEntities(aSubs);
  aResult->setValueParameter(theValue);
  return aResult;
}

ConstraintWrapperPtr createConstraintAngle(
    ConstraintPtr theConstraint,
    const GroupID& theGroupID,
    std::shared_ptr<PlaneGCSSolver_ParameterWrapper> theValue,
    std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity1,
    std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity2)
{
  std::shared_ptr<GCS::Line> aLine1 = std::dynamic_pointer_cast<GCS::Line>(theEntity1->entity());
  bool isLine1Rev = theConstraint->boolean(
      SketchPlugin_ConstraintAngle::ANGLE_REVERSED_FIRST_LINE_ID())->value();
  GCS::Point aLine1Pt1 = isLine1Rev ? aLine1->p2 : aLine1->p1;
  GCS::Point aLine1Pt2 = isLine1Rev ? aLine1->p1 : aLine1->p2;

  std::shared_ptr<GCS::Line> aLine2 = std::dynamic_pointer_cast<GCS::Line>(theEntity2->entity());
  bool isLine2Rev = theConstraint->boolean(
      SketchPlugin_ConstraintAngle::ANGLE_REVERSED_SECOND_LINE_ID())->value();
  GCS::Point aLine2Pt1 = isLine2Rev ? aLine2->p2 : aLine2->p1;
  GCS::Point aLine2Pt2 = isLine2Rev ? aLine2->p1 : aLine2->p2;

  GCSConstraintPtr aNewConstr(new GCS::ConstraintL2LAngle(
      aLine1Pt1, aLine1Pt2, aLine2Pt1, aLine2Pt2, theValue->parameter()));

  std::shared_ptr<PlaneGCSSolver_ConstraintWrapper> aResult(
      new PlaneGCSSolver_ConstraintWrapper(theConstraint, aNewConstr, CONSTRAINT_ANGLE));
  aResult->setGroup(theGroupID);
  std::list<EntityWrapperPtr> aSubs(1, theEntity1);
  aSubs.push_back(theEntity2);
  aResult->setEntities(aSubs);
  aResult->setValueParameter(theValue);
  return aResult;
}

ConstraintWrapperPtr createConstraintHorizVert(
    ConstraintPtr theConstraint,
    const GroupID& theGroupID,
    const SketchSolver_ConstraintType& theType,
    std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity)
{
  std::shared_ptr<GCS::Line> aLine = std::dynamic_pointer_cast<GCS::Line>(theEntity->entity());
  GCSConstraintPtr aNewConstr;
  if (theType == CONSTRAINT_HORIZONTAL)
    aNewConstr = GCSConstraintPtr(new GCS::ConstraintEqual(aLine->p1.y, aLine->p2.y));
  else
    aNewConstr = GCSConstraintPtr(new GCS::ConstraintEqual(aLine->p1.x, aLine->p2.x));

  ConstraintWrapperPtr aResult(new PlaneGCSSolver_ConstraintWrapper(
      theConstraint, aNewConstr, theType));
  aResult->setGroup(theGroupID);
  std::list<EntityWrapperPtr> aSubs(1, theEntity);
  aResult->setEntities(aSubs);
  return aResult;
}

ConstraintWrapperPtr createConstraintCollinear(
    ConstraintPtr theConstraint,
    const GroupID& theGroupID,
    std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity1,
    std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity2)
{
  std::shared_ptr<GCS::Line> aLine1 = std::dynamic_pointer_cast<GCS::Line>(theEntity1->entity());
  std::shared_ptr<GCS::Line> aLine2 = std::dynamic_pointer_cast<GCS::Line>(theEntity2->entity());

  // create two point-on-line constraints
  std::list<GCSConstraintPtr> aConstrList;
  aConstrList.push_back( GCSConstraintPtr(new GCS::ConstraintPointOnLine(aLine2->p1, *aLine1)) );
  aConstrList.push_back( GCSConstraintPtr(new GCS::ConstraintPointOnLine(aLine2->p2, *aLine1)) );

  ConstraintWrapperPtr aResult(new PlaneGCSSolver_ConstraintWrapper(
      theConstraint, aConstrList, CONSTRAINT_COLLINEAR));
  aResult->setGroup(theGroupID);
  std::list<EntityWrapperPtr> aSubs(1, theEntity1);
  aSubs.push_back(theEntity2);
  aResult->setEntities(aSubs);
  return aResult;
}

ConstraintWrapperPtr createConstraintParallel(
    ConstraintPtr theConstraint,
    const GroupID& theGroupID,
    std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity1,
    std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity2)
{
  std::shared_ptr<GCS::Line> aLine1 = std::dynamic_pointer_cast<GCS::Line>(theEntity1->entity());
  std::shared_ptr<GCS::Line> aLine2 = std::dynamic_pointer_cast<GCS::Line>(theEntity2->entity());
  GCSConstraintPtr aNewConstr(new GCS::ConstraintParallel(*(aLine1), *(aLine2)));

  std::shared_ptr<PlaneGCSSolver_ConstraintWrapper> aResult(
      new PlaneGCSSolver_ConstraintWrapper(theConstraint, aNewConstr, CONSTRAINT_PARALLEL));
  aResult->setGroup(theGroupID);
  std::list<EntityWrapperPtr> aSubs(1, theEntity1);
  aSubs.push_back(theEntity2);
  aResult->setEntities(aSubs);
  return aResult;
}

ConstraintWrapperPtr createConstraintPerpendicular(
    ConstraintPtr theConstraint,
    const GroupID& theGroupID,
    std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity1,
    std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity2)
{
  std::shared_ptr<GCS::Line> aLine1 = std::dynamic_pointer_cast<GCS::Line>(theEntity1->entity());
  std::shared_ptr<GCS::Line> aLine2 = std::dynamic_pointer_cast<GCS::Line>(theEntity2->entity());
  GCSConstraintPtr aNewConstr(new GCS::ConstraintPerpendicular(*(aLine1), *(aLine2)));

  std::shared_ptr<PlaneGCSSolver_ConstraintWrapper> aResult(
      new PlaneGCSSolver_ConstraintWrapper(theConstraint, aNewConstr, CONSTRAINT_PERPENDICULAR));
  aResult->setGroup(theGroupID);
  std::list<EntityWrapperPtr> aSubs(1, theEntity1);
  aSubs.push_back(theEntity2);
  aResult->setEntities(aSubs);
  return aResult;
}

ConstraintWrapperPtr createConstraintEqual(
    ConstraintPtr theConstraint,
    const GroupID& theGroupID,
    const SketchSolver_ConstraintType& theType,
    std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity1,
    std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity2,
    std::shared_ptr<PlaneGCSSolver_ParameterWrapper> theIntermed)
{
  if (theType == CONSTRAINT_EQUAL_LINE_ARC)
    return ConstraintWrapperPtr(); // line-arc equivalence is not supported yet

  std::list<GCSConstraintPtr> aConstrList;
  if (theType == CONSTRAINT_EQUAL_LINES) {
    std::shared_ptr<GCS::Line> aLine1 = std::dynamic_pointer_cast<GCS::Line>(theEntity1->entity());
    std::shared_ptr<GCS::Line> aLine2 = std::dynamic_pointer_cast<GCS::Line>(theEntity2->entity());

    aConstrList.push_back(GCSConstraintPtr(
        new GCS::ConstraintP2PDistance(aLine1->p1, aLine1->p2, theIntermed->parameter())));
    aConstrList.push_back(GCSConstraintPtr(
        new GCS::ConstraintP2PDistance(aLine2->p1, aLine2->p2, theIntermed->parameter())));
    // update value of intermediate parameter
    double x = *aLine1->p1.x - *aLine1->p2.x;
    double y = *aLine1->p1.y - *aLine1->p2.y;
    double aLen = sqrt(x*x + y*y);
    theIntermed->setValue(aLen);
  } else {
    std::shared_ptr<GCS::Circle> aCirc1 =
        std::dynamic_pointer_cast<GCS::Circle>(theEntity1->entity());
    std::shared_ptr<GCS::Circle> aCirc2 =
        std::dynamic_pointer_cast<GCS::Circle>(theEntity2->entity());

    aConstrList.push_back(GCSConstraintPtr(new GCS::ConstraintEqual(aCirc1->rad, aCirc2->rad)));
  }

  std::shared_ptr<PlaneGCSSolver_ConstraintWrapper> aResult(
      new PlaneGCSSolver_ConstraintWrapper(theConstraint, aConstrList, theType));
  aResult->setGroup(theGroupID);
  std::list<EntityWrapperPtr> aSubs(1, theEntity1);
  aSubs.push_back(theEntity2);
  aResult->setEntities(aSubs);
  if (theIntermed)
    aResult->setValueParameter(theIntermed);
  return aResult;
}

ConstraintWrapperPtr createConstraintTangent(
    ConstraintPtr theConstraint,
    const GroupID& theGroupID,
    const SketchSolver_ConstraintType& theType,
    std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity1,
    std::shared_ptr<PlaneGCSSolver_EntityWrapper> theEntity2)
{
  GCSConstraintPtr aNewConstr;
  if (theType == CONSTRAINT_TANGENT_ARC_LINE || theType == CONSTRAINT_TANGENT_CIRCLE_LINE) {
    std::shared_ptr<GCS::Circle> aCirc =
      std::dynamic_pointer_cast<GCS::Circle>(theEntity1->entity());
    std::shared_ptr<GCS::Line> aLine =
      std::dynamic_pointer_cast<GCS::Line>(theEntity2->entity());

    aNewConstr =
      GCSConstraintPtr(new GCS::ConstraintP2LDistance(aCirc->center, *aLine, aCirc->rad));
  } else {
    std::shared_ptr<GCS::Circle> aCirc1 =
      std::dynamic_pointer_cast<GCS::Circle>(theEntity1->entity());
    std::shared_ptr<GCS::Circle> aCirc2 =
      std::dynamic_pointer_cast<GCS::Circle>(theEntity2->entity());

    double aDX = *(aCirc1->center.x) - *(aCirc2->center.x);
    double aDY = *(aCirc1->center.y) - *(aCirc2->center.y);
    double aDist = sqrt(aDX * aDX + aDY * aDY);
    aNewConstr = GCSConstraintPtr(new GCS::ConstraintTangentCircumf(aCirc1->center, aCirc2->center,
        aCirc1->rad, aCirc2->rad, (aDist < *(aCirc1->rad) || aDist < *(aCirc2->rad))));
  }

  std::shared_ptr<PlaneGCSSolver_ConstraintWrapper> aResult(
      new PlaneGCSSolver_ConstraintWrapper(theConstraint, aNewConstr, theType));
  aResult->setGroup(theGroupID);
  std::list<EntityWrapperPtr> aSubs(1, theEntity1);
  aSubs.push_back(theEntity2);
  aResult->setEntities(aSubs);
  return aResult;
}





void makeMirrorPoints(EntityWrapperPtr theOriginal,
                      EntityWrapperPtr theMirrored,
                      EntityWrapperPtr theMirrorLine)
{
  BuilderPtr aBuilder = PlaneGCSSolver_Builder::getInstance();

  std::shared_ptr<GeomAPI_Lin2d> aMirrorLine = aBuilder->line(theMirrorLine);
  std::shared_ptr<GeomAPI_Dir2d> aMLDir = aMirrorLine->direction();
  // orthogonal direction
  aMLDir = std::shared_ptr<GeomAPI_Dir2d>(new GeomAPI_Dir2d(aMLDir->y(), -aMLDir->x()));

  std::shared_ptr<GeomAPI_Pnt2d> aPoint = aBuilder->point(theOriginal);
  std::shared_ptr<GeomAPI_XY> aVec = aPoint->xy()->decreased(aMirrorLine->location()->xy());
  double aDist = aVec->dot(aMLDir->xy());
  aVec = aPoint->xy()->added(aMLDir->xy()->multiplied(-2.0 * aDist));
  double aCoord[2] = {aVec->x(), aVec->y()};
  std::list<ParameterWrapperPtr>::const_iterator aMIt = theMirrored->parameters().begin();
  for (int i = 0; aMIt != theMirrored->parameters().end(); ++aMIt, ++i)
    (*aMIt)->setValue(aCoord[i]);

  // update corresponding attribute
  AttributePtr anAttr =
    std::dynamic_pointer_cast<PlaneGCSSolver_PointWrapper>(theMirrored)->baseAttribute();
  if (anAttr) {
    std::shared_ptr<GeomDataAPI_Point2D> aMirroredPnt =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAttr);
    aMirroredPnt->setValue(aCoord[0], aCoord[1]);
  }
}

void adjustPtLineDistance(ConstraintWrapperPtr theConstraint)
{
  BuilderPtr aBuilder = PlaneGCSSolver_Builder::getInstance();

  std::shared_ptr<GeomAPI_Pnt2d> aPoint;
  std::shared_ptr<GeomAPI_Lin2d> aLine;
  std::list<EntityWrapperPtr> aSubs = theConstraint->entities();
  std::list<EntityWrapperPtr>::const_iterator aSIt = aSubs.begin();
  for (; aSIt != aSubs.end(); ++aSIt) {
    if ((*aSIt)->type() == ENTITY_POINT)
      aPoint = aBuilder->point(*aSIt);
    else if ((*aSIt)->type() == ENTITY_LINE)
      aLine = aBuilder->line(*aSIt);
  }

  std::shared_ptr<GeomAPI_XY> aLineVec = aLine->direction()->xy();
  std::shared_ptr<GeomAPI_XY> aPtLineVec = aPoint->xy()->decreased(aLine->location()->xy());
  if (aPtLineVec->cross(aLineVec) * theConstraint->value() < 0.0)
    theConstraint->setValue(theConstraint->value() * (-1.0));
}

void adjustMirror(ConstraintWrapperPtr theConstraint)
{
  std::vector<EntityWrapperPtr> aPoints;
  EntityWrapperPtr aMirrorLine;

  const std::list<EntityWrapperPtr>& aSubs = theConstraint->entities();
  std::list<EntityWrapperPtr>::const_iterator anIt = aSubs.begin();
  for (; anIt != aSubs.end(); ++anIt) {
    if ((*anIt)->type() == ENTITY_POINT)
      aPoints.push_back(*anIt);
    else if ((*anIt)->type() == ENTITY_LINE)
      aMirrorLine = *anIt;
  }

  if (aPoints.size() == 2)
    makeMirrorPoints(aPoints[0], aPoints[1], aMirrorLine);

  // update scales of constraints
  std::shared_ptr<PlaneGCSSolver_ConstraintWrapper> aGCSConstraint =
      std::dynamic_pointer_cast<PlaneGCSSolver_ConstraintWrapper>(theConstraint);
  std::list<GCSConstraintPtr>::const_iterator aCIt = aGCSConstraint->constraints().begin();
  for (; aCIt != aGCSConstraint->constraints().end(); ++aCIt)
    (*aCIt)->rescale();
}

bool PlaneGCSSolver_Builder::isArcArcTangencyInternal(
    EntityWrapperPtr theArc1, EntityWrapperPtr theArc2) const
{
  std::shared_ptr<GCS::Circle> aCirc1 = std::dynamic_pointer_cast<GCS::Circle>(
      GCS_ENTITY_WRAPPER(theArc1)->entity());
  std::shared_ptr<GCS::Circle> aCirc2 = std::dynamic_pointer_cast<GCS::Circle>(
      GCS_ENTITY_WRAPPER(theArc2)->entity());

  if (!aCirc1 || !aCirc2)
    return false;

  double aDX = *(aCirc1->center.x) - *(aCirc2->center.x);
  double aDY = *(aCirc1->center.y) - *(aCirc2->center.y);
  double aDist = sqrt(aDX * aDX + aDY * aDY);

  return (aDist < *(aCirc1->rad) || aDist < *(aCirc2->rad));
}

