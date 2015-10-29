// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include "PartSet_Module.h"
#include "PartSet_WidgetSketchLabel.h"
#include "PartSet_Validators.h"
#include "PartSet_Tools.h"
#include "PartSet_WidgetPoint2d.h"
#include "PartSet_WidgetPoint2dDistance.h"
#include "PartSet_WidgetPoint2DFlyout.h"
#include "PartSet_WidgetShapeSelector.h"
#include "PartSet_WidgetPoint2dAngle.h"
#include "PartSet_WidgetMultiSelector.h"
#include "PartSet_WidgetEditor.h"
#include "PartSet_WidgetFileSelector.h"
#include "PartSet_WidgetSketchCreator.h"
#include "PartSet_SketcherMgr.h"
#include "PartSet_SketcherReetntrantMgr.h"
#include "PartSet_MenuMgr.h"
#include "PartSet_CustomPrs.h"
#include "PartSet_IconFactory.h"

#include "PartSet_Filters.h"
#include "PartSet_FilterInfinite.h"

#include <PartSetPlugin_Remove.h>
#include <PartSetPlugin_Part.h>
#include <PartSetPlugin_Duplicate.h>

#include <ModuleBase_Operation.h>
#include <ModuleBase_OperationAction.h>
#include <ModuleBase_IViewer.h>
#include <ModuleBase_IViewWindow.h>
#include <ModuleBase_IPropertyPanel.h>
#include <ModuleBase_WidgetEditor.h>
#include <ModuleBase_WidgetValidated.h>
#include <ModuleBase_FilterFactory.h>
#include <ModuleBase_Tools.h>
#include <ModuleBase_OperationFeature.h>

#include <GeomValidators_ShapeType.h>
#include <GeomValidators_Finite.h>
#include <GeomValidators_Face.h>
#include <GeomValidators_ConstructionComposite.h>
#include <GeomValidators_ZeroOffset.h>
#include <GeomValidators_BooleanArguments.h>
#include <GeomValidators_Different.h>
#include <GeomValidators_PartitionArguments.h>


#include <ModelAPI_Object.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Session.h>
#include <GeomValidators_DifferentShapes.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_AttributeString.h>

#include <GeomDataAPI_Point2D.h>
#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Dir.h>

#include <XGUI_Displayer.h>
#include <XGUI_Workshop.h>
#include <XGUI_OperationMgr.h>
#include <XGUI_PropertyPanel.h>
#include <XGUI_ModuleConnector.h>
#include <XGUI_ContextMenuMgr.h>
#include <XGUI_Tools.h>
#include <XGUI_ObjectsBrowser.h>
#include <XGUI_SelectionMgr.h>
#include <XGUI_DataModel.h>
#include <XGUI_ErrorMgr.h>

#include <SketchPlugin_Feature.h>
#include <SketchPlugin_Sketch.h>
#include <SketchPlugin_ConstraintAngle.h>
#include <SketchPlugin_ConstraintLength.h>
#include <SketchPlugin_ConstraintDistance.h>
#include <SketchPlugin_ConstraintParallel.h>
#include <SketchPlugin_ConstraintPerpendicular.h>
#include <SketchPlugin_ConstraintRadius.h>

#include <SketcherPrs_SymbolPrs.h>
#include <SketcherPrs_Tools.h>

#include <Events_Loop.h>
#include <Config_PropManager.h>
#include <Config_Keywords.h>

#include <StdSelect_TypeOfFace.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <BRep_Tool.hxx>
#include <AIS_Dimension.hxx>

#include <QObject>
#include <QMouseEvent>
#include <QString>
#include <QTimer>
#include <QApplication>
#include <QMessageBox>
#include <QMainWindow>
#include <QLineEdit>

#include <GeomAlgoAPI_FaceBuilder.h>
#include <GeomDataAPI_Dir.h>

#include <SelectMgr_ListIteratorOfListOfFilter.hxx>

#ifdef _DEBUG
#include <QDebug>
#endif

/*!Create and return new instance of XGUI_Module*/
extern "C" PARTSET_EXPORT ModuleBase_IModule* createModule(ModuleBase_IWorkshop* theWshop)
{
  return new PartSet_Module(theWshop);
}

PartSet_Module::PartSet_Module(ModuleBase_IWorkshop* theWshop)
: ModuleBase_IModule(theWshop),
  myVisualLayerId(0), myHasConstraintShown(true)
{
  new PartSet_IconFactory();

  mySketchMgr = new PartSet_SketcherMgr(this);
  mySketchReentrantMgr = new PartSet_SketcherReetntrantMgr(theWshop);

  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(theWshop);
  XGUI_Workshop* aWorkshop = aConnector->workshop();

  XGUI_OperationMgr* anOpMgr = aWorkshop->operationMgr();
  connect(anOpMgr, SIGNAL(operationActivatedByPreselection()),
          this, SLOT(onOperationActivatedByPreselection()));

  ModuleBase_IViewer* aViewer = theWshop->viewer();
  connect(aViewer, SIGNAL(keyRelease(ModuleBase_IViewWindow*, QKeyEvent*)),
          this, SLOT(onKeyRelease(ModuleBase_IViewWindow*, QKeyEvent*)));
  connect(aViewer, SIGNAL(viewTransformed(int)),
          SLOT(onViewTransformed(int)));
  connect(aViewer, SIGNAL(viewCreated(ModuleBase_IViewWindow*)),
          SLOT(onViewCreated(ModuleBase_IViewWindow*)));

  myMenuMgr = new PartSet_MenuMgr(this);
  myCustomPrs = new PartSet_CustomPrs(theWshop);

  Events_Loop* aLoop = Events_Loop::loop();
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_DOCUMENT_CHANGED));

  mySelectionFilters.Append(new PartSet_GlobalFilter(myWorkshop));
  mySelectionFilters.Append(new PartSet_FilterInfinite(myWorkshop));
}

PartSet_Module::~PartSet_Module()
{
  SelectMgr_ListIteratorOfListOfFilter aIt(mySelectionFilters);
  for (; aIt.More(); aIt.Next()) {
    Handle(SelectMgr_Filter) aFilter = aIt.Value();
    if (!aFilter.IsNull())
      aFilter.Nullify();
  }
  delete myCustomPrs;
}

void PartSet_Module::activateSelectionFilters()
{
  SelectMgr_ListIteratorOfListOfFilter aIt(mySelectionFilters);
  for (; aIt.More(); aIt.Next()) {
    Handle(SelectMgr_Filter) aFilter = aIt.Value();
    if (!aFilter.IsNull())
      myWorkshop->viewer()->addSelectionFilter(aFilter);
  }
}

void PartSet_Module::deactivateSelectionFilters()
{
  SelectMgr_ListIteratorOfListOfFilter aIt(mySelectionFilters);
  for (; aIt.More(); aIt.Next()) {
    Handle(SelectMgr_Filter) aFilter = aIt.Value();
    if (!aFilter.IsNull())
      myWorkshop->viewer()->removeSelectionFilter(aFilter);
  }
}

void PartSet_Module::registerValidators()
{
  //Registering of validators
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
  aFactory->registerValidator("PartSet_DistanceSelection", new PartSet_DistanceSelection);
  aFactory->registerValidator("PartSet_LengthSelection", new PartSet_LengthSelection);
  aFactory->registerValidator("PartSet_PerpendicularSelection", new PartSet_PerpendicularSelection);
  aFactory->registerValidator("PartSet_ParallelSelection", new PartSet_ParallelSelection);
  aFactory->registerValidator("PartSet_RadiusSelection", new PartSet_RadiusSelection);
  aFactory->registerValidator("PartSet_RigidSelection", new PartSet_RigidSelection);
  aFactory->registerValidator("PartSet_CoincidentSelection", new PartSet_CoincidentSelection);
  aFactory->registerValidator("PartSet_HVDirSelection", new PartSet_HVDirSelection);
  aFactory->registerValidator("PartSet_TangentSelection", new PartSet_TangentSelection);
  aFactory->registerValidator("PartSet_FilletSelection", new PartSet_FilletSelection);
  aFactory->registerValidator("PartSet_AngleSelection", new PartSet_AngleSelection);
  aFactory->registerValidator("PartSet_EqualSelection", new PartSet_EqualSelection);
  aFactory->registerValidator("PartSet_DifferentObjects", new PartSet_DifferentObjectsValidator);
  aFactory->registerValidator("PartSet_CoincidentAttr", new PartSet_CoincidentAttr);

  aFactory->registerValidator("GeomValidators_DifferentShapes", new GeomValidators_DifferentShapes);
  aFactory->registerValidator("GeomValidators_ShapeType", new GeomValidators_ShapeType);
  aFactory->registerValidator("GeomValidators_Face", new GeomValidators_Face);
  aFactory->registerValidator("GeomValidators_Finite", new GeomValidators_Finite);

  aFactory->registerValidator("GeomValidators_ConstructionComposite",
                              new GeomValidators_ConstructionComposite);

  aFactory->registerValidator("GeomValidators_ZeroOffset",
                              new GeomValidators_ZeroOffset);

  aFactory->registerValidator("GeomValidators_BooleanArguments",
                              new GeomValidators_BooleanArguments);

  aFactory->registerValidator("PartSet_SketchEntityValidator",
                              new PartSet_SketchEntityValidator);

  aFactory->registerValidator("GeomValidators_Different",
                              new GeomValidators_Different);

  aFactory->registerValidator("GeomValidators_PartitionArguments",
                              new GeomValidators_PartitionArguments);
}

void PartSet_Module::registerFilters()
{
  //Registering of selection filters
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(workshop());
  ModuleBase_FilterFactory* aFactory = aConnector->selectionFilters();
}

void PartSet_Module::registerProperties()
{
  Config_PropManager::registerProp("Sketch planes", "planes_size", "Size", Config_Prop::Double,
                                   PLANE_SIZE);
  Config_PropManager::registerProp("Sketch planes", "planes_thickness", "Thickness",
                                   Config_Prop::Integer, SKETCH_WIDTH);
}

void PartSet_Module::onOperationCommitted(ModuleBase_Operation* theOperation) 
{
  if (PartSet_SketcherMgr::isNestedSketchOperation(theOperation)) {
    mySketchMgr->commitNestedSketch(theOperation);
  }

  /// Restart sketcher operations automatically
  if (!mySketchReentrantMgr->operationCommitted(theOperation)) {
    // the selection is cleared after commit the create operation
    // in order to do not use the same selected objects in the restarted operation
    // for common behaviour, the selection is cleared even if the operation is not restarted
    XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(workshop());
    XGUI_Workshop* aWorkshop = aConnector->workshop();
    aWorkshop->selector()->clearSelection();
  }
}

void PartSet_Module::onOperationAborted(ModuleBase_Operation* theOperation)
{
  /// Restart sketcher operations automatically
  mySketchReentrantMgr->operationAborted(theOperation);
}

void PartSet_Module::onOperationStarted(ModuleBase_Operation* theOperation)
{
  /// Restart sketcher operations automatically
  mySketchReentrantMgr->operationStarted(theOperation);

  if (PartSet_SketcherMgr::isSketchOperation(theOperation)) {
    mySketchMgr->startSketch(theOperation);
  }
  else if (PartSet_SketcherMgr::isNestedSketchOperation(theOperation)) {
    mySketchMgr->startNestedSketch(theOperation);
  }

  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>(theOperation);
  if (aFOperation)
    myCustomPrs->activate(aFOperation->feature(), true);
}

void PartSet_Module::onOperationResumed(ModuleBase_Operation* theOperation)
{
  ModuleBase_IModule::onOperationResumed(theOperation);

  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>(theOperation);
  if (aFOperation)
    myCustomPrs->activate(aFOperation->feature(), true);
}

void PartSet_Module::onOperationStopped(ModuleBase_Operation* theOperation)
{
  bool isModified = myCustomPrs->deactivate(false);

  if (PartSet_SketcherMgr::isSketchOperation(theOperation)) {
    mySketchMgr->stopSketch(theOperation);
  }
  else if (PartSet_SketcherMgr::isNestedSketchOperation(theOperation)) {
    mySketchMgr->stopNestedSketch(theOperation);
  }

  //VSV: Viewer is updated on feature update and redisplay
  //if (isModified) {
  //  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(myWorkshop);
  //  XGUI_Displayer* aDisplayer = aConnector->workshop()->displayer();
  //  aDisplayer->updateViewer();
  //}
  mySketchMgr->onShowConstraintsToggle(myHasConstraintShown);
}

ModuleBase_Operation* PartSet_Module::currentOperation() const
{
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(workshop());
  XGUI_OperationMgr* anOpMgr = aConnector->workshop()->operationMgr();
  return anOpMgr->currentOperation();
}

bool PartSet_Module::canUndo() const
{
  bool aCanUndo = false;
  SessionPtr aMgr = ModelAPI_Session::get();
  if (aMgr->hasModuleDocument() && aMgr->canUndo()) {
    aCanUndo = !aMgr->isOperation();
    if (!aCanUndo) // check the enable state additionally by sketch manager
      aCanUndo = aMgr->canUndo();
  }
  return aCanUndo;
}

bool PartSet_Module::canRedo() const
{
  bool aCanRedo = false;
  SessionPtr aMgr = ModelAPI_Session::get();
  if (aMgr->hasModuleDocument() && aMgr->canRedo()) {
    aCanRedo = !aMgr->isOperation();
    if (!aCanRedo) // check the enable state additionally by sketch manager
      aCanRedo = aMgr->canRedo();
  }
  return aCanRedo;
}

bool PartSet_Module::canApplyAction(const ObjectPtr& theObject, const QString& theActionId) const
{
  bool aValid = true;
  if (theActionId == "DELETE_CMD" || theActionId == "MOVE_CMD") {
    FeaturePtr aFeature = ModelAPI_Feature::feature(theObject);
    if (aFeature) {
      // part features are removed in the PartSet module only.
      if (aFeature->getKind() == PartSetPlugin_Part::ID())
        aValid = false;
    }
  }
  return aValid;
}

bool PartSet_Module::canCommitOperation() const
{
  return mySketchMgr->canCommitOperation();
}

bool PartSet_Module::canEraseObject(const ObjectPtr& theObject) const
{
  // the sketch manager put the restriction to the objects erase
  return mySketchMgr->canEraseObject(theObject);
}

bool PartSet_Module::canDisplayObject(const ObjectPtr& theObject) const
{
  // the sketch manager put the restriction to the objects display
  return mySketchMgr->canDisplayObject(theObject);
}

bool PartSet_Module::canActivateSelection(const ObjectPtr& theObject) const
{
  bool aCanActivate = ModuleBase_IModule::canActivateSelection(theObject);

  ModuleBase_Operation* anOperation = myWorkshop->currentOperation();
  bool isSketchOp = PartSet_SketcherMgr::isSketchOperation(anOperation),
       isNestedOp = PartSet_SketcherMgr::isNestedSketchOperation(anOperation);
  if (isSketchOp || isNestedOp) {
    // in active sketch operation it is possible to activate operation object in selection
    // in the edit operation, e.g. points of the line can be moved when the line is edited
    ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>(anOperation);
    aCanActivate = aCanActivate || (aFOperation && aFOperation->isEditOperation());
  }
  return aCanActivate;
}

bool PartSet_Module::addViewerMenu(QMenu* theMenu, const QMap<QString, QAction*>& theStdActions) const
{
  return myMenuMgr->addViewerMenu(theMenu, theStdActions);
}

void PartSet_Module::updateViewerMenu(const QMap<QString, QAction*>& theStdActions)
{
  myMenuMgr->updateViewerMenu(theStdActions);
}

QString PartSet_Module::getFeatureError(const FeaturePtr& theFeature)
{
  QString anError = ModuleBase_IModule::getFeatureError(theFeature);

  if (anError.isEmpty())
    anError = sketchMgr()->getFeatureError(theFeature);

  if (anError.isEmpty()) {
    XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(workshop());
    XGUI_OperationMgr* anOpMgr = aConnector->workshop()->operationMgr();
    
    if (anOpMgr->isValidationLocked()) {
      ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                             (anOpMgr->currentOperation());
      if (!aFOperation || theFeature == aFOperation->feature())
        anError = "Validation is locked by the current operation";
    }
  }
  return anError;
}

void PartSet_Module::grantedOperationIds(ModuleBase_Operation* theOperation,
                                         QStringList& theIds) const
{
  myMenuMgr->grantedOperationIds(theOperation, theIds);

  if (PartSet_SketcherMgr::isSketchOperation(theOperation)) {
    XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(workshop());
    XGUI_Workshop* aWorkshop = aConnector->workshop();

    theIds.append(aWorkshop->contextMenuMgr()->action("DELETE_CMD")->text());
  }
}

void PartSet_Module::activeSelectionModes(QIntList& theModes)
{
  theModes.clear();
  if (mySketchMgr->activeSketch().get())
    PartSet_SketcherMgr::sketchSelectionModes(theModes);
}

bool PartSet_Module::isMouseOverWindow()
{
  return mySketchMgr->isMouseOverWindow();
}

void PartSet_Module::closeDocument()
{
  clearViewer();
}

void PartSet_Module::clearViewer()
{
  myCustomPrs->clearPrs();

  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(myWorkshop);
  XGUI_Workshop* aWorkshop = aConnector->workshop();
  XGUI_Displayer* aDisplayer = aWorkshop->displayer();
  aDisplayer->deactivateSelectionFilters();
}

void PartSet_Module::propertyPanelDefined(ModuleBase_Operation* theOperation)
{
  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>(theOperation);
  if (!aFOperation)
    return;

  ModuleBase_IPropertyPanel* aPanel = aFOperation->propertyPanel();
  // we have to manually activate the sketch label in edit mode
  if (PartSet_SketcherMgr::isSketchOperation(aFOperation) &&  (aFOperation->isEditOperation()))
    aPanel->activateWidget(aPanel->modelWidgets().first());
}


void PartSet_Module::onSelectionChanged()
{
  ModuleBase_Operation* aOperation = myWorkshop->currentOperation();
  if (!aOperation)
    return;

  bool isSketcherOp = false;
  // An edit operation is enable only if the current opeation is the sketch operation
  if (mySketchMgr->activeSketch()) {
    if (PartSet_Tools::sketchPlane(mySketchMgr->activeSketch()))
      isSketcherOp = PartSet_SketcherMgr::isSketchOperation(aOperation);
  }
  if (isSketcherOp) {
    // Editing of constraints can be done on selection
    ModuleBase_ISelection* aSelect = myWorkshop->selection();
    QList<ModuleBase_ViewerPrs> aSelected = aSelect->getSelected();
    if (aSelected.size() == 1) {
      ModuleBase_ViewerPrs aPrs = aSelected.first();
      ObjectPtr aObject = aPrs.object();
      FeaturePtr aFeature = ModelAPI_Feature::feature(aObject);
      if (aFeature) {
        std::string aId = aFeature->getKind();
        if ((aId == SketchPlugin_ConstraintRadius::ID()) ||
            (aId == SketchPlugin_ConstraintLength::ID()) || 
            (aId == SketchPlugin_ConstraintDistance::ID()) ||
            (aId == SketchPlugin_ConstraintAngle::ID())) {
          editFeature(aFeature);
        }
      }
    }
  } 
}

void PartSet_Module::onKeyRelease(ModuleBase_IViewWindow* theWnd, QKeyEvent* theEvent)
{
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(workshop());
  XGUI_OperationMgr* anOpMgr = aConnector->workshop()->operationMgr();
  anOpMgr->onKeyReleased(theEvent);
}

void PartSet_Module::onOperationActivatedByPreselection()
{
  if (!mySketchReentrantMgr->canBeCommittedByPreselection())
    return;

  ModuleBase_Operation* anOperation = myWorkshop->currentOperation();
  if(anOperation && PartSet_SketcherMgr::isNestedSketchOperation(anOperation)) {
    // Set final definitions if they are necessary
    //propertyPanelDefined(aOperation);

    /// Commit sketcher operations automatically
    anOperation->commit();
  }
}

ModuleBase_ModelWidget* PartSet_Module::createWidgetByType(const std::string& theType, QWidget* theParent,
                                            Config_WidgetAPI* theWidgetApi, std::string theParentId)
{
  ModuleBase_IWorkshop* aWorkshop = workshop();
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(aWorkshop);
  XGUI_Workshop* aXUIWorkshop = aConnector->workshop();
  ModuleBase_ModelWidget* aWgt = NULL;
  if (theType == "sketch-start-label") {
    PartSet_WidgetSketchLabel* aLabelWgt = new PartSet_WidgetSketchLabel(theParent, aWorkshop,
      theWidgetApi, theParentId, mySketchMgr->isConstraintsShown());
    connect(aLabelWgt, SIGNAL(planeSelected(const std::shared_ptr<GeomAPI_Pln>&)),
      mySketchMgr, SLOT(onPlaneSelected(const std::shared_ptr<GeomAPI_Pln>&)));
    connect(aLabelWgt, SIGNAL(showConstraintToggled(bool)),
      mySketchMgr, SLOT(onShowConstraintsToggle(bool)));
    aWgt = aLabelWgt;
  } else if (theType == "sketch-2dpoint_selector") {
    PartSet_WidgetPoint2D* aPointWgt = new PartSet_WidgetPoint2D(theParent, aWorkshop,
                                                                 theWidgetApi, theParentId);
    aPointWgt->setSketch(mySketchMgr->activeSketch());
    connect(aPointWgt, SIGNAL(vertexSelected()), sketchReentranceMgr(), SLOT(onVertexSelected()));
    aWgt = aPointWgt;
  } else if (theType == "sketch-2dpoint_flyout_selector") {
    PartSet_WidgetPoint2DFlyout* aPointWgt = new PartSet_WidgetPoint2DFlyout(theParent, aWorkshop,
                                                                 theWidgetApi, theParentId);
    aPointWgt->setSketch(mySketchMgr->activeSketch());
    connect(aPointWgt, SIGNAL(vertexSelected()), sketchReentranceMgr(), SLOT(onVertexSelected()));
    aWgt = aPointWgt;
  } else if (theType == "point2ddistance") {
    PartSet_WidgetPoint2dDistance* aDistanceWgt = new PartSet_WidgetPoint2dDistance(theParent,
                                                        aWorkshop, theWidgetApi, theParentId);
    aDistanceWgt->setSketch(mySketchMgr->activeSketch());
    aWgt = aDistanceWgt;
  } else if(theType == "point2dangle") {
    PartSet_WidgetPoint2dAngle* anAngleWgt = new PartSet_WidgetPoint2dAngle(theParent,
                                                           aWorkshop, theWidgetApi, theParentId);
    anAngleWgt->setSketch(mySketchMgr->activeSketch());
    aWgt = anAngleWgt;
  } else if (theType == "sketch_shape_selector") {
    PartSet_WidgetShapeSelector* aShapeSelectorWgt =
      new PartSet_WidgetShapeSelector(theParent, aWorkshop, theWidgetApi, theParentId);
    aShapeSelectorWgt->setSketcher(mySketchMgr->activeSketch());
    aWgt = aShapeSelectorWgt;
  } else if (theType == "sketch_multi_selector") {
    PartSet_WidgetMultiSelector* aShapeSelectorWgt =
      new PartSet_WidgetMultiSelector(theParent, aWorkshop, theWidgetApi, theParentId);
    aShapeSelectorWgt->setSketcher(mySketchMgr->activeSketch());
    aWgt = aShapeSelectorWgt;
  } else if (theType == WDG_DOUBLEVALUE_EDITOR) {
    aWgt = new PartSet_WidgetEditor(theParent, aWorkshop, theWidgetApi, theParentId);
  } else if (theType == "export_file_selector") {
    aWgt = new PartSet_WidgetFileSelector(theParent, aWorkshop, theWidgetApi, theParentId);
  } else if (theType == "sketch_launcher") {
    aWgt = new PartSet_WidgetSketchCreator(theParent, this, theWidgetApi, theParentId);
  }
  return aWgt;
}

ModuleBase_ModelWidget* PartSet_Module::activeWidget() const
{
  ModuleBase_ModelWidget* anActiveWidget = 0;

  anActiveWidget = mySketchReentrantMgr->internalActiveWidget();
  if (!anActiveWidget) {
    ModuleBase_Operation* aOperation = myWorkshop->currentOperation();
    if (aOperation) {
      ModuleBase_IPropertyPanel* aPanel = aOperation->propertyPanel();
      anActiveWidget = aPanel->activeWidget();
    }
  }
  return anActiveWidget;
}

bool PartSet_Module::deleteObjects()
{
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(workshop());
  XGUI_Workshop* aWorkshop = aConnector->workshop();
  XGUI_OperationMgr* anOpMgr = aWorkshop->operationMgr();

  //SessionPtr aMgr = ModelAPI_Session::get();
  // 1. check whether the delete should be processed in the module
  ModuleBase_Operation* anOperation = myWorkshop->currentOperation();
  bool isSketchOp = PartSet_SketcherMgr::isSketchOperation(anOperation),
       isNestedOp = PartSet_SketcherMgr::isNestedSketchOperation(anOperation);
  if (isSketchOp || isNestedOp) {
    // 2. find selected presentations
    // selected objects should be collected before the current operation abort because
    // the abort leads to selection lost on constraint objects. It can be corrected after #386 issue
    ModuleBase_ISelection* aSel = workshop()->selection();
    QObjectPtrList aSelectedObj = aSel->selectedPresentations();
    // if there are no selected objects in the viewer, that means that the selection in another
    // place cased this method. It is necessary to return the false value to understande in above
    // method that delete is not processed
    if (aSelectedObj.count() == 0)
      return false;

    // avoid delete of the objects, which are not belong to the current sketch
    // in order to do not delete results of other sketches
    QObjectPtrList aSketchObjects;
    QObjectPtrList::const_iterator anIt = aSelectedObj.begin(), aLast = aSelectedObj.end();
    for ( ; anIt != aLast; anIt++) {
      ObjectPtr anObject = *anIt;
      if (mySketchMgr->isObjectOfSketch(anObject)) {
        // sketch feature should be used in this list because workshop deletes features only
        // results are skipped
        FeaturePtr aSketchFeature = ModelAPI_Feature::feature(anObject);
        aSketchObjects.append(aSketchFeature);
      }
    }
    // if the selection contains only local selected presentations from other sketches,
    // the Delete operation should not be done at all
    if (aSketchObjects.size() == 0)
      return true;

    // 3. start operation
    QString aDescription = aWorkshop->contextMenuMgr()->action("DELETE_CMD")->text();
    ModuleBase_OperationAction* anOpAction = new ModuleBase_OperationAction(aDescription, this);

    // the active nested sketch operation should be aborted unconditionally
    // the Delete action should be additionally granted for the Sketch operation
    // in order to do not abort/commit it
    if (!anOpMgr->canStartOperation(anOpAction->id()))
      return true; // the objects are processed but can not be deleted

    anOpMgr->startOperation(anOpAction);

    // 4. delete features
    // sketch feature should be skipped, only sub-features can be removed
    // when sketch operation is active
    aWorkshop->deleteFeatures(aSketchObjects);
    // 5. stop operation
    anOpMgr->commitOperation();
  }
  else {
    bool isPartRemoved = false;
    // Delete part with help of PartSet plugin
    // TODO: the deleted objects has to be processed by multiselection
    QObjectPtrList aObjects = myWorkshop->selection()->selectedObjects();
    if (aObjects.size() == 1) {
      ObjectPtr aObj = aObjects.first();
      FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aObj);
      if (aFeature.get() && (aFeature->getKind() == PartSetPlugin_Part::ID())) {
        // Remove feature should be created in the document of the part results
        ResultPtr aPartResult = aFeature->firstResult();
        if (aPartResult.get()) {
          std::shared_ptr<ModelAPI_ResultPart> aPart =
                       std::dynamic_pointer_cast<ModelAPI_ResultPart>(aPartResult);
          DocumentPtr aPartDoc = aPart->partDoc();
          if (aPartDoc.get()) {
            ModuleBase_OperationAction* anOpAction = new ModuleBase_OperationAction
                                              (PartSetPlugin_Remove::ID().c_str(), this);
            if (!anOpMgr->canStartOperation(anOpAction->id()))
              return true; // the objects are processed but can not be deleted

            anOpMgr->startOperation(anOpAction);

            FeaturePtr aFeature = aPartDoc->addFeature(PartSetPlugin_Remove::ID());
            aFeature->execute();

            anOpMgr->commitOperation();
            isPartRemoved = true;
          }
        }
      }
    }
    return isPartRemoved;
  }
  return true;
}

void PartSet_Module::onFeatureTriggered()
{
  QAction* aCmd = dynamic_cast<QAction*>(sender());
  if (aCmd->isCheckable() && aCmd->isChecked()) {
    // 1. check whether the delete should be processed in the module
    ModuleBase_Operation* anOperation = myWorkshop->currentOperation();
    bool isNestedOp = PartSet_SketcherMgr::isNestedCreateOperation(anOperation);
    if (isNestedOp) {
      // in case if in the viewer nothing is displayed, the create operation should not be
      // comitted even if all values of the feature are initialized
      if (!mySketchMgr->canDisplayCurrentCreatedFeature()) {
        // the action information should be saved before the operation is aborted
        // because this abort leads to update command status, which unchecks this action
        anOperation->abort();
        launchOperation(aCmd->data().toString());
      }
    }
  }
  ModuleBase_IModule::onFeatureTriggered();
}

void PartSet_Module::launchOperation(const QString& theCmdId)
{
  if (PartSet_SketcherMgr::constraintsIdList().contains(theCmdId)) {
    // Show constraints if a constraint was anOperation
    myHasConstraintShown = mySketchMgr->isConstraintsShown();
    mySketchMgr->onShowConstraintsToggle(true);
  }
  ModuleBase_IModule::launchOperation(theCmdId);
}


void PartSet_Module::onObjectDisplayed(ObjectPtr theObject, AISObjectPtr theAIS) 
{
  Handle(AIS_InteractiveObject) anAIS = theAIS->impl<Handle(AIS_InteractiveObject)>();
  if (!anAIS.IsNull()) {
    Handle(AIS_InteractiveContext) aCtx = anAIS->GetContext();
    Handle(AIS_Dimension) aDim = Handle(AIS_Dimension)::DownCast(anAIS);
    if (!aDim.IsNull()) {
      aCtx->SetZLayer(aDim, myVisualLayerId);
    } else {
      Handle(SketcherPrs_SymbolPrs) aCons = Handle(SketcherPrs_SymbolPrs)::DownCast(anAIS);
      if (!aCons.IsNull())
        aCtx->SetZLayer(aCons, myVisualLayerId);
    }
  }
}

void PartSet_Module::onBeforeObjectErase(ObjectPtr theObject, AISObjectPtr theAIS)
{
  // this is obsolete
  // it should be recomputed in order to disappear in the viewer if the corresponded object
  // is erased
  //if (myCustomPrs->isActive())
  //  myCustomPrs->redisplay(theObject, false);
}

void PartSet_Module::onViewTransformed(int theTrsfType)
{
  // Set length of arrows constant in pixel size
  // if the operation is panning or rotate or panglobal then do nothing
  if ((theTrsfType == 1) || (theTrsfType == 3) || (theTrsfType == 4))
    return;
  ModuleBase_IViewer* aViewer = myWorkshop->viewer();
  Handle(AIS_InteractiveContext) aContext = aViewer->AISContext();
  if (aContext.IsNull())
    return;

  //Handle(V3d_View) aView = aViewer->activeView();

  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(myWorkshop);
  XGUI_Workshop* aWorkshop = aConnector->workshop();
  XGUI_Displayer* aDisplayer = aWorkshop->displayer();
  Handle(V3d_Viewer) aV3dViewer = aContext->CurrentViewer();
  Handle(V3d_View) aView;
  double aScale = 0;
  for (aV3dViewer->InitDefinedViews(); 
       aV3dViewer->MoreDefinedViews(); 
       aV3dViewer->NextDefinedViews()) {
    Handle(V3d_View) aV = aV3dViewer->DefinedView();
    double aS = aV->Scale();
    if (aS > aScale) {
      aScale = aS;
      aView = aV;
    }
  }
  if (aView.IsNull())
    return;
  double aLen = aView->Convert(20);

  double aPrevLen = SketcherPrs_Tools::getArrowSize();
  SketcherPrs_Tools::setArrowSize(aLen);
  const double aPrevScale = aViewer->Scale(aViewer->activeView());
  const double aCurScale = aViewer->activeView()->Camera()->Scale();
  aViewer->SetScale(aViewer->activeView(), aCurScale);
  SketcherPrs_Tools::setTextHeight (aCurScale / aPrevScale * SketcherPrs_Tools::getTextHeight());
  bool isModified = false;
  QList<AISObjectPtr> aPrsList = aDisplayer->displayedPresentations();
  foreach (AISObjectPtr aAIS, aPrsList) {
    Handle(AIS_InteractiveObject) aAisObj = aAIS->impl<Handle(AIS_InteractiveObject)>();

    Handle(AIS_Dimension) aDim = Handle(AIS_Dimension)::DownCast(aAisObj);
    if (!aDim.IsNull()) {
      aDim->DimensionAspect()->ArrowAspect()->SetLength(aLen);
      aContext->Redisplay(aDim, false);
      isModified = true;
    }
  }
  if (isModified)
    aDisplayer->updateViewer();
}

bool PartSet_Module::customizeObject(ObjectPtr theObject, const bool theUpdateViewer)
{
  bool isRedisplayed = false;
  if (myCustomPrs->isActive())
    isRedisplayed = myCustomPrs->redisplay(theObject, theUpdateViewer);

  return isRedisplayed;
}

void PartSet_Module::customizeObjectBrowser(QWidget* theObjectBrowser)
{
  XGUI_ObjectsBrowser* aOB = dynamic_cast<XGUI_ObjectsBrowser*>(theObjectBrowser);
  if (aOB) {
    //QLineEdit* aLabel = aOB->activeDocLabel();
    //QPalette aPalet = aLabel->palette();
    //aPalet.setColor(QPalette::Text, QColor(0, 72, 140));
    //aLabel->setPalette(aPalet);
    aOB->treeView()->setExpandsOnDoubleClick(false);
    connect(aOB->treeView(), SIGNAL(doubleClicked(const QModelIndex&)), 
      SLOT(onTreeViewDoubleClick(const QModelIndex&)));
  }
}

ObjectPtr PartSet_Module::findPresentedObject(const AISObjectPtr& theAIS) const
{
  ObjectPtr anObject;
  ModuleBase_Operation* aOperation = myWorkshop->currentOperation();
  if (aOperation) {
    /// If last line finished on vertex the lines creation sequence has to be break
    ModuleBase_IPropertyPanel* aPanel = aOperation->propertyPanel();
    ModuleBase_ModelWidget* anActiveWidget = aPanel->activeWidget();
    // if there is an active widget, find the presented object in it
    if (!anActiveWidget)
      anActiveWidget = aPanel->preselectionWidget();
    
    ModuleBase_WidgetValidated* aWidgetValidated = dynamic_cast<ModuleBase_WidgetValidated*>
                                                                           (anActiveWidget);
    if (aWidgetValidated)
      anObject = aWidgetValidated->findPresentedObject(theAIS);
  }
  return anObject;
}

void PartSet_Module::addObjectBrowserMenu(QMenu* theMenu) const
{
  QObjectPtrList aObjects = myWorkshop->selection()->selectedObjects();
  int aSelected = aObjects.size();
  SessionPtr aMgr = ModelAPI_Session::get();
  QAction* aActivatePartAction = myMenuMgr->action("ACTIVATE_PART_CMD");
  QAction* aActivatePartSetAction = myMenuMgr->action("ACTIVATE_PARTSET_CMD");

  ModuleBase_Operation* aCurrentOp = myWorkshop->currentOperation();
  if (aSelected == 1) {
    bool hasResult = false;
    bool hasFeature = false;
    bool hasParameter = false;
    bool hasSubFeature = false;
    ModuleBase_Tools::checkObjects(aObjects, hasResult, hasFeature, hasParameter, hasSubFeature);

    ObjectPtr aObject = aObjects.first();
    if (aObject) {
      ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aObject);
      FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aObject);
      bool isPart = aPart.get() || 
        (aFeature.get() && (aFeature->getKind() == PartSetPlugin_Part::ID()));
      if (isPart) {
        DocumentPtr aPartDoc;
        if (!aPart.get()) {
          aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aFeature->firstResult());
        }
        if (aPart.get()) // this may be null is Part feature is disabled
          aPartDoc = aPart->partDoc();
          
        theMenu->addAction(aActivatePartAction);
        aActivatePartAction->setEnabled((aMgr->activeDocument() != aPartDoc));

      } else if (aObject->document() == aMgr->activeDocument()) {
        if (hasParameter || hasFeature) {
          myMenuMgr->action("EDIT_CMD")->setEnabled(true);
          theMenu->addAction(myMenuMgr->action("EDIT_CMD"));
          if (aCurrentOp && aFeature.get()) {
            if (aCurrentOp->id().toStdString() == aFeature->getKind())
              myMenuMgr->action("EDIT_CMD")->setEnabled(false);
          }
        }
      }

      ResultBodyPtr aResult = std::dynamic_pointer_cast<ModelAPI_ResultBody>(aObject);
      if( aResult.get() )
        theMenu->addAction(myMenuMgr->action("SELECT_PARENT_CMD"));
    } else {  // If feature is 0 the it means that selected root object (document)
      theMenu->addAction(aActivatePartSetAction);
      aActivatePartSetAction->setEnabled((aMgr->activeDocument() != aMgr->moduleDocument()));
    }
  } else if (aSelected == 0) {
    // if there is no selection then it means that upper label is selected
    QModelIndexList aIndexes = myWorkshop->selection()->selectedIndexes();
    if (aIndexes.size() == 0) // it means that selection happens in top label outside of tree view
      theMenu->addAction(aActivatePartSetAction);
      aActivatePartSetAction->setEnabled((aMgr->activeDocument() != aMgr->moduleDocument()));
  }
  bool aNotDeactivate = (aCurrentOp == 0);
  if (!aNotDeactivate) {
    aActivatePartAction->setEnabled(false);
    aActivatePartSetAction->setEnabled(false);
  }
}

void PartSet_Module::processEvent(const std::shared_ptr<Events_Message>& theMessage)
{
  if (theMessage->eventID() == Events_Loop::loop()->eventByName(EVENT_DOCUMENT_CHANGED)) {
    // Do not change activation of parts if an operation active
    static QStringList aAllowActivationList;
    if (aAllowActivationList.isEmpty())
      aAllowActivationList << 
      QString(PartSetPlugin_Part::ID().c_str()) << 
      QString(PartSetPlugin_Duplicate::ID().c_str()) <<
      QString(PartSetPlugin_Remove::ID().c_str());
    if (myWorkshop->currentOperation() && 
      (!aAllowActivationList.contains(myWorkshop->currentOperation()->id())))
      return;
    XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(myWorkshop);
    XGUI_Workshop* aWorkshop = aConnector->workshop();
    XGUI_DataTree* aTreeView = aWorkshop->objectBrowser()->treeView();
    QLineEdit* aLabel = aWorkshop->objectBrowser()->activeDocLabel();
    QPalette aPalet = aLabel->palette();

    SessionPtr aMgr = ModelAPI_Session::get();
    DocumentPtr aActiveDoc = aMgr->activeDocument();
    if (aActivePartIndex.isValid())
      aTreeView->setExpanded(aActivePartIndex, false);
    XGUI_DataModel* aDataModel = aWorkshop->objectBrowser()->dataModel();
    aActivePartIndex = aDataModel->documentRootIndex(aActiveDoc);
    if (aActivePartIndex.isValid())
      aTreeView->setExpanded(aActivePartIndex, true);

    aLabel->setPalette(aPalet);
    aWorkshop->updateCommandStatus();

    // Update displayed objects in order to update active color
    XGUI_Displayer* aDisplayer = aWorkshop->displayer();
    QObjectPtrList aObjects = aDisplayer->displayedObjects();
    bool aHidden;
    foreach(ObjectPtr aObj, aObjects) {
      //TODO: replace by redisplay event.
      aHidden = !aObj->data() || !aObj->data()->isValid() || 
        aObj->isDisabled() || (!aObj->isDisplayed());
      if (!aHidden)
        aDisplayer->redisplay(aObj, false);
    }
    aDisplayer->updateViewer();
  }
}

void PartSet_Module::onTreeViewDoubleClick(const QModelIndex& theIndex)
{
  if (myWorkshop->currentOperation()) // Do not change activation of parts if an operation active
    return;
  SessionPtr aMgr = ModelAPI_Session::get();
  if (!theIndex.isValid()) {
    aMgr->setActiveDocument(aMgr->moduleDocument());
    return;
  }
  if (theIndex.column() != 0) // Use only first column
    return;

  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(myWorkshop);
  XGUI_Workshop* aWorkshop = aConnector->workshop();
  XGUI_DataModel* aDataModel = aWorkshop->objectBrowser()->dataModel();
  // De not use non editable Indexes
  if ((aDataModel->flags(theIndex) & Qt::ItemIsSelectable) == 0)
    return;
  ObjectPtr aObj = aDataModel->object(theIndex);

  ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aObj);
  if (!aPart.get()) { // Probably this is Feature
    FeaturePtr aPartFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aObj);
    if (aPartFeature.get() && (aPartFeature->getKind() == PartSetPlugin_Part::ID())) {
      aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aPartFeature->firstResult());
    }
  }
  if (aPart.get()) { // if this is a part
    if (aPart->partDoc() == aMgr->activeDocument()) {
      myMenuMgr->activatePartSet();
    } else {
      aPart->activate();
    }
  }
}


void PartSet_Module::onViewCreated(ModuleBase_IViewWindow*)
{
  // z layer is created for all started operations in order to visualize operation AIS presentation
  // over the object
  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  if (aContext.IsNull())
    return;

  Handle(V3d_Viewer) aViewer = aContext->CurrentViewer();
  if (myVisualLayerId == 0) {
    if (myVisualLayerId == 0)
      aViewer->AddZLayer(myVisualLayerId);
  } else {
    TColStd_SequenceOfInteger aZList;
    aViewer->GetAllZLayers(aZList);
    bool aFound = false;
    for (int i = 1; i <= aZList.Length(); i++) {
      if (aZList(i) == myVisualLayerId) {
        aFound = true;
        break;
      }
    }
    if (!aFound)
      aViewer->AddZLayer(myVisualLayerId);
  }
  // if there is an active operation with validated widget,
  // the filters of this widget should be activated in the created view
  ModuleBase_Operation* aOperation = myWorkshop->currentOperation();
  if (aOperation) {
    ModuleBase_ModelWidget* anActiveWidget = activeWidget();
    if (anActiveWidget) {
      ModuleBase_WidgetSelector* aWSelector = dynamic_cast<ModuleBase_WidgetSelector*>(anActiveWidget);
      if (aWSelector)
        aWSelector->activateSelectionAndFilters(true);
    }
  }
}
