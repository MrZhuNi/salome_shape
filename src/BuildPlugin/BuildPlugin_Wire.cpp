// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        BuildPlugin_Wire.cpp
// Created:     14 April 2016
// Author:      Dmitry Bobylev

#include "BuildPlugin_Wire.h"

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultConstruction.h>

#include <Events_InfoMessage.h>

#include <GeomAPI_PlanarEdges.h>
#include <GeomAPI_ShapeExplorer.h>

#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAlgoAPI_WireBuilder.h>

#include <algorithm>

//=================================================================================================
BuildPlugin_Wire::BuildPlugin_Wire()
{
}

//=================================================================================================
void BuildPlugin_Wire::initAttributes()
{
  data()->addAttribute(BASE_OBJECTS_ID(), ModelAPI_AttributeSelectionList::typeId());
}

//=================================================================================================
void BuildPlugin_Wire::execute()
{
  // Get base objects list.
  AttributeSelectionListPtr aSelectionList = selectionList(BASE_OBJECTS_ID());
  if(!aSelectionList.get()) {
    setError("Error: Could not get selection list.");
    return;
  }
  if(aSelectionList->size() == 0) {
    setError("Error: Empty selection list.");
    return;
  }

  // Collect base shapes.
  ListOfShape anEdges;
  for(int anIndex = 0; anIndex < aSelectionList->size(); ++anIndex) {
    AttributeSelectionPtr aSelection = aSelectionList->value(anIndex);
    GeomShapePtr aShape = aSelection->value();
    if(!aShape.get()) {
      aShape = aSelection->context()->shape();
    }
    for(GeomAPI_ShapeExplorer anExp(aShape, GeomAPI_Shape::EDGE); anExp.more(); anExp.next()) {
      GeomShapePtr anEdge = anExp.current();
      anEdges.push_back(anEdge);
    }
  }

  // Create wire.
  GeomShapePtr aWire = GeomAlgoAPI_WireBuilder::wire(anEdges);
  if(!aWire.get()) {
    setError("Error: Result wire is empty. Probably it has disconnected edges or non-manifold.");
    return;
  }

  // Store result.
  ResultBodyPtr aResultBody = document()->createBody(data());
  aResultBody->store(aWire);
  for(GeomAPI_ShapeExplorer anExp(aWire, GeomAPI_Shape::EDGE); anExp.more(); anExp.next()) {
    GeomShapePtr anEdgeInResult = anExp.current();
    for(ListOfShape::const_iterator anIt = anEdges.cbegin(); anIt != anEdges.cend(); ++anIt) {
      std::shared_ptr<GeomAPI_Edge> anEdgeInList(new GeomAPI_Edge(*anIt));
      if(anEdgeInList->isEqual(anEdgeInResult)) {
        aResultBody->modified(anEdgeInList, anEdgeInResult, "Edge");
        break;
      }
    }
  }
  setResult(aResultBody);
}

//=================================================================================================
bool BuildPlugin_Wire::customAction(const std::string& theActionId)
{
  if(theActionId == ADD_CONTOUR_ACTION_ID()) {
    return addContour();
  } else {
    std::string aMsg = "Error: Feature \"%1\" does not support action \"%2\".";
    Events_InfoMessage("BuildPlugin_Wire", aMsg).arg(getKind()).arg(theActionId).send();
  }

  return false;
}

//=================================================================================================
bool BuildPlugin_Wire::addContour()
{
  // Get base objects list.
  AttributeSelectionListPtr aSelectionList = selectionList(BASE_OBJECTS_ID());
  if(aSelectionList->size() == 0) {
    Events_InfoMessage("BuildPlugin_Wire", "Error: Empty selection list.").send();
    return false;
  }

  // Collect attributes to check.
  ListOfShape anAddedEdges;
  std::list<AttributeSelectionPtr> anAttributesToCheck;
  for(int anIndex = 0; anIndex < aSelectionList->size(); ++anIndex) {
    AttributeSelectionPtr aSelection = aSelectionList->value(anIndex);
    GeomShapePtr anEdgeInList = aSelection->value();
    if(!anEdgeInList.get()) {
      continue;
    }

    // Check that it is edge.
    if(anEdgeInList->shapeType() != GeomAPI_Shape::EDGE) {
      continue;
    }

    // Check that it is edge on sketch.
    ResultPtr aContext = aSelection->context();
    ResultConstructionPtr aConstruction =
      std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aContext);
    if(!aConstruction.get()) {
      continue;
    }
    GeomShapePtr aContextShape = aConstruction->shape();
    std::shared_ptr<GeomAPI_PlanarEdges> aPlanarEdges =
      std::dynamic_pointer_cast<GeomAPI_PlanarEdges>(aContextShape);
    if(!aPlanarEdges.get()) {
      continue;
    }

    // Check that sketch have faces.
    if(aConstruction->facesNum() == 0) {
      continue;
    }

    anAddedEdges.push_back(anEdgeInList);
    anAttributesToCheck.push_back(aSelection);
  }

  // Check if edges have contours.
  bool isAnyContourAdded = false;
  for(std::list<AttributeSelectionPtr>::const_iterator aListIt = anAttributesToCheck.cbegin();
      aListIt != anAttributesToCheck.cend();
      ++aListIt) {
    AttributeSelectionPtr aSelection = *aListIt;
    std::shared_ptr<GeomAPI_Edge> anEdgeInList(new GeomAPI_Edge(aSelection->value()));

    ResultConstructionPtr aConstruction =
      std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aSelection->context());

    // Iterate on wires and add wire with this edge.
    std::shared_ptr<GeomAPI_Shape> aFoundWire;
    for(int anIndex = 0; anIndex < aConstruction->facesNum(); ++anIndex) {
      std::shared_ptr<GeomAPI_Face> aFace = aConstruction->face(anIndex);
      for(GeomAPI_ShapeExplorer
          aWireExp(aFace, GeomAPI_Shape::WIRE); aWireExp.more(); aWireExp.next()) {
        GeomShapePtr aWireOnFace = aWireExp.current();
        for(GeomAPI_ShapeExplorer
            anExp(aWireOnFace, GeomAPI_Shape::EDGE); anExp.more(); anExp.next()) {
          std::shared_ptr<GeomAPI_Edge> anEdgeOnFace(new GeomAPI_Edge(anExp.current()));
          if(anEdgeInList->isEqual(anEdgeOnFace)) {
            aFoundWire = aWireOnFace;
            break;
          }
        }
      }
      if(aFoundWire.get()) {
        break;
      }
    }

    // If wire with the same edge found. Add all other edges to list.
    if(aFoundWire.get()) {
      for(GeomAPI_ShapeExplorer
          anExp(aFoundWire, GeomAPI_Shape::EDGE); anExp.more(); anExp.next()) {
        std::shared_ptr<GeomAPI_Edge> anEdgeOnFace(new GeomAPI_Edge(anExp.current()));
        ListOfShape::const_iterator anEdgesIt = anAddedEdges.cbegin();
        for(; anEdgesIt != anAddedEdges.cend(); ++anEdgesIt) {
          if(anEdgeOnFace->isEqual(*anEdgesIt)) {
            break;
          }
        }
        if(anEdgesIt == anAddedEdges.cend()) {
          isAnyContourAdded = true;
          anAddedEdges.push_back(anEdgeOnFace);
          aSelectionList->append(aConstruction, anEdgeOnFace);
        }
      }
    }
  }

  if(!isAnyContourAdded) {
    Events_InfoMessage("BuildPlugin_Wire",
      "Error: Contours already closed or no contours found for selected edges.").send();
    return false;
  }

  return true;
}
