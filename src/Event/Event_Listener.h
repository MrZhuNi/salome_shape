// File:	Event_Listener.hxx
// Created:	Thu Mar 13 2014
// Author:	Mikhail PONIKAROV

#ifndef Event_Listener_HeaderFile
#define Event_Listener_HeaderFile

#include <Event.h>
class Event_Message;

/**\class Event_Listener
 * \ingroup EventLoop
 * \brief Base interface for any event listener.
 *
 * If some object wants to listen some events it must inherit
 * this class and register in the Loop.
 */
class EVENT_EXPORT Event_Listener {

public:
  //! This method is called by loop when the event is started to process.
  virtual void processEvent(const Event_Message* theMessage) = 0;
};

#endif
