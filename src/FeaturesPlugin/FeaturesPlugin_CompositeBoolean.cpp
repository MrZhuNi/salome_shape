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

#include "FeaturesPlugin_CompositeBoolean.h"

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_Tools.h>

#include <GeomAlgoAPI_Boolean.h>
#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_MakeShapeList.h>
#include <GeomAlgoAPI_PaveFiller.h>
#include <GeomAlgoAPI_ShapeTools.h>

#include <GeomAPI_ShapeExplorer.h>

#include <map>

//=================================================================================================
void FeaturesPlugin_CompositeBoolean::initBooleanAttributes()
{
  myFeature->data()->addAttribute(OBJECTS_ID(), ModelAPI_AttributeSelectionList::typeId());
}

//=================================================================================================
void FeaturesPlugin_CompositeBoolean::executeCompositeBoolean()
{
  // Make generation.
  ListOfShape aGenBaseShapes;
  ListOfMakeShape aGenMakeShapes;
  if(!makeGeneration(aGenBaseShapes, aGenMakeShapes)) {
    return;
  }

  // Getting tools.
  ListOfShape aTools;
  for(ListOfMakeShape::const_iterator
      anIt = aGenMakeShapes.cbegin(); anIt != aGenMakeShapes.cend(); ++anIt) {
    aTools.push_back((*anIt)->shape());
  }

  // Make boolean.
  ListOfShape aBooleanObjects;
  ListOfMakeShape aBooleanMakeShapes;
  if(!makeBoolean(aTools, aBooleanObjects, aBooleanMakeShapes)) {
    return;
  }

  if(myOperationType == BOOL_FUSE) {
    aTools.splice(aTools.begin(), aBooleanObjects);
    aBooleanObjects.splice(aBooleanObjects.begin(), aTools, aTools.begin());
  }

  // Store result.
  int aResultIndex = 0;
  std::vector<ResultBaseAlgo> aResultBaseAlgoList;
  ListOfShape aResultShapesList;
  ListOfShape::const_iterator aBoolObjIt = aBooleanObjects.cbegin();
  ListOfMakeShape::const_iterator aBoolMSIt = aBooleanMakeShapes.cbegin();
  for(; aBoolObjIt != aBooleanObjects.cend() && aBoolMSIt != aBooleanMakeShapes.cend();
      ++aBoolObjIt, ++aBoolMSIt) {

    int aTag = 1;

    ResultBodyPtr aResultBody = myFeature->document()->createBody(myFeature->data(), aResultIndex);

    if((*aBoolObjIt)->isEqual((*aBoolMSIt)->shape())) {
      aResultBody->store((*aBoolMSIt)->shape(), false);
    }
    else
    {
      aResultBody->storeModified(*aBoolObjIt, (*aBoolMSIt)->shape(), aTag);

      aTag += 5000;

      // Store generation history.
      ListOfShape::const_iterator aGenBaseIt = aGenBaseShapes.cbegin();
      ListOfMakeShape::const_iterator aGenMSIt = aGenMakeShapes.cbegin();
      for(; aGenBaseIt != aGenBaseShapes.cend() && aGenMSIt != aGenMakeShapes.cend();
          ++aGenBaseIt, ++aGenMSIt) {
        storeGenerationHistory(aResultBody, *aGenBaseIt, *aGenMSIt, aTag);
      }

      int aModTag = aTag;
      storeModificationHistory(aResultBody, *aBoolObjIt, aTools, *aBoolMSIt, aModTag);

      ResultBaseAlgo aRBA;
      aRBA.resultBody = aResultBody;
      aRBA.baseShape = *aBoolObjIt;
      aRBA.makeShape = *aBoolMSIt;
      aRBA.delTag = aModTag;
      aResultBaseAlgoList.push_back(aRBA);
      aResultShapesList.push_back((*aBoolMSIt)->shape());
    }

    myFeature->setResult(aResultBody, aResultIndex++);
  }

  // Store deleted shapes after all results has been proceeded. This is to avoid issue when in one
  // result shape has been deleted, but in another it was modified or stayed.
  GeomShapePtr aResultShapesCompound = GeomAlgoAPI_CompoundBuilder::compound(aResultShapesList);
  storeDeletedShapes(aResultBaseAlgoList, aTools, aResultShapesCompound);

  myFeature->removeResults(aResultIndex);
}

//=================================================================================================
bool FeaturesPlugin_CompositeBoolean::makeBoolean(const ListOfShape& theTools,
                                                  ListOfShape& theObjects,
                                                  ListOfMakeShape& theMakeShapes)
{
  // Getting objects.
  ListOfShape anObjects, anEdgesAndFaces, aCompSolids;
  std::map<GeomShapePtr, ListOfShape> aCompSolidsObjects;
  AttributeSelectionListPtr anObjectsSelList = myFeature->selectionList(OBJECTS_ID());
  for(int anObjectsIndex = 0; anObjectsIndex < anObjectsSelList->size(); anObjectsIndex++) {
    AttributeSelectionPtr anObjectAttr = anObjectsSelList->value(anObjectsIndex);
    GeomShapePtr anObject = anObjectAttr->value();
    if(!anObject.get()) {
      myFeature->setError("Error: Could not get object.");
      return false;
    }
    ResultPtr aContext = anObjectAttr->context();
    ResultBodyPtr aResCompSolidPtr = ModelAPI_Tools::bodyOwner(aContext);
    if(aResCompSolidPtr.get()) {
      GeomShapePtr aContextShape = aResCompSolidPtr->shape();
      std::map<GeomShapePtr, ListOfShape>::iterator anIt = aCompSolidsObjects.begin();
      for(; anIt != aCompSolidsObjects.end(); anIt++) {
        if(anIt->first->isEqual(aContextShape)) {
          aCompSolidsObjects[anIt->first].push_back(anObject);
          break;
        }
      }
      if(anIt == aCompSolidsObjects.end()) {
        aCompSolidsObjects[aContextShape].push_back(anObject);
        aCompSolids.push_back(aContextShape);
      }
    } else {
      if(anObject->shapeType() == GeomAPI_Shape::EDGE ||
         anObject->shapeType() == GeomAPI_Shape::FACE) {
        anEdgesAndFaces.push_back(anObject);
      } else {
        anObjects.push_back(anObject);
      }
    }
  }

  switch(myOperationType) {
    case BOOL_CUT: {
      if((anObjects.empty() && aCompSolidsObjects.empty()) || theTools.empty()) {
        myFeature->setError("Error: Not enough objects for boolean operation.");
        return false;
      }

      // For solids cut each object with all tools.
      for(ListOfShape::const_iterator
          anIt = anObjects.cbegin(); anIt != anObjects.cend(); ++anIt) {
        GeomShapePtr anObject = *anIt;
        ListOfShape aListWithObject;
        aListWithObject.push_back(anObject);
        std::shared_ptr<GeomAlgoAPI_Boolean> aBoolAlgo(new GeomAlgoAPI_Boolean(aListWithObject,
                                                                theTools,
                                                                GeomAlgoAPI_Boolean::BOOL_CUT));

        // Checking that the algorithm worked properly.
        if(!aBoolAlgo->isDone() || aBoolAlgo->shape()->isNull() || !aBoolAlgo->isValid()) {
          myFeature->setError("Error: Boolean algorithm failed.");
          return false;
        }

        if(GeomAlgoAPI_ShapeTools::volume(aBoolAlgo->shape()) > 1.e-27) {
          theObjects.push_back(anObject);
          theMakeShapes.push_back(aBoolAlgo);
        }
      }

      // Compsolids handling
      for(std::map<GeomShapePtr, ListOfShape>::const_iterator anIt = aCompSolidsObjects.cbegin();
          anIt != aCompSolidsObjects.cend(); ++anIt) {
        GeomShapePtr aCompSolid = anIt->first;
        const ListOfShape& aUsedShapes = anIt->second;

        // Collecting solids from compsolids which will not be modified in boolean operation.
        ListOfShape aShapesToAdd;
        for(GeomAPI_ShapeExplorer
            anExp(aCompSolid, GeomAPI_Shape::SOLID); anExp.more(); anExp.next()) {
          GeomShapePtr aSolidInCompSolid = anExp.current();
          ListOfShape::const_iterator aUsedShapesIt = aUsedShapes.cbegin();
          for(; aUsedShapesIt != aUsedShapes.cend(); ++aUsedShapesIt) {
            if(aSolidInCompSolid->isEqual(*aUsedShapesIt)) {
              break;
            }
          }
          if(aUsedShapesIt == aUsedShapes.end()) {
            aShapesToAdd.push_back(aSolidInCompSolid);
          }
        }

        std::shared_ptr<GeomAlgoAPI_Boolean> aBoolAlgo(new GeomAlgoAPI_Boolean(aUsedShapes,
                                                                  theTools,
                                                                  GeomAlgoAPI_Boolean::BOOL_CUT));

        // Checking that the algorithm worked properly.
        if(!aBoolAlgo->isDone() || aBoolAlgo->shape()->isNull() || !aBoolAlgo->isValid()) {
          myFeature->setError("Error: Boolean algorithm failed.");
          return false;
        }

        std::shared_ptr<GeomAlgoAPI_MakeShapeList> aMakeShapeList(new GeomAlgoAPI_MakeShapeList());
        aMakeShapeList->appendAlgo(aBoolAlgo);

        // Add result to not used solids from compsolid.
        aShapesToAdd.push_back(aBoolAlgo->shape());
        std::shared_ptr<GeomAlgoAPI_PaveFiller> aFillerAlgo(
          new GeomAlgoAPI_PaveFiller(aShapesToAdd, true));
        if(!aFillerAlgo->isDone() || aFillerAlgo->shape()->isNull() || !aFillerAlgo->isValid()) {
          myFeature->setError("Error: PaveFiller algorithm failed.");
          return false;
        }

        aMakeShapeList->appendAlgo(aFillerAlgo);

        if(GeomAlgoAPI_ShapeTools::volume(aFillerAlgo->shape()) > 1.e-27) {
          theObjects.push_back(aCompSolid);
          theMakeShapes.push_back(aMakeShapeList);
        }
      }
      break;
    }
    case BOOL_FUSE: {
      // Set objects.
      theObjects.insert(theObjects.end(), anEdgesAndFaces.begin(), anEdgesAndFaces.end());
      theObjects.insert(theObjects.end(), anObjects.begin(), anObjects.end());
      theObjects.insert(theObjects.end(), aCompSolids.begin(), aCompSolids.end());

      // Filter edges and faces in tools.
      ListOfShape aTools;
      for(ListOfShape::const_iterator anIt = theTools.cbegin(); anIt != theTools.cend(); ++anIt) {
        if((*anIt)->shapeType() == GeomAPI_Shape::EDGE ||
           (*anIt)->shapeType() == GeomAPI_Shape::FACE) {
          anEdgesAndFaces.push_back(*anIt);
        } else {
          aTools.push_back(*anIt);
        }
      }

      if((anObjects.size() + aTools.size() +
          aCompSolidsObjects.size() + anEdgesAndFaces.size()) < 2) {
        myFeature->setError("Error: Not enough objects for boolean operation.");
        return false;
      }

      // Collecting all solids which will be fused.
      ListOfShape aSolidsToFuse;
      aSolidsToFuse.insert(aSolidsToFuse.end(), anObjects.begin(), anObjects.end());
      aSolidsToFuse.insert(aSolidsToFuse.end(), aTools.begin(), aTools.end());

      // Collecting solids from compsolids which will not be
      // modified in boolean operation and will be added to result.
      ListOfShape aShapesToAdd;
      for(std::map<GeomShapePtr, ListOfShape>::iterator anIt = aCompSolidsObjects.begin();
          anIt != aCompSolidsObjects.end(); anIt++) {
        GeomShapePtr aCompSolid = anIt->first;
        ListOfShape& aUsedShapes = anIt->second;
        aSolidsToFuse.insert(aSolidsToFuse.end(), aUsedShapes.begin(), aUsedShapes.end());

        // Collect solids from compsolid which will not be modified in boolean operation.
        for(GeomAPI_ShapeExplorer
            anExp(aCompSolid, GeomAPI_Shape::SOLID); anExp.more(); anExp.next()) {
          GeomShapePtr aSolidInCompSolid = anExp.current();
          ListOfShape::iterator anIt = aUsedShapes.begin();
          for(; anIt != aUsedShapes.end(); anIt++) {
            if(aSolidInCompSolid->isEqual(*anIt)) {
              break;
            }
          }
          if(anIt == aUsedShapes.end()) {
            aShapesToAdd.push_back(aSolidInCompSolid);
          }
        }
      }

      // Cut edges and faces(if we have any) with solids.
      ListOfShape aCutTools;
      aCutTools.insert(aCutTools.end(), anObjects.begin(), anObjects.end());
      aCutTools.insert(aCutTools.end(), aCompSolids.begin(), aCompSolids.end());
      aCutTools.insert(aCutTools.end(), aTools.begin(), aTools.end());

      std::shared_ptr<GeomAlgoAPI_MakeShapeList> aMakeShapeList(new GeomAlgoAPI_MakeShapeList());
      if(!anEdgesAndFaces.empty() && !aCutTools.empty()) {
        std::shared_ptr<GeomAlgoAPI_Boolean> aCutAlgo(new GeomAlgoAPI_Boolean(anEdgesAndFaces,
                                                              aCutTools,
                                                              GeomAlgoAPI_Boolean::BOOL_CUT));
        if(aCutAlgo->isDone() && !aCutAlgo->shape()->isNull() && aCutAlgo->isValid()) {
          anEdgesAndFaces.clear();
          anEdgesAndFaces.push_back(aCutAlgo->shape());
          aMakeShapeList->appendAlgo(aCutAlgo);
        }
      }

      // If we have compsolids then cut with not used solids all others.
      if(!aShapesToAdd.empty()) {
        std::shared_ptr<GeomAlgoAPI_Boolean> aCutAlgo(new GeomAlgoAPI_Boolean(aSolidsToFuse,
                                                              aShapesToAdd,
                                                              GeomAlgoAPI_Boolean::BOOL_CUT));
        if(aCutAlgo->isDone() && GeomAlgoAPI_ShapeTools::volume(aCutAlgo->shape()) > 1.e-27) {
          aSolidsToFuse.clear();
          aSolidsToFuse.push_back(aCutAlgo->shape());
          aMakeShapeList->appendAlgo(aCutAlgo);
        }
      }

      // Fuse all objects and all tools.
      GeomShapePtr aFusedShape;
      if(aSolidsToFuse.size() == 1) {
        aFusedShape = aSolidsToFuse.front();
      } else if(aSolidsToFuse.size() > 1){
        anObjects.clear();
        anObjects.push_back(aSolidsToFuse.front());
        aSolidsToFuse.pop_front();
        aTools = aSolidsToFuse;

        std::shared_ptr<GeomAlgoAPI_Boolean> aFuseAlgo(new GeomAlgoAPI_Boolean(anObjects,
                                                          aTools,
                                                          GeomAlgoAPI_Boolean::BOOL_FUSE));

        // Checking that the algorithm worked properly.
        if(!aFuseAlgo->isDone() || aFuseAlgo->shape()->isNull() || !aFuseAlgo->isValid()) {
          myFeature->setError("Error: Boolean algorithm failed.");
          return false;
        }

        aFusedShape = aFuseAlgo->shape();
        aMakeShapeList->appendAlgo(aFuseAlgo);
      }

      // Combine result with not used solids from compsolid and edges and faces (if we have any).
      aShapesToAdd.insert(aShapesToAdd.end(), anEdgesAndFaces.begin(), anEdgesAndFaces.end());
      if(!aShapesToAdd.empty()) {
        if(aFusedShape.get()) {
          aShapesToAdd.push_back(aFusedShape);
        }

        std::shared_ptr<GeomAlgoAPI_PaveFiller> aFillerAlgo(
          new GeomAlgoAPI_PaveFiller(aShapesToAdd, true));
        if(!aFillerAlgo->isDone() || aFillerAlgo->shape()->isNull() || !aFillerAlgo->isValid()) {
          myFeature->setError("Error: PaveFiller algorithm failed.");
          return false;
        }

        aMakeShapeList->appendAlgo(aFillerAlgo);
      }

      theMakeShapes.push_back(aMakeShapeList);
      break;
    }
  }

  return true;
}

//=================================================================================================
void FeaturesPlugin_CompositeBoolean::storeModificationHistory(ResultBodyPtr theResultBody,
                                const GeomShapePtr theObject,
                                const ListOfShape& theTools,
                                const std::shared_ptr<GeomAlgoAPI_MakeShape> theMakeShape,
                                int& theTag)
{
  int aModTag = theTag;
  int anEdgesAndFacesTag = ++aModTag;
  int aDelTag = ++anEdgesAndFacesTag;
  theTag = aDelTag;

  const std::string aModName = "Modfied";

  ListOfShape aTools = theTools;
  aTools.push_back(theObject);

  std::shared_ptr<GeomAPI_DataMapOfShapeShape> aMap = theMakeShape->mapOfSubShapes();

  int aTag;
  std::string aName;
  for(ListOfShape::const_iterator anIt = aTools.begin(); anIt != aTools.end(); anIt++) {
    if((*anIt)->shapeType() == GeomAPI_Shape::EDGE) {
      aTag = anEdgesAndFacesTag;
      aName = aModName + "_Edge";
    }
    else if((*anIt)->shapeType() == GeomAPI_Shape::FACE) {
      aTag = anEdgesAndFacesTag;
      aName = aModName + "_Face";
    } else {
      aTag = aModTag;
      aName = aModName;
    }
    theResultBody->loadAndOrientModifiedShapes(theMakeShape.get(), *anIt,
      (*anIt)->shapeType() == GeomAPI_Shape::EDGE ?
      GeomAPI_Shape::EDGE : GeomAPI_Shape::FACE, aTag, aName, *aMap.get(), false, false, true);
  }
}

//==================================================================================================
void FeaturesPlugin_CompositeBoolean::storeDeletedShapes(
  std::vector<ResultBaseAlgo>& theResultBaseAlgoList,
  const ListOfShape& theTools,
  const GeomShapePtr theResultShapesCompound)
{
  for (std::vector<ResultBaseAlgo>::iterator anIt = theResultBaseAlgoList.begin();
    anIt != theResultBaseAlgoList.end();
    ++anIt)
  {
    ResultBaseAlgo& aRCA = *anIt;
    aRCA.resultBody->loadDeletedShapes(aRCA.makeShape.get(),
      aRCA.baseShape,
      GeomAPI_Shape::FACE,
      aRCA.delTag,
      theResultShapesCompound);

    for (ListOfShape::const_iterator anIter = theTools.begin(); anIter != theTools.end(); anIter++)
    {
      aRCA.resultBody->loadDeletedShapes(aRCA.makeShape.get(),
        *anIter,
        GeomAPI_Shape::FACE,
        aRCA.delTag,
        theResultShapesCompound);
    }
  }
}
