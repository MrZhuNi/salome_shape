// File:        ModelAPI_Object.hxx
// Created:     19 May 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_Object_H_
#define ModelAPI_Object_H_

#include "ModelAPI.h"
#include "ModelAPI_Data.h"

#include <boost/shared_ptr.hpp>

class ModelAPI_Data;
class ModelAPI_Document;

/**\class ModelAPI_Object
 * \ingroup DataModel
 * \brief Represents any object in the data model and in the object browser.
 *
 * It may be feature or result of the feature. User just may set name for it
 * or change this name later. Generic class for Feature, Body, Parameter and other
 * objects related to the features and their results. Contains attributes of this 
 * object in the "Data".
 */
class ModelAPI_Object
{
  boost::shared_ptr<ModelAPI_Data> myData;  ///< manager of the data model of a feature
  boost::shared_ptr<ModelAPI_Document> myDoc;  ///< document this object belongs to
 public:
  /// By default object is displayed in the object browser.
  virtual bool isInHistory()
  {
    return true;
  }

  /// Returns the data manager of this object: attributes
  virtual boost::shared_ptr<ModelAPI_Data> data()
  {
    return myData;
  }

  /// Returns true if object refers to the same data model instance
  virtual bool isSame(const boost::shared_ptr<ModelAPI_Object>& theObject)
  {
    return theObject.get() == this;
  }

  /// Returns document this feature belongs to
  virtual boost::shared_ptr<ModelAPI_Document> document()
  {
    return myDoc;
  }

  /// Returns the group identifier of this object
  virtual std::string groupName() = 0;

  /// To use virtuality for destructors
  virtual ~ModelAPI_Object() {}
 protected:
  /// Sets the data manager of an object (document does)
  virtual void setData(boost::shared_ptr<ModelAPI_Data> theData)
  {
    myData = theData;
  }

  /// Sets the data manager of an object (document does)
  virtual void setDoc(boost::shared_ptr<ModelAPI_Document> theDoc)
  {
    myDoc = theDoc;
  }

  /// removes all fields from this feature
  MODELAPI_EXPORT virtual void erase() {
    if (myData) myData->erase();
    setData(DataPtr());
  }

  friend class Model_Document;

};

typedef boost::shared_ptr<ModelAPI_Object> ObjectPtr;

#endif
