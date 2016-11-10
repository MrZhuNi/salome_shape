// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        Model_ResultValidators.h
// Created:     27 Feb 2015
// Author:      Natalia ERMOLAEVA

#ifndef SketchPlugin_ExternalValidator_H
#define SketchPlugin_ExternalValidator_H

#include <SketchPlugin.h>
#include <ModelAPI_AttributeValidator.h>

/**\class SketchPlugin_ExternalValidator
 * \ingroup Validators
 * \brief Validator for the points selection
 *
 * Allows to select points only.
 */
class SketchPlugin_ExternalValidator : public ModelAPI_AttributeValidator
{
public:
  /// returns true if the feature of attribute do not contain external features
  /// in the given attribute and
  /// among attributes listed in the arguments
  /// \param theAttribute an attribute to check
  /// \param theArguments a filter parameters
  /// \param theError error message
  SKETCHPLUGIN_EXPORT virtual bool isValid(const AttributePtr& theAttribute,
                                           const std::list<std::string>& theArguments,
                                           Events_InfoMessage& theError) const;

protected:
  /// returns true if the feature of the attribute is external
  /// \param theAttribute an attribute to check
  bool isExternalAttribute(const AttributePtr& theAttribute) const;
};

#endif
