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
#include <ModelAPI_Session.h>

#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_Circ.h>
#include <GeomDataAPI_Point2D.h>
#include <GeomDataAPI_Dir.h>
#include <GeomAlgoAPI_PointBuilder.h>
#include <GeomAlgoAPI_EdgeBuilder.h>
#include <GeomAlgoAPI_CompoundBuilder.h>

SketchPlugin_Circle::SketchPlugin_Circle()
    : SketchPlugin_SketchEntity()
{
}

void SketchPlugin_Circle::initAttributes()
{
  SketchPlugin_SketchEntity::initAttributes();

  data()->addAttribute(CENTER_ID(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(RADIUS_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(EXTERNAL_ID(), ModelAPI_AttributeSelection::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), EXTERNAL_ID());
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

void SketchPlugin_Circle::move(double theDeltaX, double theDeltaY)
{
  std::shared_ptr<ModelAPI_Data> aData = data();
  if (!aData->isValid())
    return;

  std::shared_ptr<GeomDataAPI_Point2D> aPoint1 = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(CENTER_ID()));
  aPoint1->move(theDeltaX, theDeltaY);
}

bool SketchPlugin_Circle::isFixed() {
  return data()->selection(EXTERNAL_ID())->context().get() != NULL;
}

void SketchPlugin_Circle::attributeChanged(const std::string& theID) {
  // the second condition for unability to move external segments anywhere
  if (theID == EXTERNAL_ID() || isFixed()) {
    std::shared_ptr<GeomAPI_Shape> aSelection = data()->selection(EXTERNAL_ID())->value();
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
}
