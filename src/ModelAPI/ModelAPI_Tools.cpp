// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_Tools.cpp
// Created:     06 Aug 2014
// Author:      Vitaly Smetannikov

#include "ModelAPI_Tools.h"
#include <ModelAPI_Session.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_ResultParameter.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_AttributeDocRef.h>
#include <list>
#include <map>

namespace ModelAPI_Tools {

std::shared_ptr<GeomAPI_Shape> shape(const ResultPtr& theResult)
{
  return theResult->shape();
}

const char* toString(ModelAPI_ExecState theExecState) 
{
#define TO_STRING(__NAME__) case __NAME__: return #__NAME__;
  switch (theExecState) {
  TO_STRING(ModelAPI_StateDone)
  TO_STRING(ModelAPI_StateMustBeUpdated)
  TO_STRING(ModelAPI_StateExecFailed)
  TO_STRING(ModelAPI_StateInvalidArgument)
  TO_STRING(ModelAPI_StateNothing)
  default: return "Unknown ExecState.";
  }
#undef TO_STRING
}

std::string getFeatureError(const FeaturePtr& theFeature)
{
  std::string anError;
  if (!theFeature.get() || !theFeature->data()->isValid() || theFeature->isAction())
    return anError;

  // to be removed later, this error should be got from the feature
  if (theFeature->data()->execState() == ModelAPI_StateDone ||
      theFeature->data()->execState() == ModelAPI_StateMustBeUpdated)
    return anError;

  // set error indication
  anError = theFeature->error();
  if (anError.empty()) {
    bool isDone = ( theFeature->data()->execState() == ModelAPI_StateDone
                 || theFeature->data()->execState() == ModelAPI_StateMustBeUpdated );
    if (!isDone) {
      anError = toString(theFeature->data()->execState());
      // If the feature is Composite and error is StateInvalidArgument,
      // error text should include error of first invalid sub-feature. Otherwise
      // it is not clear what is the reason of the invalid argument.
      if (theFeature->data()->execState() == ModelAPI_StateInvalidArgument) {
        CompositeFeaturePtr aComposite =
                    std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(theFeature);
        if (aComposite) {
          for (int i = 0, aSize = aComposite->numberOfSubs(); i < aSize; i++) {
            FeaturePtr aSubFeature = aComposite->subFeature(i);
            std::string aSubFeatureError = getFeatureError(aSubFeature);
            if (!aSubFeatureError.empty()) {
              anError = anError + " in " + aSubFeature->getKind() + ".\n" + aSubFeatureError;
              break;
            }
          }
        }
      }
    }
  }

  return anError;
}

ObjectPtr objectByName(const DocumentPtr& theDocument, const std::string& theGroup, const std::string& theName)
{
  for (int anIndex = 0; anIndex < theDocument->size(theGroup); ++anIndex) {
    ObjectPtr anObject = theDocument->object(theGroup, anIndex);
    if (anObject->data()->name() == theName)
      return anObject;
  }
  // not found
  return ObjectPtr();
}

bool findVariable(const DocumentPtr& theDocument, 
                  const std::string& theName, double& outValue, ResultParameterPtr& theParam)
{
  ObjectPtr aParamObj = objectByName(theDocument, ModelAPI_ResultParameter::group(), theName);
  theParam = std::dynamic_pointer_cast<ModelAPI_ResultParameter>(aParamObj);
  if (!theParam.get())
    return false;
  AttributeDoublePtr aValueAttribute = theParam->data()->real(ModelAPI_ResultParameter::VALUE());
  outValue = aValueAttribute->value();
  return true;
}

bool findVariable(const std::string& theName, double& outValue, ResultParameterPtr& theParam,
                  const DocumentPtr& theDocument /*= DocumentPtr()*/)
{
  SessionPtr aSession = ModelAPI_Session::get();
  std::list<DocumentPtr> aDocList;
  DocumentPtr aDocument = theDocument.get() ? theDocument : aSession->activeDocument();
  DocumentPtr aRootDocument = aSession->moduleDocument();
  aDocList.push_back(aDocument);
  if (aDocument != aRootDocument) {
    aDocList.push_back(aRootDocument);
  }
  for(std::list<DocumentPtr>::const_iterator it = aDocList.begin(); it != aDocList.end(); ++it) {
    if (findVariable(*it, theName, outValue, theParam))
      return true;
  }
  return false;
}

static std::map<int, std::vector<int> > myColorMap;

void appendValues(std::vector<int>& theRGB, const int theRed, const int theGreen, const int theBlue)
{
  theRGB.push_back(theRed);
  theRGB.push_back(theGreen);
  theRGB.push_back(theBlue);
}

bool containsValues(std::map<int, std::vector<int> >& theColorMap, std::vector<int>& theValues)
{
  std::map<int, std::vector<int> >::const_iterator anIt = theColorMap.begin(), aLast = theColorMap.end();
  bool isFound = false;
  for (; anIt != aLast && !isFound; anIt++) {
    std::vector<int> aValues = anIt->second;
    isFound = aValues[0] == theValues[0] &&
              aValues[1] == theValues[1] &&
              aValues[2] == theValues[2];
  }
  return isFound;
}

std::vector<int> HSVtoRGB(int theH, int theS, int theV)
{
  std::vector<int> aRGB;
  if (theH < 0 || theH > 360 ||
      theS < 0 || theS > 100 ||
      theV < 0 || theV > 100)
    return aRGB;

  int aHi = (int)theH/60;

  double aV = theV;
  double aVmin = (100 - theS)*theV/100;

  double anA = (theV - aVmin)* (theH % 60) / 60;

  double aVinc = aVmin + anA;
  double aVdec = theV - anA;

  double aPercentToValue = 255./100;
  int aV_int    = (int)(aV*aPercentToValue);
  int aVinc_int = (int)(aVinc*aPercentToValue);
  int aVmin_int = (int)(aVmin*aPercentToValue);
  int aVdec_int = (int)(aVdec*aPercentToValue);

  switch(aHi) {
    case 0: appendValues(aRGB, aV_int,    aVinc_int, aVmin_int); break;
    case 1: appendValues(aRGB, aVdec_int, aV_int,    aVmin_int); break;
    case 2: appendValues(aRGB, aVmin_int, aV_int,    aVinc_int); break;
    case 3: appendValues(aRGB, aVmin_int, aVdec_int, aV_int); break;
    case 4: appendValues(aRGB, aVinc_int, aVmin_int, aV_int); break;
    case 5: appendValues(aRGB, aV_int,    aVmin_int, aVdec_int); break;
    default: break;
  }
  return aRGB;
}


void fillColorMap()
{
  if (!myColorMap.empty())
    return;

  int i = 0;
  for (int s = 100; s > 0; s = s - 50)
  {
    for (int v = 100; v >= 40; v = v - 20)
    {
      for (int h = 0; h < 359 ; h = h + 60)
      {
        std::vector<int> aColor = HSVtoRGB(h, s, v);
        if (containsValues(myColorMap, aColor))
          continue;
        myColorMap[i] = aColor;
        i++;
      }
    }
  }
}

void findRandomColor(std::vector<int>& theValues)
{
  theValues.clear();
  if (myColorMap.empty()) {
    fillColorMap();
  }

  int aSize = myColorMap.size();
  int anIndex = rand() % aSize;
  if (myColorMap.find(anIndex) != myColorMap.end()) {
    theValues = myColorMap.at(anIndex);
  }
}

ResultPtr findPartResult(const DocumentPtr& theMain, const DocumentPtr& theSub)
{
  if (theMain != theSub) { // to optimize and avoid of crash on partset document close (don't touch the sub-document structure)
    for (int a = theMain->size(ModelAPI_ResultPart::group()) - 1; a >= 0; a--) {
      ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(
          theMain->object(ModelAPI_ResultPart::group(), a));
      if (aPart && aPart->data()->document(ModelAPI_ResultPart::DOC_REF())->value() == theSub) {
        return aPart;
      }
    }
  }
  return ResultPtr();
}

FeaturePtr findPartFeature(const DocumentPtr& theMain, const DocumentPtr& theSub)
{
  if (theMain != theSub) { // to optimize and avoid of crash on partset document close (don't touch the sub-document structure)
    for (int a = theMain->size(ModelAPI_Feature::group()) - 1; a >= 0; a--) {
      FeaturePtr aPartFeat = std::dynamic_pointer_cast<ModelAPI_Feature>(
          theMain->object(ModelAPI_Feature::group(), a));
      if (aPartFeat.get()) {
        const std::list<std::shared_ptr<ModelAPI_Result> >& aResList = aPartFeat->results();
        std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aRes = aResList.begin();
        for(; aRes != aResList.end(); aRes++) {
          ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(*aRes);
          if (aPart.get()) {
            if (aPart->isActivated() && aPart->partDoc() == theSub)
              return aPartFeat;
          } else break; // if the first is not Part, others are also not
        }
      }
    }
  }
  return FeaturePtr();
}

CompositeFeaturePtr compositeOwner(const FeaturePtr& theFeature)
{
  if (theFeature.get() && theFeature->data()->isValid()) {
    const std::set<std::shared_ptr<ModelAPI_Attribute> > aRefs = theFeature->data()->refsToMe();
    std::set<std::shared_ptr<ModelAPI_Attribute> >::const_iterator aRefIter = aRefs.begin();
    for(; aRefIter != aRefs.end(); aRefIter++) {
      CompositeFeaturePtr aComp = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>
        ((*aRefIter)->owner());
      if (aComp.get() && aComp->data()->isValid() && aComp->isSub(theFeature))
        return aComp;
    }
  }
  return CompositeFeaturePtr(); // not found
}

ResultCompSolidPtr compSolidOwner(const ResultPtr& theSub)
{
  ResultBodyPtr aBody = std::dynamic_pointer_cast<ModelAPI_ResultBody>(theSub);
  if (aBody.get()) {
    FeaturePtr aFeatureOwner = aBody->document()->feature(aBody);
    if (aFeatureOwner.get()) {
      std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aResIter = 
        aFeatureOwner->results().cbegin();
      for(; aResIter != aFeatureOwner->results().cend(); aResIter++) {
        ResultCompSolidPtr aComp = std::dynamic_pointer_cast<ModelAPI_ResultCompSolid>(*aResIter);
        if (aComp && aComp->isSub(aBody))
          return aComp;
      }
    }
  }
  return ResultCompSolidPtr(); // not found
}

bool hasSubResults(const ResultPtr& theResult)
{
  ResultCompSolidPtr aCompSolid = std::dynamic_pointer_cast<ModelAPI_ResultCompSolid>(theResult);
  return aCompSolid.get() && aCompSolid->numberOfSubs() > 0;
}

void allResults(const FeaturePtr& theFeature, std::list<ResultPtr>& theResults)
{
  const std::list<std::shared_ptr<ModelAPI_Result> >& aResults = theFeature->results();
  std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aRIter = aResults.begin();
  for (; aRIter != aResults.cend(); aRIter++) {
    theResults.push_back(*aRIter);
    // iterate sub-bodies of compsolid
    ResultCompSolidPtr aComp = std::dynamic_pointer_cast<ModelAPI_ResultCompSolid>(*aRIter);
    if (aComp.get()) {
      int aNumSub = aComp->numberOfSubs();
      for(int a = 0; a < aNumSub; a++) {
        theResults.push_back(aComp->subResult(a));
      }
    }
  }
}

} // namespace ModelAPI_Tools


