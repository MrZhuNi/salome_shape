// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include "FeaturesPlugin_ValidatorTransform.h"

#include <Events_InfoMessage.h>

#include "ModelAPI_AttributeSelectionList.h"
#include "ModelAPI_ResultPart.h"
#include "ModelAPI_ResultBody.h"
#include "ModelAPI_ResultCompSolid.h"
#include "ModelAPI_Session.h"

bool FeaturesPlugin_ValidatorTransform::isValid(const AttributePtr& theAttribute,
                                                const std::list<std::string>& theArguments,
                                                Events_InfoMessage& theError) const
{
  bool aValid = true;
  std::string anAttributeType = theAttribute->attributeType();
  if (anAttributeType != ModelAPI_AttributeSelectionList::typeId()) {
    theError = "The attribute with the %1 type is not processed";
    theError.arg(theAttribute->attributeType());
    return false;
  }

  std::shared_ptr<ModelAPI_AttributeSelectionList> aCurSelList = 
                           std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);

  DocumentPtr aDocument = theAttribute->owner()->document();
  SessionPtr aMgr = ModelAPI_Session::get();
  bool isPartSetDocument = aDocument == aMgr->moduleDocument();

  std::string anErrorGroupName;
  for(int i = 0; i < aCurSelList->size() && aValid; i++) {
    std::shared_ptr<ModelAPI_AttributeSelection> aSelAttribute = aCurSelList->value(i);
    ResultPtr aResult = aSelAttribute->context();
    if (isPartSetDocument) // PartSet document: Result Part is valid
      aValid = aResult->groupName() == ModelAPI_ResultPart::group();
    else { // Part document: Result CompSolid is valid
      aValid = aResult->groupName() == ModelAPI_ResultBody::group();
      if (aValid) {
        ResultCompSolidPtr aComp = std::dynamic_pointer_cast<ModelAPI_ResultCompSolid>(aResult);
        aValid = aComp.get() != NULL;
      }
    }
    if (!aValid)
      anErrorGroupName = aResult->groupName();
  }
  if (!aValid) {
    std::string aResultGroupName = isPartSetDocument ? ModelAPI_ResultPart::group()
                                                     : ModelAPI_ResultBody::group();
    theError = "Objects from the %1 group can be selected in the %2 document, but an objects from the %3 group is selected.";
    theError.arg(aResultGroupName).arg(aDocument->kind()).arg(anErrorGroupName);
  }
  return aValid;
}
