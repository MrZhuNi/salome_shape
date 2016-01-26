// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_Events.h
// Created:     10 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef MODELAPI_EVENTS_H_
#define MODELAPI_EVENTS_H_

#include <ModelAPI.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Attribute.h>
#include <Events_MessageGroup.h>
#include <Events_Loop.h>

#include <memory>
#include <string>
#include <set>
#include <map>


class ModelAPI_Document;

/// Event ID that feature is created (comes with ModelAPI_ObjectUpdatedMessage)
static const char * EVENT_OBJECT_CREATED = "ObjectCreated";
/// Event ID that data of feature is updated (comes with Model_ObjectUpdatedMessage)
static const char * EVENT_OBJECT_UPDATED = "ObjectUpdated";
/// Event ID that data of feature is deleted (comes with Model_ObjectDeletedMessage)
static const char * EVENT_OBJECT_DELETED = "ObjectDeleted";
/// Event ID that name of feature is changed (comes with Model_ObjectRenamedMessage)
static const char * EVENT_OBJECT_RENAMED = "ObjectRenamed";
/// Event ID that data of feature is updated (comes with ModelAPI_ObjectUpdatedMessage)
static const char * EVENT_OBJECT_MOVED = "ObjectsMoved";
/// Event ID that visualization must be redisplayed (comes with ModelAPI_ObjectUpdatedMessage)
static const char * EVENT_OBJECT_TO_REDISPLAY = "ObjectsToRedisplay";
/// Event ID that error state or error message was updated for the object
static const char * EVENT_OBJECT_ERROR_CHANGED = "ObjectsErrorChanged";
/// Event ID that visualization must be redisplayed (comes with ModelAPI_ObjectUpdatedMessage)
static const char * EVENT_OPERATION_LAUNCHED = "OperationLaunched";
/// Event ID that plugin is loaded (comes with ModelAPI_ObjectUpdatedMessage)
static const char * EVENT_PLUGIN_LOADED = "PliginLoaded";
//
static const char * EVENT_DOCUMENT_CHANGED = "CurrentDocumentChanged";

static const char * EVENT_FEATURE_STATE_REQUEST = "FeatureStateRequest";
static const char * EVENT_FEATURE_STATE_RESPONSE = "FeatureStateResponse";


static const char * EVENT_UPDATE_VIEWER_BLOCKED = "UpdateViewerBlocked";
static const char * EVENT_UPDATE_VIEWER_UNBLOCKED = "UpdateViewerUnblocked";

/// Event ID that solver has conflicting constraints (comes with ModelAPI_SolverFailedMessage)
static const char * EVENT_SOLVER_FAILED = "SolverFailed";
/// Event ID that the problem in solver disappeared
static const char * EVENT_SOLVER_REPAIRED = "SolverRepaired";

/// Event ID that order of objects in group is changed, so, tree must be fully rectreated (movement of feature)
static const char * EVENT_ORDER_UPDATED = "OrderUpdated";

/// Event ID that informs that some object has changed the stability
static const char * EVENT_STABILITY_CHANGED = "StabilityChanged";

/// Message that feature was changed (used for Object Browser update): moved, updated and deleted
class MODELAPI_EXPORT ModelAPI_ObjectUpdatedMessage : public Events_MessageGroup
{
 protected:
  /// Creates an empty message
  ModelAPI_ObjectUpdatedMessage(const Events_ID theID, const void* theSender = 0);
  /// The virtual destructor
  virtual ~ModelAPI_ObjectUpdatedMessage();

 public:
  /// Returns the feature that has been updated
  virtual const std::set<ObjectPtr>& objects() const = 0;

  //! Creates a new empty group (to store it in the loop before flush)
  virtual std::shared_ptr<Events_MessageGroup> newEmpty() = 0;

  //! Allows to join the given message with the current one
  virtual void Join(const std::shared_ptr<Events_MessageGroup>& theJoined) = 0;
};

/// Message that feature was deleted (used for Object Browser update)
class MODELAPI_EXPORT ModelAPI_ObjectDeletedMessage : public Events_MessageGroup
{
protected:
  /// Creates an empty message
  ModelAPI_ObjectDeletedMessage(const Events_ID theID, const void* theSender = 0);
  /// The virtual destructor
  virtual ~ModelAPI_ObjectDeletedMessage();

public:
  /// Returns the document that has been updated
  virtual std::shared_ptr<ModelAPI_Document> document() const = 0;

  /// Returns the groups where the objects were deleted
  virtual const std::set<std::string>& groups() const = 0;

  /// Creates the new empty message of this kind
  virtual std::shared_ptr<Events_MessageGroup> newEmpty() = 0;

  /// Returns the identifier of the kind of a message
  virtual const Events_ID messageId() = 0;

  /// Appenad to this message the given one.
  virtual void Join(const std::shared_ptr<Events_MessageGroup>& theJoined) = 0;
};

/// Message that order changed (used for Object Browser update)
class MODELAPI_EXPORT ModelAPI_OrderUpdatedMessage : public Events_Message
{
protected:
  /// Creates an empty message
  ModelAPI_OrderUpdatedMessage(const Events_ID theID, const void* theSender = 0);
  /// The virtual destructor
  virtual ~ModelAPI_OrderUpdatedMessage();

public:
  /// Returns the document that has been updated
  virtual std::shared_ptr<ModelAPI_Document> document() const = 0;

  /// Returns the groups where the objects were reordered
  virtual const std::string& group() const = 0;

  /// Returns the identifier of the kind of a message
  virtual const Events_ID messageId() = 0;
};

/// Allows to create ModelAPI messages
class MODELAPI_EXPORT ModelAPI_EventCreator
{
public:
  /// creates created, updated or moved messages and sends to the loop
  virtual void sendUpdated(const ObjectPtr& theObject, const Events_ID& theEvent,
                           const bool isGroupped = true) const = 0;
  /// creates deleted message and sends to the loop
  virtual void sendDeleted(const std::shared_ptr<ModelAPI_Document>& theDoc,
                           const std::string& theGroup) const = 0;
  /// creates reordered message and sends to the loop
  virtual void sendReordered(const std::shared_ptr<ModelAPI_Document>& theDoc,
                             const std::string& theGroup) const = 0;

  /// returns the creator instance
  static const ModelAPI_EventCreator* get();

  /// sets the creator instance
  static void set(const ModelAPI_EventCreator* theCreator);
};

// TODO(sbh): Move this message into a separate package, like "GuiAPI"
/// Contains the state information about the feature: is it enabled or disabled.
class ModelAPI_FeatureStateMessage : public Events_Message
{
public:
  /// Creates an empty message
  MODELAPI_EXPORT ModelAPI_FeatureStateMessage(const Events_ID theID, const void* theSender = 0);
  /// The virtual destructor
  MODELAPI_EXPORT virtual ~ModelAPI_FeatureStateMessage();

  /// Returns the feature this message is related to
  MODELAPI_EXPORT std::shared_ptr<ModelAPI_Feature> feature() const;
  /// Stores the feature this message is related to
  MODELAPI_EXPORT void setFeature(std::shared_ptr<ModelAPI_Feature>& theFeature);
  // For response
  /// Returns true if feature has specific state
  MODELAPI_EXPORT bool hasState(const std::string& theFeatureId) const;
  /// Returns true if feature is enabled
  MODELAPI_EXPORT bool state(const  std::string& theFeatureId, bool theDefault = false) const;
  /// Stores the feature state
  MODELAPI_EXPORT void setState(const std::string& theFeatureId, bool theValue);
  /// Returns all feature IDs with states
  MODELAPI_EXPORT std::list<std::string> features() const;

 private:
  /// For Request
  std::shared_ptr<ModelAPI_Feature> myCurrentFeature;
  /// For response
  std::map<std::string, bool> myFeatureState;
};

/// Message that document (Part, PartSet) was created
class ModelAPI_DocumentCreatedMessage : public Events_Message
{
  DocumentPtr myDocument;

 public:
  /// Creates an empty message
  MODELAPI_EXPORT ModelAPI_DocumentCreatedMessage(const Events_ID theID, const void* theSender = 0);
  /// The virtual destructor
  MODELAPI_EXPORT virtual ~ModelAPI_DocumentCreatedMessage();
  /// Static. Returns EventID of the message.
  MODELAPI_EXPORT static Events_ID eventId()
  {
    static const char * MY_DOCUMENT_CREATED_EVENT_ID("DocumentCreated");
    return Events_Loop::eventByName(MY_DOCUMENT_CREATED_EVENT_ID);
  }

  /// Returns a document stored in the message
  MODELAPI_EXPORT DocumentPtr document() const;
  /// Sets a document to the message
  MODELAPI_EXPORT void setDocument(DocumentPtr theDocument);
};

/// Message that attribute text should be evaluated in the attribute value
class ModelAPI_AttributeEvalMessage : public Events_Message
{
  AttributePtr myAttribute;

 public:
  /// Static. Returns EventID of the message.
  MODELAPI_EXPORT static Events_ID& eventId()
  {
    static const char * MY_ATTRIBUTE_EVALUATION_EVENT_ID("AttributeEvaluationRequest");
    static Events_ID anId = Events_Loop::eventByName(MY_ATTRIBUTE_EVALUATION_EVENT_ID);
    return anId;
  }

  /// Useful method that creates and sends the AttributeEvalMessage event
  MODELAPI_EXPORT static void send(AttributePtr theAttribute, const void* theSender)
  {
    std::shared_ptr<ModelAPI_AttributeEvalMessage> aMessage =
      std::shared_ptr<ModelAPI_AttributeEvalMessage>(
      new ModelAPI_AttributeEvalMessage(eventId(), theSender));
    aMessage->setAttribute(theAttribute);
    Events_Loop::loop()->send(aMessage);
  }

  /// Creates an empty message
  MODELAPI_EXPORT ModelAPI_AttributeEvalMessage(const Events_ID theID, const void* theSender = 0);
  /// The virtual destructor
  MODELAPI_EXPORT virtual ~ModelAPI_AttributeEvalMessage();

  /// Returns a document stored in the message
  MODELAPI_EXPORT AttributePtr attribute() const;
  /// Sets an attribute to the message
  MODELAPI_EXPORT void setAttribute(AttributePtr theAttribute);
};

/// Message that the object is renamed
class ModelAPI_ObjectRenamedMessage : public Events_Message
{
  ObjectPtr myObject;
  std::string myOldName;
  std::string myNewName;

 public:
  /// Static. Returns EventID of the message.
  MODELAPI_EXPORT static Events_ID& eventId()
  {
    static const char * MY_OBJECT_RENAMED_EVENT_ID("ObjectRenamed");
    static Events_ID anId = Events_Loop::eventByName(MY_OBJECT_RENAMED_EVENT_ID);
    return anId;
  }

  /// Useful method that creates and sends the AttributeEvalMessage event
  MODELAPI_EXPORT static void send(ObjectPtr theObject,
                                   const std::string& theOldName,
                                   const std::string& theNewName,
                                   const void* theSender);

  /// Creates an empty message
  MODELAPI_EXPORT ModelAPI_ObjectRenamedMessage(const Events_ID theID, const void* theSender = 0);
  /// The virtual destructor
  MODELAPI_EXPORT virtual ~ModelAPI_ObjectRenamedMessage();

  /// Returns an object
  MODELAPI_EXPORT ObjectPtr object() const;
  /// Sets an object
  MODELAPI_EXPORT void setObject(ObjectPtr theObject);
  /// Returns an old name
  MODELAPI_EXPORT std::string oldName() const;
  /// Sets an old name
  MODELAPI_EXPORT void setOldName(const std::string& theOldName);
  /// Returns a new name
  MODELAPI_EXPORT std::string newName() const;
  /// Sets a new name
  MODELAPI_EXPORT void setNewName(const std::string& theNewName);
};

/// Message that the parameter should be replaced with its value
class ModelAPI_ReplaceParameterMessage : public Events_Message
{
  ObjectPtr myObject;

 public:
  /// Static. Returns EventID of the message.
  MODELAPI_EXPORT static Events_ID& eventId()
  {
    static const char * MY_EVENT_ID("ReplaceParameter");
    static Events_ID anId = Events_Loop::eventByName(MY_EVENT_ID);
    return anId;
  }

  /// Useful method that creates and sends the AttributeEvalMessage event
  MODELAPI_EXPORT static void send(ObjectPtr theObject,
                                   const void* theSender);

  /// Creates an empty message
  MODELAPI_EXPORT ModelAPI_ReplaceParameterMessage(const Events_ID theID, const void* theSender = 0);
  /// The virtual destructor
  MODELAPI_EXPORT virtual ~ModelAPI_ReplaceParameterMessage();

  /// Returns an object
  MODELAPI_EXPORT ObjectPtr object() const;
  /// Sets an object
  MODELAPI_EXPORT void setObject(ObjectPtr theObject);
};


class ModelAPI_SolverFailedMessage : public Events_Message
{
public:
  MODELAPI_EXPORT ModelAPI_SolverFailedMessage(const Events_ID theID, const void* theSender = 0);
  MODELAPI_EXPORT virtual ~ModelAPI_SolverFailedMessage();

  /// Sets list of conflicting constraints
  MODELAPI_EXPORT void setObjects(const std::set<ObjectPtr>& theObjects);
  /// Returns list of conflicting constraints
  MODELAPI_EXPORT const std::set<ObjectPtr>& objects() const;

private:
  std::set<ObjectPtr> myObjects;
};

#endif
