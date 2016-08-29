// Copyright

// File:        FeaturesPlugin_PipeApprox.cpp


#include "FeaturesPlugin_PipeApprox.h"

#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_Attribute.h>
#include <ModelAPI_Object.h>
#include <Events_Loop.h>
#include <ModelAPI_Events.h>
//??#include "ParametersPlugin_Parameter.h"
//??#include <SketchPlugin_SketchEntity.h>
#include <ModelAPI_AttributeDouble.h>
//#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_BodyBuilder.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_ResultBody.h>

#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Lin.h>

#include <sstream>
#include <string> 

namespace patch
{
	template < typename T > std::string to_string( const T& n )
	{
		std::ostringstream stm ;
		stm << n ;
		return stm.str() ;
	}
}

//static ListOfShape makeProfileList(ObjectPtr theProfileObjRef);
static ListOfShape makeProfileList(AttributeSelectionListPtr aFacesSelectionList);
//=================================================================================================
FeaturesPlugin_PipeApprox::FeaturesPlugin_PipeApprox()
{
}

//=================================================================================================
void FeaturesPlugin_PipeApprox::initAttributes()
{
	AttributeSelectionListPtr aSelection = 
		std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(data()->addAttribute(
		PROFILE_ID(), ModelAPI_AttributeSelectionList::typeId()));
	aSelection->setSelectionType("FACE");


	data()->addAttribute(PATH_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
	//data()->addAttribute(PATH_OBJECT_ID(), ModelAPI_AttributeSelectionList::typeId());
	data()->addAttribute(BINORMAL_ID(), ModelAPI_AttributeSelection::typeId());

	data()->addAttribute(LAW_REFERENCE_ID(), ModelAPI_AttributeSelection::typeId());
	data()->addAttribute(LAW_DEFINITION_ID(), ModelAPI_AttributeSelection::typeId());

	data()->addAttribute(CREATION_METHOD(), ModelAPI_AttributeString::typeId());

}

//=================================================================================================
void FeaturesPlugin_PipeApprox::execute()
{
	// Getting creation method.
	std::string aCreationMethod = string(CREATION_METHOD())->value();

	//Getting path 
	//limit to edge

	std::shared_ptr<GeomAPI_Edge> aPath;
	std::shared_ptr<ModelAPI_AttributeSelection> anObjRef = selection(PATH_OBJECT_ID());

	if(anObjRef && anObjRef->value() && anObjRef->value()->isEdge()) {
		aPath = std::shared_ptr<GeomAPI_Edge>(new GeomAPI_Edge(anObjRef->value()));
	} else if(anObjRef->context() && anObjRef->context()->shape() && anObjRef->context()->shape()->isEdge()) {
		aPath = std::shared_ptr<GeomAPI_Edge>(new GeomAPI_Edge(anObjRef->context()->shape()));
	}
	/*
	//list of edge
	ListOfShape aEdgesList;
	AttributeSelectionListPtr aEdgesSelectionList = selectionList(PATH_OBJECT_ID());
	for(int anIndex = 0; anIndex < aEdgesSelectionList->size(); anIndex++) {
	AttributeSelectionPtr aEdgeSel = aEdgesSelectionList->value(anIndex);
	std::shared_ptr<GeomAPI_Shape> aEdgeShape = aEdgeSel->value();
	if(aEdgeShape.get() && !aEdgeShape->isNull()) { 
	aEdgesList.push_back(aEdgeShape);
	}
	}
	*/

	//Getting profile
	ListOfShape aFacesList;
	AttributeSelectionListPtr aFacesSelectionList = selectionList(PROFILE_ID());
	for(int anIndex = 0; anIndex < aFacesSelectionList->size(); anIndex++) {
		AttributeSelectionPtr aFaceSel = aFacesSelectionList->value(anIndex);
		std::shared_ptr<GeomAPI_Shape> aFaceShape = aFaceSel->value();
		if(aFaceShape.get() && !aFaceShape->isNull()) { // Getting face.
			aFacesList.push_back(aFaceShape);
		} else { // This may be the whole sketch result selected, check and get faces.
			ResultPtr aContext = aFaceSel->context();
			std::shared_ptr<GeomAPI_Shape> aContextShape = aContext->shape();
			if(!aContextShape.get()) {
				static const std::string aContextError = "Error: The selection context is bad.";
				setError(aContextError);
				return;
			}
			ResultConstructionPtr aConstruction = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aContext);
			if(!aConstruction.get()) {
				static const std::string aFaceError = "Error: Can not find basis for extrusion.";
				setError(aFaceError);
				return;
			}
			int aFacesNum = aConstruction->facesNum();
			for(int aFaceIndex = 0; aFaceIndex < aFacesNum || aFacesNum == -1; aFaceIndex++) {
				aFaceShape = std::dynamic_pointer_cast<GeomAPI_Shape>(aConstruction->face(aFaceIndex));
				aFacesList.push_back(aFaceShape);
			}
		}
	}

	// Getting Bi-Normal
	std::shared_ptr<GeomAPI_Shape> aBiNormal;
	if(aCreationMethod == "binormal") {
		AttributeSelectionPtr aBiNormalSelection = selection(BINORMAL_ID());
		if(!aBiNormalSelection.get()) {
			setError("Error: Bi-Normal selection is empty.");
			return;
		}
		aBiNormal = std::dynamic_pointer_cast<GeomAPI_Shape>(aBiNormalSelection->value());
		if(!aBiNormal.get()) {
			// Probably it is a construction.
			aBiNormal = aBiNormalSelection->context()->shape();
		}
		if(!aBiNormal.get() || aBiNormal->isNull()) {
			setError("Error: Bi-Normal shape is null.");
			return;
		}
	}

	// Getting Ref and Def
	std::shared_ptr<GeomAPI_Shape> aRef;
	std::shared_ptr<GeomAPI_Shape> aDef;
	if(aCreationMethod == "with law") {
		//REFERENCE
		AttributeSelectionPtr aRefSelection = selection(LAW_REFERENCE_ID());
		if(!aRefSelection.get()) {
			setError("Error: Ref selection is empty.");
			return;
		}
		aRef = std::dynamic_pointer_cast<GeomAPI_Shape>(aRefSelection->value());
		if(!aRef.get()) {
			// Probably it is a construction.
			aRef = aRefSelection->context()->shape();
		}
		if(!aRef.get() || aRef->isNull()) {
			setError("Error: Ref shape is null.");
			return;
		}
		//DEFINITION
		AttributeSelectionPtr aDefSelection = selection(LAW_DEFINITION_ID());
		if(!aDefSelection.get()) {
			setError("Error: Def selection is empty.");
			return;
		}
		aDef = std::dynamic_pointer_cast<GeomAPI_Shape>(aDefSelection->value());
		if(!aDef.get()) {
			// Probably it is a construction.
			aDef = aDefSelection->context()->shape();
		}
		if(!aDef.get() || aDef->isNull()) {
			setError("Error: Def shape is null.");
			return;
		}
	}

	// Generating result for each shell and face.
	int aResultIndex = 0;
	if(aCreationMethod == "simple" || aCreationMethod == "binormal" 
		|| aCreationMethod == "with law") {
			for(ListOfShape::const_iterator anIter = aFacesList.cbegin(); anIter != aFacesList.cend(); anIter++) {
				std::shared_ptr<GeomAPI_Shape> aBaseShape = *anIter;
				//std::shared_ptr<GeomAPI_Shape> aBaseShape = aFaceShape;
				GeomAlgoAPI_PipeApprox aPipeApproxAlgo = GeomAlgoAPI_PipeApprox();
				if(aCreationMethod == "simple"){
					aPipeApproxAlgo = GeomAlgoAPI_PipeApprox(aBaseShape, aPath);
				}else if(aCreationMethod == "binormal"){
					aPipeApproxAlgo = GeomAlgoAPI_PipeApprox(aBaseShape, aPath, aBiNormal);
				}else if(aCreationMethod == "with law"){
					aPipeApproxAlgo = GeomAlgoAPI_PipeApprox(aBaseShape, aPath, aDef, aRef);
				}


				/*
				GeomAlgoAPI_PipeApprox aPipeApproxAlgo = aCreationMethod == "simple" ? GeomAlgoAPI_PipeApprox(aBaseShape, aEdgesList) :
				GeomAlgoAPI_PipeApprox(aBaseShape, aEdgesList, aBiNormal);
				*/
				if(!aPipeApproxAlgo.isDone()) {
					static const std::string aPrismAlgoError = "Error: Pipe algorithm failed.";
					setError(aPrismAlgoError);
					aResultIndex = 0;
					//break;
				}
				// Check if shape is valid
				if(!aPipeApproxAlgo.shape().get() || aPipeApproxAlgo.shape()->isNull()) {
					static const std::string aShapeError = "Error: Resulting shape is Null.";
					setError(aShapeError);
					aResultIndex = 0;
					//break;
				}
				if(!aPipeApproxAlgo.isValid()) {
					std::string aPrismAlgoError = "Error: Resulting shape is not valid.";
					setError(aPrismAlgoError);
					aResultIndex = 0;
					//break;
				}

				ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);
				loadNamingDS(aPipeApproxAlgo, aResultBody, aBaseShape);
				setResult(aResultBody, aResultIndex);
				aResultIndex++;
			}
	}
	removeResults(aResultIndex);
}

//=================================================================================================
void FeaturesPlugin_PipeApprox::loadNamingDS(GeomAlgoAPI_PipeApprox& thePipeApproxAlgo,
	std::shared_ptr<ModelAPI_ResultBody> theResultBody,
	std::shared_ptr<GeomAPI_Shape> theBasis)
{
	//load result
	theResultBody->storeGenerated(theBasis, thePipeApproxAlgo.shape());

	std::shared_ptr<GeomAPI_DataMapOfShapeShape> aSubShapes = thePipeApproxAlgo.mapOfSubShapes();

	//Insert lateral face : Face from Edge
	const std::string aLatName = "LateralFace";
	const int aLatTag = 1;
	theResultBody->loadAndOrientGeneratedShapes(&thePipeApproxAlgo, theBasis, GeomAPI_Shape::EDGE, aLatTag, aLatName, *aSubShapes);

	//Insert to faces
	int aToFaceIndex = 1;
	const std::string aToName = "ToFace";
	int aToTag = 2;
	const ListOfShape& aToFaces = thePipeApproxAlgo.toShapes();
	for(ListOfShape::const_iterator anIt = aToFaces.cbegin(); anIt != aToFaces.cend(); anIt++) {
		std::shared_ptr<GeomAPI_Shape> aToFace = *anIt;
		if(aSubShapes->isBound(aToFace)) {
			aToFace = aSubShapes->find(aToFace);
		}
		std::ostringstream aStr;
		aStr << aToName << "_" << aToFaceIndex++;
		theResultBody->generated(aToFace, aStr.str(), aToTag++);
	}

	//Insert from faces
	int aFromFaceIndex = 1;
	const std::string aFromName = "FromFace";
	int aFromTag = aToTag > 10000 ? aToTag : 10000;
	const ListOfShape& aFromFaces = thePipeApproxAlgo.fromShapes();
	for(ListOfShape::const_iterator anIt = aFromFaces.cbegin(); anIt != aFromFaces.cend(); anIt++) {
		std::shared_ptr<GeomAPI_Shape> aFromFace = *anIt;
		if(aSubShapes->isBound(aFromFace)) {
			aFromFace = aSubShapes->find(aFromFace);
		}
		std::ostringstream aStr;
		aStr << aFromName << "_" << aFromFaceIndex++;
		theResultBody->generated(aFromFace, aStr.str(), aFromTag++);
	}
}



//ListOfShape makeProfileList(ObjectPtr theProfileObjRef){
ListOfShape makeProfileList(AttributeSelectionListPtr aFacesSelectionList){
	ListOfShape aFacesList;
	//AttributeSelectionListPtr aFacesSelectionList = theProfileObjRef->data()->selectionList("profile");
	for(int anIndex = 0; anIndex < aFacesSelectionList->size(); anIndex++) {
		AttributeSelectionPtr aFaceSel = aFacesSelectionList->value(anIndex);
		std::shared_ptr<GeomAPI_Shape> aFaceShape = aFaceSel->value();
		if(aFaceShape.get() && !aFaceShape->isNull()) { // Getting face.
			aFacesList.push_back(aFaceShape);
		} else { // This may be the whole sketch result selected, check and get faces.
			ResultPtr aContext = aFaceSel->context();
			std::shared_ptr<GeomAPI_Shape> aContextShape = aContext->shape();
			if(!aContextShape.get()) {
				static const std::string aContextError = "Error: The selection context is bad.";
				return aFacesList;
			}
			ResultConstructionPtr aConstruction = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aContext);
			if(!aConstruction.get()) {
				static const std::string aFaceError = "Error: Can not find basis for extrusion.";
				return aFacesList;
			}
			int aFacesNum = aConstruction->facesNum();
			for(int aFaceIndex = 0; aFaceIndex < aFacesNum || aFacesNum == -1; aFaceIndex++) {
				aFaceShape = std::dynamic_pointer_cast<GeomAPI_Shape>(aConstruction->face(aFaceIndex));
				aFacesList.push_back(aFaceShape);
			}
		}
	}

	return aFacesList;
}
