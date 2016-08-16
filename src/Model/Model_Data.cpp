// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_Data.hxx
// Created:     21 Mar 2014
// Author:      Mikhail PONIKAROV

#include <Model_Data.h>
#include <Model_AttributeDocRef.h>
#include <Model_AttributeInteger.h>
#include <Model_AttributeDouble.h>
#include <Model_AttributeDoubleArray.h>
#include <Model_AttributeReference.h>
#include <Model_AttributeRefAttr.h>
#include <Model_AttributeRefList.h>
#include <Model_AttributeRefAttrList.h>
#include <Model_AttributeBoolean.h>
#include <Model_AttributeString.h>
#include <Model_AttributeSelection.h>
#include <Model_AttributeSelectionList.h>
#include <Model_AttributeIntArray.h>
#include <Model_Events.h>
#include <Model_Expression.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_ResultParameter.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_ResultCompSolid.h>
#include <ModelAPI_Tools.h>
#include <Model_Validator.h>

#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Point2D.h>

#include <GeomData_Point.h>
#include <GeomData_Point2D.h>
#include <GeomData_Dir.h>
#include <Events_Loop.h>
#include <Events_InfoMessage.h>

#include <TDataStd_Name.hxx>
#include <TDataStd_AsciiString.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <TDF_AttributeIterator.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_RelocationTable.hxx>
#include <TColStd_HArray1OfByte.hxx>

#include <string>

// myLab contains:
// TDataStd_Name - name of the object
// TDataStd_IntegerArray - state of the object execution, transaction ID of update
// TDataStd_BooleanArray - array of flags of this data:
//                             0 - is in history or not
static const int kFlagInHistory = 0;
//                             1 - is displayed or not
static const int kFlagDisplayed = 1;
//                             2 - is deleted (for results) or not
static const int kFlagDeleted = 2;

// invalid data
const static std::shared_ptr<ModelAPI_Data> kInvalid(new Model_Data());

Model_Data::Model_Data() : mySendAttributeUpdated(true), myWasChangedButBlocked(false)
{
}

void Model_Data::setLabel(TDF_Label theLab)
{
  myLab = theLab;
  // set or get the default flags
  if (!myLab.FindAttribute(TDataStd_BooleanArray::GetID(), myFlags)) {
    // set default values if not found
    myFlags = TDataStd_BooleanArray::Set(myLab, 0, 2);
    myFlags->SetValue(kFlagInHistory, Standard_True); // is in history by default is true
    myFlags->SetValue(kFlagDisplayed, Standard_True); // is displayed by default is true
    myFlags->SetValue(kFlagDeleted, Standard_False); // is deleted by default is false
  } else if (myFlags->Length() != 3) { // for old formats support
    Standard_Boolean aFlag0 = myFlags->Upper() >= 0 ? myFlags->Value(0) : Standard_True;
    Standard_Boolean aFlag1 = myFlags->Upper() >= 1 ? myFlags->Value(1) : Standard_True;
    Standard_Boolean aFlag2 = myFlags->Upper() >= 2 ? myFlags->Value(2) : Standard_True;
    Handle(TColStd_HArray1OfByte) aNewArray = new TColStd_HArray1OfByte(0, 2);
    myFlags->SetInternalArray(aNewArray);
    myFlags->SetValue(0, aFlag0); 
    myFlags->SetValue(1, aFlag1); 
    myFlags->SetValue(2, aFlag2); 
  }
}

std::string Model_Data::name()
{
  Handle(TDataStd_Name) aName;
  if (myLab.FindAttribute(TDataStd_Name::GetID(), aName))
    return std::string(TCollection_AsciiString(aName->Get()).ToCString());
  return "";  // not defined
}

void Model_Data::setName(const std::string& theName)
{
  bool isModified = false;
  std::string anOldName = name();
  Handle(TDataStd_Name) aName;
  if (!myLab.FindAttribute(TDataStd_Name::GetID(), aName)) {
    TDataStd_Name::Set(myLab, theName.c_str());
    isModified = true;
  } else {
    isModified = !aName->Get().IsEqual(theName.c_str());
    if (isModified)
      aName->Set(theName.c_str());
  }
  if (mySendAttributeUpdated && isModified)
    ModelAPI_ObjectRenamedMessage::send(myObject, anOldName, theName, this);
}

AttributePtr Model_Data::addAttribute(const std::string& theID, const std::string theAttrType)
{
  AttributePtr aResult;
  TDF_Label anAttrLab = myLab.FindChild(int(myAttrs.size()) + 1);
  ModelAPI_Attribute* anAttr = 0;
  if (theAttrType == ModelAPI_AttributeDocRef::typeId()) {
    anAttr = new Model_AttributeDocRef(anAttrLab);
  } else if (theAttrType == Model_AttributeInteger::typeId()) {
    Model_AttributeInteger* anAttribute = new Model_AttributeInteger();
    // Expression should use the same label to support backward compatibility
    TDF_Label anExpressionLab = anAttrLab;
    anAttribute->myExpression.reset(new Model_ExpressionInteger(anExpressionLab));
    anAttribute->myIsInitialized = anAttribute->myExpression->isInitialized();
    anAttr = anAttribute;
  } else if (theAttrType == ModelAPI_AttributeDouble::typeId()) {
    Model_AttributeDouble* anAttribute = new Model_AttributeDouble();
    TDF_Label anExpressionLab = anAttrLab.FindChild(1);
    anAttribute->myExpression.reset(new Model_ExpressionDouble(anExpressionLab));
    anAttribute->myIsInitialized = anAttribute->myExpression->isInitialized();
    anAttr = anAttribute;
  } else if (theAttrType == Model_AttributeBoolean::typeId()) {
    anAttr = new Model_AttributeBoolean(anAttrLab);
  } else if (theAttrType == Model_AttributeString::typeId()) {
    anAttr = new Model_AttributeString(anAttrLab);
  } else if (theAttrType == ModelAPI_AttributeReference::typeId()) {
    anAttr = new Model_AttributeReference(anAttrLab);
  } else if (theAttrType == ModelAPI_AttributeSelection::typeId()) {
    anAttr = new Model_AttributeSelection(anAttrLab);
  } else if (theAttrType == ModelAPI_AttributeSelectionList::typeId()) {
    anAttr = new Model_AttributeSelectionList(anAttrLab);
  } else if (theAttrType == ModelAPI_AttributeRefAttr::typeId()) {
    anAttr = new Model_AttributeRefAttr(anAttrLab);
  } else if (theAttrType == ModelAPI_AttributeRefList::typeId()) {
    anAttr = new Model_AttributeRefList(anAttrLab);
  } else if (theAttrType == ModelAPI_AttributeRefAttrList::typeId()) {
    anAttr = new Model_AttributeRefAttrList(anAttrLab);
  } else if (theAttrType == ModelAPI_AttributeIntArray::typeId()) {
    anAttr = new Model_AttributeIntArray(anAttrLab);
  } else if (theAttrType == ModelAPI_AttributeDoubleArray::typeId()) {
    anAttr = new Model_AttributeDoubleArray(anAttrLab);
  }
  // create also GeomData attributes here because only here the OCAF structure is known
  else if (theAttrType == GeomData_Point::typeId()) {
    GeomData_Point* anAttribute = new GeomData_Point();
    bool anAllInitialized = true;
    for (int aComponent = 0; aComponent < GeomData_Point::NUM_COMPONENTS; ++aComponent) {
      TDF_Label anExpressionLab = anAttrLab.FindChild(aComponent + 1);
      anAttribute->myExpression[aComponent].reset(new Model_ExpressionDouble(anExpressionLab));
      anAllInitialized = anAllInitialized && anAttribute->myExpression[aComponent]->isInitialized();
    }
    anAttribute->myIsInitialized = anAllInitialized;
    anAttr = anAttribute;
  } else if (theAttrType == GeomData_Dir::typeId()) {
    anAttr = new GeomData_Dir(anAttrLab);
  } else if (theAttrType == GeomData_Point2D::typeId()) {
    GeomData_Point2D* anAttribute = new GeomData_Point2D();
    bool anAllInitialized = true;
    for (int aComponent = 0; aComponent < GeomData_Point2D::NUM_COMPONENTS; ++aComponent) {
      TDF_Label anExpressionLab = anAttrLab.FindChild(aComponent + 1);
      anAttribute->myExpression[aComponent].reset(new Model_ExpressionDouble(anExpressionLab));
      anAllInitialized = anAllInitialized && anAttribute->myExpression[aComponent]->isInitialized();
    }
    anAttribute->myIsInitialized = anAllInitialized;
    anAttr = anAttribute;
  }
  if (anAttr) {
    aResult = std::shared_ptr<ModelAPI_Attribute>(anAttr);
    myAttrs[theID] = aResult;
    anAttr->setObject(myObject);
    anAttr->setID(theID);
  } else {
    Events_InfoMessage("Model_Data", "Can not create unknown type of attribute %1").arg(theAttrType).send();
  }
  return aResult;
}

// macro for the generic returning of the attribute by the ID
#define GET_ATTRIBUTE_BY_ID(ATTR_TYPE, METHOD_NAME) \
  std::shared_ptr<ATTR_TYPE> Model_Data::METHOD_NAME(const std::string& theID) { \
    std::shared_ptr<ATTR_TYPE> aRes; \
    std::map<std::string, AttributePtr >::iterator aFound = \
      myAttrs.find(theID); \
    if (aFound != myAttrs.end()) { \
      aRes = std::dynamic_pointer_cast<ATTR_TYPE>(aFound->second); \
    } \
    return aRes; \
  }
// implement nice getting methods for all ModelAPI attributes
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeDocRef, document);
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeDouble, real);
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeInteger, integer);
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeBoolean, boolean);
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeString, string);
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeReference, reference);
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeSelection, selection);
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeSelectionList, selectionList);
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeRefAttr, refattr);
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeRefList, reflist);
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeRefAttrList, refattrlist);
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeIntArray, intArray);
GET_ATTRIBUTE_BY_ID(ModelAPI_AttributeDoubleArray, realArray);

std::shared_ptr<ModelAPI_Attribute> Model_Data::attribute(const std::string& theID)
{
  std::shared_ptr<ModelAPI_Attribute> aResult;
  if (myAttrs.find(theID) == myAttrs.end())  // no such attribute
    return aResult;
  return myAttrs[theID];
}

const std::string& Model_Data::id(const std::shared_ptr<ModelAPI_Attribute>& theAttr)
{
  std::map<std::string, std::shared_ptr<ModelAPI_Attribute> >::iterator anAttr = 
    myAttrs.begin();
  for (; anAttr != myAttrs.end(); anAttr++) {
    if (anAttr->second == theAttr)
      return anAttr->first;
  }
  // not found
  static std::string anEmpty;
  return anEmpty;
}

bool Model_Data::isEqual(const std::shared_ptr<ModelAPI_Data>& theData)
{
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(theData);
  if (aData)
    return myLab.IsEqual(aData->myLab) == Standard_True ;
  return false;
}

bool Model_Data::isValid()
{
  return !myLab.IsNull() && myLab.HasAttribute();
}

std::list<std::shared_ptr<ModelAPI_Attribute> > Model_Data::attributes(const std::string& theType)
{
  std::list<std::shared_ptr<ModelAPI_Attribute> > aResult;
  std::map<std::string, std::shared_ptr<ModelAPI_Attribute> >::iterator anAttrsIter = 
    myAttrs.begin();
  for (; anAttrsIter != myAttrs.end(); anAttrsIter++) {
    if (theType.empty() || anAttrsIter->second->attributeType() == theType) {
      aResult.push_back(anAttrsIter->second);
    }
  }
  return aResult;
}

std::list<std::string> Model_Data::attributesIDs(const std::string& theType) 
{
  std::list<std::string> aResult;
  std::map<std::string, std::shared_ptr<ModelAPI_Attribute> >::iterator anAttrsIter = 
    myAttrs.begin();
  for (; anAttrsIter != myAttrs.end(); anAttrsIter++) {
    if (theType.empty() || anAttrsIter->second->attributeType() == theType) {
      aResult.push_back(anAttrsIter->first);
    }
  }
  return aResult;
}

void Model_Data::sendAttributeUpdated(ModelAPI_Attribute* theAttr)
{
  theAttr->setInitialized();
  if (theAttr->isArgument()) {
    if (mySendAttributeUpdated) {
      if (myObject) {
        myObject->attributeChanged(theAttr->id());
        static const Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);
        ModelAPI_EventCreator::get()->sendUpdated(myObject, anEvent);
      }
    } else {
      // to avoid too many duplications do not add the same like the last
      if (myWasChangedButBlocked.empty() || *(myWasChangedButBlocked.rbegin()) != theAttr)
        myWasChangedButBlocked.push_back(theAttr);
    }
  }
}

void Model_Data::blockSendAttributeUpdated(const bool theBlock, const bool theSendMessage)
{
  if (mySendAttributeUpdated == theBlock) {
    mySendAttributeUpdated = !theBlock;
    if (mySendAttributeUpdated && !myWasChangedButBlocked.empty()) { // so, now it is ok to send the update signal
      if (theSendMessage) {
        // make a copy to avoid iteration on modified list (may be cleared by attribute changed call)
        std::list<ModelAPI_Attribute*> aWasChangedButBlocked = myWasChangedButBlocked;
        myWasChangedButBlocked.clear();
        std::list<ModelAPI_Attribute*>::iterator aChangedIter = aWasChangedButBlocked.begin();
        for(; aChangedIter != aWasChangedButBlocked.end(); aChangedIter++) {
          myObject->attributeChanged((*aChangedIter)->id());
        }
        static const Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);
        ModelAPI_EventCreator::get()->sendUpdated(myObject, anEvent);
      } else {
        myWasChangedButBlocked.clear();
      }
    }
  }
}

void Model_Data::erase()
{
  if (!myLab.IsNull()) {
    if (myLab.HasAttribute()) {
      // remove in order to clear back references in other objects
      std::list<std::pair<std::string, std::list<ObjectPtr> > > aRefs;
      referencesToObjects(aRefs);
      std::list<std::pair<std::string, std::list<ObjectPtr> > >::iterator anAttrIter = aRefs.begin();
      for(; anAttrIter != aRefs.end(); anAttrIter++) {
        std::list<ObjectPtr>::iterator aReferenced = anAttrIter->second.begin();
        for(; aReferenced != anAttrIter->second.end(); aReferenced++) {
          if (aReferenced->get() && (*aReferenced)->data()->isValid()) {
            std::shared_ptr<Model_Data> aData = 
              std::dynamic_pointer_cast<Model_Data>((*aReferenced)->data());
            aData->removeBackReference(myAttrs[anAttrIter->first]);
          }
        }
      }
    }
    myAttrs.clear();
    myLab.ForgetAllAttributes();
  }
}

// indexes in the state array
enum StatesIndexes {
  STATE_INDEX_STATE = 1, // the state type itself
  STATE_INDEX_TRANSACTION = 2, // transaction ID
};

/// Returns the label array, initialises it by default values if not exists
static Handle(TDataStd_IntegerArray) stateArray(TDF_Label& theLab)
{
  Handle(TDataStd_IntegerArray) aStateArray;
  if (!theLab.FindAttribute(TDataStd_IntegerArray::GetID(), aStateArray)) {
    aStateArray = TDataStd_IntegerArray::Set(theLab, 1, 2);
    aStateArray->SetValue(STATE_INDEX_STATE, ModelAPI_StateMustBeUpdated); // default state
    aStateArray->SetValue(STATE_INDEX_TRANSACTION, 0); // default transaction ID (not existing)
  }
  return aStateArray;
}

void Model_Data::execState(const ModelAPI_ExecState theState)
{
  if (theState != ModelAPI_StateNothing) {
    if (stateArray(myLab)->Value(STATE_INDEX_STATE) != (int)theState) {
      stateArray(myLab)->SetValue(STATE_INDEX_STATE, (int)theState);
    }
  }
}

ModelAPI_ExecState Model_Data::execState()
{
  return ModelAPI_ExecState(stateArray(myLab)->Value(STATE_INDEX_STATE));
}

int Model_Data::updateID()
{
  return stateArray(myLab)->Value(STATE_INDEX_TRANSACTION);
}

void Model_Data::setUpdateID(const int theID)
{
  stateArray(myLab)->SetValue(STATE_INDEX_TRANSACTION, theID);
}

void Model_Data::setError(const std::string& theError, bool theSend)
{
  execState(ModelAPI_StateExecFailed);
  if (theSend) {
    Events_InfoMessage("Model_Data", theError).send();
  }
  TDataStd_AsciiString::Set(myLab, theError.c_str());
}

void Model_Data::eraseErrorString()
{
  myLab.ForgetAttribute(TDataStd_AsciiString::GetID());
}

std::string Model_Data::error() const
{
  Handle(TDataStd_AsciiString) anErrorAttr;
  if (myLab.FindAttribute(TDataStd_AsciiString::GetID(), anErrorAttr)) {
    return std::string(anErrorAttr->Get().ToCString());
  }
  return std::string();
}

int Model_Data::featureId() const
{
  return myLab.Father().Tag(); // tag of the feature label
}

void Model_Data::eraseBackReferences()
{
  myRefsToMe.clear();
  std::shared_ptr<ModelAPI_Result> aRes = 
    std::dynamic_pointer_cast<ModelAPI_Result>(myObject);
  if (aRes)
    aRes->setIsConcealed(false);
}

void Model_Data::removeBackReference(FeaturePtr theFeature, std::string theAttrID)
{
  AttributePtr anAttribute = theFeature->data()->attribute(theAttrID);
  removeBackReference(anAttribute);
}

void Model_Data::removeBackReference(AttributePtr theAttr)
{
  if (myRefsToMe.find(theAttr) == myRefsToMe.end())
    return;

  myRefsToMe.erase(theAttr);

  // remove concealment immideately: on deselection it must be posible to reselect in GUI the same
  FeaturePtr aFeatureOwner = std::dynamic_pointer_cast<ModelAPI_Feature>(theAttr->owner());
  if (aFeatureOwner.get() &&
    ModelAPI_Session::get()->validators()->isConcealed(aFeatureOwner->getKind(), theAttr->id())) {
    updateConcealmentFlag();
  }
}

void Model_Data::addBackReference(FeaturePtr theFeature, std::string theAttrID, 
   const bool theApplyConcealment)
{
  // it is possible to add the same attribute twice: may be last time the owner was not Stable...
  AttributePtr anAttribute = theFeature->data()->attribute(theAttrID);
  if (myRefsToMe.find(anAttribute) == myRefsToMe.end())
    myRefsToMe.insert(theFeature->data()->attribute(theAttrID));

  if (theApplyConcealment &&  theFeature->isStable() && 
      ModelAPI_Session::get()->validators()->isConcealed(theFeature->getKind(), theAttrID)) {
    std::shared_ptr<ModelAPI_Result> aRes = 
      std::dynamic_pointer_cast<ModelAPI_Result>(myObject);
    // the second condition is for history upper than concealment causer, so the feature result may
    // be displayed and previewed; also for avoiding of quick show/hide on history
    // moving deep down
    if (aRes && !theFeature->isDisabled() && 
        !ModelAPI_Session::get()->validators()->isUnconcealed(aRes, theFeature)) {
      aRes->setIsConcealed(true);
    }
  }
}

void Model_Data::updateConcealmentFlag()
{
  std::set<AttributePtr>::iterator aRefsIter = myRefsToMe.begin();
  for(; aRefsIter != myRefsToMe.end(); aRefsIter++) {
    if (aRefsIter->get()) {
      FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>((*aRefsIter)->owner());
      if (aFeature.get() && !aFeature->isDisabled() && aFeature->isStable()) {
        if (ModelAPI_Session::get()->validators()->isConcealed(
              aFeature->getKind(), (*aRefsIter)->id())) {
          std::shared_ptr<ModelAPI_Result> aRes = 
            std::dynamic_pointer_cast<ModelAPI_Result>(myObject);
          if (aRes.get()) {
            if (!ModelAPI_Session::get()->validators()->isUnconcealed(aRes, aFeature)) {
              aRes->setIsConcealed(true); // set concealed
              return;
            }
          }
        }
      }
    }
  }
  std::shared_ptr<ModelAPI_Result> aRes = 
    std::dynamic_pointer_cast<ModelAPI_Result>(myObject);
  if (aRes.get()) {
    aRes->setIsConcealed(false);
  }
}

std::set<std::string> set_union(const std::set<std::string>& theLeft, 
                                const std::set<std::string>& theRight)
{
  std::set<std::string> aResult;
  aResult.insert(theLeft.begin(), theLeft.end());
  aResult.insert(theRight.begin(), theRight.end());
  return aResult;
}

std::set<std::string> usedParameters(const AttributePointPtr& theAttribute)
{
  std::set<std::string> anUsedParameters;
  for (int aComponent = 0; aComponent < 3; ++aComponent)
    anUsedParameters = set_union(anUsedParameters, theAttribute->usedParameters(aComponent));
  return anUsedParameters;
}

std::set<std::string> usedParameters(const AttributePoint2DPtr& theAttribute)
{
  std::set<std::string> anUsedParameters;
  for (int aComponent = 0; aComponent < 2; ++aComponent)
    anUsedParameters = set_union(anUsedParameters, theAttribute->usedParameters(aComponent));
  return anUsedParameters;
}

std::list<ResultParameterPtr> findVariables(const std::set<std::string>& theParameters)
{
  std::list<ResultParameterPtr> aResult;
  std::set<std::string>::const_iterator aParamIt = theParameters.cbegin();
  for (; aParamIt != theParameters.cend(); ++aParamIt) {
    const std::string& aName = *aParamIt;
    double aValue;
    ResultParameterPtr aParam;
    // theSearcher is not needed here: in expressions of features the parameters history is not needed
    if (ModelAPI_Tools::findVariable(FeaturePtr(), aName, aValue, aParam))
      aResult.push_back(aParam);
  }
  return aResult;
}

void Model_Data::referencesToObjects(
  std::list<std::pair<std::string, std::list<ObjectPtr> > >& theRefs)
{
  static Model_ValidatorsFactory* aValidators = 
    static_cast<Model_ValidatorsFactory*>(ModelAPI_Session::get()->validators());
  FeaturePtr aMyFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(myObject);

  std::map<std::string, std::shared_ptr<ModelAPI_Attribute> >::iterator anAttr = myAttrs.begin();
  std::list<ObjectPtr> aReferenced; // not inside of cycle to avoid excess memory management
  for(; anAttr != myAttrs.end(); anAttr++) {
    // skip not-case attributes, that really may refer to anything not-used (issue 671)
    if (aMyFeature.get() && !aValidators->isCase(aMyFeature, anAttr->second->id()))
      continue;

    std::string aType = anAttr->second->attributeType();
    if (aType == ModelAPI_AttributeReference::typeId()) { // reference to object
      std::shared_ptr<ModelAPI_AttributeReference> aRef = std::dynamic_pointer_cast<
          ModelAPI_AttributeReference>(anAttr->second);
      aReferenced.push_back(aRef->value());
    } else if (aType == ModelAPI_AttributeRefAttr::typeId()) { // reference to attribute or object
      std::shared_ptr<ModelAPI_AttributeRefAttr> aRef = std::dynamic_pointer_cast<
          ModelAPI_AttributeRefAttr>(anAttr->second);
      if (aRef->isObject()) {
        aReferenced.push_back(aRef->object());
      } else {
        AttributePtr anAttr = aRef->attr();
        if (anAttr.get())
          aReferenced.push_back(anAttr->owner());
      }
    } else if (aType == ModelAPI_AttributeRefList::typeId()) { // list of references
      aReferenced = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(anAttr->second)->list();
    } else if (aType == ModelAPI_AttributeSelection::typeId()) { // selection attribute
      std::shared_ptr<ModelAPI_AttributeSelection> aRef = std::dynamic_pointer_cast<
          ModelAPI_AttributeSelection>(anAttr->second);
      aReferenced.push_back(aRef->context());
    } else if (aType == ModelAPI_AttributeSelectionList::typeId()) { // list of selection attributes
      std::shared_ptr<ModelAPI_AttributeSelectionList> aRef = std::dynamic_pointer_cast<
          ModelAPI_AttributeSelectionList>(anAttr->second);
      for(int a = aRef->size() - 1; a >= 0; a--) {
        aReferenced.push_back(aRef->value(a)->context());
      }
    } else if (aType == ModelAPI_AttributeRefAttrList::typeId()) {
      std::shared_ptr<ModelAPI_AttributeRefAttrList> aRefAttr = std::dynamic_pointer_cast<
          ModelAPI_AttributeRefAttrList>(anAttr->second);
      std::list<std::pair<ObjectPtr, AttributePtr> > aRefs = aRefAttr->list();
      std::list<std::pair<ObjectPtr, AttributePtr> >::const_iterator anIt = aRefs.begin(),
                                                                     aLast = aRefs.end();
      for (; anIt != aLast; anIt++) {
        aReferenced.push_back(anIt->first);
      }
    } else if (aType == ModelAPI_AttributeInteger::typeId()) { // integer attribute
      AttributeIntegerPtr anAttribute =
          std::dynamic_pointer_cast<ModelAPI_AttributeInteger>(anAttr->second);
      std::set<std::string> anUsedParameters = anAttribute->usedParameters();
      std::list<ResultParameterPtr> aParameters = findVariables(anUsedParameters);
      aReferenced.insert(aReferenced.end(), aParameters.begin(), aParameters.end());
    } else if (aType == ModelAPI_AttributeDouble::typeId()) { // double attribute
      AttributeDoublePtr anAttribute =
          std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(anAttr->second);
      std::set<std::string> anUsedParameters = anAttribute->usedParameters();
      std::list<ResultParameterPtr> aParameters = findVariables(anUsedParameters);
      aReferenced.insert(aReferenced.end(), aParameters.begin(), aParameters.end());
    } else if (aType == GeomDataAPI_Point::typeId()) { // point attribute
      AttributePointPtr anAttribute =
        std::dynamic_pointer_cast<GeomDataAPI_Point>(anAttr->second);
      std::set<std::string> anUsedParameters = usedParameters(anAttribute);
      std::list<ResultParameterPtr> aParameters = findVariables(anUsedParameters);
      aReferenced.insert(aReferenced.end(), aParameters.begin(), aParameters.end());
    } else if (aType == GeomDataAPI_Point2D::typeId()) { // point attribute
      AttributePoint2DPtr anAttribute =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAttr->second);
      std::set<std::string> anUsedParameters = usedParameters(anAttribute);
      std::list<ResultParameterPtr> aParameters = findVariables(anUsedParameters);
      aReferenced.insert(aReferenced.end(), aParameters.begin(), aParameters.end());
    } else
      continue; // nothing to do, not reference

    if (!aReferenced.empty()) {
      theRefs.push_back(std::pair<std::string, std::list<ObjectPtr> >(anAttr->first, aReferenced));
      aReferenced.clear();
    }
  }
}

/// makes copy of all attributes on the given label and all sub-labels
static void copyAttrs(TDF_Label theSource, TDF_Label theDestination) {
  TDF_AttributeIterator anAttrIter(theSource);
  for(; anAttrIter.More(); anAttrIter.Next()) {
    Handle(TDF_Attribute) aTargetAttr;
    if (!theDestination.FindAttribute(anAttrIter.Value()->ID(), aTargetAttr)) {
      // create a new attribute if not yet exists in the destination
	    aTargetAttr = anAttrIter.Value()->NewEmpty();
      theDestination.AddAttribute(aTargetAttr);
    }
    Handle(TDF_RelocationTable) aRelocTable = new TDF_RelocationTable(); // no relocation, empty map
    anAttrIter.Value()->Paste(aTargetAttr, aRelocTable);
  }
  // copy the sub-labels content
  TDF_ChildIterator aSubLabsIter(theSource);
  for(; aSubLabsIter.More(); aSubLabsIter.Next()) {
    copyAttrs(aSubLabsIter.Value(), theDestination.FindChild(aSubLabsIter.Value().Tag()));
  }
}

void Model_Data::copyTo(std::shared_ptr<ModelAPI_Data> theTarget)
{
  TDF_Label aTargetRoot = std::dynamic_pointer_cast<Model_Data>(theTarget)->label();
  copyAttrs(myLab, aTargetRoot);
  // reinitialize Model_Attributes by TDF_Attributes set
  std::shared_ptr<Model_Data> aTData = std::dynamic_pointer_cast<Model_Data>(theTarget);
  aTData->myAttrs.clear();
  theTarget->owner()->initAttributes(); // reinit feature attributes
}

bool Model_Data::isInHistory()
{
  return myFlags->Value(kFlagInHistory) == Standard_True;
}

void Model_Data::setIsInHistory(const bool theFlag)
{
  return myFlags->SetValue(kFlagInHistory, theFlag);
}

bool Model_Data::isDeleted()
{
  return myFlags->Value(kFlagDeleted) == Standard_True;
}

void Model_Data::setIsDeleted(const bool theFlag)
{
  return myFlags->SetValue(kFlagDeleted, theFlag);
}

bool Model_Data::isDisplayed()
{
  if (!myObject.get() || !myObject->document().get() || // object is in valid
      myFlags->Value(kFlagDisplayed) != Standard_True) // or it was not displayed before
    return false;
  if (myObject->document()->isActive()) // for active documents it must be ok anyway
    return true;
  // any object from the root document except part result may be displayed
  if (myObject->document() == ModelAPI_Session::get()->moduleDocument() &&
      myObject->groupName() != ModelAPI_ResultPart::group())
    return true;
  return false;
}

void Model_Data::setDisplayed(const bool theDisplay)
{
  if (theDisplay != isDisplayed()) {
    myFlags->SetValue(kFlagDisplayed, theDisplay);
    static Events_Loop* aLoop = Events_Loop::loop();
    static Events_ID EVENT_DISP = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);
    static const ModelAPI_EventCreator* aECreator = ModelAPI_EventCreator::get();
    aECreator->sendUpdated(myObject, EVENT_DISP);
  }
}

std::shared_ptr<ModelAPI_Data> Model_Data::invalidPtr()
{
  return kInvalid;
}

std::shared_ptr<ModelAPI_Data> Model_Data::invalidData()
{
  return kInvalid;
}

std::shared_ptr<ModelAPI_Object> Model_Data::owner()
{
  return myObject;
}
