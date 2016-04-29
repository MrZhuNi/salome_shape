// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        XGUI_Selection.cpp
// Created:     8 July 2014
// Author:      Vitaly SMETANNIKOV

#include "XGUI_Selection.h"
#include "XGUI_Workshop.h"
#include "XGUI_Displayer.h"
#include "XGUI_ViewerProxy.h"
#include "XGUI_ObjectsBrowser.h"

#include "ModuleBase_ResultPrs.h"
#include <ModuleBase_ViewerPrs.h>

#include <ModelAPI_Feature.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_ResultConstruction.h>

#include <AIS_InteractiveContext.hxx>
#include <AIS_Axis.hxx>
#include <AIS_Point.hxx>
#include <Geom_Line.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS_Edge.hxx>
#include <Geom_Point.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Prs3d_DatumAspect.hxx>

#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <StdSelect_BRepOwner.hxx>

#include <set>

#define DEBUG_DELIVERY

XGUI_Selection::XGUI_Selection(XGUI_Workshop* theWorkshop)
    : myWorkshop(theWorkshop)
{
}

QList<ModuleBase_ViewerPrsPtr> XGUI_Selection::getSelected(const SelectionPlace& thePlace) const
{
  QList<ModuleBase_ViewerPrsPtr> aPresentations;

  switch (thePlace) {
    case Browser:
      getSelectedInBrowser(aPresentations);
    break;
    case Viewer:
      getSelectedInViewer(aPresentations);
    break;
  case AllControls:
      getSelectedInViewer(aPresentations);
      getSelectedInBrowser(aPresentations);
    break;
  }
  return aPresentations;
}

Handle(AIS_InteractiveObject) XGUI_Selection::getIO(const ModuleBase_ViewerPrsPtr& thePrs)
{
  Handle(AIS_InteractiveObject) anIO = thePrs->interactive();
  if (anIO.IsNull()) {
    Handle(SelectMgr_EntityOwner) anOwner = thePrs->owner();
    if (!anOwner.IsNull())
      anIO = Handle(AIS_InteractiveObject)::DownCast(anOwner->Selectable());

    if (anIO.IsNull() && thePrs->object()) {
      XGUI_Displayer* aDisplayer = myWorkshop->displayer();
      AISObjectPtr anAISObject = aDisplayer->getAISObject(thePrs->object());
      if (anAISObject.get())
        anIO = anAISObject->impl<Handle(AIS_InteractiveObject)>();
    }
  }
  return anIO;
}

void XGUI_Selection::getSelectedInViewer(QList<ModuleBase_ViewerPrsPtr>& thePresentations) const
{
  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  if (!aContext.IsNull() && aContext->HasOpenedContext()) {
    QList<long> aSelectedIds; // Remember of selected address in order to avoid duplicates
    for (aContext->InitSelected(); aContext->MoreSelected(); aContext->NextSelected()) {
      ModuleBase_ViewerPrsPtr aPrs(new ModuleBase_ViewerPrs());
      Handle(SelectMgr_EntityOwner) anOwner = aContext->SelectedOwner();

      if (aSelectedIds.contains((long)anOwner.Access()))
        continue;
      aSelectedIds.append((long)anOwner.Access());

      fillPresentation(aPrs, anOwner);

      if (!thePresentations.contains(aPrs)) // TODO: check whether the presentation in a list
        thePresentations.append(aPrs);
    }
  }
}

void XGUI_Selection::getSelectedInBrowser(QList<ModuleBase_ViewerPrsPtr>& thePresentations) const
{
  // collect the objects  of the parameter presentation to avoid a repeted objects in the result
  QObjectPtrList aPresentationObjects;
  QList<ModuleBase_ViewerPrsPtr>::const_iterator aPrsIt = thePresentations.begin(),
                                              aPrsLast = thePresentations.end();
  for (; aPrsIt != aPrsLast; aPrsIt++) {
    aPresentationObjects.push_back((*aPrsIt)->object());
  }

  QObjectPtrList anObjects = selectedObjects();
  QObjectPtrList::const_iterator anIt = anObjects.begin(), aLast = anObjects.end();
  for (; anIt != aLast; anIt++) {
    ObjectPtr anObject = *anIt;
    if (anObject.get() != NULL && !aPresentationObjects.contains(anObject)) {
      thePresentations.append(std::shared_ptr<ModuleBase_ViewerPrs>(
               new ModuleBase_ViewerPrs(anObject, GeomShapePtr(), NULL)));
    }
  }
}

void XGUI_Selection::fillPresentation(ModuleBase_ViewerPrsPtr& thePrs,
                                      const Handle(SelectMgr_EntityOwner)& theOwner) const
{
  thePrs->setOwner(theOwner);

  Handle(AIS_InteractiveObject) anIO = 
                           Handle(AIS_InteractiveObject)::DownCast(theOwner->Selectable());
  thePrs->setInteractive(anIO);

  // we should not check the appearance of this feature because there can be some selected shapes
  // for one feature
  Handle(StdSelect_BRepOwner) aBRO = Handle(StdSelect_BRepOwner)::DownCast(theOwner);
  if( !aBRO.IsNull() && aBRO->HasShape() ) {
    // the located method is called in the context to obtain the shape by the SelectedShape() method,
    // so the shape is located by the same rules
    TopoDS_Shape aShape = aBRO->Shape().Located (aBRO->Location() * aBRO->Shape().Location());
#ifndef DEBUG_DELIVERY
    if (aShape.IsNull())
      aShape = findAxisShape(anIO);
#endif
    if (!aShape.IsNull()) {
      std::shared_ptr<GeomAPI_Shape> aGeomShape = std::shared_ptr<GeomAPI_Shape>(new GeomAPI_Shape());
      aGeomShape->setImpl(new TopoDS_Shape(aShape));
      thePrs->setShape(aGeomShape);
    }
  } else {
#ifdef DEBUG_DELIVERY
    // Fill by trihedron shapes
    Handle(AIS_Axis) aAxis = Handle(AIS_Axis)::DownCast(anIO);
    DocumentPtr aDoc = ModelAPI_Session::get()->moduleDocument();
    int aSize = aDoc->size(ModelAPI_ResultConstruction::group());
    ObjectPtr aObj;
    if (!aAxis.IsNull()) {
      // an Axis from Trihedron
      gp_Lin aLine = aAxis->Component()->Lin();
      gp_Dir aDir = aLine.Direction();
      std::string aAxName;
      if (aDir.X() == 1.) 
        aAxName = "OX";
      else if (aDir.Y() == 1.)
        aAxName = "OY";
      else if (aDir.Z() == 1.)
        aAxName = "OZ";
      if (aAxName.length() > 0) {
        ResultPtr aAx;
        for (int i = 0; i < aSize; i++) {
          aObj = aDoc->object(ModelAPI_ResultConstruction::group(), i);
          if (aObj->data()->name() == aAxName) {
            aAx = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aObj);
            break;
          }
        }
        if (aAx.get()) {
          thePrs->setObject(aAx);
          thePrs->setShape(aAx->shape());
          return;
        }
      }
    } else {
      Handle(AIS_Point) aPoint = Handle(AIS_Point)::DownCast(anIO);
      if (!aPoint.IsNull()) {
        // An origin point from trihedron
        ResultPtr aOrigin;
        for (int i = 0; i < aSize; i++) {
          aObj = aDoc->object(ModelAPI_ResultConstruction::group(), i);
          if (aObj->data()->name() == "Origin") {
            aOrigin = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aObj);
            break;
          }
        }
        if (aOrigin.get()) {
          thePrs->setObject(aOrigin);
          thePrs->setShape(aOrigin->shape());
          return;
        }
      }
    }
#endif
  }
     
  XGUI_Displayer* aDisplayer = myWorkshop->displayer();
  ObjectPtr aFeature = aDisplayer->getObject(anIO);

  Handle(ModuleBase_BRepOwner) aCompSolidBRO = Handle(ModuleBase_BRepOwner)::DownCast(theOwner);
  if (!aCompSolidBRO.IsNull()) {
    // If ModuleBase_BRepOwner object is created then it means that TopAbs_COMPSOLID selection mode
    // is On and we have to use parent result which corresponds to the CompSolid shape
    ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(aFeature);
    if (aResult.get()) {
      ResultCompSolidPtr aCompSolid = ModelAPI_Tools::compSolidOwner(aResult);
      if (aCompSolid.get()) {
        GeomShapePtr aShape = aCompSolid->shape();
        if (aShape.get() && aShape->isEqual(thePrs->shape())) {
          thePrs->setObject(aCompSolid);
          return;
        }
      }
    }
  }
  thePrs->setObject(aFeature);
}

QList<ModuleBase_ViewerPrsPtr> XGUI_Selection::getHighlighted() const
{
  QList<ModuleBase_ViewerPrsPtr> aPresentations;
  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  if (aContext.IsNull())
    return aPresentations;

  QList<long> aSelectedIds; // Remember of selected address in order to avoid duplicates
  XGUI_Displayer* aDisplayer = myWorkshop->displayer();
  for (aContext->InitDetected(); aContext->MoreDetected(); aContext->NextDetected()) {
    ModuleBase_ViewerPrsPtr aPrs(new ModuleBase_ViewerPrs());
    Handle(AIS_InteractiveObject) anIO = aContext->DetectedInteractive();
    if (aSelectedIds.contains((long)anIO.Access()))
      continue;
    
    aSelectedIds.append((long)anIO.Access());
    aPrs->setInteractive(anIO);

    ObjectPtr aResult = aDisplayer->getObject(anIO);
    // we should not check the appearance of this feature because there can be some selected shapes
    // for one feature
    aPrs->setObject(aResult);
    if (aContext->HasOpenedContext()) {
      TopoDS_Shape aShape = aContext->DetectedShape();
      if (!aShape.IsNull()) {
        std::shared_ptr<GeomAPI_Shape> aGeomShape = std::shared_ptr<GeomAPI_Shape>(new GeomAPI_Shape());
        aGeomShape->setImpl(new TopoDS_Shape(aShape));
        aPrs->setShape(aGeomShape);
      }
    }
    aPresentations.push_back(aPrs);
  }
  return aPresentations;
}

QObjectPtrList XGUI_Selection::selectedObjects() const
{
  return myWorkshop->objectBrowser()->selectedObjects();
}

void XGUI_Selection::setSelectedObjects( const QObjectPtrList& theObjects ) const
{
  return myWorkshop->objectBrowser()->setObjectsSelected( theObjects );
}

QObjectPtrList XGUI_Selection::selectedPresentations() const
{
  QObjectPtrList aSelectedList;

  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  if (!aContext.IsNull()) {
    for (aContext->InitSelected(); aContext->MoreSelected(); aContext->NextSelected()) {
      Handle(AIS_InteractiveObject) anIO = aContext->SelectedInteractive();
      ObjectPtr aResult = myWorkshop->displayer()->getObject(anIO);
      if (aResult)
        aSelectedList.append(aResult);
    }
  }
  return aSelectedList;
}

//**************************************************************
QModelIndexList XGUI_Selection::selectedIndexes() const
{
  return myWorkshop->objectBrowser()->selectedIndexes();
}

//**************************************************************
void XGUI_Selection::selectedAISObjects(AIS_ListOfInteractive& theList) const
{
  theList.Clear();

  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  if (!aContext.IsNull()) {
    for (aContext->InitSelected(); aContext->MoreSelected(); aContext->NextSelected())
      theList.Append(aContext->SelectedInteractive());
  }
}

//**************************************************************
ObjectPtr XGUI_Selection::getSelectableObject(const Handle(SelectMgr_EntityOwner)& theOwner) const
{
  ObjectPtr anObject;

  Handle(SelectMgr_EntityOwner) aEO = theOwner;
  if (!aEO.IsNull()) {
    Handle(AIS_InteractiveObject) anObj = 
      Handle(AIS_InteractiveObject)::DownCast(aEO->Selectable());
    anObject = myWorkshop->displayer()->getObject(anObj);
  }
  return anObject;
}

//**************************************************************
void XGUI_Selection::selectedOwners(SelectMgr_IndexedMapOfOwner& theSelectedOwners) const
{
  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  if (!aContext.IsNull()) {
    for (aContext->InitSelected(); aContext->MoreSelected(); aContext->NextSelected()) {
      theSelectedOwners.Add(aContext->SelectedOwner());
    }
  }
}

//**************************************************************
void XGUI_Selection::entityOwners(const Handle(AIS_InteractiveObject)& theObject,
                                  SelectMgr_IndexedMapOfOwner& theOwners) const
{
  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  if (aContext.IsNull() || theObject.IsNull())
    return;

  TColStd_ListOfInteger aModes;
  aContext->ActivatedModes(theObject, aModes);

  TColStd_ListIteratorOfListOfInteger anIt(aModes);
  for (; anIt.More(); anIt.Next()) {
    int aMode = anIt.Value();
    if (!theObject->HasSelection(aMode))
      continue;

    Handle(SelectMgr_Selection) aSelection = theObject->Selection(aMode);
    for (aSelection->Init(); aSelection->More(); aSelection->Next()) {
      Handle(SelectMgr_SensitiveEntity) anEntity = aSelection->Sensitive();
      if (anEntity.IsNull())
        continue;
      Handle(SelectMgr_EntityOwner) anOwner =
        Handle(SelectMgr_EntityOwner)::DownCast(anEntity->BaseSensitive()->OwnerId());
      if (!anOwner.IsNull())
        theOwners.Add(anOwner);
    }
  }
}

//**************************************************************
TopoDS_Shape XGUI_Selection::findAxisShape(Handle(AIS_InteractiveObject) theIO) const
{
  TopoDS_Shape aShape;
  // Fill by trihedron shapes
  Handle(AIS_Axis) aAxis = Handle(AIS_Axis)::DownCast(theIO);
  if (!aAxis.IsNull()) {
    // an Axis from Trihedron
    Handle(Geom_Line) aLine = aAxis->Component();
    Handle(Prs3d_DatumAspect) DA = aAxis->Attributes()->DatumAspect();
    Handle(Geom_TrimmedCurve) aTLine = new Geom_TrimmedCurve(aLine, 0, DA->FirstAxisLength());

    BRep_Builder aBuilder;      
    TopoDS_Edge aEdge;
    aBuilder.MakeEdge(aEdge, aTLine, Precision::Confusion());
    if (!aEdge.IsNull())
      aShape = aEdge;
  } else {
    Handle(AIS_Point) aPoint = Handle(AIS_Point)::DownCast(theIO);
    if (!aPoint.IsNull()) {
      // A point from trihedron
      Handle(Geom_Point) aPnt = aPoint->Component();
      BRep_Builder aBuilder;
      TopoDS_Vertex aVertex;
      aBuilder.MakeVertex(aVertex, aPnt->Pnt(), Precision::Confusion());
      if (!aVertex.IsNull())
        aShape = aVertex;
    }
  }
  return aShape;
}
