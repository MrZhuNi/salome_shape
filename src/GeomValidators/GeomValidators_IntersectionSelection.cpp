// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomValidators_IntersectionSelection.cpp
// Created:     16 Feb 2016
// Author:      Dmitry Bobylev

#include "GeomValidators_IntersectionSelection.h"

#include <Events_InfoMessage.h>

#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_Feature.h>

bool GeomValidators_IntersectionSelection::isValid(const AttributePtr& theAttribute,
                                                   const std::list<std::string>& theArguments,
                                                   Events_InfoMessage& theError) const
{
  if(!theAttribute.get()) {
    theError = "Error: empty selection.";
    return false;
  }
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theAttribute->owner());
  AttributeSelectionListPtr anAttrSelectionList = std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
  for(int anIndex = 0; anIndex < anAttrSelectionList->size(); ++anIndex) {
    AttributeSelectionPtr anAttrSelection = anAttrSelectionList->value(anIndex);
    if(!anAttrSelection.get()) {
      theError = "Error: empty attribute selection.";
      return false;
    }
    ResultPtr aContext = anAttrSelection->context();
    if(!aContext.get()) {
      theError = "Error: empty selection context.";
      return false;
    }
    FeaturePtr aFeature = ModelAPI_Feature::feature(aContext);
    if(!aFeature.get()) {
      theError = "Error: empty feature.";
      return false;
    }
    std::string aFeatureKind = aFeature->getKind();
    if(aFeatureKind == "Sketch" ||
       aFeatureKind == "Plane" ||
       aFeatureKind == "Axis") {
      theError = "Error: %1 shape is not allowed for selection.";
      theError.arg(aFeatureKind);
      return false;
    }
    std::shared_ptr<GeomAPI_Shape> aShape = anAttrSelection->value();
    if(!aShape.get()) {
      aShape = aContext->shape();
    }
    if(!aShape.get()) {
      theError = "Error: empty shape.";
      return false;
    }
    int aShapeType = aShape->shapeType();
    // Allow to select edges, faces and solids.
    if(aShapeType != GeomAPI_Shape::EDGE &&
       aShapeType != GeomAPI_Shape::FACE &&
       aShapeType != GeomAPI_Shape::SOLID &&
       aShapeType != GeomAPI_Shape::COMPSOLID &&
       aShapeType != GeomAPI_Shape::COMPOUND) {
      theError = "Error: selected shape has the wrong type.";
      return false;
    }
  }

  return true;
}
