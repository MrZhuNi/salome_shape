// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_Tools.cpp
// Created:     10 March 2015
// Author:      Vitaly SMETANNIKOV

#include "SketcherPrs_Tools.h"

#include <SketchPlugin_Constraint.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Line.h>

#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_AttributeRefAttr.h>

#include <GeomDataAPI_Point2D.h>
#include <GeomAPI_Lin2d.h>


namespace SketcherPrs_Tools {

ObjectPtr getResult(ModelAPI_Feature* theFeature, const std::string& theAttrName)
{
  std::shared_ptr<ModelAPI_Data> aData = theFeature->data();
  std::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = aData->refattr(theAttrName);
  return anAttr->object();
}


std::shared_ptr<GeomAPI_Shape> getShape(ObjectPtr theObject)
{
  ResultConstructionPtr aRes = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(theObject);
  if (aRes.get() != NULL) {
    return aRes->shape();
  }
  return std::shared_ptr<GeomAPI_Shape>();
}


std::shared_ptr<GeomAPI_Pnt2d> getPoint(ModelAPI_Feature* theFeature,
                                        const std::string& theAttribute)
{
  std::shared_ptr<GeomDataAPI_Point2D> aPointAttr;

  if (!theFeature->data())
    return std::shared_ptr<GeomAPI_Pnt2d>();

  FeaturePtr aFeature;
  std::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = std::dynamic_pointer_cast<
      ModelAPI_AttributeRefAttr>(theFeature->data()->attribute(theAttribute));
  if (anAttr)
    aFeature = ModelAPI_Feature::feature(anAttr->object());

  if (aFeature && aFeature->getKind() == SketchPlugin_Point::ID())
    aPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aFeature->data()->attribute(SketchPlugin_Point::COORD_ID()));

  else if (aFeature && aFeature->getKind() == SketchPlugin_Circle::ID())
    aPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aFeature->data()->attribute(SketchPlugin_Circle::CENTER_ID()));

  else if (anAttr->attr()) {
    aPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAttr->attr());
  }
  if (aPointAttr.get() != NULL)
    return aPointAttr->pnt();
  return std::shared_ptr<GeomAPI_Pnt2d>();
}

//*************************************************************************************
std::shared_ptr<GeomDataAPI_Point2D> getFeaturePoint(DataPtr theData,
                                                     const std::string& theAttribute)
{
  std::shared_ptr<GeomDataAPI_Point2D> aPointAttr;

  if (!theData)
    return aPointAttr;

  FeaturePtr aFeature;
  std::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = std::dynamic_pointer_cast<
      ModelAPI_AttributeRefAttr>(theData->attribute(theAttribute));
  if (anAttr)
    aFeature = ModelAPI_Feature::feature(anAttr->object());

  if (aFeature && aFeature->getKind() == SketchPlugin_Point::ID())
    aPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aFeature->data()->attribute(SketchPlugin_Point::COORD_ID()));

  else if (aFeature && aFeature->getKind() == SketchPlugin_Circle::ID())
    aPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aFeature->data()->attribute(SketchPlugin_Circle::CENTER_ID()));

  else if (anAttr->attr()) {
    aPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAttr->attr());
  }
  return aPointAttr;
}

//*************************************************************************************
FeaturePtr getFeatureLine(DataPtr theData,
                          const std::string& theAttribute)
{
  FeaturePtr aLine;
  if (!theData)
    return aLine;

  std::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = 
    std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theData->attribute(theAttribute));
  if (anAttr) {
    FeaturePtr aFeature = ModelAPI_Feature::feature(anAttr->object());
    if (aFeature && aFeature->getKind() == SketchPlugin_Line::ID()) {
      return aFeature;
    }
  }
  return aLine;
}

//*************************************************************************************
std::shared_ptr<GeomAPI_Pnt2d> getProjectionPoint(const FeaturePtr theLine,
                                                  const std::shared_ptr<GeomAPI_Pnt2d>& thePoint)
{
  DataPtr aData = theLine->data();
  std::shared_ptr<GeomDataAPI_Point2D> aPoint1 = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(SketchPlugin_Line::START_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> aPoint2 = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(SketchPlugin_Line::END_ID()));

  GeomAPI_Lin2d aLin2d(aPoint1->x(), aPoint1->y(), aPoint2->x(), aPoint2->y());
  return aLin2d.project(thePoint);
}


static double MyArrowSize = 30.;
double getArrowSize()
{
  return MyArrowSize;
}

void setArrowSize(double theSize)
{
  MyArrowSize = theSize;
}

double getFlyoutDistance(const ModelAPI_Feature* theConstraint, 
                         const std::shared_ptr<GeomAPI_Ax3>& thePlane)
{
  return 50;
}


};
