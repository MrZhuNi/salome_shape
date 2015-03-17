// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_Validators.h
// Created:     09 July 2014
// Author:      Vitaly SMETANNIKOV

#ifndef PartSet_Validators_H
#define PartSet_Validators_H

#include "PartSet.h"

#include <ModuleBase_SelectionValidator.h>
#include <ModuleBase_ISelection.h>
#include <ModelAPI_RefAttrValidator.h>
#include <ModelAPI_ResultValidator.h>

/*
 * Selector validators
 */

//! \ingroup Validators
//! A class to validate a selection for Distance constraint operation
class PartSet_DistanceValidator : public ModuleBase_SelectionValidator
{
 public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection) const;
};

//! \ingroup Validators
//! A class to validate a selection for Length constraint operation
class PartSet_LengthValidator : public ModuleBase_SelectionValidator
{
 public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection) const;
};

//! \ingroup Validators
//! A class to validate a selection for Perpendicular constraint operation
class PartSet_PerpendicularValidator : public ModuleBase_SelectionValidator
{
 public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection) const;
};

//! \ingroup Validators
//! A class to validate a selection for Perpendicular constraint operation
class PartSet_ParallelValidator : public ModuleBase_SelectionValidator
{
 public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection) const;
};

//! \ingroup Validators
//! A class to validate a selection for Perpendicular constraint operation
class PartSet_RadiusValidator : public ModuleBase_SelectionValidator
{
 public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection) const;
};

//! \ingroup Validators
//! A class to validate a selection for Perpendicular constraint operation
class PartSet_RigidValidator : public ModuleBase_SelectionValidator
{
 public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection) const;
};

/**
* \ingroup Validators
* A validator which checks that objects selected for feature attributes are different (not the same)
*/
class PartSet_DifferentObjectsValidator : public ModelAPI_RefAttrValidator
{
 public:
  //! Returns true if the attribute is good for the feature attribute
  //! \param theAttribute an attribute
  //! \param theArguments a list of arguments (names of attributes to check)
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments) const;

protected:
  //! Casts the attribute to an attribute kind and obtains an object value if it is possible
  //! \param theAttribute a source attribute to find object
  //! \return an attribute object or NULL
  ObjectPtr getObject(const AttributePtr& theAttribute) const;

  //! Checks whethe other feature attributes has a reference to the given attribute
  //! \param theAttribute a source attribute to find object
  //! \return a boolean value
  bool featureHasReferences(const AttributePtr& theAttribute) const;
};

/**
* \ingroup Validators
* A Validator which validates tha selected object is a Sketch
*/
class PartSet_SketchValidator : public ModelAPI_ResultValidator
{
 public:
   /// Returns True if the given object is a sketch
   /// \param theObject an object
  virtual bool isValid(const ObjectPtr theObject) const;
};

#endif