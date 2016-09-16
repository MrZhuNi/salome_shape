// Copyright (C) 2014-2016 CEA/DEN, EDF R&D

// File:        PrimitivesPlugin_Sphere.cpp
// Created:     29 Mar 2016
// Author:      CEA (delegation to Alyotech)

#include <PrimitivesPlugin_Sphere.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeString.h>

#include <GeomAlgoAPI_PointBuilder.h>

#include <memory>

//=================================================================================================
PrimitivesPlugin_Sphere::PrimitivesPlugin_Sphere() // Nothing to do during instantiation
{
}

//=================================================================================================
void PrimitivesPlugin_Sphere::initAttributes()
{

  // The center of the sphere
  data()->addAttribute(CENTER(), ModelAPI_AttributeSelection::typeId());
  AttributeSelectionPtr aBase = data()->selection(CENTER());
  if (!aBase->isInitialized()) { // on restore (undo, open), this may be already defined
    ObjectPtr aPointObj = ModelAPI_Session::get()->moduleDocument()->objectByName(ModelAPI_ResultConstruction::group(), "Origin");
    if (aPointObj.get()) { // if initialization plugin performed well
      ResultPtr aPointRes = std::dynamic_pointer_cast<ModelAPI_Result>(aPointObj);
      // second argument is null: just point result, without sub-shape selection
      aBase->setValue(aPointRes, std::shared_ptr<GeomAPI_Shape>());
    }
  }
  
  // The radius needed to create a sphere by radius at origin and by the center
  data()->addAttribute(RADIUS(), ModelAPI_AttributeDouble::typeId());
}

//=================================================================================================
void PrimitivesPlugin_Sphere::execute(){

//Get the point and the radius from the input parameters, check their soudness, then instantiate a sphere builder with these elements
  AttributeSelectionPtr aRef = data()->selection(CENTER());
  double aRadius = real(RADIUS())->value();
  
  std::shared_ptr<GeomAPI_Pnt> aCenter = GeomAlgoAPI_PointBuilder::point(aRef->context()->shape());

  std::shared_ptr<GeomAlgoAPI_Sphere> aSphereAlgo;

  if ((aRef.get() != NULL)) {
    GeomShapePtr aShape = aRef->value();
    if (!aShape.get()) //If we can't get the points directly, try getting them from the context
      aShape = aRef->context()->shape();
    if (aShape){
      std::shared_ptr<GeomAPI_Pnt> aCenter = GeomAlgoAPI_PointBuilder::point(aShape);
      aSphereAlgo = std::shared_ptr<GeomAlgoAPI_Sphere>(new GeomAlgoAPI_Sphere(aCenter, aRadius));
    }
  }
    
  // We need to check that everything went smoothly with GeomAlgoAPI_Sphere
  if (!aSphereAlgo->check()){
    setError(aSphereAlgo->getError(), false);
    return;
  }

  // Build the sphere
  aSphereAlgo->build();

  // Check if the creation of the sphere went well
  if(!aSphereAlgo->isDone()) {
    setError(aSphereAlgo->getError());
    return;
  }
  if(!aSphereAlgo->checkValid("Sphere builder")) {
    setError(aSphereAlgo->getError());
    return;
  }

  int aResultIndex = 0; // There's only going to be one result : the sphere
  ResultBodyPtr aResultSphere = document()->createBody(data(), aResultIndex);
  loadNamingDS(aSphereAlgo, aResultSphere); 
  setResult(aResultSphere, aResultIndex);
}

//=================================================================================================
  void PrimitivesPlugin_Sphere::loadNamingDS(std::shared_ptr<GeomAlgoAPI_Sphere> theSphereAlgo,
                                             std::shared_ptr<ModelAPI_ResultBody> theResultSphere)
{
    // Load the result
  theResultSphere->store(theSphereAlgo->shape());
  
  // Prepare the naming
  theSphereAlgo->prepareNamingFaces();
  
  // Insert to faces
  int num = 1;
  std::map< std::string, std::shared_ptr<GeomAPI_Shape> > listOfFaces = theSphereAlgo->getCreatedFaces();
  for (std::map< std::string, std::shared_ptr<GeomAPI_Shape> >::iterator it=listOfFaces.begin(); it!=listOfFaces.end(); ++it){
    std::shared_ptr<GeomAPI_Shape> aFace = (*it).second;
    theResultSphere->generated(aFace, (*it).first, num++);
  }
}
