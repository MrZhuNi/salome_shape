// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include "ModuleBase_EventsListener.h"
#include "ModuleBase_Events.h"

#include <ModelAPI_Events.h>

#include <Events_Loop.h>
#include <Events_InfoMessage.h>
#include <Events_LongOp.h>
#include <Config_FeatureMessage.h>



static ModuleBase_EventsListener* MYListener = NULL;

//Q_DECLARE_METATYPE(EventMsgPtr)
//qRegisterMetaType<std::shared_ptr<Events_Message>>();
//qRegisterMetaType<std::shared_ptr<Events_Message>>("std::shared_ptr<Events_Message>");


ModuleBase_EventsListener::ModuleBase_EventsListener()
  : QThread()
{
  //Initialize event listening
  Events_Loop* aLoop = Events_Loop::loop();
  aLoop->registerListener(this, Events_InfoMessage::errorID());  //!< Listening application errors.
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_CREATED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY));
  aLoop->registerListener(this, Events_LongOp::eventID());
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_PLUGIN_LOADED));

  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_UPDATE_VIEWER_BLOCKED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_UPDATE_VIEWER_UNBLOCKED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_EMPTY_AIS_PRESENTATION));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_UPDATE_BY_WIDGET_SELECTION));

  aLoop->registerListener(this, Events_Loop::eventByName("FinishOperation"));
  aLoop->registerListener(this, Events_Loop::eventByName("AbortOperation"));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_DOCUMENT_CHANGED));
  aLoop->registerListener(this, Events_Loop::eventByName(Config_FeatureMessage::GUI_EVENT()));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_DELETED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_ORDER_UPDATED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_FEATURE_STATE_RESPONSE));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_EMPTY_OPERATION_PRESENTATION));
}



ModuleBase_EventsListener* ModuleBase_EventsListener::instance()
{
  if (!MYListener) {
    MYListener = new ModuleBase_EventsListener();
    MYListener->start();
  }
  return MYListener;
}

void ModuleBase_EventsListener::processEvent(const std::shared_ptr<Events_Message>& theMessage)
{
  ModuleBase_Event* aEvnt = new ModuleBase_Event(theMessage);
  myMutex.lock();
  myMessages.append(aEvnt);
  myMutex.unlock();
}

void ModuleBase_EventsListener::run()
{
  while (1) {
    while (myMessages.size()) {
      myMutex.lock();
      ModuleBase_Event* aMsg = myMessages.first();
      myMessages.removeFirst();
      myMutex.unlock();
      emit hasEvent(aMsg);
      //aMsg->deleteLater();
    }
    msleep(10);
  }
}
