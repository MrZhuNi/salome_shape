// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include "FeaturesPlugin_BooleanFuse.h"

#include <ModelAPI_ResultBody.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_Validator.h>

#include <GeomAlgoAPI_Boolean.h>
#include <GeomAlgoAPI_MakeShapeList.h>
#include <GeomAlgoAPI_PaveFiller.h>
#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAlgoAPI_UnifySameDomain.h>
#include <GeomAPI_ShapeExplorer.h>

//==================================================================================================
FeaturesPlugin_BooleanFuse::FeaturesPlugin_BooleanFuse()
: FeaturesPlugin_Boolean(FeaturesPlugin_Boolean::BOOL_FUSE)
{
}

//==================================================================================================
void FeaturesPlugin_BooleanFuse::initAttributes()
{
  data()->addAttribute(CREATION_METHOD(), ModelAPI_AttributeString::typeId());

  data()->addAttribute(OBJECT_LIST_ID(), ModelAPI_AttributeSelectionList::typeId());
  data()->addAttribute(TOOL_LIST_ID(), ModelAPI_AttributeSelectionList::typeId());

  data()->addAttribute(REMOVE_INTERSECTION_EDGES_ID(), ModelAPI_AttributeBoolean::typeId());

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), OBJECT_LIST_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), TOOL_LIST_ID());
}

//==================================================================================================
void FeaturesPlugin_BooleanFuse::execute()
{
  ListOfShape anObjects, aTools, anEdgesAndFaces;
  std::map<std::shared_ptr<GeomAPI_Shape>, ListOfShape> aCompSolidsObjects;

  bool isSimpleCreation = false;

  AttributeStringPtr aCreationMethodAttr = string(CREATION_METHOD());
  if (aCreationMethodAttr.get()
      && aCreationMethodAttr->value() == CREATION_METHOD_SIMPLE()) {
    isSimpleCreation = true;
  }

  // Getting objects.
  AttributeSelectionListPtr anObjectsSelList =
    selectionList(FeaturesPlugin_Boolean::OBJECT_LIST_ID());
  for (int anObjectsIndex = 0; anObjectsIndex < anObjectsSelList->size(); anObjectsIndex++) {
    AttributeSelectionPtr anObjectAttr = anObjectsSelList->value(anObjectsIndex);
    std::shared_ptr<GeomAPI_Shape> anObject = anObjectAttr->value();
    if (!anObject.get()) {
      return;
    }
    ResultPtr aContext = anObjectAttr->context();
    ResultBodyPtr aResCompSolidPtr = ModelAPI_Tools::bodyOwner(aContext);
    if (!isSimpleCreation
        && aResCompSolidPtr.get()
        && aResCompSolidPtr->shape()->shapeType() == GeomAPI_Shape::COMPSOLID) {
      std::shared_ptr<GeomAPI_Shape> aContextShape = aResCompSolidPtr->shape();
      std::map<std::shared_ptr<GeomAPI_Shape>, ListOfShape>::iterator
        anIt = aCompSolidsObjects.begin();
      for (; anIt != aCompSolidsObjects.end(); anIt++) {
        if (anIt->first->isEqual(aContextShape)) {
          aCompSolidsObjects[anIt->first].push_back(anObject);
          break;
        }
      }
      if (anIt == aCompSolidsObjects.end()) {
        aCompSolidsObjects[aContextShape].push_back(anObject);
      }
    } else {
      if (anObject->shapeType() == GeomAPI_Shape::EDGE
          || anObject->shapeType() == GeomAPI_Shape::FACE) {
        anEdgesAndFaces.push_back(anObject);
      } else {
        anObjects.push_back(anObject);
      }
    }
  }

  // Getting tools.
  if (!isSimpleCreation) {
    AttributeSelectionListPtr aToolsSelList = selectionList(FeaturesPlugin_Boolean::TOOL_LIST_ID());
    for (int aToolsIndex = 0; aToolsIndex < aToolsSelList->size(); aToolsIndex++) {
      AttributeSelectionPtr aToolAttr = aToolsSelList->value(aToolsIndex);
      GeomShapePtr aTool = aToolAttr->value();
      if (aTool->shapeType() == GeomAPI_Shape::EDGE
          || aTool->shapeType() == GeomAPI_Shape::FACE)
      {
        anEdgesAndFaces.push_back(aTool);
      } else {
        aTools.push_back(aTool);
      }
    }
  }

  if ((anObjects.size() + aTools.size() +
    aCompSolidsObjects.size() + anEdgesAndFaces.size()) < 2) {
    std::string aFeatureError = "Error: Not enough objects for boolean operation.";
    setError(aFeatureError);
    return;
  }

  // Collecting all solids which will be fused.
  ListOfShape aSolidsToFuse;
  aSolidsToFuse.insert(aSolidsToFuse.end(), anObjects.begin(), anObjects.end());
  aSolidsToFuse.insert(aSolidsToFuse.end(), aTools.begin(), aTools.end());

  // Collecting solids from compsolids which will not be modified
  // in boolean operation and will be added to result.
  ListOfShape aShapesToAdd;
  for (std::map<std::shared_ptr<GeomAPI_Shape>, ListOfShape>::iterator
       anIt = aCompSolidsObjects.begin();
       anIt != aCompSolidsObjects.end(); anIt++) {
    std::shared_ptr<GeomAPI_Shape> aCompSolid = anIt->first;
    ListOfShape& aUsedInOperationSolids = anIt->second;
    aSolidsToFuse.insert(aSolidsToFuse.end(), aUsedInOperationSolids.begin(),
                         aUsedInOperationSolids.end());

    // Collect solids from compsolid which will not be modified in boolean operation.
    for (GeomAPI_ShapeExplorer
         anExp(aCompSolid, GeomAPI_Shape::SOLID); anExp.more(); anExp.next()) {
      std::shared_ptr<GeomAPI_Shape> aSolidInCompSolid = anExp.current();
      ListOfShape::iterator anIt = aUsedInOperationSolids.begin();
      for (; anIt != aUsedInOperationSolids.end(); anIt++) {
        if (aSolidInCompSolid->isEqual(*anIt)) {
          break;
        }
      }
      if (anIt == aUsedInOperationSolids.end()) {
        aShapesToAdd.push_back(aSolidInCompSolid);
      }
    }
  }

  ListOfShape anOriginalShapes = aSolidsToFuse;
  anOriginalShapes.insert(anOriginalShapes.end(), aShapesToAdd.begin(), aShapesToAdd.end());

  // Cut edges and faces(if we have any) with solids.
  GeomAlgoAPI_MakeShapeList aMakeShapeList;
  GeomAPI_DataMapOfShapeShape aMapOfShapes;
  std::shared_ptr<GeomAPI_Shape> aCuttedEdgesAndFaces;
  if (!anEdgesAndFaces.empty()) {
    std::shared_ptr<GeomAlgoAPI_Boolean> aCutAlgo(new GeomAlgoAPI_Boolean(anEdgesAndFaces,
      anOriginalShapes, GeomAlgoAPI_Boolean::BOOL_CUT));
    if (aCutAlgo->isDone()) {
      aCuttedEdgesAndFaces = aCutAlgo->shape();
      aMakeShapeList.appendAlgo(aCutAlgo);
      aMapOfShapes.merge(aCutAlgo->mapOfSubShapes());
    }
  }
  anOriginalShapes.insert(anOriginalShapes.end(), anEdgesAndFaces.begin(),
                          anEdgesAndFaces.end());

  // If we have compsolids then cut with not used solids all others.
  if (!aShapesToAdd.empty()) {
    aSolidsToFuse.clear();
    for (ListOfShape::iterator
         anIt = anOriginalShapes.begin(); anIt != anOriginalShapes.end(); anIt++) {
      ListOfShape aOneObjectList;
      aOneObjectList.push_back(*anIt);
      std::shared_ptr<GeomAlgoAPI_Boolean> aCutAlgo(
        new GeomAlgoAPI_Boolean(aOneObjectList, aShapesToAdd, GeomAlgoAPI_Boolean::BOOL_CUT));

      if (GeomAlgoAPI_ShapeTools::volume(aCutAlgo->shape()) > 1.e-27) {
        aSolidsToFuse.push_back(aCutAlgo->shape());
        aMakeShapeList.appendAlgo(aCutAlgo);
        aMapOfShapes.merge(aCutAlgo->mapOfSubShapes());
      }
    }
  }

  if (!aSolidsToFuse.empty()) {
    anObjects.clear();
    anObjects.push_back(aSolidsToFuse.back());
    aSolidsToFuse.pop_back();
    aTools = aSolidsToFuse;
  }

  // Fuse all objects and all tools.
  std::shared_ptr<GeomAPI_Shape> aShape;
  if (anObjects.size() == 1 && aTools.empty()) {
    aShape = anObjects.front();
  } else if (anObjects.empty() && aTools.size() == 1) {
    aShape = aTools.front();
  } else if ((anObjects.size() + aTools.size()) > 1) {
    std::shared_ptr<GeomAlgoAPI_Boolean> aFuseAlgo(new GeomAlgoAPI_Boolean(anObjects,
      aTools,
      GeomAlgoAPI_Boolean::BOOL_FUSE));

    // Checking that the algorithm worked properly.
    if (!aFuseAlgo->isDone()) {
      static const std::string aFeatureError = "Error: Boolean algorithm failed.";
      setError(aFeatureError);
      return;
    }
    if (aFuseAlgo->shape()->isNull()) {
      static const std::string aShapeError = "Error: Resulting shape is Null.";
      setError(aShapeError);
      return;
    }
    if (!aFuseAlgo->isValid()) {
      std::string aFeatureError = "Error: Resulting shape is not valid.";
      setError(aFeatureError);
      return;
    }

    aShape = aFuseAlgo->shape();
    aMakeShapeList.appendAlgo(aFuseAlgo);
    aMapOfShapes.merge(aFuseAlgo->mapOfSubShapes());
  }

  // Combine result with not used solids from compsolid and edges and faces (if we have any).
  if (aCuttedEdgesAndFaces.get() && !aCuttedEdgesAndFaces->isNull()) {
    aShapesToAdd.push_back(aCuttedEdgesAndFaces);
  } else {
    aShapesToAdd.insert(aShapesToAdd.end(), anEdgesAndFaces.begin(), anEdgesAndFaces.end());
  }
  if (!aShapesToAdd.empty()) {
    if (aShape.get()) {
      aShapesToAdd.push_back(aShape);
    }
    std::shared_ptr<GeomAlgoAPI_PaveFiller> aFillerAlgo(
      new GeomAlgoAPI_PaveFiller(aShapesToAdd, true));
    if (!aFillerAlgo->isDone()) {
      std::string aFeatureError = "Error: PaveFiller algorithm failed.";
      setError(aFeatureError);
      return;
    }
    if (aFillerAlgo->shape()->isNull()) {
      static const std::string aShapeError = "Error: Resulting shape is Null.";
      setError(aShapeError);
      return;
    }
    if (!aFillerAlgo->isValid()) {
      std::string aFeatureError = "Error: Resulting shape is not valid.";
      setError(aFeatureError);
      return;
    }

    aShape = aFillerAlgo->shape();
    aMakeShapeList.appendAlgo(aFillerAlgo);
    aMapOfShapes.merge(aFillerAlgo->mapOfSubShapes());
  }

  bool isRemoveEdges = false;
  AttributeBooleanPtr removeEdgesAttr = boolean(REMOVE_INTERSECTION_EDGES_ID());
  if (removeEdgesAttr.get()) {
    isRemoveEdges = removeEdgesAttr->value();
  }

  if (isRemoveEdges) {
    std::shared_ptr<GeomAlgoAPI_UnifySameDomain> aUnifyAlgo(
      new GeomAlgoAPI_UnifySameDomain(aShape));

    if (!aUnifyAlgo->isDone()) {
      std::string aFeatureError = "Error: PaveFiller algorithm failed.";
      setError(aFeatureError);
      return;
    }
    if (aUnifyAlgo->shape()->isNull()) {
      static const std::string aShapeError = "Error: Resulting shape is Null.";
      setError(aShapeError);
      return;
    }
    if (!aUnifyAlgo->isValid()) {
      std::string aFeatureError = "Error: Resulting shape is not valid.";
      setError(aFeatureError);
      return;
    }

    aShape = aUnifyAlgo->shape();
    aMakeShapeList.appendAlgo(aUnifyAlgo);
    aMapOfShapes.merge(aUnifyAlgo->mapOfSubShapes());
  }

  int aResultIndex = 0;

  std::shared_ptr<GeomAPI_Shape> aBackShape = anOriginalShapes.back();
  anOriginalShapes.pop_back();
  std::shared_ptr<ModelAPI_ResultBody> aResultBody =
    document()->createBody(data(), aResultIndex);
  loadNamingDS(aResultBody, aBackShape, anOriginalShapes,
               aShape, aMakeShapeList, aMapOfShapes);
  setResult(aResultBody, aResultIndex);
  aResultIndex++;

  // remove the rest results if there were produced in the previous pass
  removeResults(aResultIndex);
}

//==================================================================================================
void FeaturesPlugin_BooleanFuse::loadNamingDS(ResultBodyPtr theResultBody,
                                              const GeomShapePtr theBaseShape,
                                              const ListOfShape& theTools,
                                              const GeomShapePtr theResultShape,
                                              GeomAlgoAPI_MakeShape& theMakeShape,
                                              GeomAPI_DataMapOfShapeShape& theMapOfShapes)
{
  //load result
  if (theBaseShape->isEqual(theResultShape)) {
    theResultBody->store(theResultShape, false);
  } else {
    const int aModifyVTag = 1;
    const int aModifyETag = 2;
    const int aModifyFTag = 3;
    const int aDeletedTag = 4;
    /// sub solids will be placed at labels 5, 6, etc. if result is compound of solids
    const int aSubsolidsTag = 5;

    theResultBody->storeModified(theBaseShape, theResultShape, aSubsolidsTag);

    const std::string aModVName = "Modified_Vertex";
    const std::string aModEName = "Modified_Edge";
    const std::string aModFName = "Modified_Face";

    theResultBody->loadAndOrientModifiedShapes(&theMakeShape, theBaseShape, GeomAPI_Shape::VERTEX,
                                               aModifyVTag, aModVName, theMapOfShapes, false,
                                               false, true);
    theResultBody->loadAndOrientModifiedShapes(&theMakeShape, theBaseShape, GeomAPI_Shape::EDGE,
                                               aModifyETag, aModEName, theMapOfShapes, false,
                                               false, true);
    theResultBody->loadAndOrientModifiedShapes(&theMakeShape, theBaseShape, GeomAPI_Shape::FACE,
                                               aModifyFTag, aModFName, theMapOfShapes, false,
                                               false, true);

    theResultBody->loadDeletedShapes(&theMakeShape, theBaseShape,
                                     GeomAPI_Shape::VERTEX, aDeletedTag);
    theResultBody->loadDeletedShapes(&theMakeShape, theBaseShape,
                                     GeomAPI_Shape::EDGE, aDeletedTag);
    theResultBody->loadDeletedShapes(&theMakeShape, theBaseShape,
                                     GeomAPI_Shape::FACE, aDeletedTag);

    for (ListOfShape::const_iterator anIter = theTools.begin(); anIter != theTools.end(); anIter++)
    {
      theResultBody->loadAndOrientModifiedShapes(&theMakeShape, *anIter, GeomAPI_Shape::VERTEX,
                                                 aModifyVTag, aModVName, theMapOfShapes, false,
                                                 false, true);

      theResultBody->loadAndOrientModifiedShapes(&theMakeShape, *anIter, GeomAPI_Shape::EDGE,
                                                 aModifyETag, aModEName, theMapOfShapes, false,
                                                 false, true);

      theResultBody->loadAndOrientModifiedShapes(&theMakeShape, *anIter, GeomAPI_Shape::FACE,
                                                 aModifyFTag, aModFName, theMapOfShapes, false,
                                                 false, true);

      theResultBody->loadDeletedShapes(&theMakeShape, *anIter, GeomAPI_Shape::VERTEX, aDeletedTag);
      theResultBody->loadDeletedShapes(&theMakeShape, *anIter, GeomAPI_Shape::EDGE, aDeletedTag);
      theResultBody->loadDeletedShapes(&theMakeShape, *anIter, GeomAPI_Shape::FACE, aDeletedTag);
    }
  }
}