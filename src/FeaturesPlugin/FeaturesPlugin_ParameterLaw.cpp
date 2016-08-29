// Copyright

// File:        FeaturesPlugin_ParameterLaw.cpp


#include "FeaturesPlugin_ParameterLaw.h"
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
#include <ModelAPI_AttributeBoolean.h>
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

//=================================================================================================
FeaturesPlugin_ParameterLaw::FeaturesPlugin_ParameterLaw()
{
}

//=================================================================================================
void FeaturesPlugin_ParameterLaw::initAttributes()
{
	data()->addAttribute(LAW_REFERENCE_ID(), ModelAPI_AttributeSelection::typeId());
	data()->addAttribute(LAW_DEFINITION_ID(), ModelAPI_AttributeSelection::typeId());
	data()->addAttribute(VARIABLE_ID(), ModelAPI_AttributeString::typeId());

	data()->addAttribute(REVERSE_ID(), ModelAPI_AttributeBoolean::typeId());

	data()->addAttribute(CREATION_METHOD(), ModelAPI_AttributeString::typeId());
}

//=================================================================================================
void FeaturesPlugin_ParameterLaw::execute()
{
	// Getting Ref and Def
	std::shared_ptr<GeomAPI_Shape> aRef;
	std::shared_ptr<GeomAPI_Shape> aDef;
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

	// Flags for reverse path
    bool isReverse = boolean(REVERSE_ID())->value();

	GeomAlgoAPI_ParameterLaw aParameterLawAlgo = GeomAlgoAPI_ParameterLaw(aDef, aRef, isReverse);
	int aResultIndex = 0;

	if(!aParameterLawAlgo.isDone()) {
		static const std::string aPrismAlgoError = "Error: Param algo failed.";
		setError(aPrismAlgoError);
		aResultIndex = 0;
		//break;
	}
	// Check if shape is valid
	if(!aParameterLawAlgo.shape().get() || aParameterLawAlgo.shape()->isNull()) {
		static const std::string aShapeError = "Error: Resulting shape is Null.";
		setError(aShapeError);
		aResultIndex = 0;
		return;
	}
	if(!aParameterLawAlgo.isValid()) {
		std::string aPrismAlgoError = "Error: Resulting shape is not valid.";
		setError(aPrismAlgoError);
		aResultIndex = 0;
		//break;
	}

	ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);
	
	loadNamingDS(aParameterLawAlgo, aResultBody);
	setResult(aResultBody, aResultIndex);
	aResultIndex++;
	removeResults(aResultIndex);
}

void FeaturesPlugin_ParameterLaw::loadNamingDS(GeomAlgoAPI_ParameterLaw& theParameterLawAlgo,
                    std::shared_ptr<ModelAPI_ResultBody> theResultBody)
{
	theResultBody->store(theParameterLawAlgo.shape());
	
	int aIndex = 0, aTag = 0;
	const std::string aName = "Law";
	std::ostringstream aStr;
	aStr << aName << "_" << aIndex++;
	theResultBody->generated(theParameterLawAlgo.shape(), aStr.str(), aTag++);
}