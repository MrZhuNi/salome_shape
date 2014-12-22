// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        TemplatePlugin_TemplateFeature.cpp
// Created:     30 May 2014
// Author:      Vitaly SMETANNIKOV

#include "TemplatePlugin_TemplateFeature.h"
#include <ModelAPI_Session.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeBoolean.h>
#include <GeomAlgoAPI_Extrusion.h>

using namespace std;
#define _LATERAL_TAG 1
#define _FIRST_TAG 2
#define _LAST_TAG 3
#define EDGE 6

TemplatePlugin_TemplateFeature::TemplatePlugin_TemplateFeature(const std::string& theId)
{
  myId = theId;
}

void TemplatePlugin_TemplateFeature::initAttributes()
{
  data()->addAttribute(TemplatePlugin_TemplateFeature::FACE_ID(), ModelAPI_AttributeSelection::type());
  data()->addAttribute(TemplatePlugin_TemplateFeature::SIZE_ID(), ModelAPI_AttributeDouble::type());
  data()->addAttribute(TemplatePlugin_TemplateFeature::REVERSE_ID(), ModelAPI_AttributeBoolean::type());
}

void TemplatePlugin_TemplateFeature::execute()
{

}

//============================================================================
void TemplatePlugin_TemplateFeature::LoadNamingDS(GeomAlgoAPI_Extrusion& theFeature,
  std::shared_ptr<ModelAPI_ResultBody> theResultBody, 
  std::shared_ptr<GeomAPI_Shape> theBasis,
  std::shared_ptr<GeomAPI_Shape> theContext)
{  


  //load result
  if(theBasis->isEqual(theContext))
    theResultBody->store(theFeature.shape());
  else
    theResultBody->storeGenerated(theContext, theFeature.shape()); 

  GeomAPI_DataMapOfShapeShape* aSubShapes = new GeomAPI_DataMapOfShapeShape();
  theFeature.mapOfShapes(*aSubShapes);

    //Insert lateral face : Face from Edge
  theResultBody->loadAndOrientGeneratedShapes(theFeature.makeShape(), theBasis, EDGE,_LATERAL_TAG, *aSubShapes);

  //Insert bottom face
  std::shared_ptr<GeomAPI_Shape> aBottomFace = theFeature.firstShape();  
  if (!aBottomFace->isNull()) {
	if (aSubShapes->isBound(aBottomFace)) {	 
		aBottomFace = aSubShapes->find(aBottomFace);		
    }    
    theResultBody->generated(aBottomFace, _FIRST_TAG);
  }



  //Insert top face
  std::shared_ptr<GeomAPI_Shape> aTopFace = theFeature.lastShape();
  if (!aTopFace->isNull()) {
    if (aSubShapes->isBound(aTopFace)) {	 
      aTopFace = aSubShapes->find(aTopFace);	
    }
    theResultBody->generated(aTopFace, _LAST_TAG);
  }

  
}
