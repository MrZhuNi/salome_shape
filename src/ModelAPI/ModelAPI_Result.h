// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_Result.hxx
// Created:     07 Jul 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_Result_H_
#define ModelAPI_Result_H_

#include "ModelAPI_Object.h"
#include <GeomAPI_Shape.h>

class ModelAPI_Feature;

/**\class ModelAPI_Result
 * \ingroup DataModel
 * \brief The result of a feature.
 *
 * A generic class that .
 */
class ModelAPI_Result : public ModelAPI_Object
{
  bool myIsConcealed; ///< the result is concealed from the data tree (referenced by other objects)
 protected:
  bool myIsDisabled; ///< the result is disabled: removed for the user, but keeps the general info
 public:

  /// Reference to the color of the result.
  /// The integer array is used. It contains tree values for red green and blue values. The values are in
  /// [0, 255] range
  inline static const std::string& COLOR_ID()
  {
    static const std::string MY_COLOR_ID("Color");
    return MY_COLOR_ID;
  }

  /// Returns true if the result is concealed from the data tree (referenced by other objects)
  inline bool isConcealed()
  {
    return myIsConcealed;
  }

  /// Returns true if the result is concealed from the data tree (referenced by other objects)
  MODELAPI_EXPORT void setIsConcealed(const bool theValue);

  /// Enables/disables the result. The disabled result does not participate in any calculation
  /// and visualization: like it was removed. But it keeps the general parameters: colors, 
  /// visibility, etc.
  /// \param theThis pointer to this object, needed to generate all events if it is neccessary
  /// \param theFlag makes disabled if it is true
  /// \returns true if state is really changed
  MODELAPI_EXPORT virtual bool setDisabled(std::shared_ptr<ModelAPI_Result> theThis,
    const bool theFlag);

  /// Returns the result is disabled or not.
  MODELAPI_EXPORT virtual bool isDisabled() const;

  // Retuns the parameters of color definition in the resources config manager
  virtual void colorConfigInfo(std::string& theSection, std::string& theName,
    std::string& theDefault) {}

  /// Request for initialization of data model of the result: adding all attributes
  virtual void initAttributes() {};

  /// To virtually destroy the fields of successors
  MODELAPI_EXPORT virtual ~ModelAPI_Result();

  /// Returns the shape-result produced by this feature (or null if no shapes)
  MODELAPI_EXPORT virtual std::shared_ptr<GeomAPI_Shape> shape();

  /// On change of attribute of the result update presentation of this result:
  /// for the current moment there are only presentation attributes assigned to results
  MODELAPI_EXPORT virtual void attributeChanged(const std::string& theID);
};

//! Pointer on feature object
typedef std::shared_ptr<ModelAPI_Result> ResultPtr;

#endif
