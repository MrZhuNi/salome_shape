// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_Tools.cpp
// Created:     10 March 2015
// Author:      Vitaly SMETANNIKOV

#include "SketcherPrs_Tools.h"

#include <SketchPlugin_Constraint.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Arc.h>

#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Events.h>

#include <ModelGeomAlgo_Point2D.h>

#include <Events_InfoMessage.h>

#include <GeomDataAPI_Point2D.h>
#include <GeomAPI_Lin2d.h>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>

#include <Prs3d_DimensionAspect.hxx>

#include <BRep_Tool.hxx>
#include <Precision.hxx>

#include <AIS_Dimension.hxx>

namespace SketcherPrs_Tools {

AttributePtr getAttribute(ModelAPI_Feature* theFeature, const std::string& theAttrName)
{
  AttributePtr anAttribute;
  if (theFeature) {
    std::shared_ptr<ModelAPI_Data> aData = theFeature->data();
    if (aData.get() && aData->isValid()) { /// essential check as it is called in openGl thread
      std::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = aData->refattr(theAttrName);
      if (!anAttr->isObject())
        anAttribute = anAttr->attr();
    }
  }
  return anAttribute;
}

ObjectPtr getResult(ModelAPI_Feature* theFeature, const std::string& theAttrName)
{
  ObjectPtr anObject;
  if (theFeature) {
    std::shared_ptr<ModelAPI_Data> aData = theFeature->data();
    if (aData.get() && aData->isValid()) { /// essential check as it is called in openGl thread
      std::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = aData->refattr(theAttrName);
      if (anAttr.get())
        anObject = anAttr->object();
    }
  }
  return anObject;
}


std::shared_ptr<GeomAPI_Shape> getShape(ObjectPtr theObject)
{
  ResultConstructionPtr aRes = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(theObject);
  if (aRes.get() != NULL && aRes->data()->isValid()) {
    /// essential check as it is called in openGl thread
    return aRes->shape();
  }
  return std::shared_ptr<GeomAPI_Shape>();
}


std::shared_ptr<GeomAPI_Pnt2d> getPoint(ModelAPI_Feature* theFeature,
                                        const std::string& theAttribute)
{
  std::shared_ptr<GeomDataAPI_Point2D> aPointAttr = ModelGeomAlgo_Point2D::getPointOfRefAttr(
               theFeature, theAttribute, SketchPlugin_Point::ID(), SketchPlugin_Point::COORD_ID());
  if (aPointAttr.get() != NULL)
    return aPointAttr->pnt();
  return std::shared_ptr<GeomAPI_Pnt2d>();
}

//*************************************************************************************
/// Find an attribute of the given object which corresponds to a vetrex
/// defined by a shape
/// \param theObject an object
/// \param theShape a vertex
/// \param thePlane a projection plane (sketcher plane)
std::shared_ptr<GeomDataAPI_Point2D> findGeomPoint(ObjectPtr theObject,
                                    const TopoDS_Shape& theShape,
                                    const std::shared_ptr<GeomAPI_Ax3>& thePlane)
{
  std::shared_ptr<GeomDataAPI_Point2D> aGeomPoint;

  FeaturePtr anObjectFeature = ModelAPI_Feature::feature(theObject);
  if (anObjectFeature) {
    if (theShape.ShapeType() == TopAbs_VERTEX) {
      const TopoDS_Vertex& aShapeVertex = TopoDS::Vertex(theShape);
      if (!aShapeVertex.IsNull())  {
        gp_Pnt aShapePoint = BRep_Tool::Pnt(aShapeVertex);
        std::shared_ptr<GeomAPI_Pnt> aShapeGeomPnt = std::shared_ptr<GeomAPI_Pnt>(
            new GeomAPI_Pnt(aShapePoint.X(), aShapePoint.Y(), aShapePoint.Z()));

        // find the given point in the feature attributes
        std::list<AttributePtr> anObjectAttiributes =
          anObjectFeature->data()->attributes(GeomDataAPI_Point2D::typeId());
        std::list<AttributePtr>::const_iterator anIt = anObjectAttiributes.begin(),
                                                aLast = anObjectAttiributes.end();
        for (; anIt != aLast && !aGeomPoint; anIt++) {
          std::shared_ptr<GeomDataAPI_Point2D> anAttributePoint =
            std::dynamic_pointer_cast<GeomDataAPI_Point2D>(*anIt);

          std::shared_ptr<GeomAPI_Pnt> anAttributePnt = thePlane->to3D(anAttributePoint->x(),
                                                                       anAttributePoint->y());
          if (anAttributePnt.get() &&
              anAttributePnt->distance(aShapeGeomPnt) < Precision::Confusion()) {
            aGeomPoint = anAttributePoint;
            break;
          }
        }
      }
    }
  }
  return aGeomPoint;
}

//*************************************************************************************
std::shared_ptr<GeomDataAPI_Point2D> getFeaturePoint(DataPtr theData,
                                                     const std::string& theAttribute,
                                                     const std::shared_ptr<GeomAPI_Ax3>& thePlane)
{
  std::shared_ptr<GeomDataAPI_Point2D> aPointAttr;

  if (!theData.get() || !theData->isValid()) /// essential check as it is called in openGl thread
    return aPointAttr;

  FeaturePtr aFeature;
  std::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = std::dynamic_pointer_cast<
      ModelAPI_AttributeRefAttr>(theData->attribute(theAttribute));
  if (anAttr) {
    if (anAttr->isObject()) {
      ObjectPtr anObject = anAttr->object();
      aFeature = ModelAPI_Feature::feature(anObject);
      if (aFeature && aFeature->getKind() == SketchPlugin_Point::ID()) {
        // Attribute refers to a point
        aPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                     aFeature->data()->attribute(SketchPlugin_Point::COORD_ID()));
      }
      else {
        // if the attribute refers on another object
        ResultPtr aRes = std::dynamic_pointer_cast<ModelAPI_Result>(anObject);
        if (aRes.get()) {
          GeomShapePtr aShape = aRes->shape();
          if (aShape.get()) {
            TopoDS_Shape aTDSShape = aShape->impl<TopoDS_Shape>();
            aPointAttr = findGeomPoint(anObject, aTDSShape, thePlane);
          }
        }
      }
    }
    else if (anAttr->attr()) {
      // If attribute is a point
      aPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAttr->attr());
    }
  }
  return aPointAttr;
}

//*************************************************************************************
FeaturePtr getFeatureLine(DataPtr theData,
                          const std::string& theAttribute)
{
  FeaturePtr aLine;
  if (!theData.get() || !theData->isValid()) /// essential check as it is called in openGl thread)
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
  if (!aData.get() || !aData->isValid())
    return std::shared_ptr<GeomAPI_Pnt2d>();

  std::shared_ptr<GeomDataAPI_Point2D> aPoint1 = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(SketchPlugin_Line::START_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> aPoint2 = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(SketchPlugin_Line::END_ID()));

  GeomAPI_Lin2d aLin2d(aPoint1->x(), aPoint1->y(), aPoint2->x(), aPoint2->y());
  return aLin2d.project(thePoint);
}


static double MyArrowSize = 20;
double getArrowSize()
{
  return MyArrowSize;
}

void setArrowSize(double theSize)
{
  MyArrowSize = theSize;
}

int getDefaultArrowSize()
{
  return 20;
}

static double MyTextHeight = 16;
double getTextHeight()
{
  return MyTextHeight;
}

void setTextHeight(double theHeight)
{
  MyTextHeight = theHeight;
}

double getDefaultTextHeight()
{
  return 16;
}

double getFlyoutDistance(const ModelAPI_Feature* theConstraint)
{
  std::shared_ptr<GeomDataAPI_Point2D> aFlyoutPoint =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      const_cast<ModelAPI_Feature*>(
      theConstraint)->attribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT()));
  // for not initialized values return zero distance to avoid Presentation crash
  if (!aFlyoutPoint->isInitialized())
    return 0;
  return aFlyoutPoint->y();
}

std::shared_ptr<GeomAPI_Pnt> getAnchorPoint(const ModelAPI_Feature* theConstraint,
                                            const std::shared_ptr<GeomAPI_Ax3>& thePlane)
{
  ModelAPI_Feature* aConstraint = const_cast<ModelAPI_Feature*>(theConstraint);
  AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
      aConstraint->attribute(SketchPlugin_Constraint::ENTITY_A()));
  if (!aRefAttr || !aRefAttr->isObject() || !aRefAttr->object())
    return std::shared_ptr<GeomAPI_Pnt>();

  FeaturePtr aFeature = ModelAPI_Feature::feature(aRefAttr->object());
  std::shared_ptr<GeomAPI_Pnt2d> aCenter;
  if (aFeature->getKind() == SketchPlugin_Arc::ID()) { // arc
    aCenter = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aFeature->attribute(SketchPlugin_Arc::CENTER_ID()))->pnt();
  } else if (aFeature->getKind() == SketchPlugin_Circle::ID()) { // circle
    aCenter = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aFeature->attribute(SketchPlugin_Circle::CENTER_ID()))->pnt();
  } else
    return std::shared_ptr<GeomAPI_Pnt>();

  std::shared_ptr<GeomAPI_Pnt2d> anOrigin(new GeomAPI_Pnt2d(0.0, 0.0));
  std::shared_ptr<GeomAPI_Pnt2d> aFlyoutPnt = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aConstraint->attribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT()))->pnt();
  double aRadius = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
      aConstraint->attribute(SketchPlugin_Constraint::VALUE()))->value();
  double aLen = aFlyoutPnt->distance(anOrigin);
  aRadius *= 1.001; // a gap to make point much closer to the circle, but not lying on it
  aFlyoutPnt->setX(aCenter->x() + aFlyoutPnt->x() * aRadius / aLen);
  aFlyoutPnt->setY(aCenter->y() + aFlyoutPnt->y() * aRadius / aLen);
  return thePlane->to3D(aFlyoutPnt->x(), aFlyoutPnt->y());
}

void sendExpressionShownEvent(const bool& theState)
{
  static Events_ID anId = SketcherPrs_ParameterStyleMessage::eventId();
  std::shared_ptr<SketcherPrs_ParameterStyleMessage> aMessage = std::shared_ptr
    <SketcherPrs_ParameterStyleMessage>(new SketcherPrs_ParameterStyleMessage(anId, 0));
  aMessage->setStyle(theState ? SketcherPrs_ParameterStyleMessage::ParameterText
                              : SketcherPrs_ParameterStyleMessage::ParameterValue);
  Events_Loop::loop()->send(aMessage);
  Events_Loop::loop()->flush(anId);
}

Handle(Prs3d_DimensionAspect) createDimensionAspect()
{
  Handle(Prs3d_DimensionAspect) anAspect = new Prs3d_DimensionAspect();
  anAspect->MakeArrows3d(false);
  anAspect->MakeText3d(false);
  anAspect->MakeTextShaded(false);
  anAspect->MakeUnitsDisplayed(false);
  anAspect->TextAspect()->SetHeight(SketcherPrs_Tools::getDefaultTextHeight());
  anAspect->ArrowAspect()->SetLength(SketcherPrs_Tools::getArrowSize());

  return anAspect;
}

void updateArrows(Handle(Prs3d_DimensionAspect) theDimAspect,
                  double theDimValue, double theTextSize)
{
  double anArrowLength = theDimAspect->ArrowAspect()->Length();
   // This is not realy correct way to get viewer scale.
  double aViewerScale = (double) SketcherPrs_Tools::getDefaultArrowSize() / anArrowLength;

  if(theTextSize > ((theDimValue - 3 * SketcherPrs_Tools::getArrowSize()) * aViewerScale)) {
    theDimAspect->SetTextHorizontalPosition(Prs3d_DTHP_Left);
    theDimAspect->SetArrowOrientation(Prs3d_DAO_External);
    theDimAspect->SetExtensionSize(
      (theTextSize / aViewerScale + SketcherPrs_Tools::getArrowSize()) / 2.0);
  } else {
    theDimAspect->SetTextHorizontalPosition(Prs3d_DTHP_Center);
    theDimAspect->SetArrowOrientation(Prs3d_DAO_Internal);
  }
  theDimAspect->SetArrowTailSize(theDimAspect->ArrowAspect()->Length());
  // The value of vertical aligment is sometimes changed
  theDimAspect->TextAspect()->SetVerticalJustification(Graphic3d_VTA_CENTER);
}

void sendEmptyPresentationError(ModelAPI_Feature* theFeature, const std::string theError)
{
  Events_InfoMessage("SketcherPrs_Tools",
    "An empty AIS presentation: SketcherPrs_LengthDimension").send();
  static const Events_ID anEvent = Events_Loop::eventByName(EVENT_EMPTY_AIS_PRESENTATION);
  std::shared_ptr<ModelAPI_Object> aConstraintPtr(theFeature);
  ModelAPI_EventCreator::get()->sendUpdated(aConstraintPtr, anEvent);
}
};
