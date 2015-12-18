// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchShapePlugin_Feature.cpp
// Created:     25 Nov 2015
// Author:      Natalia ERMOLAEVA

#include "SketchShapePlugin_Feature.h"

#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeBoolean.h>

#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

SketchShapePlugin_Feature::SketchShapePlugin_Feature()
: ModelAPI_Feature(), myIsAttributeChangeBlocked(false)
{
}

void SketchShapePlugin_Feature::initAttributes()
{
  data()->addAttribute(SKETCH_ID(), ModelAPI_AttributeSelection::typeId());

  data()->addAttribute(VERTEX_LIST_ID(), ModelAPI_AttributeSelectionList::typeId());
  data()->addAttribute(VERTEX_CHOICE_ID(), ModelAPI_AttributeBoolean::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), VERTEX_CHOICE_ID());

  data()->addAttribute(EDGE_LIST_ID(), ModelAPI_AttributeSelectionList::typeId());
  data()->addAttribute(EDGE_CHOICE_ID(), ModelAPI_AttributeBoolean::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), EDGE_CHOICE_ID());

  data()->addAttribute(FACE_LIST_ID(), ModelAPI_AttributeSelectionList::typeId());
  data()->addAttribute(FACE_CHOICE_ID(), ModelAPI_AttributeBoolean::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), FACE_CHOICE_ID());
}

void SketchShapePlugin_Feature::execute()
{
}

void SketchShapePlugin_Feature::attributeChanged(const std::string& theID)
{
  if (myIsAttributeChangeBlocked)
    return;

  if (theID == VERTEX_CHOICE_ID() ||
      theID == EDGE_CHOICE_ID() ||
      theID == FACE_CHOICE_ID()) {
    std::string aListAttrId = theID == VERTEX_CHOICE_ID() ? VERTEX_LIST_ID() : (
                              theID == EDGE_CHOICE_ID() ? EDGE_LIST_ID() :
                              FACE_LIST_ID());

    AttributeBooleanPtr aChoiceAttribute = std::dynamic_pointer_cast<ModelAPI_AttributeBoolean>(
                                           data()->attribute(theID));
    if (!aChoiceAttribute->value()) {
      AttributeSelectionListPtr aListAttribute = data()->selectionList(aListAttrId);
      aListAttribute->clear();
    }
  }
  else if (theID == VERTEX_LIST_ID() ||
           theID == EDGE_LIST_ID() ||
           theID == FACE_LIST_ID()) {
    myIsAttributeChangeBlocked = true;

    DataPtr aData = data();
    AttributeSelectionListPtr aChangedAttr = aData->selectionList(theID);

    AttributeSelectionListPtr aVertexAttr = aData->selectionList(VERTEX_LIST_ID());
    AttributeSelectionListPtr anEdgeAttr = aData->selectionList(EDGE_LIST_ID());
    AttributeSelectionListPtr aFaceAttr = aData->selectionList(FACE_LIST_ID());

    if (theID != VERTEX_LIST_ID())
      aVertexAttr->clear();
    if (theID != EDGE_LIST_ID())
      anEdgeAttr->clear();
    if (theID != FACE_LIST_ID())
      aFaceAttr->clear();

    std::list<std::pair<ResultPtr, GeomShapePtr> > aChangedAttrValues;
    for (int i = 0, aSize = aChangedAttr->size(); i < aSize; i++) {
      AttributeSelectionPtr aSelectAttr = aChangedAttr->value(i);
      ResultPtr anObject = aSelectAttr->context();
      if (!anObject.get())
        continue;
      else {
        GeomShapePtr aGeomShape = aSelectAttr->value();
        if (aGeomShape.get() == NULL)
          aGeomShape = anObject->shape();

        if (aGeomShape.get() == NULL)
          continue;

        GeomAPI_Shape::ShapeType aShapeType = aGeomShape->shapeType();
        if (aGeomShape->isVertex()) {
          if (theID == VERTEX_LIST_ID())
            aChangedAttrValues.push_back(std::make_pair(anObject, aGeomShape));
          else {
            aVertexAttr->append(anObject, aGeomShape);
          }
        }
        else if (aGeomShape->isEdge()) {
          if (theID == EDGE_LIST_ID())
            aChangedAttrValues.push_back(std::make_pair(anObject, aGeomShape));
          else {
            anEdgeAttr->append(anObject, aGeomShape);
          }
        }
        else if (aGeomShape->isFace()) {
          if (theID == FACE_LIST_ID())
            aChangedAttrValues.push_back(std::make_pair(anObject, aGeomShape));
          else {
            aFaceAttr->append(anObject, aGeomShape);
          }
        }
      }
    }
    //aChangedAttr->clear();
    std::list<std::pair<ResultPtr, GeomShapePtr> >::const_iterator anIt = aChangedAttrValues.begin(),
                                                                   aLast = aChangedAttrValues.end();
    for (; anIt != aLast; anIt++)
      aChangedAttr->append((*anIt).first, (*anIt).second);

    myIsAttributeChangeBlocked = false;
  }
}

