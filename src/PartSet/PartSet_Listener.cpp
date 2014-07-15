// File:        PartSet_Listener.h
// Created:     28 Apr 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_Listener.h>

#include <PartSet_Module.h>

#include <XGUI_Displayer.h>
#include <XGUI_Workshop.h>

#include <Events_Loop.h>
#include <ModelAPI_Events.h>

#include <SketchPlugin_Sketch.h>

#ifdef _DEBUG
#include <QDebug>
#endif

using namespace std;

PartSet_Listener::PartSet_Listener(PartSet_Module* theModule)
: myModule(theModule)
{
  Events_Loop* aLoop = Events_Loop::loop();
  aLoop->registerListener(this, aLoop->eventByName(EVENT_OBJECT_UPDATED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_CREATED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_DELETED));
}

PartSet_Listener::~PartSet_Listener()
{
}

//******************************************************
void PartSet_Listener::processEvent(const Events_Message* theMessage)
{
  QString aType = QString(theMessage->eventID().eventText());
  if (aType == EVENT_OBJECT_UPDATED ||
      aType == EVENT_OBJECT_CREATED)
  {
    const ModelAPI_ObjectUpdatedMessage* aUpdMsg = 
      dynamic_cast<const ModelAPI_ObjectUpdatedMessage*>(theMessage);
    std::set<ObjectPtr > aFeatures = aUpdMsg->objects();
    std::set<ObjectPtr >::const_iterator anIt = aFeatures.begin(), aLast = aFeatures.end();
    for (; anIt != aLast; anIt++) {
      ObjectPtr aObject = *anIt;
      FeaturePtr aFeature = boost::dynamic_pointer_cast<ModelAPI_Feature>(aObject);
      if (aFeature) {
        if (myModule->workshop()->displayer()->isVisible(aFeature->firstResult()) ||
            aType == EVENT_OBJECT_CREATED) {
          myModule->visualizePreview(aFeature->firstResult(), true, false);
          //if (aType == EVENT_OBJECT_CREATED)
          myModule->activateFeature(aFeature, true);
        }
      }
    }
    myModule->workshop()->displayer()->updateViewer();
  }
  if (aType == EVENT_OBJECT_DELETED)
  {
    const ModelAPI_ObjectDeletedMessage* aDelMsg = 
      dynamic_cast<const ModelAPI_ObjectDeletedMessage*>(theMessage);
    boost::shared_ptr<ModelAPI_Document> aDoc = aDelMsg->document();

    std::set<std::string> aGroups = aDelMsg->groups();
    std::set<std::string>::const_iterator anIt = aGroups.begin(), aLast = aGroups.end();
    for (; anIt != aLast; anIt++) {
      std::string aGroup = *anIt;
      if (aGroup.compare(SKETCH_KIND) == 0) { // Update only Sketch group
        myModule->workshop()->displayer()->eraseDeletedResults();
        myModule->updateCurrentPreview(aGroup);
      }
    }
  }
}
