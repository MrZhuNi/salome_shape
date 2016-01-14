// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesPlugin_Boolean.cpp
// Created:     02 Sept 2014
// Author:      Vitaly SMETANNIKOV

#include "FeaturesPlugin_Boolean.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Tools.h>

#include <GeomAlgoAPI_Boolean.h>
#include <GeomAlgoAPI_CellsBuilder.h>
#include <GeomAlgoAPI_MakeShapeList.h>
#include <GeomAlgoAPI_PaveFiller.h>
#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAPI_ShapeExplorer.h>

#include <algorithm>

//=================================================================================================
FeaturesPlugin_Boolean::FeaturesPlugin_Boolean()
{
}

//=================================================================================================
void FeaturesPlugin_Boolean::initAttributes()
{
  data()->addAttribute(FeaturesPlugin_Boolean::TYPE_ID(), ModelAPI_AttributeInteger::typeId());

  AttributeSelectionListPtr aSelection =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(data()->addAttribute(
    FeaturesPlugin_Boolean::OBJECT_LIST_ID(), ModelAPI_AttributeSelectionList::typeId()));
  aSelection->setSelectionType("SOLID");

  aSelection = std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(data()->addAttribute(
    FeaturesPlugin_Boolean::TOOL_LIST_ID(), ModelAPI_AttributeSelectionList::typeId()));
  aSelection->setSelectionType("SOLID");

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), OBJECT_LIST_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), TOOL_LIST_ID());
}

//=================================================================================================
std::shared_ptr<GeomAPI_Shape> FeaturesPlugin_Boolean::getShape(const std::string& theAttrName)
{
  std::shared_ptr<ModelAPI_AttributeReference> aObjRef = std::dynamic_pointer_cast<
      ModelAPI_AttributeReference>(data()->attribute(theAttrName));
  if (aObjRef) {
    std::shared_ptr<ModelAPI_ResultBody> aConstr = std::dynamic_pointer_cast<
        ModelAPI_ResultBody>(aObjRef->value());
    if (aConstr)
      return aConstr->shape();
  }
  return std::shared_ptr<GeomAPI_Shape>();
}

//=================================================================================================
void FeaturesPlugin_Boolean::execute()
{
  // Getting operation type.
  std::shared_ptr<ModelAPI_AttributeInteger> anOperationTypeAttr = std::dynamic_pointer_cast<
      ModelAPI_AttributeInteger>(data()->attribute(FeaturesPlugin_Boolean::TYPE_ID()));
  if (!anOperationTypeAttr)
    return;
  GeomAlgoAPI_Boolean::OperationType anOperationType = (GeomAlgoAPI_Boolean::OperationType)anOperationTypeAttr->value();

  ListOfShape anObjects, aTools;
  std::map<std::shared_ptr<GeomAPI_Shape>, ListOfShape> aCompSolidsObjects;

  // Getting objects.
  AttributeSelectionListPtr anObjectsSelList = selectionList(FeaturesPlugin_Boolean::OBJECT_LIST_ID());
  for(int anObjectsIndex = 0; anObjectsIndex < anObjectsSelList->size(); anObjectsIndex++) {
    AttributeSelectionPtr anObjectAttr = anObjectsSelList->value(anObjectsIndex);
    std::shared_ptr<GeomAPI_Shape> anObject = anObjectAttr->value();
    if(!anObject.get()) {
      return;
    }
    ResultPtr aContext = anObjectAttr->context();
    ResultCompSolidPtr aResCompSolidPtr = ModelAPI_Tools::compSolidOwner(aContext);
    if(aResCompSolidPtr.get()) {
      std::shared_ptr<GeomAPI_Shape> aContextShape = aResCompSolidPtr->shape();
      std::map<std::shared_ptr<GeomAPI_Shape>, ListOfShape>::iterator anIt = aCompSolidsObjects.begin();
      for(; anIt != aCompSolidsObjects.end(); anIt++) {
        if(anIt->first->isEqual(aContextShape)) {
          aCompSolidsObjects[anIt->first].push_back(anObject);
          break;
        }
      }
      if(anIt == aCompSolidsObjects.end()) {
        aCompSolidsObjects[aContextShape].push_back(anObject);
      }
    } else {
      anObjects.push_back(anObject);
    }
  }

  // Getting tools.
  AttributeSelectionListPtr aToolsSelList = selectionList(FeaturesPlugin_Boolean::TOOL_LIST_ID());
  for(int aToolsIndex = 0; aToolsIndex < aToolsSelList->size(); aToolsIndex++) {
    AttributeSelectionPtr aToolAttr = aToolsSelList->value(aToolsIndex);
    std::shared_ptr<GeomAPI_Shape> aTool = aToolAttr->value();
    if(!aTool.get()) {
      return;
    }
    aTools.push_back(aTool);
  }

  int aResultIndex = 0;

  switch(anOperationType) {
    case GeomAlgoAPI_Boolean::BOOL_CUT:
    case GeomAlgoAPI_Boolean::BOOL_COMMON:{
      if((anObjects.empty() && aCompSolidsObjects.empty()) || aTools.empty()) {
        std::string aFeatureError = "Error: not enough objects for boolean operation.";
        setError(aFeatureError);
        return;
      }

      // For solids cut each object with all tools.
      for(ListOfShape::iterator anObjectsIt = anObjects.begin(); anObjectsIt != anObjects.end(); anObjectsIt++) {
        std::shared_ptr<GeomAPI_Shape> anObject = *anObjectsIt;
        ListOfShape aListWithObject;
        aListWithObject.push_back(anObject);
        GeomAlgoAPI_Boolean aBoolAlgo(aListWithObject, aTools, anOperationType);

        // Checking that the algorithm worked properly.
        if(!aBoolAlgo.isDone()) {
          static const std::string aFeatureError = "Error: boolean algorithm failed.";
          setError(aFeatureError);
          return;
        }
        if(aBoolAlgo.shape()->isNull()) {
          static const std::string aShapeError = "Error: resulting shape is Null.";
          setError(aShapeError);
          return;
        }
        if(!aBoolAlgo.isValid()) {
          std::string aFeatureError = "Error: resulting shape is not valid.";
          setError(aFeatureError);
          return;
        }

        if(GeomAlgoAPI_ShapeTools::volume(aBoolAlgo.shape()) > 1.e-7) {
          std::shared_ptr<ModelAPI_ResultBody> aResultBody = document()->createBody(data(), aResultIndex);
          loadNamingDS(aResultBody, anObject, aTools, aBoolAlgo.shape(), aBoolAlgo, *aBoolAlgo.mapOfSubShapes().get());
          setResult(aResultBody, aResultIndex);
          aResultIndex++;
        }
      }

      // Compsolids handling
      int aMaterialId = 1;
      for(std::map<std::shared_ptr<GeomAPI_Shape>, ListOfShape>::iterator anIt = aCompSolidsObjects.begin();
        anIt != aCompSolidsObjects.end(); anIt++) {
        std::shared_ptr<GeomAPI_Shape> aCompSolid = anIt->first;
        ListOfShape& aUsedInOperationSolids = anIt->second;

        // Collecting solids from compsolids which will not be modified in boolean operation.
        ListOfShape aNotUsedInOperationSolids;
        GeomAlgoAPI_ShapeTools::getSolidsInCompSolid(aCompSolid, aUsedInOperationSolids, aNotUsedInOperationSolids);

        // Collecting all solids from compsolid and tools, and setting them as arguments for builder.
        ListOfShape anArguments;
        anArguments.insert(anArguments.end(), aUsedInOperationSolids.begin(), aUsedInOperationSolids.end());
        anArguments.insert(anArguments.end(), aNotUsedInOperationSolids.begin(), aNotUsedInOperationSolids.end());
        anArguments.insert(anArguments.end(), aTools.begin(), aTools.end());

        // Perform splitting into cells.
        GeomAlgoAPI_CellsBuilder aCellsBuilder;
        aCellsBuilder.setArguments(anArguments);
        aCellsBuilder.setFuzzyValue(0.0);
        aCellsBuilder.setRunParallel(false);
        aCellsBuilder.perform();

        // Taking not used solids in compsolid.
        ListOfShape aLSToTake, aLSToAvoid;
        for(ListOfShape::const_iterator anIt = aNotUsedInOperationSolids.cbegin(); anIt != aNotUsedInOperationSolids.cend(); anIt++) {
          aLSToTake.clear(); aLSToAvoid.clear();
          aLSToTake.push_back(*anIt);
          aCellsBuilder.addToResult(aLSToTake, aLSToAvoid, ++aMaterialId);
        }

        // Taking result solids after boolean operation.
        for(ListOfShape::const_iterator anIt = aUsedInOperationSolids.cbegin(); anIt != aUsedInOperationSolids.cend(); anIt++) {
          aLSToTake.clear(); aLSToAvoid.clear();
          aLSToTake.push_back(*anIt);
          if(anOperationType == BOOL_CUT) {
            aLSToAvoid.insert(aLSToAvoid.end(), aTools.begin(), aTools.end());
            aCellsBuilder.addToResult(aLSToTake, aLSToAvoid);
          } else {
            for(ListOfShape::const_iterator aToolsIt = aTools.cbegin(); aToolsIt != aTools.cend(); aToolsIt++) {
              aLSToTake.push_back(*aToolsIt);
              aCellsBuilder.addToResult(aLSToTake, aLSToAvoid);
            }
          }
        }

        aCellsBuilder.removeInternalBoundaries();

        // Checking that the algorithm worked properly.
        if(!aCellsBuilder.isDone()) {
          static const std::string aFeatureError = "Error: cells builder algorithm failed.";
          setError(aFeatureError);
          return;
        }
        if(aCellsBuilder.shape()->isNull()) {
          static const std::string aShapeError = "Error: resulting shape is Null.";
          setError(aShapeError);
          return;
        }
        if(!aCellsBuilder.isValid()) {
          std::string aFeatureError = "Error: resulting shape is not valid.";
          setError(aFeatureError);
          return;
        }

        std::shared_ptr<GeomAPI_Shape> aResultShape = aCellsBuilder.shape();

        if(GeomAlgoAPI_ShapeTools::volume(aResultShape) > 1.e-7) {
          std::shared_ptr<ModelAPI_ResultBody> aResultBody = document()->createBody(data(), aResultIndex);
          loadNamingDS(aResultBody, aCompSolid, aTools, aCellsBuilder.shape(), aCellsBuilder, *(aCellsBuilder.mapOfSubShapes()));
          setResult(aResultBody, aResultIndex);
          aResultIndex++;
        }
      }
      break;
    }
    case GeomAlgoAPI_Boolean::BOOL_FUSE: {
      if((anObjects.size() + aTools.size() + aCompSolidsObjects.size()) < 2) {
        std::string aFeatureError = "Error: not enough objects for boolean operation.";
        setError(aFeatureError);
        return;
      }

      // Adding solids from compsolids to list of solids which will be fused.
      ListOfShape aUsedInOperationSolids, aNotUsedInOperationSolids;
      for(std::map<std::shared_ptr<GeomAPI_Shape>, ListOfShape>::iterator anIt = aCompSolidsObjects.begin();
        anIt != aCompSolidsObjects.end(); anIt++) {
        std::shared_ptr<GeomAPI_Shape> aCompSolid = anIt->first;
        ListOfShape& aSolidsToAvoid = anIt->second;
        aUsedInOperationSolids.insert(aUsedInOperationSolids.end(), aSolidsToAvoid.begin(), aSolidsToAvoid.end());

        // Collect solids from compsolid which will not be modified in boolean operation.
        GeomAlgoAPI_ShapeTools::getSolidsInCompSolid(aCompSolid, aSolidsToAvoid, aNotUsedInOperationSolids);
      }

      // Collecting objects, tools and all solids from compsolid, and setting them as arguments for builder.
      ListOfShape anArguments;
      anArguments.insert(anArguments.end(), anObjects.begin(), anObjects.end());
      anArguments.insert(anArguments.end(), aTools.begin(), aTools.end());
      anArguments.insert(anArguments.end(), aUsedInOperationSolids.begin(), aUsedInOperationSolids.end());
      anArguments.insert(anArguments.end(), aNotUsedInOperationSolids.begin(), aNotUsedInOperationSolids.end());

      // Perform splitting into cells.
      GeomAlgoAPI_CellsBuilder aCellsBuilder;
      aCellsBuilder.setArguments(anArguments);
      aCellsBuilder.setFuzzyValue(0.0);
      aCellsBuilder.setRunParallel(false);
      aCellsBuilder.perform();

      // Taking objects.
      int aMaterialId = 1;
      ListOfShape aLSToTake, aLSToAvoid;
      for(ListOfShape::const_iterator anIt = anObjects.cbegin(); anIt != anObjects.cend(); anIt++) {
        aLSToTake.clear();
        aLSToTake.push_back(*anIt);
        aCellsBuilder.addToResult(aLSToTake, aNotUsedInOperationSolids, aMaterialId);
      }

      // Taking tools.
      for(ListOfShape::const_iterator anIt = aTools.cbegin(); anIt != aTools.cend(); anIt++) {
        aLSToTake.clear();
        aLSToTake.push_back(*anIt);
        aCellsBuilder.addToResult(aLSToTake, aNotUsedInOperationSolids, aMaterialId);
      }

      // Taking used solids in compsolids.
      for(ListOfShape::const_iterator anIt = aUsedInOperationSolids.cbegin(); anIt != aUsedInOperationSolids.cend(); anIt++) {
        aLSToTake.clear();
        aLSToTake.push_back(*anIt);
        aCellsBuilder.addToResult(aLSToTake, aNotUsedInOperationSolids, aMaterialId);
      }

      // Taking not used solids in compsolid.
      for(ListOfShape::const_iterator anIt = aNotUsedInOperationSolids.cbegin(); anIt != aNotUsedInOperationSolids.cend(); anIt++) {
        aLSToTake.clear();
        aLSToTake.push_back(*anIt);
        aCellsBuilder.addToResult(aLSToTake, aLSToAvoid, ++aMaterialId);
      }

      aCellsBuilder.removeInternalBoundaries();

      // Checking that the algorithm worked properly.
      if(!aCellsBuilder.isDone()) {
        static const std::string aFeatureError = "Error: cells builder algorithm failed.";
        setError(aFeatureError);
        return;
      }
      if(aCellsBuilder.shape()->isNull()) {
        static const std::string aShapeError = "Error: resulting shape is Null.";
        setError(aShapeError);
        return;
      }
      if(!aCellsBuilder.isValid()) {
        std::string aFeatureError = "Error: resulting shape is not valid.";
        setError(aFeatureError);
        return;
      }

      std::shared_ptr<GeomAPI_Shape> aResultShape = aCellsBuilder.shape();

      if(GeomAlgoAPI_ShapeTools::volume(aResultShape) > 1.e-7) {
        std::shared_ptr<ModelAPI_ResultBody> aResultBody = document()->createBody(data(), aResultIndex);
        loadNamingDS(aResultBody, anArguments.front(), anArguments, aCellsBuilder.shape(), aCellsBuilder, *(aCellsBuilder.mapOfSubShapes()));
        setResult(aResultBody, aResultIndex);
        aResultIndex++;
      }
      break;
    }
    default: {
      std::string anOperationError = "Error: wrong type of operation.";
      setError(anOperationError);
      return;
    }
  }
  // remove the rest results if there were produced in the previous pass
  removeResults(aResultIndex);
}

//=================================================================================================
void FeaturesPlugin_Boolean::loadNamingDS(std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                                          const std::shared_ptr<GeomAPI_Shape> theBaseShape,
                                          const ListOfShape& theTools,
                                          const std::shared_ptr<GeomAPI_Shape> theResultShape,
                                          GeomAlgoAPI_MakeShape& theMakeShape,
                                          GeomAPI_DataMapOfShapeShape& theMapOfShapes)
{
  //load result
  if(theBaseShape->isEqual(theResultShape)) {
    theResultBody->store(theResultShape);
  } else {
    const int aModifyTag = 1;
    const int aDeletedTag = 2;
    const int aSubsolidsTag = 3; /// sub solids will be placed at labels 3, 4, etc. if result is compound of solids

    theResultBody->storeModified(theBaseShape, theResultShape, aSubsolidsTag);

    std::string aModName = "Modified";
    theResultBody->loadAndOrientModifiedShapes(&theMakeShape, theBaseShape, GeomAPI_Shape::FACE,
                                               aModifyTag, aModName, theMapOfShapes);
    theResultBody->loadDeletedShapes(&theMakeShape, theBaseShape, GeomAPI_Shape::FACE, aDeletedTag);

    for(ListOfShape::const_iterator anIter = theTools.begin(); anIter != theTools.end(); anIter++) {
      theResultBody->loadAndOrientModifiedShapes(&theMakeShape, *anIter, GeomAPI_Shape::FACE,
                                                 aModifyTag, aModName, theMapOfShapes);
      theResultBody->loadDeletedShapes(&theMakeShape, *anIter, GeomAPI_Shape::FACE, aDeletedTag);
    }
  }
}
