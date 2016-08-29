// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_AttributeReference.h
// Created:     8 May 2014
// Author:      Mikhail PONIKAROV

#ifndef Model_AttributeReference_H_
#define Model_AttributeReference_H_

#include "Model.h"
#include "ModelAPI_AttributeReference.h"
#include <ModelAPI_Feature.h>
#include <TDF_Label.hxx>
#include <TDF_Reference.hxx>

/**\class Model_AttributeReference
 * \ingroup DataModel
 * \brief Attribute that contains reference to feature (located in the same document).
 */

class Model_AttributeReference : public ModelAPI_AttributeReference
{
  TDF_Label myLab; ///< the main label of this attribute
  Handle_TDF_Reference myRef;  ///< references to the feature label
 public:
  /// Defines the object referenced from this attribute
  MODEL_EXPORT virtual void setValue(ObjectPtr theObject);

  /// Returns object referenced from this attribute
  MODEL_EXPORT virtual ObjectPtr value();

  MODEL_EXPORT ~Model_AttributeReference();

  MODEL_EXPORT virtual void setObject(const std::shared_ptr<ModelAPI_Object>& theObject);

  /// Returns true if attribute was  initialized by some value
  MODEL_EXPORT virtual bool isInitialized();


protected:
  /// Objects are created for features automatically
  MODEL_EXPORT Model_AttributeReference(TDF_Label& theLabel);
  /// Reinitializes the internal state of the attribute (may be needed on undo/redo, abort, etc)
  virtual void reinit();

  friend class Model_Data;
  friend class Model_AttributeSelection;
};

#endif
