// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

#ifndef XGUI_WORKSHOP_LISTENER_H
#define XGUI_WORKSHOP_LISTENER_H

#include "XGUI.h"
#include <Events_Listener.h>
#include <Events_Message.h>

#include <ModelAPI_Feature.h>
#include <Events_InfoMessage.h>

#include <QObject>

class Config_FeatureMessage;

class ModelAPI_ObjectUpdatedMessage;
class XGUI_Workshop;

class ModuleBase_IWorkshop;

class QString;
class Events_InfoMessage;

/**\class XGUI_WorkshopListener
 * \ingroup GUI
 * \brief Class which process the events from the event loop.
 */
class XGUI_EXPORT XGUI_WorkshopListener : public QObject, public Events_Listener
{
  Q_OBJECT
public:
  /// Constructor. Used only if the workshop is launched in Salome environment
  /// \param theWorkshop a reference to workshop. 
   XGUI_WorkshopListener(ModuleBase_IWorkshop* theWorkshop);
  virtual ~XGUI_WorkshopListener();

  /// Register this class in the events loop for several types of events
  void initializeEventListening();

  //! Redefinition of Events_Listener method
  virtual void processEvent(const std::shared_ptr<Events_Message>& theMessage);

signals:
  /// Emitted when error in applivation happens
  void errorOccurred(std::shared_ptr<Events_InfoMessage> theMsg);

protected:
  /// Procedure to process postponed events
  bool event(QEvent * theEvent);

  /// Process feature update message
  /// \param theMsg a message with a container of objects
  void onFeatureUpdatedMsg(const std::shared_ptr<ModelAPI_ObjectUpdatedMessage>& theMsg);

  /// Process feature created message
  /// \param theMsg a message with a container of objects
  void onFeatureCreatedMsg(const std::shared_ptr<ModelAPI_ObjectUpdatedMessage>& theMsg);

  /// Process feature redisplay message
  /// \param theMsg a message with a container of objects
  void onFeatureRedisplayMsg(const std::shared_ptr<ModelAPI_ObjectUpdatedMessage>& theMsg);

  /// Process clearing of presentation
  /// \param theMsg an event message
  void onFeatureEmptyPresentationMsg(const std::shared_ptr<ModelAPI_ObjectUpdatedMessage>& theMsg);

  /// Displaus object and fit all viewer if the object is first (update viewer will not be called)
  /// Asks the module whether the object can be displayed
  /// \param theObj an object
  /// \param theFirstVisualizedBody an output state whether there are not object displayed in the
  /// view and the displayed object is a body
  /// \return true if the object is displayed
  bool displayObject(ObjectPtr theObj);

  /// Answer if the viewer needed to be fit all after the object visualization
  /// The condition is there is not shown objects and the object is a group or body result
  /// \param theObject a shown object
  /// \param theNbOfShownObjects a count of already shown objects in the viewer
  /// \return true if fit all should be done
  bool neededFitAll(ObjectPtr theObject, const int theNbOfShownObjects);

  /// Calls the module method of cusomize object for the feature of the current operation
  /// Perform cusomize if the force redisplay flag is true or the list of objects contains the
  /// current operation feature
  /// \param theObjects a list of objects to find current operation feature 
  ///                   if forced redisplay is false
  /// \param theForceRedisplay a flag to customize object even always
  /// \return true if the object is modified
  bool customizeCurrentObject(const std::set<ObjectPtr>& theObjects, bool theForceRedisplay);

  /// Returns the workshop
  XGUI_Workshop* workshop() const;

private:
  ModuleBase_IWorkshop* myWorkshop; // the current workshop

  bool myUpdatePrefs;
};

#endif
