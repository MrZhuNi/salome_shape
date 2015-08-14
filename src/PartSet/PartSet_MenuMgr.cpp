// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_MenuMgr.cpp
// Created:     03 April 2015
// Author:      Vitaly SMETANNIKOV

#include "PartSet_MenuMgr.h"
#include "PartSet_Module.h"
#include "PartSet_SketcherMgr.h"
#include "PartSet_Tools.h"

#include <PartSetPlugin_Part.h>

#include <GeomAPI_Pnt2d.h>
#include <GeomDataAPI_Point2D.h>

#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_Sketch.h>

#include <ModuleBase_ISelection.h>
#include <ModuleBase_Operation.h>

#include <XGUI_ModuleConnector.h>
#include <XGUI_Workshop.h>
#include <XGUI_Displayer.h>
#include <XGUI_DataModel.h>
#include <XGUI_ObjectsBrowser.h>

#include <Events_Loop.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_ResultParameter.h>

#include <QAction>
#include <QMenu>

#include <TopoDS.hxx>
#include <BRep_Tool.hxx>

PartSet_MenuMgr::PartSet_MenuMgr(PartSet_Module* theModule)
  : QObject(theModule), myModule(theModule), myPrevId(-1)
{
  createActions();
}


QAction* PartSet_MenuMgr::action(const QString& theId) const
{
  if (myActions.contains(theId))
    return myActions[theId];
  return 0;
}

void PartSet_MenuMgr::addAction(const QString& theId, QAction* theAction)
{
  if (myActions.contains(theId))
    qCritical("A command with Id = '%s' already defined!", qPrintable(theId));
  theAction->setData(theId);
  connect(theAction, SIGNAL(triggered(bool)), this, SLOT(onAction(bool)));
  myActions[theId] = theAction;
}

void PartSet_MenuMgr::createActions()
{
  QAction* aAction;

  aAction = new QAction(tr("Auxiliary"), this);
  aAction->setCheckable(true);
  addAction("AUXILIARY_CMD", aAction);

  aAction = new QAction(QIcon(":icons/activate.png"), tr("Activate"), this);
  connect(aAction, SIGNAL(triggered(bool)), this, SLOT(onActivatePart(bool)));
  myActions["ACTIVATE_PART_CMD"] = aAction;

  aAction = new QAction(QIcon(":icons/deactivate.png"), tr("Deactivate"), this);
  connect(aAction, SIGNAL(triggered(bool)), this, SLOT(onActivatePartSet(bool)));
  myActions["DEACTIVATE_PART_CMD"] = aAction;

  // Activate PartSet
  aAction = new QAction(QIcon(":icons/activate.png"), tr("Activate"), this);
  connect(aAction, SIGNAL(triggered(bool)), this, SLOT(onActivatePartSet(bool)));
  myActions["ACTIVATE_PARTSET_CMD"] = aAction;

  aAction = new QAction(QIcon(":icons/edit.png"), tr("Edit..."), this);
  connect(aAction, SIGNAL(triggered(bool)), this, SLOT(onEdit(bool)));
  myActions["EDIT_CMD"] = aAction;

  aAction = new QAction(QIcon(), tr("Select parent feature"), this);
  aAction->setCheckable(false);
  connect(aAction, SIGNAL(triggered(bool)), this, SLOT(onSelectParentFeature()));
  myActions["SELECT_PARENT_CMD"] = aAction;
}


void PartSet_MenuMgr::onAction(bool isChecked)
{
  QAction* aAction = static_cast<QAction*>(sender());
  QString anId = aAction->data().toString();

  if (anId == "AUXILIARY_CMD") {
    setAuxiliary(isChecked);
  }
}

bool PartSet_MenuMgr::addViewerMenu(QMenu* theMenu, const QMap<QString, QAction*>& theStdActions) const
{
  ModuleBase_Operation* anOperation = myModule->workshop()->currentOperation();
  if (!PartSet_SketcherMgr::isSketchOperation(anOperation) &&
      !PartSet_SketcherMgr::isNestedSketchOperation(anOperation))
    return false;

  myCoinsideLines.clear();
  ModuleBase_ISelection* aSelection = myModule->workshop()->selection();

  bool aIsDetach = false;
  bool hasAttribute = false;
  bool hasFeature = false;

  QList<ModuleBase_ViewerPrs> aPrsList = aSelection->getSelected(ModuleBase_ISelection::Viewer);
  TopoDS_Shape aShape;
  ResultPtr aResult;
  FeaturePtr aFeature;
  foreach(ModuleBase_ViewerPrs aPrs, aPrsList) {
    aResult = std::dynamic_pointer_cast<ModelAPI_Result>(aPrs.object());
    if (aResult.get() != NULL) {
      aShape = aPrs.shape();
      if (aShape.IsEqual(aResult->shape()->impl<TopoDS_Shape>()))
        hasFeature = true;
      else
        hasAttribute = true;
    } else {
      aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aPrs.object());
      hasFeature = (aFeature.get() != NULL);
    }
  }

  if (aPrsList.size() == 1) {
    TopoDS_Shape aShape = aPrsList.first().shape();
    if ((!aShape.IsNull()) && aShape.ShapeType() == TopAbs_VERTEX) {
      // Find 2d coordinates
      FeaturePtr aSketchFea = myModule->sketchMgr()->activeSketch();
      if (aSketchFea->getKind() == SketchPlugin_Sketch::ID()) {
        gp_Pnt aPnt = BRep_Tool::Pnt(TopoDS::Vertex(aShape));
        std::shared_ptr<GeomAPI_Pnt> aPnt3d(new GeomAPI_Pnt(aPnt.X(), aPnt.Y(), aPnt.Z()));
        std::shared_ptr<GeomAPI_Pnt2d> aSelPnt = PartSet_Tools::convertTo2D(aSketchFea, aPnt3d);

        // Find coincident in these coordinates
        ObjectPtr aObj = aPrsList.first().object();
        FeaturePtr aFeature = ModelAPI_Feature::feature(aObj);
        const std::set<AttributePtr>& aRefsList = aFeature->data()->refsToMe();
        std::set<AttributePtr>::const_iterator aIt;
        FeaturePtr aCoincident;
        for (aIt = aRefsList.cbegin(); aIt != aRefsList.cend(); ++aIt) {
          std::shared_ptr<ModelAPI_Attribute> aAttr = (*aIt);
          FeaturePtr aConstrFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aAttr->owner());
          if (aConstrFeature->getKind() == SketchPlugin_ConstraintCoincidence::ID()) { 
            std::shared_ptr<GeomAPI_Pnt2d> a2dPnt = 
              PartSet_Tools::getPoint(aConstrFeature, SketchPlugin_ConstraintCoincidence::ENTITY_A());
            if (a2dPnt.get() && aSelPnt->isEqual(a2dPnt)) { 
              aCoincident = aConstrFeature;
              break;
            } else {
              a2dPnt = PartSet_Tools::getPoint(aConstrFeature,
                                               SketchPlugin_ConstraintCoincidence::ENTITY_B());
              if (a2dPnt.get() && aSelPnt->isEqual(a2dPnt)) { 
                aCoincident = aConstrFeature;
                break;
              }
            }
          }
        }
        // If we have coincidence then add Detach menu
        if (aCoincident.get() != NULL) {
          mySelectedFeature = aCoincident;
          PartSet_Tools::findCoincidences(mySelectedFeature, myCoinsideLines,
                                          SketchPlugin_ConstraintCoincidence::ENTITY_A());
          PartSet_Tools::findCoincidences(mySelectedFeature, myCoinsideLines,
                                          SketchPlugin_ConstraintCoincidence::ENTITY_B());
          if (myCoinsideLines.size() > 0) {
            aIsDetach = true;
            QMenu* aSubMenu = theMenu->addMenu(tr("Detach"));
            QAction* aAction;
            int i = 0;
            foreach (FeaturePtr aCoins, myCoinsideLines) {
              aAction = aSubMenu->addAction(aCoins->data()->name().c_str());
              aAction->setData(QVariant(i));
              i++;
            }
            connect(aSubMenu, SIGNAL(hovered(QAction*)), SLOT(onLineHighlighted(QAction*)));
            connect(aSubMenu, SIGNAL(aboutToHide()), SLOT(onDetachMenuHide()));
            connect(aSubMenu, SIGNAL(triggered(QAction*)), SLOT(onLineDetach(QAction*)));
          } 
        }
      }
    }
  }
  if ((!aIsDetach) && hasFeature) {
    theMenu->addAction(theStdActions["DELETE_CMD"]);
  }
  if (hasAttribute)
    return true;
  bool isAuxiliary;
  if (canSetAuxiliary(isAuxiliary)) {
    QAction* anAction = action("AUXILIARY_CMD");
    theMenu->addAction(anAction);
    anAction->setChecked(isAuxiliary);
  }
  return true;
}

void PartSet_MenuMgr::updateViewerMenu(const QMap<QString, QAction*>& theStdActions)
{
  ModuleBase_Operation* anOperation = myModule->workshop()->currentOperation();

  bool isActiveSketch = PartSet_SketcherMgr::isSketchOperation(anOperation) ||
                        PartSet_SketcherMgr::isNestedSketchOperation(anOperation);
  if (isActiveSketch) {
    theStdActions["WIREFRAME_CMD"]->setEnabled(false);
    theStdActions["SHADING_CMD"]->setEnabled(false);
    theStdActions["SHOW_ONLY_CMD"]->setEnabled(false);
    theStdActions["SHOW_CMD"]->setEnabled(false);
    theStdActions["HIDE_CMD"]->setEnabled(false);
    theStdActions["HIDEALL_CMD"]->setEnabled(false);
  }
}


void PartSet_MenuMgr::onLineHighlighted(QAction* theAction)
{
  if (myPrevId != -1) {
    // Restore color for previous object
    setLineColor(myPrevId, myColor, false);
  }
  myPrevId = theAction->data().toInt();
  myColor = setLineColor(myPrevId, Qt::white, true);
}

QColor PartSet_MenuMgr::setLineColor(int theId, const QColor theColor, bool theUpdate)
{
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(myModule->workshop());
  XGUI_Workshop* aWorkshop = aConnector->workshop();
  XGUI_Displayer* aDisplayer = aWorkshop->displayer();

  FeaturePtr aLine = myCoinsideLines.at(myPrevId);
  std::list<ResultPtr>::const_iterator aIt;
  const std::list<ResultPtr>& aResults = aLine->results();
  QColor aColor;
  for (aIt = aResults.cbegin(); aIt != aResults.cend(); ++aIt) {
    aColor = aDisplayer->setObjectColor((*aIt), theColor, false);
  }
  if (theUpdate)
    aDisplayer->updateViewer();
  return aColor;
}


void PartSet_MenuMgr::onLineDetach(QAction* theAction)
{
  int aId = theAction->data().toInt();
  FeaturePtr aLine = myCoinsideLines.at(aId);
  std::shared_ptr<GeomAPI_Pnt2d> aOrig = PartSet_Tools::getPoint(mySelectedFeature,
                                                        SketchPlugin_ConstraintCoincidence::ENTITY_A());
  if (aOrig.get() == NULL)
    aOrig = PartSet_Tools::getPoint(mySelectedFeature,
                                    SketchPlugin_ConstraintCoincidence::ENTITY_B());
  
  gp_Pnt aOr = aOrig->impl<gp_Pnt>();
  const std::set<AttributePtr>& aRefsList = aLine->data()->refsToMe();

  QObjectPtrList aToDelFeatures;
  std::set<AttributePtr>::const_iterator aIt;
  // Find all coincedences corresponded to the selected line in the selected point
  for (aIt = aRefsList.cbegin(); aIt != aRefsList.cend(); ++aIt) {
    std::shared_ptr<ModelAPI_Attribute> aAttr = (*aIt);
    FeaturePtr aConstrFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aAttr->owner());
    if (aConstrFeature->getKind() == SketchPlugin_ConstraintCoincidence::ID()) { 
      std::shared_ptr<GeomAPI_Pnt2d> aPnt = PartSet_Tools::getPoint(aConstrFeature,
                                            SketchPlugin_ConstraintCoincidence::ENTITY_A());
      if (aPnt.get() == NULL)
        aPnt = PartSet_Tools::getPoint(aConstrFeature,
                                       SketchPlugin_ConstraintCoincidence::ENTITY_B());
      if (aPnt.get() == NULL)
        return;
      gp_Pnt aP = aPnt->impl<gp_Pnt>();
      if (aOrig->isEqual(aPnt)) {
        aToDelFeatures.append(aConstrFeature);
      } else {
        aPnt = PartSet_Tools::getPoint(aConstrFeature,
                                       SketchPlugin_ConstraintCoincidence::ENTITY_B());
        aP = aPnt->impl<gp_Pnt>();
        if (aOrig->isEqual(aPnt)) {
          aToDelFeatures.append(aConstrFeature);
          break;
        }
      }
    }
  }
  if (aToDelFeatures.size() > 0) {
    XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(myModule->workshop());
    XGUI_Workshop* aWorkshop = aConnector->workshop();
    ModuleBase_Operation* anOperation = myModule->workshop()->currentOperation();
    if (PartSet_SketcherMgr::isNestedSketchOperation(anOperation))
      anOperation->abort();

    SessionPtr aMgr = ModelAPI_Session::get();

    QString aName = tr("Detach %1").arg(aLine->data()->name().c_str());
    aMgr->startOperation(aName.toStdString());
    aWorkshop->deleteFeatures(aToDelFeatures);
    aMgr->finishOperation();
  }
  myCoinsideLines.clear();
}


void PartSet_MenuMgr::onDetachMenuHide()
{
  if (myPrevId != -1) {
    // Restore color for previous object
    setLineColor(myPrevId, myColor, false);
  }
  // Clear previous definitions
  myPrevId = -1;
}

  
void PartSet_MenuMgr::setAuxiliary(const bool isChecked)
{
  ModuleBase_Operation* anOperation = myModule->workshop()->currentOperation();

  bool isActiveSketch = PartSet_SketcherMgr::isSketchOperation(anOperation) ||
                        PartSet_SketcherMgr::isNestedSketchOperation(anOperation);
  if (!isActiveSketch)
    return;

  QObjectPtrList anObjects;
  bool isUseTransaction = false;
  // 1. change auxiliary type of a created feature
  if (PartSet_SketcherMgr::isNestedCreateOperation(anOperation) &&
    PartSet_SketcherMgr::isEntity(anOperation->id().toStdString()) ) {
    anObjects.append(anOperation->feature());
  }
  else {
    isUseTransaction = true;
    // 2. change auxiliary type of selected sketch entities
    ModuleBase_ISelection* aSelection = myModule->workshop()->selection();
    anObjects = aSelection->selectedPresentations();
  }

  QAction* anAction = action("AUXILIARY_CMD");
  SessionPtr aMgr = ModelAPI_Session::get();
  if (isUseTransaction) {
    if (PartSet_SketcherMgr::isNestedSketchOperation(anOperation))
      anOperation->abort();
    aMgr->startOperation(anAction->text().toStdString());
  }
  myModule->sketchMgr()->storeSelection();

  if (anObjects.size() > 0) {
    QObjectPtrList::const_iterator anIt = anObjects.begin(), aLast = anObjects.end();
    for (; anIt != aLast; anIt++) {
      FeaturePtr aFeature = ModelAPI_Feature::feature(*anIt);
      if (aFeature.get() != NULL) {
        std::shared_ptr<SketchPlugin_Feature> aSketchFeature =
                            std::dynamic_pointer_cast<SketchPlugin_Feature>(aFeature);
        if (aSketchFeature.get() != NULL) {
          std::string anAttribute = SketchPlugin_SketchEntity::AUXILIARY_ID();

          std::shared_ptr<ModelAPI_AttributeBoolean> anAuxiliaryAttr = 
            std::dynamic_pointer_cast<ModelAPI_AttributeBoolean>(aSketchFeature->data()->attribute(anAttribute));
          if (anAuxiliaryAttr)
            anAuxiliaryAttr->setValue(isChecked);
        }
      }
    }
  }
  if (isUseTransaction) {
    aMgr->finishOperation();
    XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(myModule->workshop());
    XGUI_Workshop* aWorkshop = aConnector->workshop();
    aWorkshop->updateCommandStatus();
  }

  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
  myModule->sketchMgr()->restoreSelection();
}

bool PartSet_MenuMgr::canSetAuxiliary(bool& theValue) const
{
  bool anEnabled = false;
  ModuleBase_Operation* anOperation = myModule->workshop()->currentOperation();

  bool isActiveSketch = PartSet_SketcherMgr::isSketchOperation(anOperation) ||
                        PartSet_SketcherMgr::isNestedSketchOperation(anOperation);
  if (!isActiveSketch)
    return anEnabled;

  QObjectPtrList anObjects;
  // 1. change auxiliary type of a created feature
  if (PartSet_SketcherMgr::isNestedCreateOperation(anOperation) &&
    PartSet_SketcherMgr::isEntity(anOperation->id().toStdString()) ) {
    anObjects.append(anOperation->feature());
  }
  else {
    /// The operation should not be aborted here, because the method does not changed
    /// the auxilliary state, but checks the possibility to perform this
    ///if (PartSet_SketcherMgr::isNestedSketchOperation(anOperation))
    ///  anOperation->abort();
    // 2. change auxiliary type of selected sketch entities
    ModuleBase_ISelection* aSelection = myModule->workshop()->selection();
    anObjects = aSelection->selectedPresentations();
  }

  bool isNotAuxiliaryFound = false;
  if (anObjects.size() > 0) {
    QObjectPtrList::const_iterator anIt = anObjects.begin(), aLast = anObjects.end();
    for (; anIt != aLast && !isNotAuxiliaryFound; anIt++) {
      FeaturePtr aFeature = ModelAPI_Feature::feature(*anIt);
      if ((aFeature.get() != NULL) && PartSet_SketcherMgr::isEntity(aFeature->getKind())) {
        anEnabled = true;
        std::shared_ptr<SketchPlugin_Feature> aSketchFeature =
                            std::dynamic_pointer_cast<SketchPlugin_Feature>(aFeature);
        if (aSketchFeature.get() != NULL) {
          std::string anAttribute = SketchPlugin_SketchEntity::AUXILIARY_ID();

          std::shared_ptr<ModelAPI_AttributeBoolean> anAuxiliaryAttr = 
            std::dynamic_pointer_cast<ModelAPI_AttributeBoolean>(aSketchFeature->data()->attribute(anAttribute));
          if (anAuxiliaryAttr)
            isNotAuxiliaryFound = !anAuxiliaryAttr->value();
        }
      }
    }
  }
  theValue = anObjects.size() && !isNotAuxiliaryFound;
  return anEnabled;
}

void PartSet_MenuMgr::onActivatePart(bool)
{
  if (myModule->workshop()->currentOperation())
    return;
  QObjectPtrList aObjects = myModule->workshop()->selection()->selectedObjects();
  if (aObjects.size() > 0) {
    ObjectPtr aObj = aObjects.first();
    ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aObj);
    if (!aPart.get()) {
      FeaturePtr aPartFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aObj);
      if (aPartFeature.get() && (aPartFeature->getKind() == PartSetPlugin_Part::ID())) {
        aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aPartFeature->firstResult());
      }
    }
    if (aPart.get())
      aPart->activate();
  }
}

void PartSet_MenuMgr::onActivatePartSet(bool)
{
  if (myModule->workshop()->currentOperation())
    return;
  SessionPtr aMgr = ModelAPI_Session::get();
  bool isNewTransaction = !aMgr->isOperation();
  // activation may cause changes in current features in document, so it must be in transaction
  if (isNewTransaction) {
    aMgr->startOperation("Activation");
  }
  aMgr->setActiveDocument(aMgr->moduleDocument());
  if (isNewTransaction) {
    aMgr->finishOperation();
  }
}

void PartSet_MenuMgr::onEdit(bool)
{
  QObjectPtrList aObjects = myModule->workshop()->selection()->selectedObjects();
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aObjects.first());
  if (aFeature == NULL) {
    ResultParameterPtr aParam = 
      std::dynamic_pointer_cast<ModelAPI_ResultParameter>(aObjects.first());
    if (aParam.get() != NULL) {
      aFeature = ModelAPI_Feature::feature(aParam);
    }
  }
  if (aFeature.get() != NULL)
    myModule->editFeature(aFeature);
}

void PartSet_MenuMgr::onSelectParentFeature()
{
  QObjectPtrList aObjects = myModule->workshop()->selection()->selectedObjects();
  if (aObjects.size() != 1)
    return;

  SessionPtr aMgr = ModelAPI_Session::get();
  ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>( aObjects.first() );
  if( !aResult.get() )
    return;

  FeaturePtr aParentFeature = aResult->document()->feature( aResult );
  QObjectPtrList aSelection;
  aSelection.append( aParentFeature );
  myModule->workshop()->selection()->setSelectedObjects( aSelection );
}
