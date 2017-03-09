// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_WidgetSubShapeSelector.cpp
// Created:     21 Jul 2016
// Author:      Natalia ERMOLAEVA

#include "PartSet_WidgetSubShapeSelector.h"
#include "PartSet_Tools.h"

#include <ModuleBase_ISelection.h>
#include <ModuleBase_ViewerPrs.h>

#include <ModelAPI_Feature.h>
#include <ModelAPI_Tools.h>
#include <GeomDataAPI_Point2D.h>

#include <GeomDataAPI_Point.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Pnt2d.h>

#include <GeomAlgoAPI_ShapeTools.h>
#include <ModelGeomAlgo_Point2D.h>

#include <ModelGeomAlgo_Point2D.h>

#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_Constraint.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_Trim.h>

#include <ModuleBase_IViewWindow.h>
#include <ModuleBase_IWorkshop.h>
#include <ModuleBase_IModule.h>

#include <Config_WidgetAPI.h>

#include <XGUI_Tools.h>
#include <XGUI_Workshop.h>
#include <XGUI_Displayer.h>

#include <QWidget>
#include <QMouseEvent>

PartSet_WidgetSubShapeSelector::PartSet_WidgetSubShapeSelector(QWidget* theParent,
                                                         ModuleBase_IWorkshop* theWorkshop,
                                                         const Config_WidgetAPI* theData)
: ModuleBase_WidgetShapeSelector(theParent, theWorkshop, theData)
{
  myUseGraphicIntersection = theData->getBooleanAttribute("use_graphic_intersection", false);
  myCurrentSubShape = std::shared_ptr<ModuleBase_ViewerPrs>(new ModuleBase_ViewerPrs());
}

PartSet_WidgetSubShapeSelector::~PartSet_WidgetSubShapeSelector()
{
  myCashedShapes.clear();
}

//********************************************************************
void PartSet_WidgetSubShapeSelector::activateCustom()
{
  ModuleBase_WidgetShapeSelector::activateCustom();

  myWorkshop->module()->activateCustomPrs(myFeature,
                            ModuleBase_IModule::CustomizeHighlightedObjects, true);
}

//********************************************************************
void PartSet_WidgetSubShapeSelector::deactivate()
{
  ModuleBase_WidgetShapeSelector::deactivate();

  myWorkshop->module()->deactivateCustomPrs(ModuleBase_IModule::CustomizeHighlightedObjects, true);
}

//********************************************************************
void PartSet_WidgetSubShapeSelector::mouseMoved(ModuleBase_IViewWindow* theWindow,
                                                QMouseEvent* theEvent)
{
  ModuleBase_ISelection* aSelect = myWorkshop->selection();
  QList<ModuleBase_ViewerPrsPtr> aHighlighted = aSelect->getHighlighted();

  if (!aHighlighted.empty()) {
    ModuleBase_ViewerPrsPtr aPrs = aHighlighted.first();
    if (aPrs.get() && aPrs->object().get()) {
      ObjectPtr anObject = aPrs->object();
      if (myCashedShapes.find(anObject) == myCashedShapes.end())
        fillObjectShapes(anObject);
      const std::set<GeomShapePtr>& aShapes = myCashedShapes[anObject];
      if (!aShapes.empty()) {
        gp_Pnt aPnt = PartSet_Tools::convertClickToPoint(theEvent->pos(), theWindow->v3dView());
        double aX, anY;
        Handle(V3d_View) aView = theWindow->v3dView();
        PartSet_Tools::convertTo2D(aPnt, mySketch, aView, aX, anY);
        std::shared_ptr<GeomAPI_Pnt> aPoint = PartSet_Tools::convertTo3D(aX, anY, mySketch);

        std::set<GeomShapePtr>::const_iterator anIt = aShapes.begin(), aLast = aShapes.end();
        for (; anIt != aLast; anIt++) {
          GeomShapePtr aBaseShape = *anIt;
          std::shared_ptr<GeomAPI_Pnt> aProjectedPoint;
          if (ModelGeomAlgo_Point2D::isPointOnEdge(aBaseShape, aPoint, aProjectedPoint)) {
            XGUI_Tools::workshop(myWorkshop)->displayer()->clearSelected(false);
            if (myCurrentSubShape->object() != anObject ||
                myCurrentSubShape->shape() != aBaseShape) {
              myCurrentSubShape->setObject(anObject);
              myCurrentSubShape->setShape(aBaseShape);

              ModuleBase_IModule* aModule = myWorkshop->module();


              if (!aModule->isCustomPrsActivated(ModuleBase_IModule::CustomizeHighlightedObjects))
                aModule->activateCustomPrs(myFeature,
                                           ModuleBase_IModule::CustomizeHighlightedObjects, true);
              aModule->customizeObject(myFeature,
                                       ModuleBase_IModule::CustomizeHighlightedObjects, true);
            }
            else
              XGUI_Tools::workshop(myWorkshop)->displayer()->updateViewer();;
            break;
          }
        }
      }
    }
  }
}

//********************************************************************
void PartSet_WidgetSubShapeSelector::getGeomSelection(const ModuleBase_ViewerPrsPtr& thePrs,
                                                      ObjectPtr& theObject,
                                                      GeomShapePtr& theShape)
{
  ModuleBase_ISelection* aSelection = myWorkshop->selection();
  theObject = aSelection->getResult(thePrs);
  if (!theObject.get() && myCurrentSubShape->object())
    theObject = myCurrentSubShape->object();
}

//********************************************************************
QList<ModuleBase_ViewerPrsPtr> PartSet_WidgetSubShapeSelector::getAttributeSelection() const
{
  return QList<ModuleBase_ViewerPrsPtr>();
}


//********************************************************************
bool PartSet_WidgetSubShapeSelector::setSelection(
                                          QList<std::shared_ptr<ModuleBase_ViewerPrs>>& theValues,
                                          const bool theToValidate)
{
  ObjectPtr aBaseObject = myCurrentSubShape->object();
  GeomShapePtr aBaseShape = myCurrentSubShape->shape();
  bool aResult = aBaseObject.get() && aBaseShape.get();
  // firstly set the selection to the attribute
  if (aResult) {
    QList<ModuleBase_ViewerPrsPtr> aValues;
    aValues.append(myCurrentSubShape);
    aResult = ModuleBase_WidgetShapeSelector::setSelection(aValues, theToValidate);
  }
  // secondly fill additional attributes
  if (aResult) {
    aResult = false;
    if (aBaseShape->shapeType() == GeomAPI_Shape::EDGE) {
      std::shared_ptr<GeomAPI_Edge> anEdge(new GeomAPI_Edge(aBaseShape));

      std::shared_ptr<GeomAPI_Pnt> aFirstPnt = anEdge->firstPoint();
      std::shared_ptr<GeomAPI_Pnt> aLastPnt = anEdge->lastPoint();

      std::shared_ptr<GeomDataAPI_Point2D> aFirstPointAttr, aLastPointAttr;
      /// find the points in base feature attributes
      FeaturePtr aBaseFeature = ModelAPI_Feature::feature(aBaseObject);
      std::list<AttributePtr> a2DPointAttributes = aBaseFeature->data()->attributes(
                                                        GeomDataAPI_Point2D::typeId());
      std::list<AttributePtr>::const_iterator anIt = a2DPointAttributes.begin(),
                                              aLast = a2DPointAttributes.end();
      for (; anIt != aLast; anIt++) {
        std::shared_ptr<GeomDataAPI_Point2D> anAttributePoint =
                                      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(*anIt);
        std::shared_ptr<GeomAPI_Pnt2d> aPoint2D = anAttributePoint->pnt();
        std::shared_ptr<GeomAPI_Pnt> aPoint3D = PartSet_Tools::convertTo3D(aPoint2D->x(),
                                                                   aPoint2D->y(), mySketch);
        if (aFirstPnt->isEqual(aPoint3D))
          aFirstPointAttr = anAttributePoint;
        else if (aLastPnt->isEqual(aPoint3D))
          aLastPointAttr = anAttributePoint;
      }

      /// find the points in coincident features
      PntToAttributesMap aRefAttributes = myCashedReferences[aBaseObject];
      PntToAttributesMap::const_iterator
        aRIt = aRefAttributes.begin(), aRLast = aRefAttributes.end();
      for (PntToAttributesMap::const_iterator aRIt = aRefAttributes.begin(),
           aRLast = aRefAttributes.end(); aRIt != aRLast; aRIt++) {
        std::shared_ptr<GeomDataAPI_Point2D> anAttribute = aRIt->first;
        std::shared_ptr<GeomAPI_Pnt> aPoint = aRIt->second;
        if (!aFirstPointAttr.get() && aFirstPnt->isEqual(aPoint))
          aFirstPointAttr = anAttribute;
        if (!aLastPointAttr.get() && aLastPnt->isEqual(aPoint))
          aLastPointAttr = anAttribute;
        if (aFirstPointAttr.get() && aLastPointAttr.get())
          break;
      }

      /// find the points in objects that intersect the base feature
      ObjectPtr aFirstPointObject, aLastPointObject;
      if (myUseGraphicIntersection && (!aFirstPointAttr.get() || !aLastPointAttr.get())) {
        PntToObjectsMap aRefObjects = myCashedObjects[aBaseObject];
        PntToObjectsMap::const_iterator
          anObjectIt = aRefObjects.begin(), anObjectLast = aRefObjects.end();
        for (; anObjectIt != anObjectLast; anObjectIt++) {
          std::list< std::shared_ptr<ModelAPI_Object> > anObjects = anObjectIt->second;
          if (anObjects.empty())
            continue;
          std::shared_ptr<ModelAPI_Object> anObject = anObjects.front();
          std::shared_ptr<GeomAPI_Pnt> aPoint = anObjectIt->first;
          if (!aFirstPointAttr.get() && !aFirstPointObject.get() && aFirstPnt->isEqual(aPoint))
            aFirstPointObject = anObject;
          if (!aLastPointAttr.get() && !aLastPointObject.get() && aLastPnt->isEqual(aPoint))
            aLastPointObject = anObject;
          if (aFirstPointAttr.get() && aLastPointAttr.get())
            break;
        }
      }

      if ((!aFirstPointAttr.get() && !aFirstPointObject.get()) ||
          (!aLastPointAttr.get() && !aLastPointObject.get()))
        return false;

      FeaturePtr aFeature = feature();
      AttributeRefAttrPtr anAPointAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
                                          aFeature->attribute(SketchPlugin_Constraint::ENTITY_A()));
      AttributeRefAttrPtr aBPointAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(
                                          aFeature->attribute(SketchPlugin_Constraint::ENTITY_B()));
      if (aFirstPointAttr.get())
        anAPointAttr->setAttr(aFirstPointAttr);
      else
        anAPointAttr->setObject(aFirstPointObject);

      if (aLastPointAttr.get())
        aBPointAttr->setAttr(aLastPointAttr);
      else
        aBPointAttr->setObject(aLastPointObject);

      /*if (myUseGraphicIntersection) {
        // fill geometrical points
        AttributePtr anAPointAttr = aFeature->attribute(SketchPlugin_Trim::ENTITY_A_POINT());
        AttributePtr aBPointAttr = aFeature->attribute(SketchPlugin_Trim::ENTITY_B_POINT());
        if (anAPointAttr.get() && aBPointAttr.get()) {
          std::shared_ptr<GeomAPI_Pln> aPlane = PartSet_Tools::sketchPlane(sketch());

          std::shared_ptr<GeomDataAPI_Point2D> anAPoint =
                                std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAPointAttr);
          anAPoint->setValue(aFirstPnt->to2D(aPlane));

          std::shared_ptr<GeomDataAPI_Point2D> aBPoint =
                                std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aBPointAttr);
          aBPoint->setValue(aLastPnt->to2D(aPlane));
        }
      }*/
      aResult = true;
    }
  }

  return aResult;
}

//********************************************************************
void PartSet_WidgetSubShapeSelector::getHighlighted(
                           QList<std::shared_ptr<ModuleBase_ViewerPrs>>& theValues)
{
  if (myCurrentSubShape.get() && myCurrentSubShape->object().get())
    theValues.append(myCurrentSubShape);
}

//********************************************************************
void PartSet_WidgetSubShapeSelector::fillObjectShapes(const ObjectPtr& theObject)
{
  std::set<std::shared_ptr<GeomAPI_Shape> > aShapes;
  std::map<std::shared_ptr<GeomDataAPI_Point2D>, std::shared_ptr<GeomAPI_Pnt> > aPointToAttributes;
   std::map<std::shared_ptr<GeomAPI_Pnt>,
                              std::list< std::shared_ptr<ModelAPI_Object> > > aPointToObjects;

  std::set<std::shared_ptr<GeomDataAPI_Point2D> > aRefAttributes;
  // current feature
  FeaturePtr aFeature = ModelAPI_Feature::feature(theObject);
  std::set<ResultPtr> anEdgeShapes;
  // edges on feature
  ModelAPI_Tools::shapesOfType(aFeature, GeomAPI_Shape::EDGE, anEdgeShapes);
  if (!anEdgeShapes.empty()) {
    GeomShapePtr aFeatureShape = (*anEdgeShapes.begin())->shape();

    // coincidences to the feature
    std::list<std::shared_ptr<GeomAPI_Pnt> > aPoints;

    ModelGeomAlgo_Point2D::getPointsOfReference(aFeature, SketchPlugin_ConstraintCoincidence::ID(),
                         aRefAttributes, SketchPlugin_Point::ID(), SketchPlugin_Point::COORD_ID());
    // layed on feature coincidences to divide it on several shapes
    CompositeFeaturePtr aSketch = sketch();
    std::shared_ptr<ModelAPI_Data> aData = aSketch->data();
    std::shared_ptr<GeomDataAPI_Point> aC = std::dynamic_pointer_cast<GeomDataAPI_Point>(
        aData->attribute(SketchPlugin_Sketch::ORIGIN_ID()));
    std::shared_ptr<GeomDataAPI_Dir> aX = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
        aData->attribute(SketchPlugin_Sketch::DIRX_ID()));
    std::shared_ptr<GeomDataAPI_Dir> aNorm = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
        aData->attribute(SketchPlugin_Sketch::NORM_ID()));
    std::shared_ptr<GeomAPI_Dir> aY(new GeomAPI_Dir(aNorm->dir()->cross(aX->dir())));
    ModelGeomAlgo_Point2D::getPointsInsideShape_p(aFeatureShape, aRefAttributes, aC->pnt(),
                                                aX->dir(), aY, aPoints, aPointToAttributes);

    // intersection points
    /*if (myUseGraphicIntersection) {
      std::list<FeaturePtr> aFeatures;
      for (int i = 0; i < aSketch->numberOfSubs(); i++) {
        FeaturePtr aFeature = aSketch->subFeature(i);
        if (aFeature.get())
          aFeatures.push_back(aFeature);
      }
      ModelGeomAlgo_Point2D::getPointsIntersectedShape(aFeature, aFeatures, aPoints,
                                                       aPointToObjects);
    }*/
    GeomAlgoAPI_ShapeTools::splitShape_p(aFeatureShape, aPoints, aShapes);
  }
  myCashedShapes[theObject] = aShapes;
  myCashedReferences[theObject] = aPointToAttributes;
  if (myUseGraphicIntersection)
    myCashedObjects[theObject] = aPointToObjects;
}

