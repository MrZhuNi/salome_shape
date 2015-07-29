// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesPlugin_Extrusion.cpp
// Created:     30 May 2014
// Author:      Vitaly SMETANNIKOV

#include <FeaturesPlugin_Extrusion.h>

#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_ResultCompSolid.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeReference.h>

#include <GeomAlgoAPI_Prism.h>

#define _LATERAL_TAG 1
#define _FIRST_TAG 2
#define _LAST_TAG 3
#define EDGE 6

//#define DEBUG_COMPSOLID

//=================================================================================================
FeaturesPlugin_Extrusion::FeaturesPlugin_Extrusion()
{
}

//=================================================================================================
void FeaturesPlugin_Extrusion::initAttributes()
{
  AttributeSelectionListPtr aSelection = 
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(data()->addAttribute(
    LIST_ID(), ModelAPI_AttributeSelectionList::typeId()));
  // extrusion works with faces always
  aSelection->setSelectionType("FACE");

  data()->addAttribute(CREATION_METHOD(), ModelAPI_AttributeString::typeId());

  data()->addAttribute(TO_SIZE_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(FROM_SIZE_ID(), ModelAPI_AttributeDouble::typeId());

  data()->addAttribute(TO_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(TO_OFFSET_ID(), ModelAPI_AttributeDouble::typeId());

  data()->addAttribute(FROM_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(FROM_OFFSET_ID(), ModelAPI_AttributeDouble::typeId());

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), TO_OBJECT_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), FROM_OBJECT_ID());
}

//=================================================================================================
void FeaturesPlugin_Extrusion::execute()
{
  AttributeSelectionListPtr aFaceRefs = selectionList(LIST_ID());

  // Getting sizes.
  double aToSize = 0.0;
  double aFromSize = 0.0;

  if(string(CREATION_METHOD())->value() == "BySizes") {
    aToSize = real(TO_SIZE_ID())->value();
    aFromSize =  real(FROM_SIZE_ID())->value();
  } else {
    aToSize = real(TO_OFFSET_ID())->value();
    aFromSize =  real(FROM_OFFSET_ID())->value();
  }

  // Getting bounding planes.
  std::shared_ptr<GeomAPI_Shape> aToShape;
  std::shared_ptr<GeomAPI_Shape> aFromShape;

  if(string(CREATION_METHOD())->value() == "ByPlanesAndOffsets") {
    std::shared_ptr<ModelAPI_AttributeSelection> anObjRef = selection(TO_OBJECT_ID());
    if(anObjRef.get() != NULL) {
      aToShape = std::dynamic_pointer_cast<GeomAPI_Shape>(anObjRef->value());
      if(aToShape.get() == NULL && anObjRef->context().get() != NULL) {
        aToShape =  anObjRef->context()->shape();
      }
    }
    anObjRef = selection(FROM_OBJECT_ID());
    if(anObjRef.get() != NULL) {
      aFromShape = std::dynamic_pointer_cast<GeomAPI_Shape>(anObjRef->value());
      if(aFromShape.get() == NULL && anObjRef->context().get() != NULL) {
        aFromShape = anObjRef->context()->shape();
      }
    }
  }

  // for each selected face generate a result
  int anIndex = 0, aResultIndex = 0;

  for(; anIndex < aFaceRefs->size(); anIndex++) {
    std::shared_ptr<ModelAPI_AttributeSelection> aFaceRef = aFaceRefs->value(anIndex);
    ResultPtr aContextRes = aFaceRef->context();
    std::shared_ptr<GeomAPI_Shape> aContext = aContextRes->shape();
    if (!aContext.get()) {
      static const std::string aContextError = "The selection context is bad";
      setError(aContextError);
      break;
    }

    std::shared_ptr<GeomAPI_Shape> aValueFace = aFaceRef->value();
    int aFacesNum = -1; // this mean that "aFace" is used
    ResultConstructionPtr aConstruction =
      std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aContextRes);
    if (!aValueFace.get()) { // this may be the whole sketch result selected, check and get faces
      if (aConstruction.get()) {
        aFacesNum = aConstruction->facesNum();
      } else {
        static const std::string aFaceError = "Can not find basis for extrusion";
        setError(aFaceError);
        break;
      }
    }

#ifdef DEBUG_COMPSOLID
    ResultCompSolidPtr aCompSolidResult = document()->createCompSolid(data(), aResultIndex);
    setResult(aCompSolidResult, aResultIndex);
    aResultIndex++;
#endif
    for(int aFaceIndex = 0; aFaceIndex < aFacesNum || aFacesNum == -1; aFaceIndex++) {
#ifdef DEBUG_COMPSOLID
      ResultBodyPtr aResultBody = aCompSolidResult->addResult(aResultIndex);
#else
      ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);
#endif
      std::shared_ptr<GeomAPI_Shape> aBaseShape;
      if (aFacesNum == -1) {
        aBaseShape = aValueFace;
      } else {
        aBaseShape = std::dynamic_pointer_cast<GeomAPI_Shape>(aConstruction->face(aFaceIndex));
      }

      GeomAlgoAPI_Prism aFeature(aBaseShape, aToShape, aToSize, aFromShape, aFromSize);
      if(!aFeature.isDone()) {
        static const std::string aFeatureError = "Extrusion algorithm failed";
        setError(aFeatureError);
        break;
      }

      // Check if shape is valid
      if(!aFeature.shape().get() || aFeature.shape()->isNull()) {
        static const std::string aShapeError = "Resulting shape is Null";
        setError(aShapeError);
        break;
      }
      if(!aFeature.isValid()) {
        std::string aFeatureError = "Warning: resulting shape is not valid";
        setError(aFeatureError);
        break;
      }
      //LoadNamingDS
//#ifdef DEBUG_COMPSOLID
//#else
      LoadNamingDS(aFeature, aResultBody, aBaseShape, aContext);
//#endif
      setResult(aResultBody, aResultIndex);
      aResultIndex++;

      if (aFacesNum == -1)
        break;
    }
  }
  // remove the rest results if there were produced in the previous pass
  removeResults(aResultIndex);
}

//=================================================================================================
void FeaturesPlugin_Extrusion::LoadNamingDS(GeomAlgoAPI_Prism& theFeature,
                                            std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                                            std::shared_ptr<GeomAPI_Shape> theBasis,
                                            std::shared_ptr<GeomAPI_Shape> theContext)
{
  //load result
  if(theBasis->isEqual(theContext))
    theResultBody->store(theFeature.shape());
  else
    theResultBody->storeGenerated(theBasis, theFeature.shape());

  std::shared_ptr<GeomAPI_DataMapOfShapeShape> aSubShapes = theFeature.mapOfShapes();

  //Insert lateral face : Face from Edge
  std::string aLatName = "LateralFace";
  theResultBody->loadAndOrientGeneratedShapes(theFeature.makeShape().get(), theBasis, EDGE,_LATERAL_TAG, aLatName, *aSubShapes);

  //Insert bottom face
  std::string aBotName = "BottomFace";
  std::shared_ptr<GeomAPI_Shape> aBottomFace = theFeature.firstShape();
  if(!aBottomFace->isNull()) {
    if(aSubShapes->isBound(aBottomFace)) {
      aBottomFace = aSubShapes->find(aBottomFace);
    }
    theResultBody->generated(aBottomFace, aBotName, _FIRST_TAG);
  }

  //Insert top face
  std::string aTopName = "TopFace";
  std::shared_ptr<GeomAPI_Shape> aTopFace = theFeature.lastShape();
  if (!aTopFace->isNull()) {
    if (aSubShapes->isBound(aTopFace)) {
      aTopFace = aSubShapes->find(aTopFace);
    }
    theResultBody->generated(aTopFace, aTopName, _LAST_TAG);
  }
}
