// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_Session.cxx
// Created:     20 Mar 2014
// Author:      Mikhail PONIKAROV

#include <Model_Session.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Plugin.h>
#include <Model_Data.h>
#include <Model_Document.h>
#include <Model_Objects.h>
#include <Model_Application.h>
#include <Model_Events.h>
#include <Model_Validator.h>
#include <ModelAPI_Events.h>
#include <Events_Loop.h>
#include <Events_InfoMessage.h>
#include <Config_FeatureMessage.h>
#include <Config_AttributeMessage.h>
#include <Config_ValidatorMessage.h>
#include <Config_ModuleReader.h>
#include <Config_ValidatorReader.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_Tools.h>

#include <TDF_CopyTool.hxx>
#include <TDF_DataSet.hxx>
#include <TDF_RelocationTable.hxx>
#include <TDF_ClosureTool.hxx>

static Model_Session* myImpl = new Model_Session();

// t oredirect all calls to the root document
#define ROOT_DOC std::dynamic_pointer_cast<Model_Document>(moduleDocument())

bool Model_Session::load(const char* theFileName)
{
  bool aRes = ROOT_DOC->load(theFileName, "root", ROOT_DOC);
  return aRes;
}

bool Model_Session::save(const char* theFileName, std::list<std::string>& theResults)
{
  return ROOT_DOC->save(theFileName, "root", theResults);
}

void Model_Session::closeAll()
{
  Model_Application::getApplication()->deleteAllDocuments();
}

void Model_Session::startOperation(const std::string& theId, const bool theAttachedToNested)
{
  myOperationAttachedToNext = theAttachedToNested;
  ROOT_DOC->startOperation();
  ROOT_DOC->operationId(theId);
  static std::shared_ptr<Events_Message> aStartedMsg
    (new Events_Message(Events_Loop::eventByName("StartOperation")));
  Events_Loop::loop()->send(aStartedMsg);
  // remove all useless documents that has been closed: on start of operation undo/redo is cleared
  // MPV: this code is dangerous now because it may close the document that is activated right now
  // but not in the list of the opened documents yet (create, delete, undo, activate Part)
  // later this must be updated by correct usage of uniques IDs of documents, not names of results
  //std::list<std::shared_ptr<ModelAPI_Document> > aUsedDocs = allOpenedDocuments();
  //Model_Application::getApplication()->removeUselessDocuments(aUsedDocs);
}

void Model_Session::finishOperation()
{
  setCheckTransactions(false);
  ROOT_DOC->finishOperation();
  if (myOperationAttachedToNext) { // twice, with nested
    ROOT_DOC->finishOperation();
    myOperationAttachedToNext = false;
  }
  setCheckTransactions(true);
}

void Model_Session::abortOperation()
{
  setCheckTransactions(false);
  ROOT_DOC->abortOperation();
  if (myOperationAttachedToNext) { // twice, with nested
    ROOT_DOC->abortOperation();
    myOperationAttachedToNext = false;
  }
  setCheckTransactions(true);
  // here the update mechanism may work after abort, so, supress the warnings about
  // modifications outside of the transactions
  bool aWasCheck = myCheckTransactions;
  myCheckTransactions = false;
  static std::shared_ptr<Events_Message> anAbortMsg
    (new Events_Message(Events_Loop::eventByName("AbortOperation")));
  Events_Loop::loop()->send(anAbortMsg);
  myCheckTransactions = true;
  myCheckTransactions = aWasCheck;
}

bool Model_Session::isOperation()
{
  return ROOT_DOC->isOperation();
}

bool Model_Session::isModified()
{
  return ROOT_DOC->isModified();
}

bool Model_Session::canUndo()
{
  return ROOT_DOC->canUndo();
}

void Model_Session::undo()
{
  setCheckTransactions(false);
  ROOT_DOC->undo();
  setCheckTransactions(true);
}

bool Model_Session::canRedo()
{
  return ROOT_DOC->canRedo();
}

void Model_Session::redo()
{
  setCheckTransactions(false);
  ROOT_DOC->redo();
  setCheckTransactions(true);
}

//! Returns stack of performed operations
std::list<std::string> Model_Session::undoList()
{
  return ROOT_DOC->undoList();
}
//! Returns stack of rolled back operations
std::list<std::string> Model_Session::redoList()
{
  return ROOT_DOC->redoList();
}

FeaturePtr Model_Session::createFeature(std::string theFeatureID, Model_Document* theDocOwner)
{
  if (this != myImpl) {
    return myImpl->createFeature(theFeatureID, theDocOwner);
  }

  // load all information about plugins, features and attributes
  LoadPluginsInfo();

  if (myPlugins.find(theFeatureID) != myPlugins.end()) {
    std::pair<std::string, std::string>& aPlugin = myPlugins[theFeatureID]; // plugin and doc kind
    if (!aPlugin.second.empty() && aPlugin.second != theDocOwner->kind()) {
      Events_InfoMessage("Model_Session",
          "Feature '%1' can be created only in document '%2' by the XML definition")
          .arg(theFeatureID).arg(aPlugin.second).send();
      return FeaturePtr();
    }
    myCurrentPluginName = aPlugin.first;
    if (myPluginObjs.find(myCurrentPluginName) == myPluginObjs.end()) {
      // load plugin library if not yet done
      Config_ModuleReader::loadPlugin(myCurrentPluginName);
    }
    if (myPluginObjs.find(myCurrentPluginName) != myPluginObjs.end()) {
      FeaturePtr aCreated = myPluginObjs[myCurrentPluginName]->createFeature(theFeatureID);
      if (!aCreated) {
        Events_InfoMessage("Model_Session",
            "Can not initialize feature '%1' in plugin '%2'")
            .arg(theFeatureID).arg(myCurrentPluginName).send();
      }
      return aCreated;
    } else {
      Events_InfoMessage("Model_Session",
        "Can not load plugin '%1'").arg(myCurrentPluginName).send();
    }
  } else {
    Events_InfoMessage("Model_Session",
      "Feature '%1' not found in any plugin").arg(theFeatureID).send();
  }

  return FeaturePtr();  // return nothing
}

std::shared_ptr<ModelAPI_Document> Model_Session::moduleDocument()
{
  Handle(Model_Application) anApp = Model_Application::getApplication();
  bool aFirstCall = !anApp->hasRoot();
  if (aFirstCall) {
    // to be sure that plugins are loaded,
    // even before the first "createFeature" call (in unit tests)

    LoadPluginsInfo();
    // creation of the root document is always outside of the transaction, so, avoid checking it
    setCheckTransactions(false);
    anApp->createDocument(0); // 0 is a root ID
    // creation of the root document is always outside of the transaction, so, avoid checking it
    setCheckTransactions(true);
  }
  return anApp->rootDocument();
}

std::shared_ptr<ModelAPI_Document> Model_Session::document(int theDocID)
{
  return std::shared_ptr<ModelAPI_Document>(
      Model_Application::getApplication()->document(theDocID));
}

bool Model_Session::hasModuleDocument()
{
  return Model_Application::getApplication()->hasRoot();
}

std::shared_ptr<ModelAPI_Document> Model_Session::activeDocument()
{
  if (!myCurrentDoc || !Model_Application::getApplication()->hasDocument(myCurrentDoc->id()))
    myCurrentDoc = moduleDocument();
  return myCurrentDoc;
}

/// makes the last feature in the document as the current
static void makeCurrentLast(std::shared_ptr<ModelAPI_Document> theDoc) {
  if (theDoc.get()) {
    FeaturePtr aLast = std::dynamic_pointer_cast<Model_Document>(theDoc)->lastFeature();
    // if last is nested into something else, make this something else as last:
    // otherwise it will look like edition of sub-element, so, the main will be disabled
    if (aLast.get()) {
      CompositeFeaturePtr aMain = ModelAPI_Tools::compositeOwner(aLast);
      while(aMain.get()) {
        aLast = aMain;
        aMain = ModelAPI_Tools::compositeOwner(aLast);
      }
    }
    theDoc->setCurrentFeature(aLast, false);
  }
}

void Model_Session::setActiveDocument(
  std::shared_ptr<ModelAPI_Document> theDoc, bool theSendSignal)
{
  if (myCurrentDoc != theDoc) {
    if (myCurrentDoc.get())
      myCurrentDoc->setActive(false);
    if (theDoc.get())
      theDoc->setActive(true);

    std::shared_ptr<ModelAPI_Document> aPrevious = myCurrentDoc;
    myCurrentDoc = theDoc;
    if (theDoc.get() && theSendSignal) {
      // this must be before the synchronisation call because features in PartSet lower than this
      // part feature must be disabled and don't recomputed anymore (issue 1156,
      // translation feature is failed on activation of Part 2)
      if (isOperation()) { // do it only in transaction, not on opening of document
        DocumentPtr aRoot = moduleDocument();
        if (myCurrentDoc != aRoot) {
          FeaturePtr aPartFeat = ModelAPI_Tools::findPartFeature(aRoot, myCurrentDoc);
          if (aPartFeat.get()) {
            aRoot->setCurrentFeature(aPartFeat, false);
          }
        }
      }
      // syncronize the document: it may be just opened or opened but removed before
      std::shared_ptr<Model_Document> aDoc = std::dynamic_pointer_cast<Model_Document>(theDoc);
      if (aDoc.get()) {
        bool aWasChecked = myCheckTransactions;
        setCheckTransactions(false);
        TDF_LabelList anEmptyUpdated;
        aDoc->objects()->synchronizeFeatures(anEmptyUpdated, true, true, false, true);
        if (aWasChecked)
            setCheckTransactions(true);
      }
      static std::shared_ptr<Events_Message> aMsg(
          new Events_Message(Events_Loop::eventByName(EVENT_DOCUMENT_CHANGED)));
      Events_Loop::loop()->send(aMsg);
    }
    // make the current state correct and synchronised in the module and sub-documents
    if (isOperation()) { // do it only in transaction, not on opening of document
      if (myCurrentDoc == moduleDocument()) {
        // make the current feature the latest in root, in previous root current become also last
        makeCurrentLast(aPrevious);
        makeCurrentLast(myCurrentDoc);
      } else {
        // make the current feature the latest in sub, root current feature becomes this sub
        makeCurrentLast(myCurrentDoc);
      }
    }
  }
}

std::list<std::shared_ptr<ModelAPI_Document> > Model_Session::allOpenedDocuments()
{
  std::list<std::shared_ptr<ModelAPI_Document> > aResult;
  aResult.push_back(moduleDocument());
  // add subs recursively
  std::list<std::shared_ptr<ModelAPI_Document> >::iterator aDoc = aResult.begin();
  for(; aDoc != aResult.end(); aDoc++) {
    DocumentPtr anAPIDoc = *aDoc;
    std::shared_ptr<Model_Document> aDoc = std::dynamic_pointer_cast<Model_Document>(anAPIDoc);
    if (aDoc) {
      const std::set<int> aSubs = aDoc->subDocuments();
      std::set<int>::const_iterator aSubIter = aSubs.cbegin();
      for(; aSubIter != aSubs.cend(); aSubIter++) {
        aResult.push_back(Model_Application::getApplication()->document(*aSubIter));
      }
    }
  }
  return aResult;
}

bool Model_Session::isLoadByDemand(const std::string theDocID)
{
  return Model_Application::getApplication()->isLoadByDemand(theDocID);
}

std::shared_ptr<ModelAPI_Document> Model_Session::copy(
    std::shared_ptr<ModelAPI_Document> theSource, const int theDestID)
{
  std::shared_ptr<Model_Document> aNew = Model_Application::getApplication()->document(theDestID);
  // make a copy of all labels
  TDF_Label aSourceRoot = std::dynamic_pointer_cast<Model_Document>(theSource)->document()->Main()
      .Father();
  TDF_Label aTargetRoot = aNew->document()->Main().Father();
  Handle(TDF_DataSet) aDS = new TDF_DataSet;
  aDS->AddLabel(aSourceRoot);
  TDF_ClosureTool::Closure(aDS);
  Handle(TDF_RelocationTable) aRT = new TDF_RelocationTable(Standard_True);
  aRT->SetRelocation(aSourceRoot, aTargetRoot);
  TDF_CopyTool::Copy(aDS, aRT);

  TDF_LabelList anEmptyUpdated;
  aNew->objects()->synchronizeFeatures(anEmptyUpdated, true, true, true, true);
  return aNew;
}

Model_Session::Model_Session()
{
  myPluginsInfoLoaded = false;
  myCheckTransactions = true;
  myOperationAttachedToNext = false;
  ModelAPI_Session::setSession(std::shared_ptr<ModelAPI_Session>(this));
  // register the configuration reading listener
  Events_Loop* aLoop = Events_Loop::loop();
  static const Events_ID kFeatureEvent =
    Events_Loop::eventByName(Config_FeatureMessage::MODEL_EVENT());
  aLoop->registerListener(this, kFeatureEvent);
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_CREATED), 0, true);
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_UPDATED), 0, true);
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_DELETED), 0, true);
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_VALIDATOR_LOADED));
}

void Model_Session::processEvent(const std::shared_ptr<Events_Message>& theMessage)
{
  static const Events_ID kFeatureEvent =
    Events_Loop::eventByName(Config_FeatureMessage::MODEL_EVENT());
  static const Events_ID kValidatorEvent = Events_Loop::eventByName(EVENT_VALIDATOR_LOADED);
  if (theMessage->eventID() == kFeatureEvent) {
    const std::shared_ptr<Config_FeatureMessage> aMsg =
      std::dynamic_pointer_cast<Config_FeatureMessage>(theMessage);
    if (aMsg) {

      // process the plugin info, load plugin
      if (myPlugins.find(aMsg->id()) == myPlugins.end()) {
        myPlugins[aMsg->id()] = std::pair<std::string, std::string>(
          aMsg->pluginLibrary(), aMsg->documentKind());
      }
    } else {
      const std::shared_ptr<Config_AttributeMessage> aMsgAttr =
        std::dynamic_pointer_cast<Config_AttributeMessage>(theMessage);
      if (aMsgAttr) {

        if (!aMsgAttr->isObligatory()) {
          validators()->registerNotObligatory(aMsgAttr->featureId(), aMsgAttr->attributeId());
        }
        if(aMsgAttr->isConcealment()) {
          validators()->registerConcealment(aMsgAttr->featureId(), aMsgAttr->attributeId());
        }
        const std::list<std::pair<std::string, std::string> >& aCases = aMsgAttr->getCases();
        if (!aCases.empty()) {
          validators()->registerCase(aMsgAttr->featureId(), aMsgAttr->attributeId(), aCases);
        }
      }
    }
    // plugins information was started to load, so, it will be loaded
    myPluginsInfoLoaded = true;
  } else if (theMessage->eventID() == kValidatorEvent) {
    std::shared_ptr<Config_ValidatorMessage> aMsg =
      std::dynamic_pointer_cast<Config_ValidatorMessage>(theMessage);
    if (aMsg) {
      if (aMsg->attributeId().empty()) {  // feature validator
        validators()->assignValidator(aMsg->validatorId(), aMsg->featureId(), aMsg->parameters());
      } else {  // attribute validator
        validators()->assignValidator(aMsg->validatorId(), aMsg->featureId(), aMsg->attributeId(),
                                      aMsg->parameters());
      }
    }
  } else {  // create/update/delete
    if (myCheckTransactions && !isOperation())
      Events_InfoMessage("Model_Session",
        "Modification of data structure outside of the transaction").send();
    // if part is deleted, make the root as the current document (on undo of Parts creations)
    static const Events_ID kDeletedEvent = Events_Loop::eventByName(EVENT_OBJECT_DELETED);
    if (theMessage->eventID() == kDeletedEvent) {
      std::shared_ptr<ModelAPI_ObjectDeletedMessage> aDeleted =
        std::dynamic_pointer_cast<ModelAPI_ObjectDeletedMessage>(theMessage);
      if (aDeleted &&
          aDeleted->groups().find(ModelAPI_ResultPart::group()) != aDeleted->groups().end())
      {
         // check that the current feature of the session is still the active Part (even disabled)
        bool aFound = false;
        FeaturePtr aCurrentPart = moduleDocument()->currentFeature(true);
        if (aCurrentPart.get()) {
          const std::list<std::shared_ptr<ModelAPI_Result> >& aResList = aCurrentPart->results();
          std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aRes = aResList.begin();
          for(; !aFound && aRes != aResList.end(); aRes++) {
            ResultPartPtr aPRes = std::dynamic_pointer_cast<ModelAPI_ResultPart>(*aRes);
            if (aPRes.get() && aPRes->isActivated() && aPRes->partDoc() == activeDocument()) {
              aFound = true;

            }
          }
        }
        if (!aFound) { // if not, the part was removed, so activate the module document
          if (myCurrentDoc.get())
            setActiveDocument(moduleDocument());
        }
      }
    }
  }
}

void Model_Session::LoadPluginsInfo()
{
  if (myPluginsInfoLoaded)  // nothing to do
    return;
  Config_ModuleReader::loadScript("salome.shaper.initConfig", false);
  // Read plugins information from XML files
  Config_ModuleReader aModuleReader(Config_FeatureMessage::MODEL_EVENT());
  aModuleReader.readAll();
  std::set<std::string> aFiles = aModuleReader.modulePluginFiles();
  std::set<std::string>::iterator it = aFiles.begin();
  for ( ; it != aFiles.end(); it++ ) {
    Config_ValidatorReader aValidatorReader (*it);
    aValidatorReader.readAll();
  };

}

void Model_Session::registerPlugin(ModelAPI_Plugin* thePlugin)
{
  myPluginObjs[myCurrentPluginName] = thePlugin;
  static Events_ID EVENT_LOAD = Events_Loop::loop()->eventByName(EVENT_PLUGIN_LOADED);
  ModelAPI_EventCreator::get()->sendUpdated(ObjectPtr(), EVENT_LOAD);
  Events_Loop::loop()->flush(EVENT_LOAD);
  // If the plugin has an ability to process GUI events, register it
  Events_Listener* aListener = dynamic_cast<Events_Listener*>(thePlugin);
  if (aListener) {
    Events_Loop* aLoop = Events_Loop::loop();
    static Events_ID aStateRequestEventId =
        Events_Loop::loop()->eventByName(EVENT_FEATURE_STATE_REQUEST);
    aLoop->registerListener(aListener, aStateRequestEventId);
  }
}

ModelAPI_ValidatorsFactory* Model_Session::validators()
{
  static Model_ValidatorsFactory* aFactory = new Model_ValidatorsFactory;
  return aFactory;
}

int Model_Session::transactionID()
{
  return ROOT_DOC->transactionID();
}
