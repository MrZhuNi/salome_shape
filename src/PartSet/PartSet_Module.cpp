#include <PartSet_Module.h>
#include <PartSet_OperationSketch.h>
#include <PartSet_OperationFeatureCreate.h>
#include <PartSet_OperationFeatureEditMulti.h>
#include <PartSet_OperationFeatureEdit.h>
#include <ModuleBase_Operation.h>
#include <ModuleBase_OperationDescription.h>
#include <ModuleBase_WidgetFactory.h>
#include <PartSet_Listener.h>
#include <PartSet_TestOCC.h>
#include <PartSet_WidgetSketchLabel.h>
#include <PartSet_Validators.h>

#include <ModuleBase_Operation.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Validator.h>

#include <ModelAPI_Data.h>
#include <GeomDataAPI_Point2D.h>
#include <PartSet_Tools.h>

#include <XGUI_MainWindow.h>
#include <XGUI_Displayer.h>
#include <XGUI_Viewer.h>
#include <XGUI_Workshop.h>
#include <XGUI_OperationMgr.h>
#include <XGUI_SelectionMgr.h>
#include <XGUI_Selection.h>
#include <XGUI_ViewPort.h>
#include <XGUI_ActionsMgr.h>
#include <XGUI_ViewerProxy.h>
#include <XGUI_ContextMenuMgr.h>
#include <XGUI_PropertyPanel.h>
#include <XGUI_ModuleConnector.h>
#include <XGUI_Tools.h>

#include <SketchPlugin_Line.h>

#include <Config_PointerMessage.h>
#include <Config_ModuleReader.h>
#include <Config_WidgetReader.h>
#include <Events_Loop.h>
#include <Events_Message.h>
#include <Events_Error.h>

#include <GeomAPI_Shape.h>
#include <GeomAPI_AISObject.h>
#include <AIS_Shape.hxx>

#include <QObject>
#include <QMouseEvent>
#include <QString>

#include <GeomAlgoAPI_FaceBuilder.h>
#include <GeomDataAPI_Dir.h>

#ifdef _DEBUG
#include <QDebug>
#endif


//const int SKETCH_PLANE_COLOR = Colors::COLOR_BROWN; /// the plane edge color
const double SKETCH_WIDTH = 4.0; /// the plane edge width
// face of the square-face displayed for selection of general plane
const double PLANE_SIZE = 200;


/*!Create and return new instance of XGUI_Module*/
extern "C" PARTSET_EXPORT ModuleBase_IModule* createModule(XGUI_Workshop* theWshop)
{
  return new PartSet_Module(theWshop);
}

PartSet_Module::PartSet_Module(XGUI_Workshop* theWshop)
{
  myWorkshop = theWshop;
  myListener = new PartSet_Listener(this);

  XGUI_OperationMgr* anOperationMgr = myWorkshop->operationMgr();

  connect(anOperationMgr, SIGNAL(operationStarted()),
          this, SLOT(onOperationStarted()));

  connect(anOperationMgr, SIGNAL(operationStopped(ModuleBase_Operation*)),
          this, SLOT(onOperationStopped(ModuleBase_Operation*)));

  XGUI_ContextMenuMgr* aContextMenuMgr = myWorkshop->contextMenuMgr();
  connect(aContextMenuMgr, SIGNAL(actionTriggered(const QString&, bool)), 
          this, SLOT(onContextMenuCommand(const QString&, bool)));

  connect(myWorkshop->viewer(), SIGNAL(mousePress(QMouseEvent*)),
          this, SLOT(onMousePressed(QMouseEvent*)));
  connect(myWorkshop->viewer(), SIGNAL(mouseRelease(QMouseEvent*)),
          this, SLOT(onMouseReleased(QMouseEvent*)));
  connect(myWorkshop->viewer(), SIGNAL(mouseMove(QMouseEvent*)),
          this, SLOT(onMouseMoved(QMouseEvent*)));
  connect(myWorkshop->viewer(), SIGNAL(keyRelease(QKeyEvent*)),
          this, SLOT(onKeyRelease(QKeyEvent*)));
  connect(myWorkshop->viewer(), SIGNAL(mouseDoubleClick(QMouseEvent*)),
          this, SLOT(onMouseDoubleClick(QMouseEvent*)));
}

PartSet_Module::~PartSet_Module()
{
}

XGUI_Workshop* PartSet_Module::workshop() const
{
  return myWorkshop;
}

void PartSet_Module::createFeatures()
{
  Config_ModuleReader aXMLReader = Config_ModuleReader();
  aXMLReader.readAll();
  myFeaturesInFiles = aXMLReader.featuresInFiles();

  //!! Test registering of validators
  PluginManagerPtr aMgr = ModelAPI_PluginManager::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
  aFactory->registerValidator("PartSet_DistanceValidator", new PartSet_DistanceValidator);
  aFactory->registerValidator("PartSet_LengthValidator", new PartSet_LengthValidator);
  aFactory->registerValidator("PartSet_PerpendicularValidator", new PartSet_PerpendicularValidator);
  aFactory->registerValidator("PartSet_ParallelValidator", new PartSet_ParallelValidator);
  aFactory->registerValidator("PartSet_RadiusValidator", new PartSet_RadiusValidator);
}

void PartSet_Module::featureCreated(QAction* theFeature)
{
  connect(theFeature, SIGNAL(triggered(bool)), this, SLOT(onFeatureTriggered()));
}

QStringList PartSet_Module::nestedFeatures(QString)
{
  return QStringList();
}

std::string PartSet_Module::featureFile(const std::string& theFeatureId)
{
  return myFeaturesInFiles[theFeatureId];
}

/*
 *
 */
void PartSet_Module::onFeatureTriggered()
{
  //PartSet_TestOCC::local_selection_change_shape(myWorkshop->viewer()->AISContext(),
  //                                   myWorkshop->viewer()->activeView());

  //PartSet_TestOCC::local_selection_erase(myWorkshop->viewer()->AISContext(),
  //                                       myWorkshop->viewer()->activeView());
  QAction* aCmd = dynamic_cast<QAction*>(sender());
  //Do nothing on uncheck
  if(aCmd->isCheckable() && !aCmd->isChecked())
    return;
  launchOperation(aCmd->data().toString());
}
  
void PartSet_Module::launchOperation(const QString& theCmdId)
{
  ModuleBase_Operation* anOperation = createOperation(theCmdId.toStdString());
  PartSet_OperationSketchBase* aPreviewOp = dynamic_cast<PartSet_OperationSketchBase*>(anOperation);
  if (aPreviewOp) {
    XGUI_Selection* aSelection = myWorkshop->selector()->selection();
    // Initialise operation with preliminary selection
    std::list<ModuleBase_ViewerPrs> aSelected = aSelection->getSelected();
    std::list<ModuleBase_ViewerPrs> aHighlighted = aSelection->getHighlighted();
    aPreviewOp->initSelection(aSelected, aHighlighted);
  } 
  sendOperation(anOperation);
}

void PartSet_Module::onOperationStarted()
{
  PartSet_OperationSketchBase* aPreviewOp = dynamic_cast<PartSet_OperationSketchBase*>(
                                       myWorkshop->operationMgr()->currentOperation());
  if (aPreviewOp) {
    XGUI_PropertyPanel* aPropPanel = myWorkshop->propertyPanel();
    connect(aPropPanel, SIGNAL(storedPoint2D(FeaturePtr, const std::string&)),
      this, SLOT(onStorePoint2D(FeaturePtr, const std::string&)), Qt::UniqueConnection);
  }
}

void PartSet_Module::onOperationStopped(ModuleBase_Operation* theOperation)
{
  if (!theOperation)
    return;
  PartSet_OperationSketchBase* aPreviewOp = dynamic_cast<PartSet_OperationSketchBase*>(theOperation);
  if (aPreviewOp) {
    XGUI_PropertyPanel* aPropPanel = myWorkshop->propertyPanel();
    //disconnect(aPropPanel, SIGNAL(storedPoint2D(FeaturePtr, const std::string&)),
    //           this, SLOT(onStorePoint2D(FeaturePtr, const std::string&)));
  }
}

void PartSet_Module::onContextMenuCommand(const QString& theId, bool isChecked)
{
  QList<ObjectPtr> aFeatures = myWorkshop->selector()->selection()->selectedObjects();
  if (theId == "EDIT_CMD" && (aFeatures.size() > 0)) {
    FeaturePtr aFeature = boost::dynamic_pointer_cast<ModelAPI_Feature>(aFeatures.first());
    if (aFeature)
      editFeature(aFeature);
  }
}

void PartSet_Module::onMousePressed(QMouseEvent* theEvent)
{
  PartSet_OperationSketchBase* aPreviewOp = dynamic_cast<PartSet_OperationSketchBase*>(
                                       myWorkshop->operationMgr()->currentOperation());
  if (aPreviewOp) {
    XGUI_Selection* aSelection = myWorkshop->selector()->selection();
    // Initialise operation with preliminary selection
    std::list<ModuleBase_ViewerPrs> aSelected = aSelection->getSelected();
    std::list<ModuleBase_ViewerPrs> aHighlighted = aSelection->getHighlighted();

    aPreviewOp->mousePressed(theEvent, myWorkshop->viewer()->activeView(), aSelected, aHighlighted);
  }
}

void PartSet_Module::onMouseReleased(QMouseEvent* theEvent)
{
  PartSet_OperationSketchBase* aPreviewOp = dynamic_cast<PartSet_OperationSketchBase*>(
                                       myWorkshop->operationMgr()->currentOperation());
  if (aPreviewOp) {
    XGUI_Selection* aSelection = myWorkshop->selector()->selection();
    // Initialise operation with preliminary selection
    std::list<ModuleBase_ViewerPrs> aSelected = aSelection->getSelected();
    std::list<ModuleBase_ViewerPrs> aHighlighted = aSelection->getHighlighted();

    PartSet_OperationSketch* aSketchOp = dynamic_cast<PartSet_OperationSketch*>(aPreviewOp);
    if (aSketchOp) {
      if ((!aSketchOp->hasSketchPlane()) && (aSelected.size() > 0)) {
        Handle(AIS_InteractiveObject) aAIS = aSelected.front().interactive();
        if ((aAIS == myXPlane->impl<Handle(AIS_InteractiveObject)>()) ||
            (aAIS == myYPlane->impl<Handle(AIS_InteractiveObject)>()) ||
            (aAIS == myZPlane->impl<Handle(AIS_InteractiveObject)>()) ) {

          Handle(AIS_Shape) aAISShape = Handle(AIS_Shape)::DownCast(aAIS);
          aSketchOp->setSketchPlane(aAISShape->Shape());
        }
      } else 
        aSketchOp->mouseReleased(theEvent, myWorkshop->viewer()->activeView(), aSelected, aHighlighted);
    } else
      aPreviewOp->mouseReleased(theEvent, myWorkshop->viewer()->activeView(), aSelected, aHighlighted);
  }
}

void PartSet_Module::onMouseMoved(QMouseEvent* theEvent)
{
  PartSet_OperationSketchBase* aPreviewOp = dynamic_cast<PartSet_OperationSketchBase*>(
                                       myWorkshop->operationMgr()->currentOperation());
  if (aPreviewOp)
    aPreviewOp->mouseMoved(theEvent, myWorkshop->viewer()->activeView());
}

void PartSet_Module::onKeyRelease(QKeyEvent* theEvent)
{
  ModuleBase_Operation* anOperation = myWorkshop->operationMgr()->currentOperation();
  PartSet_OperationSketchBase* aPreviewOp = dynamic_cast<PartSet_OperationSketchBase*>(anOperation);
  if (aPreviewOp) {
    aPreviewOp->keyReleased(theEvent->key());
  }
}

void PartSet_Module::onMouseDoubleClick(QMouseEvent* theEvent)
{
  PartSet_OperationSketchBase* aPreviewOp = dynamic_cast<PartSet_OperationSketchBase*>(
                                       myWorkshop->operationMgr()->currentOperation());
  if (aPreviewOp)
  {
    XGUI_Selection* aSelection = myWorkshop->selector()->selection();
    // Initialise operation with preliminary selection
    std::list<ModuleBase_ViewerPrs> aSelected = aSelection->getSelected();
    std::list<ModuleBase_ViewerPrs> aHighlighted = aSelection->getHighlighted();
    aPreviewOp->mouseDoubleClick(theEvent, myWorkshop->viewer()->activeView(), aSelected,
                                 aHighlighted);
  }
}

void PartSet_Module::onPlaneSelected(double theX, double theY, double theZ)
{
  erasePlanes();
  myWorkshop->viewer()->setViewProjection(theX, theY, theZ);
  myWorkshop->actionsMgr()->update();

  //PartSet_TestOCC::testSelection(myWorkshop);
}

void PartSet_Module::onFitAllView()
{
  myWorkshop->viewer()->fitAll();
}

void PartSet_Module::onLaunchOperation(std::string theName, FeaturePtr theFeature)
{
  ModuleBase_Operation* anOperation = createOperation(theName.c_str(),
                                                      theFeature ? theFeature->getKind() : "");
  PartSet_OperationSketchBase* aPreviewOp = dynamic_cast<PartSet_OperationSketchBase*>(anOperation);
  if (aPreviewOp)
  {
    XGUI_Selection* aSelection = myWorkshop->selector()->selection();
    // Initialise operation with preliminary selection
    std::list<ModuleBase_ViewerPrs> aSelected = aSelection->getSelected();
    std::list<ModuleBase_ViewerPrs> aHighlighted = aSelection->getHighlighted();
    aPreviewOp->initFeature(theFeature);
    aPreviewOp->initSelection(aSelected, aHighlighted);
  } else {
    anOperation->setEditingFeature(theFeature);
  }
  sendOperation(anOperation);
  myWorkshop->actionsMgr()->updateCheckState();
}

void PartSet_Module::onMultiSelectionEnabled(bool theEnabled)
{
  XGUI_ViewerProxy* aViewer = myWorkshop->viewer();
  aViewer->enableMultiselection(theEnabled);
}

void PartSet_Module::onStopSelection(const QList<ObjectPtr>& theFeatures, const bool isStop)
{
  XGUI_Displayer* aDisplayer = myWorkshop->displayer();
  if (!isStop) {
    foreach(ObjectPtr aObject, theFeatures) {
      activateFeature(aObject, false);
    }
  }
  //ObjectPtr aResults;
  //foreach(ObjectPtr aFeature, theFeatures) {
/* TODO    if (aFeature->results().size() > 0) {
      const std::list<ResultPtr>& aResList = aFeature->results();
      std::list<ResultPtr>::const_iterator aIt;
      for (aIt = aResList.cbegin(); aIt != aResList.cend(); ++aIt)
        aResults.append(*aIt);
    }
  }*/
  aDisplayer->stopSelection(theFeatures, isStop, false);

  XGUI_ViewerProxy* aViewer = myWorkshop->viewer();
  aViewer->enableSelection(!isStop);

  aDisplayer->updateViewer();
}

void PartSet_Module::onSetSelection(const QList<ObjectPtr>& theFeatures)
{
  XGUI_Displayer* aDisplayer = myWorkshop->displayer();
  aDisplayer->setSelected(theFeatures, false);
  aDisplayer->updateViewer();
}

void PartSet_Module::onCloseLocalContext()
{
  XGUI_Displayer* aDisplayer = myWorkshop->displayer();
  aDisplayer->closeLocalContexts();
}

void PartSet_Module::onFeatureConstructed(FeaturePtr theFeature, int theMode)
{
  bool isDisplay = theMode != PartSet_OperationSketchBase::FM_Hide;
  // TODO visualizePreview(theFeature, isDisplay, false);
  if (!isDisplay) {
    ModuleBase_Operation* aCurOperation = myWorkshop->operationMgr()->currentOperation();
    FeaturePtr aSketch;
    PartSet_OperationSketchBase* aPrevOp = dynamic_cast<PartSet_OperationSketchBase*>(aCurOperation);
    if (aPrevOp) {
      std::list<FeaturePtr> aList = aPrevOp->subFeatures();
      XGUI_Displayer* aDisplayer = myWorkshop->displayer();
      std::list<int> aModes = aPrevOp->getSelectionModes(aPrevOp->feature());

      std::list<FeaturePtr>::const_iterator anIt = aList.begin(),
                                            aLast = aList.end();
      for (; anIt != aLast; anIt++)
        visualizePreview((*anIt), false, false);
      aDisplayer->updateViewer();
    }
  }

  if (theMode == PartSet_OperationSketchBase::FM_Activation ||
      theMode == PartSet_OperationSketchBase::FM_Deactivation)
    activateFeature(theFeature, true);
}

ModuleBase_Operation* PartSet_Module::createOperation(const std::string& theCmdId,
                                                      const std::string& theFeatureKind)
{
  // create the operation
  ModuleBase_Operation* anOperation = 0;
  if (theCmdId == PartSet_OperationSketch::Type()) {
    anOperation = new PartSet_OperationSketch(theCmdId.c_str(), this);
  }
  else {
    ModuleBase_Operation* aCurOperation = myWorkshop->operationMgr()->currentOperation();
    FeaturePtr aSketch;
    PartSet_OperationSketchBase* aPrevOp = dynamic_cast<PartSet_OperationSketchBase*>(aCurOperation);
    if (aPrevOp)
      aSketch = aPrevOp->sketch();
    if (PartSet_OperationFeatureCreate::canProcessKind(theCmdId))
      anOperation = new PartSet_OperationFeatureCreate(theCmdId.c_str(), this, aSketch);
    else if (theCmdId == PartSet_OperationFeatureEditMulti::Type())
		anOperation = new PartSet_OperationFeatureEditMulti(theCmdId.c_str(), this, aSketch);
    else if (theCmdId == PartSet_OperationFeatureEdit::Type())
      anOperation = new PartSet_OperationFeatureEdit(theCmdId.c_str(), this, aSketch);
  }

  if (!anOperation) {
    anOperation = new ModuleBase_Operation(theCmdId.c_str(), this);
  }

  // set operation description and list of widgets corresponding to the feature xml definition
  std::string aFeatureKind = theFeatureKind.empty() ? theCmdId : theFeatureKind;

  std::string aPluginFileName = featureFile(aFeatureKind);
  Config_WidgetReader aWdgReader = Config_WidgetReader(aPluginFileName);
  aWdgReader.readAll();
  std::string aXmlCfg = aWdgReader.featureWidgetCfg(aFeatureKind);
  std::string aDescription = aWdgReader.featureDescription(aFeatureKind);

  //QString aXmlRepr = QString::fromStdString(aXmlCfg);
  //ModuleBase_WidgetFactory aFactory = ModuleBase_WidgetFactory(aXmlRepr.toStdString(),
  //                                                             myWorkshop->moduleConnector());
  //QWidget* aContent = myWorkshop->propertyPanel()->contentWidget();
  //qDeleteAll(aContent->children());
  //aFactory.createWidget(aContent);

  anOperation->getDescription()->setDescription(QString::fromStdString(aDescription));
  anOperation->getDescription()->setXmlRepresentation(QString::fromStdString(aXmlCfg));

  //anOperation->setModelWidgets(aXmlRepr.toStdString(), aFactory.getModelWidgets());

  // connect the operation
  PartSet_OperationSketchBase* aPreviewOp = dynamic_cast<PartSet_OperationSketchBase*>(anOperation);
  if (aPreviewOp) {
    connect(aPreviewOp, SIGNAL(featureConstructed(FeaturePtr, int)),
            this, SLOT(onFeatureConstructed(FeaturePtr, int)));
    connect(aPreviewOp, SIGNAL(launchOperation(std::string, FeaturePtr)),
            this, SLOT(onLaunchOperation(std::string, FeaturePtr)));
    connect(aPreviewOp, SIGNAL(multiSelectionEnabled(bool)),
            this, SLOT(onMultiSelectionEnabled(bool)));

    connect(aPreviewOp, SIGNAL(stopSelection(const QList<ObjectPtr>&, const bool)),
            this, SLOT(onStopSelection(const QList<ObjectPtr>&, const bool)));
    connect(aPreviewOp, SIGNAL(setSelection(const QList<ObjectPtr>&)),
            this, SLOT(onSetSelection(const QList<ObjectPtr>&)));

     connect(aPreviewOp, SIGNAL(closeLocalContext()),
             this, SLOT(onCloseLocalContext()));

    PartSet_OperationSketch* aSketchOp = dynamic_cast<PartSet_OperationSketch*>(aPreviewOp);
    if (aSketchOp) {
      connect(aSketchOp, SIGNAL(planeSelected(double, double, double)),
              this, SLOT(onPlaneSelected(double, double, double)));
      connect(aSketchOp, SIGNAL(fitAllView()),
              this, SLOT(onFitAllView()));
    }
  }

  return anOperation;
}

void PartSet_Module::sendOperation(ModuleBase_Operation* theOperation)
{
  static Events_ID aModuleEvent = Events_Loop::eventByName(EVENT_OPERATION_LAUNCHED);
  Config_PointerMessage aMessage(aModuleEvent, this);
  aMessage.setPointer(theOperation);
  Events_Loop::loop()->send(aMessage);
}

boost::shared_ptr<GeomAPI_Shape> getPlane(double theX, double theY, double theZ)
{
  boost::shared_ptr<GeomAPI_Pnt> anOrigin(new GeomAPI_Pnt(0, 0, 0));
  boost::shared_ptr<GeomAPI_Dir> aNormal(new GeomAPI_Dir(theX, theY, theZ));
  return GeomAlgoAPI_FaceBuilder::square(anOrigin, aNormal, PLANE_SIZE);
}

void PartSet_Module::showPlanes()
{
  XGUI_Displayer* aDisplayer = myWorkshop->displayer();
  // Show selection planes
  if (!myXPlane) {
    boost::shared_ptr<GeomAPI_Shape> aPlaneX = getPlane(1, 0, 0);
    myXPlane = boost::shared_ptr<GeomAPI_AISObject>(new GeomAPI_AISObject());
    myXPlane->createShape(aPlaneX);
    myXPlane->setColor(Colors::COLOR_RED);
    myXPlane->setWidth(SKETCH_WIDTH);
  }
  if (!myYPlane) {
    boost::shared_ptr<GeomAPI_Shape> aPlaneY = getPlane(0, 1, 0);
    myYPlane = boost::shared_ptr<GeomAPI_AISObject>(new GeomAPI_AISObject());
    myYPlane->createShape(aPlaneY);
    myYPlane->setColor(Colors::COLOR_GREEN);
    myYPlane->setWidth(SKETCH_WIDTH);
  }
  if (!myZPlane) {
    boost::shared_ptr<GeomAPI_Shape> aPlaneZ = getPlane(0, 0, 1);
    myZPlane = boost::shared_ptr<GeomAPI_AISObject>(new GeomAPI_AISObject());
    myZPlane->createShape(aPlaneZ);
    myZPlane->setColor(Colors::COLOR_BLUE);
    myZPlane->setWidth(SKETCH_WIDTH);
  }
  aDisplayer->display(myXPlane, false);
  aDisplayer->display(myYPlane, false);
  aDisplayer->display(myZPlane, false);
  aDisplayer->updateViewer();
}

void PartSet_Module::erasePlanes()
{
  XGUI_Displayer* aDisplayer = myWorkshop->displayer();
  aDisplayer->erase(myXPlane, false);
  aDisplayer->erase(myYPlane, false);
  aDisplayer->erase(myZPlane, false);
  aDisplayer->updateViewer();
}

void PartSet_Module::visualizePreview(FeaturePtr theFeature, bool isDisplay,
                                      const bool isUpdateViewer)
{
  ModuleBase_Operation* anOperation = myWorkshop->operationMgr()->currentOperation();
  if (!anOperation)
    return;

  PartSet_OperationSketchBase* aPreviewOp = dynamic_cast<PartSet_OperationSketchBase*>(anOperation);
  if (!aPreviewOp)
    return;

  ResultPtr aResult = theFeature->firstResult();
  XGUI_Displayer* aDisplayer = myWorkshop->displayer();
  if (isDisplay) {
    boost::shared_ptr<SketchPlugin_Feature> aSPFeature = 
      boost::dynamic_pointer_cast<SketchPlugin_Feature>(theFeature);
    if (aSPFeature) {
      PartSet_OperationSketch* aSketchOp = dynamic_cast<PartSet_OperationSketch*>(aPreviewOp);
      if (aSketchOp && !aSketchOp->hasSketchPlane())
        showPlanes();
    }
  }
  else
    aDisplayer->erase(aResult, false);

  if (isUpdateViewer)
    aDisplayer->updateViewer();
}

void PartSet_Module::activateFeature(ObjectPtr theFeature, const bool isUpdateViewer)
{
  ModuleBase_Operation* anOperation = myWorkshop->operationMgr()->currentOperation();
  PartSet_OperationSketchBase* aPreviewOp = dynamic_cast<PartSet_OperationSketchBase*>(anOperation);
  if (aPreviewOp) {
    PartSet_OperationSketch* aOp = dynamic_cast<PartSet_OperationSketch*>(aPreviewOp);
    if (!aOp) {
      XGUI_Displayer* aDisplayer = myWorkshop->displayer();
      aDisplayer->activateInLocalContext(theFeature, aPreviewOp->getSelectionModes(theFeature),
                                         isUpdateViewer);
    }
  }
}

void PartSet_Module::updateCurrentPreview(const std::string& theCmdId)
{
  ModuleBase_Operation* anOperation = myWorkshop->operationMgr()->currentOperation();
  if (!anOperation)
    return;

  PartSet_OperationSketchBase* aPreviewOp = dynamic_cast<PartSet_OperationSketchBase*>(anOperation);
  if (!aPreviewOp)
    return;

  FeaturePtr aFeature = aPreviewOp->feature();
  if (!aFeature || aFeature->getKind() != theCmdId)
    return;

  std::list<FeaturePtr> aList = aPreviewOp->subFeatures();
  XGUI_Displayer* aDisplayer = myWorkshop->displayer();
  std::list<int> aModes = aPreviewOp->getSelectionModes(aPreviewOp->feature());

  std::list<FeaturePtr>::const_iterator anIt = aList.begin(), 
                                        aLast = aList.end();
  for (; anIt != aLast; anIt++) {
    boost::shared_ptr<SketchPlugin_Feature> aSPFeature = 
      boost::dynamic_pointer_cast<SketchPlugin_Feature>(*anIt);
    if (!aSPFeature)
      continue;
    visualizePreview((*anIt), true, false);
    aDisplayer->activateInLocalContext((*anIt)->firstResult(), aModes, false);
  }
  aDisplayer->updateViewer();
}

void PartSet_Module::editFeature(FeaturePtr theFeature)
{
  if (!theFeature)
    return;

//  if (theFeature->getKind() == SKETCH_KIND) {
    //FeaturePtr aFeature = theFeature;
    //if (XGUI_Tools::isModelObject(aFeature)) {
    //  ObjectPtr aObject = boost::dynamic_pointer_cast<ModelAPI_Object>(aFeature);
    //  aFeature = aObject->featureRef();
    //}

    //if (aFeature) {
      onLaunchOperation(theFeature->getKind(), theFeature);
      updateCurrentPreview(theFeature->getKind());
    //}
//  }
}

void PartSet_Module::onStorePoint2D(FeaturePtr theFeature, const std::string& theAttribute)
{
  PartSet_OperationSketchBase* aPreviewOp = dynamic_cast<PartSet_OperationSketchBase*>(
                                       myWorkshop->operationMgr()->currentOperation());
  if (!aPreviewOp)
    return;

  boost::shared_ptr<GeomDataAPI_Point2D> aPoint =
        boost::dynamic_pointer_cast<GeomDataAPI_Point2D>(theFeature->data()->attribute(theAttribute));

  PartSet_Tools::setConstraints(aPreviewOp->sketch(), theFeature, theAttribute,
                                aPoint->x(), aPoint->y());
}

/*bool PartSet_Module::isFeatureEnabled(const QString& theCmdId) const
{
  XGUI_OperationMgr* aOpMgr = myWorkshop->operationMgr();
  XGUI_ActionsMgr* aActMgr = myWorkshop->actionsMgr();

  ModuleBase_Operation* aOperation = aOpMgr->currentOperation();
  if (!aOperation)
    return !aActMgr->isNested(theCmdId);

  PartSet_OperationFeatureEdit* aSketchEdtOp = dynamic_cast<PartSet_OperationFeatureEdit*>(aOperation);
  if (aSketchEdtOp) {
    QStringList aConstraintList;
    aConstraintList<<"SketchConstraintDistance"<<"SketchConstraintLength"
      <<"SketchConstraintRadius"<<"SketchConstraintParallel"<<"SketchConstraintPerpendicular";
    return aConstraintList.contains(theCmdId);
  }
  QStringList aList = aActMgr->nestedCommands(aOperation->id());
  return aList.contains(theCmdId);
}*/

QWidget* PartSet_Module::createWidgetByType(const std::string& theType, QWidget* theParent, 
                         Config_WidgetAPI* theWidgetApi, QList<ModuleBase_ModelWidget*>& theModelWidgets)
{
  if (theType == "sketch-start-label") {
    PartSet_WidgetSketchLabel* aWgt = new PartSet_WidgetSketchLabel(theParent, theWidgetApi);
    aWgt->setOperationsMgr(myWorkshop->operationMgr());
    theModelWidgets.append(aWgt);
    return aWgt->getControl();
  } else
    return 0;
}
