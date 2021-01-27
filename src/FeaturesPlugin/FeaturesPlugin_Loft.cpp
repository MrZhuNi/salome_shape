// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "FeaturesPlugin_Loft.h"
#include "FeaturesPlugin_Tools.h"


#include <ModelAPI_AttributeSelection.h>

#include <GeomAlgoAPI_Tools.h>
#include <GeomAlgoAPI_Copy.h>
#include <GeomAlgoAPI_Filling.h>
#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAPI_ShapeExplorer.h>
#include <GeomAPI_Pnt.h>

#include <map>
#include <sstream>
#include <cmath>

//==================================================================================================
FeaturesPlugin_Loft::FeaturesPlugin_Loft()
{
}

//==================================================================================================
void FeaturesPlugin_Loft::initAttributes()
{
  data()->addAttribute(FIRST_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(SECOND_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
}

//==================================================================================================
void FeaturesPlugin_Loft::execute()
{
  AttributeSelectionPtr aFirstSelection = selection(FIRST_OBJECT_ID());
  AttributeSelectionPtr aSecondelection = selection(SECOND_OBJECT_ID());

  if (aFirstSelection->isInitialized() && aSecondelection->isInitialized()) {

    GeomShapePtr aFirstShape, aSecondShape;

    if (aFirstSelection && aSecondelection) {
      aFirstShape = aFirstSelection->value();
      if (!aFirstShape && aFirstSelection->context())
        aFirstShape = aFirstSelection->context()->shape();

      aSecondShape = aSecondelection->value();
      if (!aSecondShape && aSecondelection->context())
        aSecondShape = aSecondelection->context()->shape();

    }
    // Loft for two edges
    if(aFirstShape->isEdge()) {

      std::shared_ptr<GeomAlgoAPI_Filling> aFilling(
                          new GeomAlgoAPI_Filling(2.0, 5.0, 0, 0.0001, 0.0001 ));

      // collect base shapes
      GeomEdgePtr aFirstEdge = toEdge(aFirstShape);
      if (!aFirstEdge) {
        myLastEdgeStartPoint = GeomPointPtr();
        myLastEdgeEndPoint = GeomPointPtr();
        return;
      }
      aFilling->add(aFirstEdge);

      GeomEdgePtr aSecondEdge = toEdge(aSecondShape);
      if (!aSecondEdge) {
        myLastEdgeStartPoint = GeomPointPtr();
        myLastEdgeEndPoint = GeomPointPtr();
        return;
      }
      aFilling->add(aSecondEdge);
      myLastEdgeStartPoint = GeomPointPtr();
      myLastEdgeEndPoint = GeomPointPtr();

      // build result
      aFilling->build();
      std::string anError;
      if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aFilling, getKind(), anError)) {
        setError(anError);
        removeResults(0);
        return;
      }

      /// store result
      GeomShapePtr aCreatedFace = aFilling->shape();
      ResultBodyPtr aResultBody = document()->createBody(data());
      aResultBody->store(aCreatedFace);
      // store edges
      int anEdgeInd = 0;
      for(GeomAPI_ShapeExplorer anExp(aCreatedFace, GeomAPI_Shape::EDGE);
                                anExp.more(); anExp.next()) {
        GeomShapePtr anEdge = anExp.current();
        aResultBody->generated(anEdge, "Loft_Edge_" + std::to_string((long long)anEdgeInd));
      }
      setResult(aResultBody, 0);

    } else {

      std::shared_ptr<GeomAlgoAPI_Loft> aLoftAlgo(new GeomAlgoAPI_Loft(aFirstShape, aSecondShape));

      std::string anError;

      if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aLoftAlgo, getKind(), anError)) {
        setError(anError);
        return;
      }
      ListOfShape  theBoundaryShapes;
      theBoundaryShapes.push_back(aFirstShape);
      theBoundaryShapes.push_back(aSecondShape);

      // Create result body.
      ResultBodyPtr aResultBody = document()->createBody(data());

      aResultBody->store(aLoftAlgo->shape());
      // store edges
      int anEdgeInd = 0;
      for(GeomAPI_ShapeExplorer anExp(aLoftAlgo->shape(), GeomAPI_Shape::EDGE);
                                anExp.more(); anExp.next()) {
        GeomShapePtr anEdge = anExp.current();
        aResultBody->generated(anEdge, "Loft_Edge_" + std::to_string((long long)anEdgeInd));
      }
      int anFaceInd = 0;
      for(GeomAPI_ShapeExplorer anExp(aLoftAlgo->shape(), GeomAPI_Shape::FACE);
                                anExp.more(); anExp.next()) {
        GeomShapePtr anEdge = anExp.current();
        aResultBody->generated(anEdge, "Loft_Face_" + std::to_string((long long)anFaceInd));
      }
      setResult(aResultBody, 0);
    }
  }
}
/*=================================================================================================
GeomShapePtr FeaturesPlugin_Loft::getShape(const std::string& anID)
{
  AttributeSelectionPtr theSelection = selection(anID);
  if (!theSelection.get()) {
      setError("Error: One of the selected base objects is empty.");
      return;
  }

  GeomShapePtr aShape = theSelection->value();

  if (aShape.get() && !aShape->isNull()) {
    std::cout<<"C'est un shape"<<std::endl;  
    return aShape;
  } else {
    // This may be the whole sketch result selected, check and get faces.
    ResultConstructionPtr aConstruction =
      std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(theSelection->context());
    if(!aConstruction.get()) {
      setError("Error: One of selected sketches does not have results.");
      return;
    }
    int aFacesNum = aConstruction->facesNum();
    if(aFacesNum == 0) {
      // Probably it can be construction.
      aShape = aConstruction->shape();
      if(aShape.get() && !aShape->isNull()) {
        std::cout<<"C'est un aConstruction"<<std::endl;  
        return aShape;
      }
    } else {
      setError("Error: One of the selected base objects is not vaidated");
      return;
    }
  }
}
*/

//=================================================================================================
GeomEdgePtr FeaturesPlugin_Loft::toEdge(const GeomShapePtr& theShape)
{
  GeomEdgePtr anEdge = GeomEdgePtr(new GeomAPI_Edge(GeomAlgoAPI_Copy(theShape).shape()));

  if (!anEdge || anEdge->empty()) {
    static const std::string aFeatureError =
        "Error: incorrect type of input feature (edges are supported only).";
    setError(aFeatureError);
    return anEdge;
  }

  // correct edge orientation according to filling method
  // check the distance to previous edge boundaries, reverse edge if necessary
  GeomPointPtr aStartPnt = anEdge->firstPoint();
  GeomPointPtr aEndPnt = anEdge->lastPoint();
  if (anEdge->orientation() == GeomAPI_Shape::REVERSED) {
    aStartPnt = anEdge->lastPoint();
    aEndPnt = anEdge->firstPoint();
  }
  bool isReverse = false;
  if (myLastEdgeStartPoint) {
    double d1 = myLastEdgeStartPoint->distance(aStartPnt)
              + myLastEdgeEndPoint->distance(aEndPnt);
    double d2 = myLastEdgeStartPoint->distance(aEndPnt)
              + myLastEdgeEndPoint->distance(aStartPnt);
    if (fabs(d1 - d2) < 1.e-7) {
      // undefined case => check distance to start point only
      d1 = myLastEdgeStartPoint->distance(aStartPnt);
      d2 = myLastEdgeStartPoint->distance(aEndPnt);
    }
    isReverse = d2 < d1;
  }

  if (isReverse) {
    anEdge->reverse();
    myLastEdgeStartPoint = aEndPnt;
    myLastEdgeEndPoint = aStartPnt;
  } else {
    myLastEdgeStartPoint = aStartPnt;
    myLastEdgeEndPoint = aEndPnt;
  }

  return anEdge;
}

/*==================================================================================================
void FeaturesPlugin_Loft::storeResult(const std::shared_ptr<GeomAPI_Shape> theFirstShape,
                                      const std::shared_ptr<GeomAPI_Shape> theSecondShape,
                                      const std::shared_ptr<GeomAlgoAPI_Loft> theLoftAlgo,
                                      const int theResultIndex)
{
  // Create result body.
  ResultBodyPtr aResultBody = document()->createBody(data(), theResultIndex);

  // Store generated shape.
  aResultBody->storeGenerated(theFirstShape, theLoftAlgo->shape());
  aResultBody->storeGenerated(theSecondShape, theLoftAlgo->shape());

  // Store generated edges/faces.
  GeomAPI_Shape::ShapeType aBaseShapeType = theFirstShape->shapeType();
  GeomAPI_Shape::ShapeType aShapeTypeToExplode = GeomAPI_Shape::SHAPE;

  switch(aBaseShapeType) {
    case GeomAPI_Shape::VERTEX: {
      aShapeTypeToExplode = GeomAPI_Shape::VERTEX;
      break;
    }
    case GeomAPI_Shape::EDGE:
    case GeomAPI_Shape::WIRE: {
      std::shared_ptr<GeomAPI_Vertex> aV1, aV2;
      GeomAlgoAPI_ShapeTools::findBounds(theFirstShape, aV1, aV2);
      ListOfShape aV1History, aV2History;
      theLoftAlgo->generated(aV1, aV1History);
      theLoftAlgo->generated(aV2, aV2History);
      if(!aV1History.empty()) {
        aResultBody->generated(aV1, aV1History.front());
      }
      if(!aV2History.empty()) {
        aResultBody->generated(aV2, aV2History.front());
      }
      GeomAlgoAPI_ShapeTools::findBounds(theSecondShape, aV1, aV2);
      theLoftAlgo->generated(aV1, aV1History);
      theLoftAlgo->generated(aV2, aV2History);
      if(!aV1History.empty()) {
        aResultBody->generated(aV1, aV1History.front());
      }
      if(!aV2History.empty()) {
        aResultBody->generated(aV2, aV2History.front());
      }
    }
    case GeomAPI_Shape::FACE:
    case GeomAPI_Shape::SHELL: {
      aShapeTypeToExplode = GeomAPI_Shape::EDGE;
      break;
    }
    case GeomAPI_Shape::COMPOUND: {
      aShapeTypeToExplode = GeomAPI_Shape::COMPOUND;
      break;
    }
    default:
      return; // unsupported type of shape
  }

  if(aShapeTypeToExplode == GeomAPI_Shape::VERTEX ||
      aShapeTypeToExplode == GeomAPI_Shape::COMPOUND) {
    aResultBody->loadGeneratedShapes(theLoftAlgo, theFirstShape, GeomAPI_Shape::VERTEX);
    aResultBody->loadGeneratedShapes(theLoftAlgo, theSecondShape, GeomAPI_Shape::VERTEX);
  }
  if(aShapeTypeToExplode == GeomAPI_Shape::EDGE ||
      aShapeTypeToExplode == GeomAPI_Shape::COMPOUND) {
    aResultBody->loadGeneratedShapes(theLoftAlgo, theFirstShape, GeomAPI_Shape::EDGE);
    aResultBody->loadGeneratedShapes(theLoftAlgo, theSecondShape, GeomAPI_Shape::EDGE);
  }

  // Store from shapes.
  storeShapes(aResultBody, aBaseShapeType, theLoftAlgo->fromShapes(), "From_");

  // Store to shapes.
  storeShapes(aResultBody, aBaseShapeType, theLoftAlgo->toShapes(), "To_");

  setResult(aResultBody, theResultIndex);
}

//==================================================================================================
void FeaturesPlugin_Loft::storeShapes(ResultBodyPtr theResultBody,
                                      const GeomAPI_Shape::ShapeType theBaseShapeType,
                                      const ListOfShape& theShapes,
                                      const std::string theName)
{
  GeomAPI_Shape::ShapeType aShapeTypeToExplore = GeomAPI_Shape::FACE;
  std::string aShapeTypeStr = "Face";
  switch(theBaseShapeType) {
    case GeomAPI_Shape::VERTEX: {
      aShapeTypeToExplore = GeomAPI_Shape::VERTEX;
      aShapeTypeStr = "Vertex";
      break;
    }
    case GeomAPI_Shape::EDGE:
    case GeomAPI_Shape::WIRE: {
      aShapeTypeToExplore = GeomAPI_Shape::EDGE;
      aShapeTypeStr = "Edge";
      break;
    }
    case GeomAPI_Shape::FACE:
    case GeomAPI_Shape::SHELL: {
      aShapeTypeToExplore = GeomAPI_Shape::FACE;
      aShapeTypeStr = "Face";
      break;
    }
    case GeomAPI_Shape::COMPOUND: {
      aShapeTypeToExplore = GeomAPI_Shape::COMPOUND;
      break;
    }
    default:
      return; // unsupported type of shape
  }

  // Store shapes.
  int aShapeIndex = 1;
  int aFaceIndex = 1;
  for(ListOfShape::const_iterator anIt = theShapes.cbegin(); anIt != theShapes.cend(); ++anIt) {
    GeomShapePtr aShape = *anIt;

    if(aShapeTypeToExplore == GeomAPI_Shape::COMPOUND) {
      std::string aName = theName + (aShape->shapeType() == GeomAPI_Shape::EDGE ? "Edge" : "Face");
      storeSubShapeTT(theResultBody, aShape,
                    aShape->shapeType(),
                    aName,
                    aShape->shapeType() == GeomAPI_Shape::EDGE ? aShapeIndex : aFaceIndex);
    } else {
      std::string aName = theName + aShapeTypeStr;
      storeSubShapeTT(theResultBody, aShape, aShapeTypeToExplore, aName, aShapeIndex);
    }
  }
}

//==================================================================================================
void storeSubShapeTT(ResultBodyPtr theResultBody,
                   const GeomShapePtr theShape,
                   const GeomAPI_Shape::ShapeType theType,
                   const std::string theName,
                   int& theShapeIndex)
{
  for(GeomAPI_ShapeExplorer anExp(theShape, theType); anExp.more(); anExp.next()) {
    GeomShapePtr aSubShape = anExp.current();
    std::ostringstream aStr;
    aStr << theName << "_" << theShapeIndex++;
    theResultBody->generated(aSubShape, aStr.str());
  }
}
*/