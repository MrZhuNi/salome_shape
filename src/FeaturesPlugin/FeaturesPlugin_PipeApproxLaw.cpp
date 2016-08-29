// Copyright

// File:        FeaturesPlugin_PipeApproxLaw.cpp

//TODO: SORT ALL THE INCLUDES
#include "FeaturesPlugin_PipeApproxLaw.h"

#include <FeaturesPlugin_ParameterLaw.h>
#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_Attribute.h>
#include <ModelAPI_Object.h>
#include <Events_Loop.h>
#include <ModelAPI_Events.h>
//??#include "ParametersPlugin_Parameter.h"
//??#include <SketchPlugin_SketchEntity.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelectionList.h>
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

namespace patch
{
	template < typename T > std::string to_string( const T& n )
	{
		std::ostringstream stm ;
		stm << n ;
		return stm.str() ;
	}
}


static ListOfShape makeProfileList(AttributeSelectionListPtr aFacesSelectionList);


//for splitting string
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}


//=================================================================================================
FeaturesPlugin_PipeApproxLaw::FeaturesPlugin_PipeApproxLaw()
{
}

//=================================================================================================
void FeaturesPlugin_PipeApproxLaw::initAttributes()
{
	mySelection = 
		std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(data()->addAttribute(
		PROFILE_ID(), ModelAPI_AttributeSelectionList::typeId()));
	mySelection->setSelectionType("FACE");
	mySelection->setImmutable(false);

	data()->addAttribute(PATH_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
	data()->addAttribute(BINORMAL_ID(), ModelAPI_AttributeSelection::typeId());


	myParameter = data()->addAttribute(VARIABLE_ID(), ModelAPI_AttributeSelectionList::typeId());
	data()->addAttribute(NBSECT_ID(), ModelAPI_AttributeInteger::typeId());
	data()->addAttribute(REVERSE_ID(), ModelAPI_AttributeBoolean::typeId());
	data()->addAttribute(FRENET_ID(), ModelAPI_AttributeBoolean::typeId());
	data()->addAttribute(CREATION_METHOD(), ModelAPI_AttributeString::typeId());

}

//=================================================================================================
void FeaturesPlugin_PipeApproxLaw::performSketchIteration()
{
	for(int i = 0; i < myNbLaws; i++){ 		
		const std::string & theValue = patch::to_string(myIterationMOI[i][myCPT]); //it -> devient compteur sur le vect
		FeaturePtr aIterationParamFeature = ModelAPI_Feature::feature(myVectResultParameterPtr[i]);
		aIterationParamFeature->data()->string("expression")->setValue(theValue);
	}
}

//=================================================================================================
void FeaturesPlugin_PipeApproxLaw::getParamLaws(GeomAlgoAPI_ParameterLaw& aParameterLawAlgo)
{
	ObjectPtr aFeatureObj = mySelection->owner();
	DocumentPtr aDocument = aFeatureObj->document();

	bool haveParam;

	AttributeSelectionListPtr aLawsSelectionList = selectionList(VARIABLE_ID());
	myNbLaws = aLawsSelectionList->size();

	for(int i = 0; i < myNbLaws; i++){
		AttributeSelectionPtr aFaceSel = aLawsSelectionList->value(i);
		ObjectPtr aLawObj = aFaceSel->context();
		FeaturePtr aLawFeature = ModelAPI_Feature::feature(aLawObj);

		//check existence of the laws
		haveParam = false;

		//get the param, and the def and ref curves
		std::string aParameterName = aLawFeature->data()->string(FeaturesPlugin_ParameterLaw::VARIABLE_ID())->value();

		int anIndex2 = 0, aSize2 = aDocument->size(ModelAPI_ResultParameter::group());
		ObjectPtr aParamObj = aDocument->object(ModelAPI_ResultParameter::group(), anIndex2);

		while(anIndex2 < aSize2 && aParamObj->data()->name() != aParameterName){
			anIndex2++;
			aParamObj = aDocument->object(ModelAPI_ResultParameter::group(), anIndex2);
		}
		if(aParamObj->data()->name() == aParameterName)
		{
			ResultParameterPtr aParam = std::dynamic_pointer_cast<ModelAPI_ResultParameter>(aParamObj);
			if (aParam.get()){
				FeaturePtr aFeature = ModelAPI_Feature::feature(aParam);
				haveParam = (aFeature != myParameter->owner());
			}
		}
		ResultParameterPtr aParam;
		if (haveParam) {
			aParam = std::dynamic_pointer_cast<ModelAPI_ResultParameter>(aParamObj);
			haveParam = aParam.get() != NULL;
		}
		if(!haveParam)
		{
			break;
		}else{
			myVectResultParameterPtr.push_back(aParam);

			bool isReverse = 
				aLawFeature->data()->boolean(FeaturesPlugin_ParameterLaw::REVERSE_ID())->value();
			std::shared_ptr<GeomAPI_Shape> aDef = 
				aLawFeature->data()->selection(FeaturesPlugin_ParameterLaw::LAW_DEFINITION_ID())->value();

			std::shared_ptr<GeomAPI_Shape> aRef = 
				aLawFeature->data()->selection(FeaturesPlugin_ParameterLaw::LAW_REFERENCE_ID())->value();

			std::vector<double> tempVec = aParameterLawAlgo.getVectorOfScalar(aDef, aRef, isReverse, myNbSections);
			myIterationMOI.insert(std::pair<int, std::vector<double>>(i, tempVec));
		}
	}
}

//=================================================================================================
bool FeaturesPlugin_PipeApproxLaw::checkParamLaws()
{
	ObjectPtr aFeatureObj = mySelection->owner();
	DocumentPtr aDocument = aFeatureObj->document();

	bool res;

	AttributeSelectionListPtr aLawsSelectionList = selectionList(VARIABLE_ID());
	myNbLaws = aLawsSelectionList->size();

	for(int i = 0; i < myNbLaws; i++){
		AttributeSelectionPtr aFaceSel = aLawsSelectionList->value(i);

		ObjectPtr aLawObj = aFaceSel->context();
		FeaturePtr aLawFeature = ModelAPI_Feature::feature(aLawObj);
		//check existence of the laws

		res = false;

		std::string aParameterName = aLawFeature->data()->string(FeaturesPlugin_ParameterLaw::VARIABLE_ID())->value();

		int anIndex2 = 0, aSize2 = aDocument->size(ModelAPI_ResultParameter::group());
		ObjectPtr aParamObj = aDocument->object(ModelAPI_ResultParameter::group(), anIndex2);

		while(anIndex2 < aSize2 && aParamObj->data()->name() != aParameterName){
			anIndex2++;
			aParamObj = aDocument->object(ModelAPI_ResultParameter::group(), anIndex2);
		}
		if(aParamObj->data()->name() == aParameterName)
		{
			ResultParameterPtr aParam = std::dynamic_pointer_cast<ModelAPI_ResultParameter>(aParamObj);
			if (aParam.get()){
				FeaturePtr aFeature = ModelAPI_Feature::feature(aParam);

				res = (aFeature != myParameter->owner());
			}
		}
		ResultParameterPtr aParam;

		if (res) {
			aParam = std::dynamic_pointer_cast<ModelAPI_ResultParameter>(aParamObj);

			res = aParam.get() != NULL;
		}
	}
	return res;
}

//=================================================================================================
void FeaturesPlugin_PipeApproxLaw::execute()
{
	int cpt = 0;
	if (!myIterationMOI.empty()) { // iteration is performed
		// do the iteration
		// get the updated sketch

		AttributeSelectionPtr aUpdatedShapeSel = myIterationUpdatedFacesSelectionList->value(cpt);
		std::shared_ptr<GeomAPI_Shape> aUpdatedShape = aUpdatedShapeSel->value();
		myIterationLOS.push_back(aUpdatedShape);

		myCPT--;


		if (myCPT >= 0) {
			// set new param to update sketch
			performSketchIteration();
			return; // the execution will be called again with an updated sketch
		} else {
			myIterationMOI.clear(); // finish the iteration
		}
	}

	// Getting creation method.
	std::string aCreationMethod = string(CREATION_METHOD())->value();

	// Flags for reverse path
	bool isReverse = boolean(REVERSE_ID())->value();

	// Flags for min torsion
	bool withFrenet = boolean(FRENET_ID())->value();

	// Getting nb sections.
	myNbSections = integer(NBSECT_ID())->value();


	// Getting path.
	AttributeSelectionPtr aPathSelection = selection(PATH_OBJECT_ID());
	if(!aPathSelection.get()) {
		setError("Error: Path selection is empty.");
		return;
	}
	std::shared_ptr<GeomAPI_Shape> aPathShape = std::dynamic_pointer_cast<GeomAPI_Shape>(aPathSelection->value());
	if(!aPathShape.get()) {
		// Probaply it is a construction.
		aPathShape = aPathSelection->context()->shape();
	}
	if(!aPathShape.get() || aPathShape->isNull()) {
		setError("Error: Path shape is null.");
		return;
	}

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
	if(aCreationMethod == "with binormal") {
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

	// Generating result for each shell and face.
	int aResultIndex = 0;
	//this is "fake" for loop
	//we expect to only treat one face
	//think I need the selectionlist to have access to sketch, not sure
	for(ListOfShape::const_iterator anIter = aFacesList.cbegin(); anIter != aFacesList.cend(); anIter++) {
		std::shared_ptr<GeomAPI_Shape> aBaseShape = *anIter;
		GeomAlgoAPI_PipeApprox aPipeApproxLawAlgo = GeomAlgoAPI_PipeApprox();
		GeomAlgoAPI_ParameterLaw aParameterLawAlgo = GeomAlgoAPI_ParameterLaw();

		ObjectPtr aFeatureObj = mySelection->owner();
		DocumentPtr aDocument = aFeatureObj->document();
		if(checkParamLaws())
		{
			if (myIterationLOS.empty()) { // starts the iteration
				myCPT = myNbSections - 1;
				getParamLaws(aParameterLawAlgo);
				myIterationUpdatedFacesSelectionList = aFeatureObj->data()->selectionList(PROFILE_ID());
				// init the iteration by first modification of the sketch
				performSketchIteration();
				return;
			} else {  // perform the algo on already computed contours in the previous iteration
				if(aCreationMethod == "simple"){
					aPipeApproxLawAlgo = GeomAlgoAPI_PipeApprox(myIterationLOS, aPathShape, isReverse, myNbSections, withFrenet);
				}
				if(aCreationMethod == "with binormal"){
					aPipeApproxLawAlgo = GeomAlgoAPI_PipeApprox(myIterationLOS, aPathShape, aBiNormal, isReverse, myNbSections/*, crossSections*/);
				}
				myIterationLOS.clear();
				myCPT = myNbSections - 1;
			}
		}


		if(!aPipeApproxLawAlgo.isDone()) {
			static const std::string aPrismAlgoError = "Error: Pipe algorithm failed.";
			setError(aPrismAlgoError);
			aResultIndex = 0;
			//break;
		}
		// Check if shape is valid
		if(!aPipeApproxLawAlgo.shape().get() || aPipeApproxLawAlgo.shape()->isNull()) {
			static const std::string aShapeError = "Error: Resulting shape is Null.";
			setError(aShapeError);
			aResultIndex = 0;
			//break;
		}
		if(!aPipeApproxLawAlgo.isValid()) {
			std::string aPrismAlgoError = "Error: Resulting shape is not valid.";
			setError(aPrismAlgoError);
			aResultIndex = 0;
			//break;
		}

		ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);
		loadNamingDS(aPipeApproxLawAlgo, aResultBody, aBaseShape);
		setResult(aResultBody, aResultIndex);
		aResultIndex++;

		cpt++;
	}

	removeResults(aResultIndex);

}

//=================================================================================================
void FeaturesPlugin_PipeApproxLaw::loadNamingDS(GeomAlgoAPI_PipeApprox& thePipeApproxLawAlgo,
	std::shared_ptr<ModelAPI_ResultBody> theResultBody,
	std::shared_ptr<GeomAPI_Shape> theBasis)
{
	//load result
	theResultBody->storeGenerated(theBasis, thePipeApproxLawAlgo.shape());

	std::shared_ptr<GeomAPI_DataMapOfShapeShape> aSubShapes = thePipeApproxLawAlgo.mapOfSubShapes();

	//Insert lateral face : Face from Edge
	const std::string aLatName = "LateralFace";
	const int aLatTag = 1;
	theResultBody->loadAndOrientGeneratedShapes(&thePipeApproxLawAlgo, theBasis, GeomAPI_Shape::EDGE, aLatTag, aLatName, *aSubShapes);

	//Insert to faces
	int aToFaceIndex = 1;
	const std::string aToName = "ToFace";
	int aToTag = 2;
	const ListOfShape& aToFaces = thePipeApproxLawAlgo.toShapes();
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
	const ListOfShape& aFromFaces = thePipeApproxLawAlgo.fromShapes();
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



ListOfShape makeProfileList(AttributeSelectionListPtr aFacesSelectionList){
	ListOfShape aFacesList;
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
