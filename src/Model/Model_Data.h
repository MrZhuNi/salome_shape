// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_Data.hxx
// Created:     21 Mar 2014
// Author:      Mikhail PONIKAROV

#ifndef Model_Data_H_
#define Model_Data_H_

#include <Model.h>
#include <ModelAPI_Attribute.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeDocRef.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeRefAttrList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeStringArray.h>
#include <ModelAPI_AttributeIntArray.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Object.h>

#include <TDF_Label.hxx>
#include <TDataStd_BooleanArray.hxx>

#include <memory>

#include <map>
#include <list>
#include <string>
#include <set>

class ModelAPI_Attribute;

/**\class Model_Data
 * \ingroup DataModel
 * \brief General object of the application that allows
 * to get/set attributes from the document and compute result of an operation.
 */

class Model_Data : public ModelAPI_Data
{
  TDF_Label myLab;  ///< label of the feature in the document
  /// All attributes of the object identified by the attribute ID
  std::map<std::string, std::shared_ptr<ModelAPI_Attribute> > myAttrs;
  /// Array of flags of this data
  Handle(TDataStd_BooleanArray) myFlags;

  /// needed here to emit signal that object changed on change of the attribute
  ObjectPtr myObject;

  /// List of attributes referenced to owner (updated only during the transaction change)
  std::set<AttributePtr> myRefsToMe;
  /// flag that may block the "attribute updated" sending
  bool mySendAttributeUpdated;
  /// if some attribute was changed, but mySendAttributeUpdated was false, this stores this
  std::list<ModelAPI_Attribute*> myWasChangedButBlocked;

  /// Returns label of this feature
  TDF_Label label()
  {
    return myLab;
  }

  friend class Model_Document;
  friend class Model_Objects;
  friend class Model_Update;
  friend class Model_AttributeReference;
  friend class Model_AttributeRefAttr;
  friend class Model_AttributeRefList;
  friend class Model_AttributeRefAttrList;
  friend class Model_AttributeSelection;
  friend class Model_AttributeSelectionList;
  friend class Model_ValidatorsFactory;
  friend class Model_SelectionNaming;

 public:
  /// The simplest constructor. "setLabel" must be called just after to initialize correctly.
  Model_Data();
  /// Returns the name of the feature visible by the user in the object browser
  MODEL_EXPORT virtual std::string name();
  /// Defines the name of the feature visible by the user in the object browser
  MODEL_EXPORT virtual void setName(const std::string& theName);
  /// Returns the attribute that references to another document
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_AttributeDocRef> document(const std::string& theID);
  /// Returns the attribute that contains real value with double precision
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_AttributeDouble> real(const std::string& theID);
  /// Returns the attribute that contains double values array
  MODEL_EXPORT virtual
    std::shared_ptr<ModelAPI_AttributeDoubleArray> realArray(const std::string& theID);
  /// Returns the attribute that contains integer value
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_AttributeInteger>
    integer(const std::string& theID);
  /// Returns the attribute that contains reference to a feature
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_AttributeReference>
    reference(const std::string& theID);
  /// Returns the attribute that contains selection to a shape
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_AttributeSelection>
    selection(const std::string& theID);
  /// Returns the attribute that contains selection to a shape
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_AttributeSelectionList>
    selectionList(const std::string& theID);
  /// Returns the attribute that contains reference to an attribute of a feature
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_AttributeRefAttr>
    refattr(const std::string& theID);
  /// Returns the attribute that contains list of references to features
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_AttributeRefList>
    reflist(const std::string& theID);
  /// Returns the attribute that contains list of references to features
  /// or reference to an attribute of a feature
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_AttributeRefAttrList>
    refattrlist(const std::string& theID);
  /// Returns the attribute that contains boolean value
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_AttributeBoolean>
    boolean(const std::string& theID);
  /// Returns the attribute that contains real value with double precision
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_AttributeString>
    string(const std::string& theID);
  /// Returns the attribute that contains integer values array
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_AttributeIntArray>
    intArray(const std::string& theID);
  /// Returns the attribute that contains string values array
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_AttributeStringArray>
    stringArray(const std::string& theID);
  /// Returns the attribute that contains string values array
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_AttributeTables>
    tables(const std::string& theID);

  /// Returns the generic attribute by identifier
  /// \param theID identifier of the attribute
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_Attribute> attribute(const std::string& theID);
  /// Returns all attributes of the feature of the given type
  /// or all attributes if "theType" is empty
  MODEL_EXPORT virtual std::list<std::shared_ptr<ModelAPI_Attribute> >
    attributes(const std::string& theType);
  /// Returns all attributes ids of the feature of the given type
  /// or all attributes if "theType" is empty
  MODEL_EXPORT virtual std::list<std::string> attributesIDs(const std::string& theType);

  /// Identifier by the id (not fast, iteration by map)
  /// \param theAttr attribute already created in this data
  MODEL_EXPORT virtual const std::string& id(const std::shared_ptr<ModelAPI_Attribute>& theAttr);
  /// Returns true if data belongs to same features
  MODEL_EXPORT virtual bool isEqual(const std::shared_ptr<ModelAPI_Data>& theData);
  /// Returns true if it is correctly connected t othe data model
  MODEL_EXPORT virtual bool isValid();

  /// Returns the label where the shape must be stored (used in ResultBody)
  TDF_Label& shapeLab()
  {
    return myLab;
  }

  /// Initializes object by the attributes: must be called just after the object is created
  /// for each attribute of the object
  /// \param theID identifier of the attribute that can be referenced by this ID later
  /// \param theAttrType type of the created attribute (received from the type method)
  /// \returns the just created attribute
  MODEL_EXPORT virtual AttributePtr
    addAttribute(const std::string& theID, const std::string theAttrType);

  /// Useful method for "set" methods of the attributes: sends an UPDATE event and
  /// makes attribute initialized
  MODEL_EXPORT virtual void sendAttributeUpdated(ModelAPI_Attribute* theAttr);
  /// Blocks sending "attribute updated" if theBlock is true
  /// \param theBlock allows switching on/off the blocking state
  /// \param theSendMessage if false, it does not send the update message
  ///            even if something is changed
  ///            (normally is it used in attributeChanged because this message will be sent anyway)
  MODEL_EXPORT virtual void blockSendAttributeUpdated(
    const bool theBlock, const bool theSendMessage = true);

  /// Puts feature to the document data sub-structure
  MODEL_EXPORT void setLabel(TDF_Label theLab);

  /// Sets the object of this data
  MODEL_EXPORT virtual void setObject(ObjectPtr theObject)
  {
    myObject = theObject;
  }

  /// Erases all the data from the data model
  MODEL_EXPORT virtual void erase();

  /// Stores the state of the object to execute it later accordingly
  MODEL_EXPORT virtual void execState(const ModelAPI_ExecState theState);

  /// Returns the state of the latest execution of the feature
  MODEL_EXPORT virtual ModelAPI_ExecState execState();

  /// Registers error during the execution, causes the ExecutionFailed state
  MODEL_EXPORT virtual void setError(const std::string& theError, bool theSend = true);

  /// Erases the error string if it is not empty
  void eraseErrorString();

  /// Registers error during the execution, causes the ExecutionFailed state
  MODEL_EXPORT virtual std::string error() const;

  /// Returns the identifier of feature-owner, unique in this document
  MODEL_EXPORT virtual int featureId() const;

  /// returns all objects referenced to this
  MODEL_EXPORT virtual const std::set<AttributePtr>& refsToMe() {return myRefsToMe;}

  /// returns all references by attributes of this data
  /// \param theRefs returned list of pairs:
  ///                id of referenced attribute and list of referenced objects
  MODEL_EXPORT virtual void referencesToObjects(
    std::list<std::pair<std::string, std::list<ObjectPtr> > >& theRefs);

  /// Copies all atributes content into theTarget data
  MODEL_EXPORT virtual void copyTo(std::shared_ptr<ModelAPI_Data> theTarget);

  /// Returns the invalid data pointer (to avoid working with NULL shared ptrs in swig)
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_Data> invalidPtr();

  /// Returns the invalid data pointer: static method
  static std::shared_ptr<ModelAPI_Data> invalidData();

  /// Identifier of the transaction when object (feature or result) was updated last time.
  MODEL_EXPORT virtual int updateID();

  /// Identifier of the transaction when object (feature or result) was updated last time.
  /// This method is called by the updater.
  MODEL_EXPORT virtual void setUpdateID(const int theID);

  /// Returns true if the given object is owner of this data (needed for correct erase of object
  /// with duplicated data)
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_Object> owner();

protected:
  /// Returns true if "is in history" custom behaviors is defined for the feature
  MODEL_EXPORT virtual bool isInHistory();

  /// Defines the custom "is in history" behavior
  MODEL_EXPORT virtual void setIsInHistory(const bool theFlag);

  /// Returns true if the object is deleted, but some data is still keept in memory
  MODEL_EXPORT virtual bool isDeleted();

  /// Sets true if the object is deleted, but some data is still keept in memory
  MODEL_EXPORT virtual void setIsDeleted(const bool theFlag);

private:
  /// Removes all information about back references
  void eraseBackReferences();
  /// Removes a back reference (with identifier which attribute references to this object)
  /// \param theFeature feature referenced to this
  /// \param theAttrID identifier of the attribute that is references from theFeature to this
  void removeBackReference(FeaturePtr theFeature, std::string theAttrID);
  /// Removes a back reference (by the attribute)
  /// \param theAttr the referenced attribute
  void removeBackReference(AttributePtr theAttr);
  /// Adds a back reference (with identifier which attribute references to this object
  /// \param theFeature feature referenced to this
  /// \param theAttrID identifier of the attribute that is references from theFeature to this
  /// \param theApplyConcealment applies consealment flag changes
  void addBackReference(FeaturePtr theFeature, std::string theAttrID,
    const bool theApplyConcealment = true);

  /// Makes the concealment flag up to date for this object-owner.
  MODEL_EXPORT virtual void updateConcealmentFlag();

  /// Returns true if object must be displayed in the viewer: flag is stored in the
  /// data model, so on undo/redo, open/save or recreation of object by history-playing it keeps
  /// the original state i nthe current transaction.
  MODEL_EXPORT virtual bool isDisplayed();

  /// Sets the displayed/hidden state of the object. If it is changed, sends the "redisplay"
  /// signal.
  MODEL_EXPORT virtual void setDisplayed(const bool theDisplay);
};

/// Generic method to register back reference, used in referencing attributes.
/// Without concealment change, it will be done later, on synchronization.
#define ADD_BACK_REF(TARGET) \
  if (TARGET.get() != NULL) { \
    FeaturePtr anAttributeOwnerFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(owner()); \
    if (anAttributeOwnerFeature.get()) { \
      std::shared_ptr<Model_Data> aTargetData = std::dynamic_pointer_cast<Model_Data>( \
        (TARGET)->data()); \
      aTargetData->addBackReference(anAttributeOwnerFeature, id(), false); \
    } \
  }

/// Generic method to unregister back reference, used in referencing attributes.
/// Without concealment change, it will be done later, on synchronization.
#define REMOVE_BACK_REF(TARGET) \
  if (TARGET.get() != NULL) { \
    FeaturePtr anAttOwnerFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(owner()); \
    if (anAttOwnerFeature.get()) { \
      std::shared_ptr<Model_Data> aTargetData = std::dynamic_pointer_cast<Model_Data>( \
        (TARGET)->data()); \
      aTargetData->removeBackReference(anAttOwnerFeature, id()); \
    } \
  }

#endif
