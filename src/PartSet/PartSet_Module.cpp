#include <PartSet_Module.h>
#include <PartSet_OperationSketch.h>
#include <PartSet_OperationSketchLine.h>
#include <PartSet_OperationEditLine.h>
#include <ModuleBase_Operation.h>
#include <ModuleBase_OperationDescription.h>
#include <PartSet_Listener.h>

#include <ModuleBase_Operation.h>

#include <XGUI_MainWindow.h>
#include <XGUI_Displayer.h>
#include <XGUI_Viewer.h>
#include <XGUI_Workshop.h>
#include <XGUI_OperationMgr.h>
#include <XGUI_SelectionMgr.h>
#include <XGUI_ViewPort.h>
#include <XGUI_ActionsMgr.h>
#include <XGUI_ViewerProxy.h>

#include <Config_PointerMessage.h>
#include <Config_ModuleReader.h>
#include <Config_WidgetReader.h>
#include <Events_Loop.h>
#include <Events_Message.h>
#include <Events_Error.h>

#include <GeomAPI_Shape.h>

#include <AIS_ListOfInteractive.hxx>

#include <QObject>
#include <QMouseEvent>
#include <QString>

#ifdef _DEBUG
#include <QDebug>
#endif


/*!Create and return new instance of XGUI_Module*/
extern "C" PARTSET_EXPORT XGUI_Module* createModule(XGUI_Workshop* theWshop)
{
  return new PartSet_Module(theWshop);
}

PartSet_Module::PartSet_Module(XGUI_Workshop* theWshop)
{
  myWorkshop = theWshop;
  myListener = new PartSet_Listener(this);

  XGUI_OperationMgr* anOperationMgr = myWorkshop->operationMgr();

  connect(anOperationMgr, SIGNAL(operationStopped(ModuleBase_Operation*)),
          this, SLOT(onOperationStopped(ModuleBase_Operation*)));

  connect(myWorkshop->viewer(), SIGNAL(mousePress(QMouseEvent*)),
          this, SLOT(onMousePressed(QMouseEvent*)));
  connect(myWorkshop->viewer(), SIGNAL(mouseRelease(QMouseEvent*)),
          this, SLOT(onMouseReleased(QMouseEvent*)));
  connect(myWorkshop->viewer(), SIGNAL(mouseMove(QMouseEvent*)),
          this, SLOT(onMouseMoved(QMouseEvent*)));
  connect(myWorkshop->viewer(), SIGNAL(keyRelease(QKeyEvent*)),
          this, SLOT(onKeyRelease(QKeyEvent*)));
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
  QAction* aCmd = dynamic_cast<QAction*>(sender());
  //Do nothing on uncheck
  if(aCmd->isCheckable() && !aCmd->isChecked())
    return;
  launchOperation(aCmd->data().toString());
}
  
void PartSet_Module::launchOperation(const QString& theCmdId)
{
  ModuleBase_Operation* anOperation = createOperation(theCmdId.toStdString());
  sendOperation(anOperation);
}

void PartSet_Module::onOperationStopped(ModuleBase_Operation* theOperation)
{
  if (!theOperation)
    return;
  PartSet_OperationSketchBase* aPreviewOp = dynamic_cast<PartSet_OperationSketchBase*>(theOperation);
  if (aPreviewOp) {
  }
}

void PartSet_Module::onMousePressed(QMouseEvent* theEvent)
{
  PartSet_OperationSketchBase* aPreviewOp = dynamic_cast<PartSet_OperationSketchBase*>(
                                       myWorkshop->operationMgr()->currentOperation());
  if (aPreviewOp)
  {
    XGUI_Displayer* aDisplayer = myWorkshop->displayer();
    std::list<XGUI_ViewerPrs> aSelected = aDisplayer->GetSelected();
    std::list<XGUI_ViewerPrs> aHighlighted = aDisplayer->GetHighlighted();

    aPreviewOp->mousePressed(theEvent, myWorkshop->viewer()->activeView(), aSelected, aHighlighted);
  }
}

void PartSet_Module::onMouseReleased(QMouseEvent* theEvent)
{
  PartSet_OperationSketchBase* aPreviewOp = dynamic_cast<PartSet_OperationSketchBase*>(
                                       myWorkshop->operationMgr()->currentOperation());
  if (aPreviewOp)
  {
    XGUI_Displayer* aDisplayer = myWorkshop->displayer();
    std::list<XGUI_ViewerPrs> aSelected = aDisplayer->GetSelected();
    std::list<XGUI_ViewerPrs> aHighlighted = aDisplayer->GetHighlighted();

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

void PartSet_Module::onPlaneSelected(double theX, double theY, double theZ)
{
  myWorkshop->viewer()->setViewProjection(theX, theY, theZ);
  myWorkshop->actionsMgr()->update();
}

void PartSet_Module::onLaunchOperation(std::string theName, boost::shared_ptr<ModelAPI_Feature> theFeature)
{
  ModuleBase_Operation* anOperation = createOperation(theName.c_str());
  PartSet_OperationSketchBase* aPreviewOp = dynamic_cast<PartSet_OperationSketchBase*>(anOperation);
  if (aPreviewOp)
  {
    XGUI_Displayer* aDisplayer = myWorkshop->displayer();
    std::list<XGUI_ViewerPrs> aSelected = aDisplayer->GetSelected();
    std::list<XGUI_ViewerPrs> aHighlighted = aDisplayer->GetHighlighted();
    aPreviewOp->init(theFeature, aSelected, aHighlighted);
  }
  myWorkshop->actionsMgr()->updateCheckState();
  sendOperation(anOperation);
}

void PartSet_Module::onMultiSelectionEnabled(bool theEnabled)
{
  XGUI_ViewerProxy* aViewer = myWorkshop->viewer();
  aViewer->enableMultiselection(theEnabled);
}

void PartSet_Module::onStopSelection(const std::list<XGUI_ViewerPrs>& theFeatures, const bool isStop)
{
  XGUI_Displayer* aDisplayer = myWorkshop->displayer();
  if (!isStop) {
    std::list<XGUI_ViewerPrs>::const_iterator anIt = theFeatures.begin(), aLast = theFeatures.end();
    boost::shared_ptr<ModelAPI_Feature> aFeature;
    for (; anIt != aLast; anIt++) {
      activateFeature((*anIt).feature(), false);
    }
  }
  aDisplayer->StopSelection(theFeatures, isStop, false);

  XGUI_ViewerProxy* aViewer = myWorkshop->viewer();
  aViewer->enableSelection(!isStop);

  aDisplayer->UpdateViewer();
}

void PartSet_Module::onSetSelection(const std::list<XGUI_ViewerPrs>& theFeatures)
{
  XGUI_Displayer* aDisplayer = myWorkshop->displayer();
  aDisplayer->SetSelected(theFeatures, false);
  aDisplayer->UpdateViewer();
}

void PartSet_Module::onCloseLocalContext()
{
  XGUI_Displayer* aDisplayer = myWorkshop->displayer();
  aDisplayer->CloseLocalContexts();
}

void PartSet_Module::onFeatureConstructed(boost::shared_ptr<ModelAPI_Feature> theFeature,
                                          int theMode)
{
  bool isDisplay = theMode != PartSet_OperationSketchBase::FM_Hide;
  visualizePreview(theFeature, isDisplay, false);
  if (!isDisplay) {
    ModuleBase_Operation* aCurOperation = myWorkshop->operationMgr()->currentOperation();
    boost::shared_ptr<ModelAPI_Feature> aSketch;
    PartSet_OperationSketchBase* aPrevOp = dynamic_cast<PartSet_OperationSketchBase*>(aCurOperation);
    if (aPrevOp) {
      std::map<boost::shared_ptr<ModelAPI_Feature>, boost::shared_ptr<GeomAPI_Shape> >
                                                                         aList = aPrevOp->subPreview();
      XGUI_Displayer* aDisplayer = myWorkshop->displayer();
      std::list<int> aModes = aPrevOp->getSelectionModes(aPrevOp->feature());

      std::map<boost::shared_ptr<ModelAPI_Feature>, boost::shared_ptr<GeomAPI_Shape> >::const_iterator
                                                             anIt = aList.begin(), aLast = aList.end();
      for (; anIt != aLast; anIt++) {
        boost::shared_ptr<ModelAPI_Feature> aFeature = (*anIt).first;
        visualizePreview(aFeature, false, false);
      }
      aDisplayer->UpdateViewer();
    }
  }

  if (theMode == PartSet_OperationSketchBase::FM_Activation ||
      theMode == PartSet_OperationSketchBase::FM_Deactivation)
    activateFeature(theFeature, true);
}

ModuleBase_Operation* PartSet_Module::createOperation(const std::string& theCmdId)
{
  // get operation xml description
  std::string aStdCmdId = theCmdId;
  if (aStdCmdId == PartSet_OperationEditLine::Type())
    aStdCmdId = PartSet_OperationSketchLine::Type();
  std::string aPluginFileName = featureFile(aStdCmdId);
  Config_WidgetReader aWdgReader = Config_WidgetReader(aPluginFileName);
  aWdgReader.readAll();
  std::string aXmlCfg = aWdgReader.featureWidgetCfg(aStdCmdId);
  std::string aDescription = aWdgReader.featureDescription(aStdCmdId);

  // create the operation
  ModuleBase_Operation* anOperation;
  if (theCmdId == PartSet_OperationSketch::Type()) {
    anOperation = new PartSet_OperationSketch(theCmdId.c_str(), this);
  }
  else if(theCmdId == PartSet_OperationSketchLine::Type() ||
          theCmdId == PartSet_OperationEditLine::Type()) {
    ModuleBase_Operation* aCurOperation = myWorkshop->operationMgr()->currentOperation();
    boost::shared_ptr<ModelAPI_Feature> aSketch;
    PartSet_OperationSketchBase* aPrevOp = dynamic_cast<PartSet_OperationSketchBase*>(aCurOperation);
    if (aPrevOp)
      aSketch = aPrevOp->sketch();
    if (theCmdId == PartSet_OperationSketchLine::Type())
      anOperation = new PartSet_OperationSketchLine(theCmdId.c_str(), this, aSketch);
    else
      anOperation = new PartSet_OperationEditLine(theCmdId.c_str(), this, aSketch);
  }
  else {
    anOperation = new ModuleBase_Operation(theCmdId.c_str(), this);
  }
  anOperation->getDescription()->setXmlRepresentation(QString::fromStdString(aXmlCfg));
  anOperation->getDescription()->setDescription(QString::fromStdString(aDescription));

  // connect the operation
  PartSet_OperationSketchBase* aPreviewOp = dynamic_cast<PartSet_OperationSketchBase*>(anOperation);
  if (aPreviewOp) {
    connect(aPreviewOp, SIGNAL(featureConstructed(boost::shared_ptr<ModelAPI_Feature>, int)),
            this, SLOT(onFeatureConstructed(boost::shared_ptr<ModelAPI_Feature>, int)));
    connect(aPreviewOp, SIGNAL(launchOperation(std::string, boost::shared_ptr<ModelAPI_Feature>)),
            this, SLOT(onLaunchOperation(std::string, boost::shared_ptr<ModelAPI_Feature>)));
    connect(aPreviewOp, SIGNAL(multiSelectionEnabled(bool)),
            this, SLOT(onMultiSelectionEnabled(bool)));

    connect(aPreviewOp, SIGNAL(multiSelectionEnabled(bool)),
            this, SLOT(onMultiSelectionEnabled(bool)));
    connect(aPreviewOp, SIGNAL(stopSelection(const std::list<XGUI_ViewerPrs>&, const bool)),
            this, SLOT(onStopSelection(const std::list<XGUI_ViewerPrs>&, const bool)));
    connect(aPreviewOp, SIGNAL(setSelection(const std::list<XGUI_ViewerPrs>&)),
            this, SLOT(onSetSelection(const std::list<XGUI_ViewerPrs>&)));

     connect(aPreviewOp, SIGNAL(closeLocalContext()),
             this, SLOT(onCloseLocalContext()));

    PartSet_OperationSketch* aSketchOp = dynamic_cast<PartSet_OperationSketch*>(aPreviewOp);
    if (aSketchOp) {
      connect(aSketchOp, SIGNAL(planeSelected(double, double, double)),
              this, SLOT(onPlaneSelected(double, double, double)));
    }
  }

  return anOperation;
}

void PartSet_Module::sendOperation(ModuleBase_Operation* theOperation)
{
  //TODO(sbh): Implement static method to extract event id [SEID]
  static Events_ID aModuleEvent = Events_Loop::eventByName("PartSetModuleEvent");
  Config_PointerMessage aMessage(aModuleEvent, this);
  aMessage.setPointer(theOperation);
  Events_Loop::loop()->send(aMessage);
}

void PartSet_Module::visualizePreview(boost::shared_ptr<ModelAPI_Feature> theFeature, bool isDisplay,
                                      const bool isUpdateViewer)
{
  ModuleBase_Operation* anOperation = myWorkshop->operationMgr()->currentOperation();
  if (!anOperation)
    return;

  PartSet_OperationSketchBase* aPreviewOp = dynamic_cast<PartSet_OperationSketchBase*>(anOperation);
  if (!aPreviewOp)
    return;

  XGUI_Displayer* aDisplayer = myWorkshop->displayer();
  if (isDisplay) {
    boost::shared_ptr<GeomAPI_Shape> aPreview = aPreviewOp->preview(theFeature);
    aDisplayer->Redisplay(theFeature,
                          aPreview ? aPreview->impl<TopoDS_Shape>() : TopoDS_Shape(), false);
  }
  else
    aDisplayer->Erase(theFeature, false);

  if (isUpdateViewer)
    aDisplayer->UpdateViewer();
}

void PartSet_Module::activateFeature(boost::shared_ptr<ModelAPI_Feature> theFeature,
                                     const bool isUpdateViewer)
{
  ModuleBase_Operation* anOperation = myWorkshop->operationMgr()->currentOperation();
  PartSet_OperationSketchBase* aPreviewOp = dynamic_cast<PartSet_OperationSketchBase*>(anOperation);
  if (aPreviewOp) {
    XGUI_Displayer* aDisplayer = myWorkshop->displayer();
    aDisplayer->ActivateInLocalContext(theFeature, aPreviewOp->getSelectionModes(theFeature),
                                       isUpdateViewer);
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

  boost::shared_ptr<ModelAPI_Feature> aFeature = aPreviewOp->feature();
  if (!aFeature || aFeature->getKind() != theCmdId)
    return;

  std::map<boost::shared_ptr<ModelAPI_Feature>, boost::shared_ptr<GeomAPI_Shape> >
                                                                     aList = aPreviewOp->subPreview();
  XGUI_Displayer* aDisplayer = myWorkshop->displayer();
  std::list<int> aModes = aPreviewOp->getSelectionModes(aPreviewOp->feature());

  std::map<boost::shared_ptr<ModelAPI_Feature>, boost::shared_ptr<GeomAPI_Shape> >::const_iterator
                                                         anIt = aList.begin(), aLast = aList.end();
  for (; anIt != aLast; anIt++) {
    boost::shared_ptr<ModelAPI_Feature> aFeature = (*anIt).first;
    boost::shared_ptr<GeomAPI_Shape> aPreview = (*anIt).second;
    aDisplayer->Redisplay(aFeature,
                          aPreview ? aPreview->impl<TopoDS_Shape>() : TopoDS_Shape(), false);
    aDisplayer->ActivateInLocalContext(aFeature, aModes, false);
  }
  aDisplayer->UpdateViewer();
}

