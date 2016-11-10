// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomValidators_DifferentShapes.cpp
// Created:     2 Feb 2015
// Author:      Natalia ERMOLAEVA

#include "GeomValidators_DifferentShapes.h"

#include <Events_InfoMessage.h>

#include <ModelAPI_AttributeSelection.h>
#include "ModelAPI_Object.h"

bool GeomValidators_DifferentShapes::isValid(const AttributePtr& theAttribute,
                                      const std::list<std::string>& theArguments,
                                      Events_InfoMessage& theError) const
{
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theAttribute->owner());
  AttributeSelectionPtr aSelectionAttribute =
                     std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
  GeomShapePtr aShape = aSelectionAttribute->value();
  if (!aShape.get()) {
    ResultPtr aResult = aSelectionAttribute->context();
    if (aResult.get())
      aShape = aResult->shape();
  }

  std::string aCurrentAttributeId = theAttribute->id();
  // get all feature attributes
  std::list<AttributePtr> anAttrs =
      aFeature->data()->attributes(ModelAPI_AttributeSelection::typeId());
  if (anAttrs.size() > 0 && aShape.get() != NULL) {
    std::list<AttributePtr>::iterator anAttr = anAttrs.begin();
    for(; anAttr != anAttrs.end(); anAttr++) {
      AttributePtr anAttribute = *anAttr;
      // take into concideration only other attributes
      if (anAttribute.get() != NULL && anAttribute->id() != aCurrentAttributeId) {
        aSelectionAttribute =
          std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(anAttribute);
        // the shape of the attribute should be not the same
        if (aSelectionAttribute.get() != NULL) {
          GeomShapePtr anAttrShape = aSelectionAttribute->value();
          if (!anAttrShape.get()) {
            ResultPtr aResult = aSelectionAttribute->context();
            if (aResult.get())
              anAttrShape = aResult->shape();
          }
          if (aShape->isEqual(anAttrShape)) {
            theError = "The feature uses equal shapes.";
            return false;
          }
        }
      }
    }
  }
  return true;
}
