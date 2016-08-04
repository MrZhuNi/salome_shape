// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_Validator.cpp
// Created:     2 Jul 2014
// Author:      Mikhail PONIKAROV

#include "Model_Validator.h"

#include "Model_AttributeValidator.h"
#include "Model_FeatureValidator.h"

#include <ModelAPI_Attribute.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeValidator.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Result.h>
#include <Model_Data.h>

#include <Events_InfoMessage.h>

void Model_ValidatorsFactory::registerValidator(const std::string& theID,
  ModelAPI_Validator* theValidator)
{
  if (myIDs.find(theID) != myIDs.end()) {
    Events_InfoMessage("Model_Validator", "Validator %1 is already registered").arg(theID).send();
  } else {
    myIDs[theID] = theValidator;
  }
}

void Model_ValidatorsFactory::assignValidator(const std::string& theID,
  const std::string& theFeatureID)
{
  if (myFeatures.find(theFeatureID) == myFeatures.end()) {
    myFeatures[theFeatureID] = AttrValidators();
  }
  if (myFeatures[theFeatureID].find(theID) != myFeatures[theFeatureID].end()) {
    //Events_Error::send(std::string("Validator ") + theID + 
    //  " for feature " + theFeatureID + "is already registered");
  } else {
    myFeatures[theFeatureID][theID] = std::list<std::string>();
  }
}

void Model_ValidatorsFactory::assignValidator(const std::string& theID,
  const std::string& theFeatureID,
  const std::list<std::string>& theArguments)
{
  if (myFeatures.find(theFeatureID) == myFeatures.end()) {
    myFeatures[theFeatureID] = AttrValidators();
  }

  if (myFeatures[theFeatureID].find(theID) != myFeatures[theFeatureID].end()) {
    //Events_Error::send(std::string("Validator ") + theID + 
    //  " for feature " + theFeatureID + "is already registered");
  } else {
    myFeatures[theFeatureID][theID] = theArguments;
  }
}

void Model_ValidatorsFactory::assignValidator(const std::string& theID,
  const std::string& theFeatureID,
  const std::string& theAttrID,
  const std::list<std::string>& theArguments)
{
  // create feature-structures if not exist
  std::map<std::string, std::map<std::string, AttrValidators> >::iterator aFeature = myAttrs.find(
    theFeatureID);
  if (aFeature == myAttrs.end()) {
    myAttrs[theFeatureID] = std::map<std::string, AttrValidators>();
    aFeature = myAttrs.find(theFeatureID);
  }
  // add attr-structure if not exist, or generate error if already exist
  std::map<std::string, AttrValidators>::iterator anAttr = aFeature->second.find(theAttrID);
  if (anAttr == aFeature->second.end()) {
    aFeature->second[theAttrID] = AttrValidators();
  }
  aFeature->second[theAttrID][theID] = theArguments;
}

void Model_ValidatorsFactory::validators(const std::string& theFeatureID,
                                         Validators& theValidators) const
{
  std::map<std::string, AttrValidators>::const_iterator aFeatureIt = 
      myFeatures.find(theFeatureID);
  if (aFeatureIt != myFeatures.cend()) {
    AttrValidators::const_iterator aValidatorsIt = aFeatureIt->second.cbegin();
    for (; aValidatorsIt != aFeatureIt->second.cend(); aValidatorsIt++) {
      if (!validator(aValidatorsIt->first)) {
        Events_InfoMessage("Model_Validator", "Validator %1 was not registered")
          .arg(aValidatorsIt->first).send();
      } else {
        theValidators.push_back(std::make_pair(aValidatorsIt->first, aValidatorsIt->second));
      }
    }
  }
  addDefaultValidators(theValidators);
}

void Model_ValidatorsFactory::validators(const std::string& theFeatureID, const std::string& theAttrID,
                                         Validators& theValidators) const
{
  std::map<std::string, std::map<std::string, AttrValidators> >::const_iterator aFeatureIt = 
      myAttrs.find(theFeatureID);
  if (aFeatureIt != myAttrs.cend()) {
    std::map<std::string, AttrValidators>::const_iterator anAttrIt = aFeatureIt->second.find(theAttrID);
    if (anAttrIt != aFeatureIt->second.end()) {
      AttrValidators::const_iterator aValidatorsIt = anAttrIt->second.cbegin();
      for (; aValidatorsIt != anAttrIt->second.cend(); aValidatorsIt++) {
        if (!validator(aValidatorsIt->first)) {
          Events_InfoMessage("Model_Validator", "Validator %1 was not registered")
            .arg(aValidatorsIt->first).send();
        } else {
          theValidators.push_back(std::make_pair(aValidatorsIt->first, aValidatorsIt->second));
        }
      }
    }
  }
  addDefaultAttributeValidators(theValidators);
}

Model_ValidatorsFactory::Model_ValidatorsFactory()
  : ModelAPI_ValidatorsFactory()
{
  registerValidator("Model_FeatureValidator", new Model_FeatureValidator);
  registerValidator("Model_AttributeValidator", new Model_AttributeValidator);
}

const ModelAPI_Validator* Model_ValidatorsFactory::validator(const std::string& theID) const
{
  std::map<std::string, ModelAPI_Validator*>::const_iterator aIt = myIDs.find(theID);
  if (aIt != myIDs.end()) {
    return aIt->second;
  }
  return NULL;
}

void Model_ValidatorsFactory::addDefaultValidators(Validators& theValidators) const
{
  const static std::string kDefaultId = "Model_FeatureValidator";
  if (!validator(kDefaultId))
    return;
  theValidators.push_back(std::make_pair(kDefaultId, std::list<std::string>()));
}

void Model_ValidatorsFactory::addDefaultAttributeValidators(Validators& theValidators) const
{
  const static std::string kDefaultId = "Model_AttributeValidator";
  if (!validator(kDefaultId))
    return;
  theValidators.push_back(std::make_pair(kDefaultId, std::list<std::string>()));
}

bool Model_ValidatorsFactory::validate(const std::shared_ptr<ModelAPI_Feature>& theFeature) const
{
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(theFeature->data());
  if (aData.get() && aData->isValid()) {
    if (aData->execState() == ModelAPI_StateDone)
      aData->eraseErrorString(); // no error => erase the information string
  } else {
    return false; // feature is broken, already not presented in the data structure
  }

  // check feature validators first
  Validators aValidators;
  validators(theFeature->getKind(), aValidators);

  if (!aValidators.empty()) {
    Validators::const_iterator aValidatorIt = aValidators.cbegin();
    for(; aValidatorIt != aValidators.cend(); aValidatorIt++) {
      const std::string& aValidatorID = aValidatorIt->first;
      const std::list<std::string>& anArguments = aValidatorIt->second;
      // validators() checks invalid validator names
      //if (!aValidator) {
      //  Events_Error::send(std::string("Validator ") + aValidatorID + " was not registered");
      //  continue;
      //}
      const ModelAPI_FeatureValidator* aFValidator = 
        dynamic_cast<const ModelAPI_FeatureValidator*>(validator(aValidatorID));
      if (aFValidator) {
        Events_InfoMessage anError;
        if (!aFValidator->isValid(theFeature, anArguments, anError)) {
          if (anError.empty())
            anError = "Unknown error.";
          anError = aValidatorID + ": " + anError.messageString();
          theFeature->setError(anError.messageString(), false);
          theFeature->data()->execState(ModelAPI_StateInvalidArgument);
          return false;
        }
      }
    }
  }
  // The default validator was retrned by validators() and was checked in previous cycle
  //// check default validator
  //std::map<std::string, ModelAPI_Validator*>::const_iterator aDefaultVal = myIDs.find(kDefaultId);
  //if(aDefaultVal != myIDs.end()) {
  //  static const std::list<std::string> anEmptyArgList;
  //  const ModelAPI_FeatureValidator* aFValidator = 
  //    dynamic_cast<const ModelAPI_FeatureValidator*>(aDefaultVal->second);
  //  if (aFValidator) {
  //    std::string anError;
  //    if (!aFValidator->isValid(theFeature, anEmptyArgList, anError)) {
  //      if (anError.empty())
  //        anError = "Unknown error.";
  //      anError = "Feature invalidated by \"" + kDefaultId + "\" with error: " + anError;
  //      theFeature->setError(anError, false);
  //      theFeature->data()->execState(ModelAPI_StateInvalidArgument);
  //      return false;
  //    }
  //  }
  //}

  // check all attributes for validity
  // Validity of data is checked by "Model_FeatureValidator" (kDefaultId)
  // if (!aData || !aData->isValid())
  //   return false;
  static const std::string kAllTypes = "";
  std::list<std::string> aLtAttributes = aData->attributesIDs(kAllTypes);
  std::list<std::string>::const_iterator anAttrIt = aLtAttributes.cbegin();
  for (; anAttrIt != aLtAttributes.cend(); anAttrIt++) {
    const std::string& anAttributeID = *anAttrIt; 
    AttributePtr anAttribute = theFeature->data()->attribute(anAttributeID);

    std::string aValidatorID;
    Events_InfoMessage anError;
    if (!validate(anAttribute, aValidatorID, anError)) {
      if (anError.empty())
        anError = "Unknown error.";
      anError = anAttributeID + " - " + aValidatorID + ": " + anError.messageString();
      theFeature->setError(anError.messageString(), false);
      theFeature->data()->execState(ModelAPI_StateInvalidArgument);
      return false;
    } 
  }

  return true;
}

bool Model_ValidatorsFactory::validate(const std::shared_ptr<ModelAPI_Attribute>& theAttribute,
                                       std::string& theValidator,
                                       Events_InfoMessage& theError) const
{
  FeaturePtr aFeature = ModelAPI_Feature::feature(theAttribute->owner());
  if (!aFeature.get()) {
    theValidator = "Model_ValidatorsFactory";
    theError = "Attribute has no feature.";
    return false;
  }

  // skip not-case attributes, that really may be invalid (issue 671)
  if (!const_cast<Model_ValidatorsFactory*>(this)->isCase(aFeature, theAttribute->id()))
    return true;

  Validators aValidators;
  validators(aFeature->getKind(), theAttribute->id(), aValidators);

  Validators::iterator aValidatorIt = aValidators.begin();
  for (; aValidatorIt != aValidators.end(); ++aValidatorIt) {
    const std::string& aValidatorID = aValidatorIt->first;
    const std::list<std::string>& anArguments = aValidatorIt->second;
    const ModelAPI_AttributeValidator* anAttrValidator =
        dynamic_cast<const ModelAPI_AttributeValidator*>(validator(aValidatorID));
    if (!anAttrValidator)
      continue;
    if (!anAttrValidator->isValid(theAttribute, anArguments, theError)) {
      theValidator = aValidatorID;
      return false;
    } 
  }

  return true;
}

void Model_ValidatorsFactory::registerNotObligatory(std::string theFeature, std::string theAttribute)
{
  const static std::string kDefaultId = "Model_FeatureValidator";
  std::map<std::string, ModelAPI_Validator*>::const_iterator it = myIDs.find(kDefaultId);
  if (it != myIDs.end()) {
    Model_FeatureValidator* aValidator = dynamic_cast<Model_FeatureValidator*>(it->second);
    if (aValidator) {
      aValidator->registerNotObligatory(theFeature, theAttribute);
    }
  }
}

bool Model_ValidatorsFactory::isNotObligatory(std::string theFeature, std::string theAttribute)
{
  const static std::string kDefaultId = "Model_FeatureValidator";
  std::map<std::string, ModelAPI_Validator*>::const_iterator it = myIDs.find(kDefaultId);
  if (it != myIDs.end()) {
    Model_FeatureValidator* aValidator = dynamic_cast<Model_FeatureValidator*>(it->second);
    if (aValidator) {
      return aValidator->isNotObligatory(theFeature, theAttribute);
    }
  }
  return false; // default
}

void Model_ValidatorsFactory::registerConcealment(std::string theFeature, std::string theAttribute)
{
  std::map<std::string, std::set<std::string> >::iterator aFind = myConcealed.find(theFeature);
  if (aFind == myConcealed.end()) {
    std::set<std::string> aNewSet;
    aNewSet.insert(theAttribute);
    myConcealed[theFeature] = aNewSet;
  } else {
    aFind->second.insert(theAttribute);
  }
}

bool Model_ValidatorsFactory::isConcealed(std::string theFeature, std::string theAttribute)
{
  std::map<std::string, std::set<std::string> >::iterator aFind = myConcealed.find(theFeature);
  return aFind != myConcealed.end() && aFind->second.find(theAttribute) != aFind->second.end();
}

void Model_ValidatorsFactory::registerUnconcealment(std::shared_ptr<ModelAPI_Result> theUnconcealed,
    std::shared_ptr<ModelAPI_Feature> theCanceledFeat)
{
  if (myUnconcealed.find(theUnconcealed) == myUnconcealed.end()) {
    myUnconcealed[theUnconcealed] = std::list<std::shared_ptr<ModelAPI_Feature> >();
  }
  myUnconcealed[theUnconcealed].push_back(theCanceledFeat);
  std::dynamic_pointer_cast<Model_Data>(theUnconcealed->data())->updateConcealmentFlag();
}

void Model_ValidatorsFactory::disableUnconcealment(std::shared_ptr<ModelAPI_Result> theUnconcealed,
    std::shared_ptr<ModelAPI_Feature> theCanceledFeat)
{
  std::map<std::shared_ptr<ModelAPI_Result>, std::list<std::shared_ptr<ModelAPI_Feature> > >
    ::iterator aResFound = myUnconcealed.find(theUnconcealed);
  if (aResFound != myUnconcealed.end()) {
    std::list<std::shared_ptr<ModelAPI_Feature> >::iterator anIter = aResFound->second.begin();
    for(; anIter != aResFound->second.end(); anIter++) {
      if (*anIter == theCanceledFeat) {
        aResFound->second.erase(anIter);
        std::dynamic_pointer_cast<Model_Data>(theUnconcealed->data())->updateConcealmentFlag();
        break;
      }
    }
  }
}

bool Model_ValidatorsFactory::isUnconcealed(std::shared_ptr<ModelAPI_Result> theUnconcealed,
    std::shared_ptr<ModelAPI_Feature> theCanceledFeat)
{
  std::map<std::shared_ptr<ModelAPI_Result>, std::list<std::shared_ptr<ModelAPI_Feature> > >
    ::iterator aResFound = myUnconcealed.find(theUnconcealed);
  if (aResFound != myUnconcealed.end()) {
    std::list<std::shared_ptr<ModelAPI_Feature> >::iterator aFeatIter = aResFound->second.begin();
    for(; aFeatIter != aResFound->second.end(); aFeatIter++) {
      if (aFeatIter->get()) {
        if ((*aFeatIter)->isDisabled()) continue;
        if (*aFeatIter == theCanceledFeat)
          return true; // this is exactly canceled
        if (theCanceledFeat->document()->isLater(*aFeatIter, theCanceledFeat))
          return true; // if unconcealed feature (from the list) is later than concealed
      } else
        return true; // empty attribute means that everything is canceled
    }
  }
  return false;
}

void Model_ValidatorsFactory::registerCase(std::string theFeature, std::string theAttribute,
                            const std::list<std::pair<std::string, std::string> >& theCases)
{
  std::map<std::string, std::map<std::string, std::map<std::string, std::set<std::string> > > >
    ::iterator aFindFeature = myCases.find(theFeature);
  if (aFindFeature == myCases.end()) {
    myCases[theFeature] = std::map<std::string, std::map<std::string, std::set<std::string> > >();
    aFindFeature = myCases.find(theFeature);
  }
  std::map<std::string, std::map<std::string, std::set<std::string> > >::iterator aFindAttrID =
    aFindFeature->second.find(theAttribute);

  if (aFindAttrID == aFindFeature->second.end()) {
    aFindFeature->second[theAttribute] =
      std::map<std::string, std::set<std::string> >();
    aFindAttrID = aFindFeature->second.find(theAttribute);
  }
  std::list<std::pair<std::string, std::string> >::const_iterator aCasesIt = theCases.begin(),
                                                                       aCasesLast = theCases.end();
  std::map<std::string, std::set<std::string> > aFindCases = aFindAttrID->second;
  for (; aCasesIt != aCasesLast; aCasesIt++) {
    std::pair<std::string, std::string> aCasePair = *aCasesIt;
    std::string aSwitch = aCasePair.first;
    if (aFindAttrID->second.find(aSwitch) == aFindAttrID->second.end()) {
      aFindAttrID->second[aSwitch] = std::set<std::string>();
    }
    aFindAttrID->second[aSwitch].insert(aCasePair.second);
  }
}

bool Model_ValidatorsFactory::isCase(FeaturePtr theFeature, std::string theAttribute)
{
  bool anInCase = true;
  std::map<std::string, std::map<std::string, std::map<std::string, std::set<std::string> > > >
    ::iterator aFindFeature = myCases.find(theFeature->getKind());
  if (aFindFeature != myCases.end()) {
    std::map<std::string, std::map<std::string, std::set<std::string> > >::iterator
      aFindAttrID = aFindFeature->second.find(theAttribute);
    if (aFindAttrID != aFindFeature->second.end()) {
      std::map<std::string, std::set<std::string> >::iterator
              aCasesIt = aFindAttrID->second.begin(), aCasesLast = aFindAttrID->second.end();
      for (; aCasesIt != aCasesLast && anInCase; aCasesIt++) {
        // the the switch-attribute that contains the case value
        AttributeStringPtr aSwitch = theFeature->string(aCasesIt->first);
        if (aSwitch.get()) {
          // the second has the case identifier
          anInCase =  aCasesIt->second.find(aSwitch->value()) != aCasesIt->second.end();
        }
      }
    }
  }
  return anInCase; // if no additional conditions, this attribute is the case to be validated
}
