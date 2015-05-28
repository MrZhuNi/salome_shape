// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_AttributeRefList.h
// Created:     8 May 2014
// Author:      Mikhail PONIKAROV

#ifndef Model_AttributeRefList_H_
#define Model_AttributeRefList_H_

#include "Model.h"
#include "ModelAPI_AttributeRefList.h"
#include "ModelAPI_Feature.h"

#include <TDataStd_ReferenceList.hxx>

/**\class Model_AttributeRefList
 * \ingroup DataModel
 * \brief Attribute that contains list of references to features (located in the same document).
 * For the current moment it does not support references t oobjects in other documents.
 */

class Model_AttributeRefList : public ModelAPI_AttributeRefList
{
  Handle_TDataStd_ReferenceList myRef;  ///< references to the features labels
 public:
  /// Appends the feature to the end of a list
  MODEL_EXPORT virtual void append(ObjectPtr theObject);

  /// Erases the first meet of the feature in the list
  MODEL_EXPORT virtual void remove(ObjectPtr theObject);

  /// Returns number of features in the list
  MODEL_EXPORT virtual int size() const;

  /// Removes all references from the list
  MODEL_EXPORT virtual void clear();

  /// Returns the list of features
  MODEL_EXPORT virtual std::list<ObjectPtr> list();

  /// Returns the list of features
  MODEL_EXPORT virtual ObjectPtr object(const int theIndex) const;

  /// Returns true if attribute was  initialized by some value
  MODEL_EXPORT virtual bool isInitialized();
 protected:
  /// Objects are created for features automatically
  MODEL_EXPORT Model_AttributeRefList(TDF_Label& theLabel);

  friend class Model_Data;
};

#endif
