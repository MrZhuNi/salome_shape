// File:        ModelAPI_Attribute.h
// Created:     2 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_Attribute_H_
#define ModelAPI_Attribute_H_

#include "ModelAPI.h"
#include <string>
#include <boost/shared_ptr.hpp>

class ModelAPI_Object;

/**\class ModelAPI_Attribute
 * \ingroup DataModel
 * \brief Generic attribute of the Object.
 */
class ModelAPI_Attribute
{
  ///< needed here to emit signal that feature changed on change of the attribute
  boost::shared_ptr<ModelAPI_Object> myObject;
 protected:
  // accessible from the attributes
  bool myIsInitialized;
  bool myIsComputedDefault;
  bool myIsArgument;
 public:

  /// Returns the type of this class of attributes, not static method
  MODELAPI_EXPORT virtual std::string attributeType() = 0;

  /// To virtually destroy the fields of successors
  MODELAPI_EXPORT virtual ~ModelAPI_Attribute()
  {
  }

  /// Sets the owner of this attribute
  MODELAPI_EXPORT void setObject(const boost::shared_ptr<ModelAPI_Object>& theObject)
  {
    myObject = theObject;
  }

  /// Returns the owner of this attribute
  MODELAPI_EXPORT const boost::shared_ptr<ModelAPI_Object>& owner() const
  {
    return myObject;
  }

  /// Returns true if attribute was  initialized by some value
  MODELAPI_EXPORT bool isInitialized()
  {
    return myIsInitialized;
  }

  /// Makes attribute initialized
  MODELAPI_EXPORT void setInitialized()
  {
    myIsInitialized = true;
  }

  /// Returns true if attribute's default value was computed
  MODELAPI_EXPORT bool isComputedDefault()
  {
    return myIsComputedDefault;
  }

  /// Tells that attribute's default value was computed
  MODELAPI_EXPORT void setComputedDefault()
  {
    myIsComputedDefault = true;
    myIsInitialized = false;
  }

  /// Set this attribute is argument for result (change of this attribute requires update of result).
  /// By default it is true.
  MODELAPI_EXPORT void setIsArgument(const bool theFlag)
  {
    myIsArgument = theFlag;
  }

  /// Returns true if attribute causes the result change
  MODELAPI_EXPORT bool isArgument()
  {
    return myIsArgument;
  }

 protected:
  /// Objects are created for features automatically
  ModelAPI_Attribute()
  {
    myIsInitialized = false;
    myIsComputedDefault = false;
    myIsArgument = true;
  }

};

//! Pointer on attribute object
typedef boost::shared_ptr<ModelAPI_Attribute> AttributePtr;

#endif
