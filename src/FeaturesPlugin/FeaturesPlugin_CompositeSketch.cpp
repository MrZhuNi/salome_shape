// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_CompositeSketch.cpp
// Created:     11 September 2015
// Author:      Dmitry Bobylev

#include <FeaturesPlugin_CompositeSketch.h>

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_BodyBuilder.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_Prism.h>
#include <GeomAlgoAPI_Revolution.h>
#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAlgoAPI_SketchBuilder.h>

#include <GeomAPI_PlanarEdges.h>
#include <GeomAPI_ShapeExplorer.h>

#include <map>
#include <sstream>

static void storeSubShape(ResultBodyPtr theResultBody,
                          const GeomShapePtr theShape,
                          const GeomAPI_Shape::ShapeType theType,
                          const std::shared_ptr<GeomAPI_DataMapOfShapeShape> theMapOfSubShapes,
                          const std::string theName,
                          int& theShapeIndex,
                          int& theTag);

//=================================================================================================
void FeaturesPlugin_CompositeSketch::initCompositeSketchAttribtues(const int theInitFlags)
{
  // Initialize sketch launcher.
  if(theInitFlags & InitSketchLauncher) {
    data()->addAttribute(SKETCH_ID(), ModelAPI_AttributeReference::typeId());
    ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), SKETCH_ID());
  }

  // Initialize selection list.
  if(theInitFlags & InitBaseObjectsList) {
    data()->addAttribute(BASE_OBJECTS_ID(), ModelAPI_AttributeSelectionList::typeId());
  }
}

//=================================================================================================
std::shared_ptr<ModelAPI_Feature> FeaturesPlugin_CompositeSketch::addFeature(std::string theID)
{
  FeaturePtr aNew = document()->addFeature(theID, false);
  if(aNew) {
    data()->reference(SKETCH_ID())->setValue(aNew);
  }

  // Set as current also after it becomes sub to set correctly enabled for other sketch subs.
  document()->setCurrentFeature(aNew, false);
  return aNew;
}

//=================================================================================================
int FeaturesPlugin_CompositeSketch::numberOfSubs(bool forTree) const
{
  ObjectPtr aObj = data()->reference(SKETCH_ID())->value();
  return aObj.get() ? 1 : 0;
}

//=================================================================================================
std::shared_ptr<ModelAPI_Feature> FeaturesPlugin_CompositeSketch::subFeature(const int theIndex, bool forTree)
{
  if(theIndex == 0) {
    return std::dynamic_pointer_cast<ModelAPI_Feature>(data()->reference(SKETCH_ID())->value());
  }

  return std::shared_ptr<ModelAPI_Feature>();
}

//=================================================================================================
int FeaturesPlugin_CompositeSketch::subFeatureId(const int theIndex) const
{
  if(theIndex == 0) {
    FeaturePtr aFeature =
      std::dynamic_pointer_cast<ModelAPI_Feature>(data()->reference(SKETCH_ID())->value());
    if(aFeature.get()) {
      return aFeature->data()->featureId();
    }
  }

  return -1;
}

//=================================================================================================
bool FeaturesPlugin_CompositeSketch::isSub(ObjectPtr theObject) const
{
  // Check is this feature of result
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theObject);
  if(!aFeature.get()) {
    return false;
  }

  ObjectPtr aSub = data()->reference(SKETCH_ID())->value();
  return aSub == theObject;
}

//=================================================================================================
void FeaturesPlugin_CompositeSketch::removeFeature(std::shared_ptr<ModelAPI_Feature> theFeature)
{
  AttributeSelectionListPtr aBaseObjectsSelectionList = selectionList(BASE_OBJECTS_ID());
  if(aBaseObjectsSelectionList.get() && aBaseObjectsSelectionList->size() > 0) {
    aBaseObjectsSelectionList->clear();
  }

  reference(SKETCH_ID())->setValue(ObjectPtr());
}

//=================================================================================================
void FeaturesPlugin_CompositeSketch::getBaseShapes(ListOfShape& theBaseShapesList,
                                                   const bool theIsMakeShells)
{
  theBaseShapesList.clear();

  ListOfShape aBaseFacesList;
  std::map<ResultConstructionPtr, ListOfShape> aSketchWiresMap;
  AttributeSelectionListPtr aBaseObjectsSelectionList = selectionList(BASE_OBJECTS_ID());
  if(!aBaseObjectsSelectionList.get()) {
    setError("Error: Could not get base objects selection list.");
    return;
  }
  if(aBaseObjectsSelectionList->size() == 0) {
    setError("Error: Base objects list is empty.");
    return;
  }
  for(int anIndex = 0; anIndex < aBaseObjectsSelectionList->size(); anIndex++) {
    AttributeSelectionPtr aBaseObjectSelection = aBaseObjectsSelectionList->value(anIndex);
    if(!aBaseObjectSelection.get()) {
      setError("Error: Selected base object is empty.");
      return;
    }
    GeomShapePtr aBaseShape = aBaseObjectSelection->value();
    if(aBaseShape.get() && !aBaseShape->isNull()) {
      GeomAPI_Shape::ShapeType aST = aBaseShape->shapeType();
      if(aST == GeomAPI_Shape::SOLID || aST == GeomAPI_Shape::COMPSOLID) {
        setError("Error: Selected shapes has unsupported type.");
        return;
      }
      ResultConstructionPtr aConstruction =
        std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aBaseObjectSelection->context());
      if(aConstruction.get() && !aBaseShape->isEqual(aConstruction->shape()) && aST == GeomAPI_Shape::WIRE) {
        // It is a wire on the sketch, store it to make face later.
        aSketchWiresMap[aConstruction].push_back(aBaseShape);
        continue;
      } else {
      aST == GeomAPI_Shape::FACE ? aBaseFacesList.push_back(aBaseShape) :
                                   theBaseShapesList.push_back(aBaseShape);
      }
    } else {
      // This may be the whole sketch result selected, check and get faces.
      ResultConstructionPtr aConstruction =
        std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aBaseObjectSelection->context());
      if(!aConstruction.get()) {
        setError("Error: Selected sketches does not have results.");
        return;
      }
      int aFacesNum = aConstruction->facesNum();
      if(aFacesNum == 0) {
        // Probably it can be construction.
        aBaseShape = aConstruction->shape();
        if(aBaseShape.get() && !aBaseShape->isNull()) {
          GeomAPI_Shape::ShapeType aST = aBaseShape->shapeType();
          if(aST != GeomAPI_Shape::VERTEX && aST != GeomAPI_Shape::EDGE && aST != GeomAPI_Shape::WIRE &&
             aST != GeomAPI_Shape::FACE && aST != GeomAPI_Shape::SHELL) {
            setError("Error: Selected shapes has unsupported type.");
            return;
          }
          aST == GeomAPI_Shape::FACE ? aBaseFacesList.push_back(aBaseShape) :
                                       theBaseShapesList.push_back(aBaseShape);
        }
      } else {
        for(int aFaceIndex = 0; aFaceIndex < aFacesNum; aFaceIndex++) {
          GeomShapePtr aBaseFace = aConstruction->face(aFaceIndex);
          if(!aBaseFace.get() || aBaseFace->isNull()) {
            setError("Error: One of the faces on selected sketch is null.");
            return;
          }
          aBaseFacesList.push_back(aBaseFace);
        }
      }
    }
  }

  // Make faces from sketch wires.
  for(std::map<ResultConstructionPtr, ListOfShape>::const_iterator anIt = aSketchWiresMap.cbegin();
      anIt != aSketchWiresMap.cend(); ++anIt) {
    const std::shared_ptr<GeomAPI_PlanarEdges> aSketchPlanarEdges =
      std::dynamic_pointer_cast<GeomAPI_PlanarEdges>((*anIt).first->shape());
    const ListOfShape& aWiresList = (*anIt).second;
    ListOfShape aFaces;
    GeomAlgoAPI_ShapeTools::makeFacesWithHoles(aSketchPlanarEdges->origin(),
                                               aSketchPlanarEdges->norm(),
                                               aWiresList,
                                               aFaces);
    aBaseFacesList.insert(aBaseFacesList.end(), aFaces.begin(), aFaces.end());
  }

  // Searching faces with common edges.
  if(theIsMakeShells) {
    ListOfShape aShells;
    ListOfShape aFreeFaces;
    GeomShapePtr aFacesCompound = GeomAlgoAPI_CompoundBuilder::compound(aBaseFacesList);
    GeomAlgoAPI_ShapeTools::combineShapes(aFacesCompound, GeomAPI_Shape::SHELL, aShells, aFreeFaces);
    theBaseShapesList.insert(theBaseShapesList.end(), aFreeFaces.begin(), aFreeFaces.end());
    theBaseShapesList.insert(theBaseShapesList.end(), aShells.begin(), aShells.end());
  } else {
    theBaseShapesList.insert(theBaseShapesList.end(), aBaseFacesList.begin(), aBaseFacesList.end());
  }
}

//=================================================================================================
bool FeaturesPlugin_CompositeSketch::isMakeShapeValid(const std::shared_ptr<GeomAlgoAPI_MakeShape> theMakeShape)
{
  // Check that algo is done.
  if(!theMakeShape->isDone()) {
    setError("Error: " + getKind() + " algorithm failed.");
    return false;
  }

  // Check if shape is not null.
  if(!theMakeShape->shape().get() || theMakeShape->shape()->isNull()) {
    setError("Error: Resulting shape is null.");
    return false;
  }

  // Check that resulting shape is valid.
  if(!theMakeShape->isValid()) {
    setError("Error: Resulting shape is not valid.");
    return false;
  }

  return true;
}

//=================================================================================================
void FeaturesPlugin_CompositeSketch::storeResult(const GeomShapePtr theBaseShape,
                                                 const std::shared_ptr<GeomAlgoAPI_MakeShape> theMakeShape,
                                                 const int theIndex)
{
  // Create result body.
  ResultBodyPtr aResultBody = document()->createBody(data(), theIndex);

  // Store generated shape.
  aResultBody->storeGenerated(theBaseShape, theMakeShape->shape());

  // Store generated edges/faces.
  int aGenTag = 1;
  storeGenerationHistory(aResultBody, theBaseShape, theMakeShape, aGenTag);

  setResult(aResultBody, theIndex);
}

//=================================================================================================
void FeaturesPlugin_CompositeSketch::storeGenerationHistory(ResultBodyPtr theResultBody,
                                                            const GeomShapePtr theBaseShape,
                                                            const std::shared_ptr<GeomAlgoAPI_MakeShape> theMakeShape,
                                                            int& theTag)
{
  GeomAPI_Shape::ShapeType aBaseShapeType = theBaseShape->shapeType();
  GeomAPI_Shape::ShapeType aShapeTypeToExplode;
  std::string aGenName = "Generated_";

  std::shared_ptr<GeomAPI_DataMapOfShapeShape> aMapOfSubShapes = theMakeShape->mapOfSubShapes();
  switch(aBaseShapeType) {
    case GeomAPI_Shape::VERTEX: {
      aShapeTypeToExplode = GeomAPI_Shape::VERTEX;
      break;
    }
    case GeomAPI_Shape::EDGE:
    case GeomAPI_Shape::WIRE: {
      std::shared_ptr<GeomAPI_Vertex> aV1, aV2;
      GeomAlgoAPI_ShapeTools::findBounds(theBaseShape, aV1, aV2);
      ListOfShape aV1History, aV2History;
      theMakeShape->generated(aV1, aV1History);
      theMakeShape->generated(aV2, aV2History);
      if(!aV1History.empty()) {
        theResultBody->generated(aV1, aV1History.front(), aGenName + "Edge_1", theTag++);
      }
      if(!aV2History.empty()) {
        theResultBody->generated(aV2, aV2History.front(), aGenName + "Edge_2", theTag++);
      }
    }
    case GeomAPI_Shape::FACE:
    case GeomAPI_Shape::SHELL: {
      aShapeTypeToExplode = GeomAPI_Shape::EDGE;
      break;
    }
    case GeomAPI_Shape::COMPOUND: {
      aShapeTypeToExplode = GeomAPI_Shape::COMPOUND;
    }
  }

  if(aShapeTypeToExplode == GeomAPI_Shape::VERTEX || aShapeTypeToExplode == GeomAPI_Shape::COMPOUND) {
    theResultBody->loadAndOrientGeneratedShapes(theMakeShape.get(), theBaseShape, GeomAPI_Shape::VERTEX,
                                                theTag++, aGenName + "Edge", *aMapOfSubShapes.get());
  }
  if(aShapeTypeToExplode == GeomAPI_Shape::EDGE || aShapeTypeToExplode == GeomAPI_Shape::COMPOUND) {
    theResultBody->loadAndOrientGeneratedShapes(theMakeShape.get(), theBaseShape, GeomAPI_Shape::EDGE,
                                                theTag++, aGenName + "Face", *aMapOfSubShapes.get());
  }

  std::shared_ptr<GeomAlgoAPI_MakeSweep> aMakeSweep = std::dynamic_pointer_cast<GeomAlgoAPI_MakeSweep>(theMakeShape);
  if(aMakeSweep.get()) {
    // Store from shapes.
    storeShapes(theResultBody, aBaseShapeType, aMapOfSubShapes, aMakeSweep->fromShapes(), "From_", theTag);

    // Store to shapes.
    storeShapes(theResultBody, aBaseShapeType, aMapOfSubShapes, aMakeSweep->toShapes(), "To_", theTag);
  }
}

//=================================================================================================
void FeaturesPlugin_CompositeSketch::storeShapes(ResultBodyPtr theResultBody,
                                                 const GeomAPI_Shape::ShapeType theBaseShapeType,
                                                 const std::shared_ptr<GeomAPI_DataMapOfShapeShape> theMapOfSubShapes,
                                                 const ListOfShape& theShapes,
                                                 const std::string theName,
                                                 int& theTag)
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
  }

  // Store shapes.
  int aShapeIndex = 1;
  int aFaceIndex = 1;
  for(ListOfShape::const_iterator anIt = theShapes.cbegin(); anIt != theShapes.cend(); ++anIt) {
    GeomShapePtr aShape = *anIt;

    if(aShapeTypeToExplore == GeomAPI_Shape::COMPOUND) {
      std::string aName = theName + (aShape->shapeType() == GeomAPI_Shape::EDGE ? "Edge" : "Face");
      storeSubShape(theResultBody,
                    aShape,
                    aShape->shapeType(),
                    theMapOfSubShapes,
                    aName,
                    aShape->shapeType() == GeomAPI_Shape::EDGE ? aShapeIndex : aFaceIndex,
                    theTag);
    } else {
      std::string aName = theName + aShapeTypeStr;
      storeSubShape(theResultBody, aShape, aShapeTypeToExplore,
                    theMapOfSubShapes, aName, aShapeIndex, theTag);
    }
  }
}

void storeSubShape(ResultBodyPtr theResultBody,
                   const GeomShapePtr theShape,
                   const GeomAPI_Shape::ShapeType theType,
                   const std::shared_ptr<GeomAPI_DataMapOfShapeShape> theMapOfSubShapes,
                   const std::string theName,
                   int& theShapeIndex,
                   int& theTag)
{
  for(GeomAPI_ShapeExplorer anExp(theShape, theType); anExp.more(); anExp.next()) {
    GeomShapePtr aSubShape = anExp.current();
    if(theMapOfSubShapes->isBound(aSubShape)) {
      aSubShape = theMapOfSubShapes->find(aSubShape);
    }
    std::ostringstream aStr;
    aStr << theName << "_" << theShapeIndex++;
    theResultBody->generated(aSubShape, aStr.str(), theTag++);
  }
}
