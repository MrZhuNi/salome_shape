// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_Expression.h
// Created:     5 Aug 2015
// Author:      Sergey POKHODENKO

#ifndef ModelAPI_Expression_H_
#define ModelAPI_Expression_H_

#include "ModelAPI.h"

#include <memory>
#include <set>
#include <string>

/**\class ModelAPI_Expression
 * \ingroup DataModel
 * \brief Expression for calculated values.
 */
class ModelAPI_Expression
{
 public:
  /// To virtually destroy the fields of successors
  MODELAPI_EXPORT virtual ~ModelAPI_Expression();

  /// Returns true if attribute was initialized by some value
  MODELAPI_EXPORT virtual bool isInitialized();

  /// Makes attribute initialized
  MODELAPI_EXPORT virtual void setInitialized();

  /// Sets the text of this Expression
  MODELAPI_EXPORT virtual void setText(const std::string& theText) = 0;

  /// Returns the text of this Expression
  MODELAPI_EXPORT virtual std::string text() const = 0;

  /// Allows to set expression (text) as invalid (by the parameters listener)
  MODELAPI_EXPORT virtual void setInvalid(const bool theFlag) = 0;

  /// Returns true if text is invalid
  MODELAPI_EXPORT virtual bool isInvalid() = 0;

  /// Allows to set expression (text) error (by the parameters listener)
  MODELAPI_EXPORT virtual void setError(const std::string& theError) = 0;

  /// Returns an expression error
  MODELAPI_EXPORT virtual std::string error() = 0;

  /// Defines the used parameters (by the parameters listener)
  MODELAPI_EXPORT virtual
    void setUsedParameters(const std::set<std::string>& theUsedParameters) = 0;

  /// Returns the used parameters
  MODELAPI_EXPORT virtual std::set<std::string> usedParameters() const = 0;

  /// Returns True if the given string can be defined as a name of an expression variable
  MODELAPI_EXPORT static bool isVariable(const std::string& theString);

 protected:
  /// Objects are created for features automatically
  MODELAPI_EXPORT ModelAPI_Expression();
  /// Reinitializes the internal state of the attribute (may be needed on undo/redo, abort, etc)
  MODELAPI_EXPORT virtual void reinit() = 0;
  /// Resets attribute to deafult state.
  MODELAPI_EXPORT virtual void reset() {
    myIsInitialized = false;
  };

  bool myIsInitialized; ///< is some value assigned to this attribute

  friend class Model_Data;
  friend class Model_AttributeDouble;
  friend class Model_AttributeInteger;
  friend class GeomData_Point;
  friend class GeomData_Point2D;
};


/**\class ModelAPI_ExpressionDouble
 * \ingroup DataModel
 * \brief Expression for calculated double values.
 */
class ModelAPI_ExpressionDouble : public virtual ModelAPI_Expression
{
 public:
  /// Defines the double value
  MODELAPI_EXPORT virtual void setValue(const double theValue) = 0;

  /// Returns the double value
  MODELAPI_EXPORT virtual double value() = 0;

 protected:
  /// Objects are created for features automatically
  MODELAPI_EXPORT ModelAPI_ExpressionDouble();

  friend class Model_Data;
};


/**\class ModelAPI_ExpressionInteger
 * \ingroup DataModel
 * \brief Expression for calculated integer values.
 */
class ModelAPI_ExpressionInteger : public virtual ModelAPI_Expression
{
 public:
  /// Defines the integer value
  MODELAPI_EXPORT virtual void setValue(const int theValue) = 0;

  /// Returns the integer value
  MODELAPI_EXPORT virtual int value() = 0;

 protected:
  /// Objects are created for features automatically
  MODELAPI_EXPORT ModelAPI_ExpressionInteger();

  friend class Model_Data;
};


//! Smart pointers for objects
typedef std::shared_ptr<ModelAPI_Expression> ExpressionPtr;
typedef std::shared_ptr<ModelAPI_ExpressionDouble> ExpressionDoublePtr;
typedef std::shared_ptr<ModelAPI_ExpressionInteger> ExpressionIntegerPtr;

#endif
