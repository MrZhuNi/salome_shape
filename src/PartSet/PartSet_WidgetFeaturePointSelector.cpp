// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_WidgetFeaturePointSelector.cpp
// Created:     28 Feb 2017
// Author:      Natalia ERMOLAEVA

#include <Config_WidgetAPI.h>

#include <Events_Loop.h>

#include <GeomDataAPI_Point2D.h>
#include <GeomDataAPI_Point.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAlgoAPI_ShapeTools.h>

#include <ModuleBase_ISelection.h>
#include <ModuleBase_ViewerPrs.h>

#include <ModelAPI_Events.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Tools.h>

#include <ModuleBase_IViewWindow.h>
#include <ModuleBase_IWorkshop.h>
#include <ModuleBase_IModule.h>
#include <ModelGeomAlgo_Point2D.h>

#include "PartSet_WidgetFeaturePointSelector.h"
#include "PartSet_Tools.h"

#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_Constraint.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_Trim.h>

#include <XGUI_Tools.h>
#include <XGUI_Workshop.h>
#include <XGUI_Displayer.h>
#include <XGUI_ViewerProxy.h>

#include <QWidget>
#include <QMouseEvent>

#define HIGHLIGHT_STAYS_PROBLEM
#ifdef HIGHLIGHT_STAYS_PROBLEM
#include <Quantity_Color.hxx>
#define SKETCH_ENTITY_COLOR "225,0,0"
#endif

PartSet_WidgetFeaturePointSelector::PartSet_WidgetFeaturePointSelector(QWidget* theParent,
                                                         ModuleBase_IWorkshop* theWorkshop,
                                                         const Config_WidgetAPI* theData)
: ModuleBase_WidgetShapeSelector(theParent, theWorkshop, theData)
{
  myUseGraphicIntersection = theData->getBooleanAttribute("use_graphic_intersection", false);
  //myCurrentSubShape = std::shared_ptr<ModuleBase_ViewerPrs>(new ModuleBase_ViewerPrs());
}

PartSet_WidgetFeaturePointSelector::~PartSet_WidgetFeaturePointSelector()
{
  //myCashedShapes.clear();
}

//********************************************************************
bool PartSet_WidgetFeaturePointSelector::isValidSelection(
                                        const std::shared_ptr<ModuleBase_ViewerPrs>& theValue)
{
  return true;
}

//********************************************************************
void PartSet_WidgetFeaturePointSelector::activateCustom()
{
  ModuleBase_WidgetShapeSelector::activateCustom();

  myWorkshop->module()->activateCustomPrs(myFeature,
                            ModuleBase_IModule::CustomizeHighlightedObjects, true);

  Handle(AIS_InteractiveContext) aContext =
                          XGUI_Tools::workshop(myWorkshop)->viewer()->AISContext();

  std::vector<int> aColors;
  aColors = Config_PropManager::color("Visualization", "sketch_entity_color",
                                     SKETCH_ENTITY_COLOR);
  Quantity_Color aColor(aColors[0] / 255., aColors[1] / 255., aColors[2] / 255., Quantity_TOC_RGB);

#ifdef HIGHLIGHT_STAYS_PROBLEM
  Handle(Graphic3d_HighlightStyle) aHStyle = aContext->HighlightStyle();
  myHighlightColor = aHStyle->Color();
  aHStyle->SetColor(aColor);
  aContext->SetHighlightStyle(aHStyle);

  Handle(Graphic3d_HighlightStyle) aSStyle = aContext->SelectionStyle();
  mySelectionColor = aSStyle->Color();
  aSStyle->SetColor(aColor);
  aContext->SetSelectionStyle(aSStyle);
#endif
}

//********************************************************************
void PartSet_WidgetFeaturePointSelector::deactivate()
{
  ModuleBase_WidgetShapeSelector::deactivate();

  Handle(AIS_InteractiveContext) aContext =
                          XGUI_Tools::workshop(myWorkshop)->viewer()->AISContext();

#ifdef HIGHLIGHT_STAYS_PROBLEM
  Handle(Graphic3d_HighlightStyle) aHStyle = aContext->HighlightStyle();
  aHStyle->SetColor(myHighlightColor);
  aContext->SetHighlightStyle(aHStyle);

  Handle(Graphic3d_HighlightStyle) aSStyle = aContext->SelectionStyle();
  aSStyle->SetColor(mySelectionColor);
  aContext->SetSelectionStyle(aSStyle);
#endif
  //myWorkshop->module()->deactivateCustomPrs(ModuleBase_IModule::CustomizeHighlightedObjects, true);
}

//********************************************************************
void PartSet_WidgetFeaturePointSelector::mouseMoved(ModuleBase_IViewWindow* theWindow,
                                                    QMouseEvent* theEvent)
{
  ModuleBase_ISelection* aSelect = myWorkshop->selection();
  QList<ModuleBase_ViewerPrsPtr> aHighlighted = aSelect->getHighlighted();

  if (!aHighlighted.empty()) {
    ModuleBase_ViewerPrsPtr aPrs = aHighlighted.first();
    fillFeature(aPrs, theWindow, theEvent);
  }
}

//********************************************************************
void PartSet_WidgetFeaturePointSelector::mouseReleased(ModuleBase_IViewWindow* theWindow,
                                                       QMouseEvent* theEvent)
{
  // the contex menu release by the right button should not be processed by this widget
  if (theEvent->button() != Qt::LeftButton)
    return;

  emit focusOutWidget(this);
}

//********************************************************************
bool PartSet_WidgetFeaturePointSelector::fillFeature(
                            const std::shared_ptr<ModuleBase_ViewerPrs>& theSelectedPrs,
                            ModuleBase_IViewWindow* theWindow,
                            QMouseEvent* theEvent)
{
  bool aFilled = false;
  if (theSelectedPrs.get() && theSelectedPrs->object().get()) {
    ObjectPtr anObject = theSelectedPrs->object();
    //if (myCashedShapes.find(anObject) == myCashedShapes.end())
    //  fillObjectShapes(anObject);
    //const std::set<GeomShapePtr>& aShapes = myCashedShapes[anObject];
    //if (!aShapes.empty()) {
    gp_Pnt aPnt = PartSet_Tools::convertClickToPoint(theEvent->pos(), theWindow->v3dView());
    double aX, anY;
    Handle(V3d_View) aView = theWindow->v3dView();
    PartSet_Tools::convertTo2D(aPnt, mySketch, aView, aX, anY);
    //std::shared_ptr<GeomAPI_Pnt> aPoint = PartSet_Tools::convertTo3D(aX, anY, mySketch);

    std::shared_ptr<ModelAPI_AttributeReference> aRef =
                            std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                            feature()->data()->attribute(SketchPlugin_Trim::BASE_OBJECT()));
    aRef->setValue(anObject);

    std::shared_ptr<GeomDataAPI_Point2D> anAttributePoint =
                    std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                    feature()->data()->attribute(SketchPlugin_Trim::ENTITY_POINT()));
    //std::shared_ptr<GeomAPI_Pnt2d> aPoint2D = anAttributePoint->pnt();
    anAttributePoint->setValue(aX, anY);
    // redisplay AIS presentation in viewer
#ifndef HIGHLIGHT_STAYS_PROBLEM
    // an attempt to clear highlighted item in the viewer: but of OCCT
    XGUI_Tools::workshop(myWorkshop)->displayer()->clearSelected(true);
#endif
    updateObject(feature());
    aFilled = true;

    /*
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
              XGUI_Tools::workshop(myWorkshop)->displayer()->updateViewer();
            break;
          }
        }*/
      //}
    //}
  }
  return aFilled;
}

//********************************************************************
/*void PartSet_WidgetFeaturePointSelector::getGeomSelection(const ModuleBase_ViewerPrsPtr& thePrs,
                                                      ObjectPtr& theObject,
                                                      GeomShapePtr& theShape)
{
  ModuleBase_ISelection* aSelection = myWorkshop->selection();
  theObject = aSelection->getResult(thePrs);
  if (!theObject.get() && myCurrentSubShape->object())
    theObject = myCurrentSubShape->object();
}*/

//********************************************************************
QList<ModuleBase_ViewerPrsPtr> PartSet_WidgetFeaturePointSelector::getAttributeSelection() const
{
  return QList<ModuleBase_ViewerPrsPtr>();
}


//********************************************************************
bool PartSet_WidgetFeaturePointSelector::setSelection(
                                          QList<std::shared_ptr<ModuleBase_ViewerPrs>>& theValues,
                                          const bool theToValidate)
{
  return false;
  //return !theValues.empty();

  /*ObjectPtr aBaseObject = myCurrentSubShape->object();
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

      if (myUseGraphicIntersection) {
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
      }
      aResult = true;
    }
  }
  return aResult;*/
  return false;
}

void PartSet_WidgetFeaturePointSelector::setPreSelection(
                                  const std::shared_ptr<ModuleBase_ViewerPrs>& thePreSelected,
                                  ModuleBase_IViewWindow* theWnd,
                                  QMouseEvent* theEvent)
{
  if (fillFeature(thePreSelected, theWnd, theEvent))
    mouseReleased(theWnd, theEvent);
}

//********************************************************************
/*bool PartSet_WidgetFeaturePointSelector::isValidSelectionCustom(
                                         const std::shared_ptr<ModuleBase_ViewerPrs>& thePrs)
{
   // as we are modfying the attribute in move, we should not check validity here, by highlight
  return false;
}*/

//********************************************************************
/*void PartSet_WidgetFeaturePointSelector::getHighlighted(
                           QList<std::shared_ptr<ModuleBase_ViewerPrs>>& theValues)
{
  if (myCurrentSubShape.get() && myCurrentSubShape->object().get())
    theValues.append(myCurrentSubShape);
}*/

//********************************************************************
/*void PartSet_WidgetFeaturePointSelector::fillObjectShapes(const ObjectPtr& theObject)
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
    ModelGeomAlgo_Point2D::getPointsInsideShape(aFeatureShape, aRefAttributes, aC->pnt(),
                                                aX->dir(), aY, aPoints, aPointToAttributes);

    // intersection points
    if (myUseGraphicIntersection) {
      std::list<FeaturePtr> aFeatures;
      for (int i = 0; i < aSketch->numberOfSubs(); i++) {
        FeaturePtr aFeature = aSketch->subFeature(i);
        if (aFeature.get())
          aFeatures.push_back(aFeature);
      }
      ModelGeomAlgo_Point2D::getPointsIntersectedShape(aFeature, aFeatures, aPoints,
                                                       aPointToObjects);
    }
    GeomAlgoAPI_ShapeTools::splitShape(aFeatureShape, aPoints, aShapes);
  }
  myCashedShapes[theObject] = aShapes;
  myCashedReferences[theObject] = aPointToAttributes;
  if (myUseGraphicIntersection)
    myCashedObjects[theObject] = aPointToObjects;
}
*/
