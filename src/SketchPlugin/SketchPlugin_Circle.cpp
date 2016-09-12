// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchPlugin_Circle.cpp
// Created:     26 May 2014
// Author:      Artem ZHIDKOV

#include "SketchPlugin_Circle.h"
#include "SketchPlugin_Sketch.h"
#include <ModelAPI_Data.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Session.h>

#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_Circ.h>
#include <GeomAPI_Circ2d.h>
#include <GeomAPI_Vertex.h>
#include <GeomAPI_XY.h>
#include <GeomDataAPI_Point2D.h>
#include <GeomDataAPI_Dir.h>
#include <GeomAlgoAPI_PointBuilder.h>
#include <GeomAlgoAPI_EdgeBuilder.h>
#include <GeomAlgoAPI_CompoundBuilder.h>

#include <cmath>

const double tolerance = 1e-7;

namespace {
  static const std::string& POINT_ID(int theIndex)
  {
    switch (theIndex) {
    case 1: return SketchPlugin_Circle::FIRST_POINT_ID();
    case 2: return SketchPlugin_Circle::SECOND_POINT_ID();
    case 3: return SketchPlugin_Circle::THIRD_POINT_ID();
    }

    static const std::string DUMMY;
    return DUMMY;
  }
}


SketchPlugin_Circle::SketchPlugin_Circle()
    : SketchPlugin_SketchEntity()
{
}

void SketchPlugin_Circle::initDerivedClassAttributes()
{
  data()->addAttribute(CENTER_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(RADIUS_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(EXTERNAL_ID(), ModelAPI_AttributeSelection::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), EXTERNAL_ID());

  data()->addAttribute(CIRCLE_TYPE(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(FIRST_POINT_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(SECOND_POINT_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(THIRD_POINT_ID(), GeomDataAPI_Point2D::typeId());
  std::dynamic_pointer_cast<ModelAPI_AttributeString>(
      data()->attribute(CIRCLE_TYPE()))->setValue(CIRCLE_TYPE_CENTER_AND_RADIUS());
}

void SketchPlugin_Circle::execute()
{
  SketchPlugin_Sketch* aSketch = sketch();
  if (aSketch) {
    std::list<std::shared_ptr<GeomAPI_Shape> > aShapes;

    // compute a circle point in 3D view
    std::shared_ptr<GeomDataAPI_Point2D> aCenterAttr = std::dynamic_pointer_cast<
        GeomDataAPI_Point2D>(data()->attribute(CENTER_ID()));
    AttributeDoublePtr aRadiusAttr = 
      std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(data()->attribute(RADIUS_ID()));
    if (aCenterAttr->isInitialized() && aRadiusAttr->isInitialized()) {
      std::shared_ptr<GeomAPI_Pnt> aCenter(aSketch->to3D(aCenterAttr->x(), aCenterAttr->y()));
      //std::cout<<"Execute circle "<<aCenter->x()<<" "<<aCenter->y()<<" "<<aCenter->z()<<std::endl;
      // make a visible point
      SketchPlugin_Sketch::createPoint2DResult(this, sketch(), CENTER_ID(), 0);

      // make a visible circle
      std::shared_ptr<GeomDataAPI_Dir> aNDir = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
        aSketch->data()->attribute(SketchPlugin_Sketch::NORM_ID()));
      std::shared_ptr<GeomAPI_Dir> aNormal(new GeomAPI_Dir(aNDir->x(), aNDir->y(), aNDir->z()));
      // compute the circle radius
      double aRadius = aRadiusAttr->value();

      std::shared_ptr<GeomAPI_Shape> aCircleShape = GeomAlgoAPI_EdgeBuilder::lineCircle(
        aCenter, aNormal, aRadius);
      aShapes.push_back(aCircleShape);
      std::shared_ptr<ModelAPI_ResultConstruction> aConstr2 = document()->createConstruction(
        data(), 1);
      aConstr2->setShape(aCircleShape);
      aConstr2->setIsInHistory(false);
      setResult(aConstr2, 1);
    }
  }
}

AISObjectPtr SketchPlugin_Circle::getAISObject(AISObjectPtr thePrevious)
{
  SketchPlugin_Sketch* aSketch = sketch();
  if (aSketch && !isFeatureValid()) {
    // compute a circle point in 3D view
    std::shared_ptr<GeomDataAPI_Point2D> aCenterAttr = std::dynamic_pointer_cast<
        GeomDataAPI_Point2D>(data()->attribute(CENTER_ID()));
    AttributeDoublePtr aRadiusAttr = 
        std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(attribute(RADIUS_ID()));
    if (aCenterAttr->isInitialized() && aRadiusAttr->isInitialized()) {
        std::shared_ptr<GeomAPI_Pnt> aCenter(aSketch->to3D(aCenterAttr->x(), aCenterAttr->y()));

        // make a visible circle
        std::shared_ptr<GeomDataAPI_Dir> aNDir = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
            aSketch->data()->attribute(SketchPlugin_Sketch::NORM_ID()));
        std::shared_ptr<GeomAPI_Dir> aNormal = aNDir->dir();

        double aRadius = aRadiusAttr->value();
        std::shared_ptr<GeomAPI_Shape> aCircleShape = GeomAlgoAPI_EdgeBuilder::lineCircle(
            aCenter, aNormal, aRadius);
        if (aCircleShape && aRadius != 0) {
          std::list<std::shared_ptr<GeomAPI_Shape> > aShapes;
          // make a visible point
          std::shared_ptr<GeomAPI_Shape> aCenterPointShape = GeomAlgoAPI_PointBuilder::vertex(aCenter);
          aShapes.push_back(aCenterPointShape);
          aShapes.push_back(aCircleShape);

          std::shared_ptr<GeomAPI_Shape> aCompound = GeomAlgoAPI_CompoundBuilder::compound(aShapes);
          AISObjectPtr anAIS = thePrevious;
          if (!anAIS)
            anAIS = AISObjectPtr(new GeomAPI_AISObject);
          anAIS->createShape(aCompound);
          anAIS->setWidth(3);
          return anAIS;
        }
    }
  }
  return AISObjectPtr();
}

bool SketchPlugin_Circle::isFeatureValid()
{
  std::shared_ptr<GeomDataAPI_Point2D> aCenter = 
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(CENTER_ID()));
  bool aValid = aCenter->isInitialized();

  std::string aType = std::dynamic_pointer_cast<ModelAPI_AttributeString>(
                                         data()->attribute(CIRCLE_TYPE()))->value();
  if (aType == CIRCLE_TYPE_THREE_POINTS()) {
    std::shared_ptr<GeomDataAPI_Point2D> aFirstPnt =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(FIRST_POINT_ID()));
    std::shared_ptr<GeomDataAPI_Point2D> aSecondPnt =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(SECOND_POINT_ID()));
    std::shared_ptr<GeomDataAPI_Point2D> aThirdPnt =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(THIRD_POINT_ID()));
    aValid = aValid &&
             aFirstPnt->isInitialized() &&
             aSecondPnt->isInitialized() &&
             aThirdPnt->isInitialized();
  }
  return aValid;
}

void SketchPlugin_Circle::move(double theDeltaX, double theDeltaY)
{
  std::shared_ptr<ModelAPI_Data> aData = data();
  if (!aData->isValid())
    return;

  std::shared_ptr<GeomDataAPI_Point2D> aPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(CENTER_ID()));
  if (aPoint->isInitialized())
    aPoint->move(theDeltaX, theDeltaY);

  aPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(FIRST_POINT_ID()));
  if (aPoint->isInitialized())
    aPoint->move(theDeltaX, theDeltaY);
  aPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(SECOND_POINT_ID()));
  if (aPoint->isInitialized())
    aPoint->move(theDeltaX, theDeltaY);
  aPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(THIRD_POINT_ID()));
  if (aPoint->isInitialized())
    aPoint->move(theDeltaX, theDeltaY);
}

bool SketchPlugin_Circle::isFixed() {
  return data()->selection(EXTERNAL_ID())->context().get() != NULL;
}

void SketchPlugin_Circle::attributeChanged(const std::string& theID) {
  // the second condition for unability to move external segments anywhere
  if (theID == EXTERNAL_ID() || isFixed()) {
    std::shared_ptr<GeomAPI_Shape> aSelection = data()->selection(EXTERNAL_ID())->value();
    if (!aSelection) {
      // empty shape in selection shows that the shape is equal to context
      ResultPtr anExtRes = selection(EXTERNAL_ID())->context();
      if (anExtRes)
        aSelection = anExtRes->shape();
    }
    // update arguments due to the selection value
    if (aSelection && !aSelection->isNull() && aSelection->isEdge()) {
      std::shared_ptr<GeomAPI_Edge> anEdge( new GeomAPI_Edge(aSelection));
      std::shared_ptr<GeomAPI_Circ> aCirc = anEdge->circle();
      std::shared_ptr<GeomDataAPI_Point2D> aCenterAttr = 
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(CENTER_ID()));
      aCenterAttr->setValue(sketch()->to2D(aCirc->center()));
      real(RADIUS_ID())->setValue(aCirc->radius());
    }
  }
  else if (theID == CENTER_ID() || theID == RADIUS_ID()) {
    std::string aType = std::dynamic_pointer_cast<ModelAPI_AttributeString>(
      data()->attribute(CIRCLE_TYPE()))->value();
    if (aType == CIRCLE_TYPE_THREE_POINTS() && lastResult())  // adjust data from the solver
      adjustThreePoints();
  } else if (theID == FIRST_POINT_ID() || theID == SECOND_POINT_ID() || theID == THIRD_POINT_ID()) {
    // support the center and radius attributes enev in other mode: solver uses them
    std::string aType = std::dynamic_pointer_cast<ModelAPI_AttributeString>(
      data()->attribute(CIRCLE_TYPE()))->value();
    if (aType == CIRCLE_TYPE_CENTER_AND_RADIUS())
      return;
    data()->blockSendAttributeUpdated(true); // to modify two attributes at once
    std::shared_ptr<GeomAPI_Pnt2d> aPoints[3];
    int aNbInitialized = 0;
    for (int i = 1; i <= 3; ++i) {
      std::shared_ptr<GeomDataAPI_Point2D> aCurPnt =
          std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(POINT_ID(i)));
      if (aCurPnt->isInitialized())
        aPoints[aNbInitialized++] = aCurPnt->pnt();
    }

    std::shared_ptr<GeomDataAPI_Point2D> aCenterAttr = std::dynamic_pointer_cast<
        GeomDataAPI_Point2D>(data()->attribute(CENTER_ID()));
    AttributeDoublePtr aRadiusAttr = 
      std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(data()->attribute(RADIUS_ID()));

    if (aNbInitialized == 1)
      aCenterAttr->setValue(aPoints[0]->x(), aPoints[0]->y());
    else if (aNbInitialized == 2) {
      std::shared_ptr<GeomAPI_XY> aCoord =
          aPoints[0]->xy()->added(aPoints[1]->xy())->multiplied(0.5);
      double aRadius = aPoints[0]->distance(aPoints[1]) * 0.5;
      aCenterAttr->setValue(aCoord->x(), aCoord->y());
      aRadiusAttr->setValue(aRadius);
    } else {
      std::shared_ptr<GeomAPI_Circ2d> aCircle(
          new GeomAPI_Circ2d(aPoints[0], aPoints[1], aPoints[2]));

      std::shared_ptr<GeomAPI_Pnt2d> aCenter = aCircle->center();
      if (aCenter) {
        double aRadius = aCircle->radius();
        aCenterAttr->setValue(aCenter->x(), aCenter->y());
        aRadiusAttr->setValue(aRadius);
      }
    }
    data()->blockSendAttributeUpdated(false, false);

  } else if (theID == CIRCLE_TYPE()) { // if switched to 3 points mode, adjust the needed attributes
    std::string aType = std::dynamic_pointer_cast<ModelAPI_AttributeString>(
      data()->attribute(CIRCLE_TYPE()))->value();
    if (aType == CIRCLE_TYPE_THREE_POINTS()) {
      adjustThreePoints();
    }
  }
}

void SketchPlugin_Circle::adjustThreePoints()
{
  std::shared_ptr<GeomDataAPI_Point2D> aCenterAttr =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(CENTER_ID()));
  if (!aCenterAttr->isInitialized())
    return;
  AttributeDoublePtr aRadiusAttr = 
    std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(attribute(RADIUS_ID()));
  if (!aRadiusAttr->isInitialized())
    return;

  data()->blockSendAttributeUpdated(true);
  std::shared_ptr<GeomDataAPI_Point2D> aFirstPnt =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(FIRST_POINT_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> aSecondPnt =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(SECOND_POINT_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> aThirdPnt =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(attribute(THIRD_POINT_ID()));
  double aRadius = aRadiusAttr->value();

  bool isInitialized = aFirstPnt->isInitialized() &&
      aSecondPnt->isInitialized() && aThirdPnt->isInitialized();

  if (!isInitialized ||
      fabs(aFirstPnt->pnt()->distance(aCenterAttr->pnt()) - aRadius) > tolerance ||
      fabs(aSecondPnt->pnt()->distance(aCenterAttr->pnt()) - aRadius) > tolerance ||
      fabs(aThirdPnt->pnt()->distance(aCenterAttr->pnt()) - aRadius) > tolerance) {
    aFirstPnt->setValue(aCenterAttr->x() + aRadius, aCenterAttr->y());
    aSecondPnt->setValue(aCenterAttr->x(), aCenterAttr->y() + aRadius);
    aThirdPnt->setValue(aCenterAttr->x() - aRadius, aCenterAttr->y());
  }
  data()->blockSendAttributeUpdated(false, false);
}
