#pragma once
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

#ifndef ModuleBase_EventsListener_H
#define ModuleBase_EventsListener_H

#include "ModuleBase.h"

#include <Events_Listener.h>
#include <Events_Message.h>

#include <QThread>
#include <QList>
#include <QMutex>


class MODULEBASE_EXPORT ModuleBase_Event: public QObject
{
  Q_OBJECT
public:
  ModuleBase_Event() {}

  ModuleBase_Event(const std::shared_ptr<Events_Message>& theMsg) : myMsg(theMsg) {}

  void setMessage(const std::shared_ptr<Events_Message>& theMsg) {
    myMsg = theMsg;
  }

  std::shared_ptr<Events_Message> message() const { return myMsg; }

private:
  std::shared_ptr<Events_Message> myMsg;
};


class MODULEBASE_EXPORT ModuleBase_EventsListener : public QThread, public Events_Listener
{
  Q_OBJECT
public:
  static ModuleBase_EventsListener* instance();

  //! Redefinition of Events_Listener method
  virtual void processEvent(const std::shared_ptr<Events_Message>& theMessage);

signals:
  void hasEvent(ModuleBase_Event* theMsg);

protected:
  virtual void run();

private:
  ModuleBase_EventsListener();

private:
  QList<ModuleBase_Event*> myMessages;
  QMutex myMutex;
};

#endif