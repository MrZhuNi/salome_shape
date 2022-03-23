// Copyright (C) 2018-2021  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "FeaturesPlugin_DuplicatedFaces.h"

#include <FeaturesPlugin_GroupDuplicatedFaces.h>

#include <ModelAPI_Attribute.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_Validator.h>

#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_ShapeTools.h>

#include <Locale_Convert.h>
#include <Config_PropManager.h>

//=================================================================================================
FeaturesPlugin_DuplicatedFaces::FeaturesPlugin_DuplicatedFaces()
{
}

void FeaturesPlugin_DuplicatedFaces::initAttributes()
{
  // attribute for object selected
  data()->addAttribute(OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
  AttributeSelectionListPtr aList = std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(
    data()->addAttribute(LIST_FACES_ID(), ModelAPI_AttributeSelectionList::typeId()));

  data()->addAttribute(NUMBER_FACES_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(TRANSPARENCY_ID(), ModelAPI_AttributeInteger::typeId());
  data()->addAttribute(TOLERANCE_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(CREATE_GROUP_ID(), ModelAPI_AttributeBoolean::typeId());
  data()->addAttribute(GROUP_NAME_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(COMPUTE_ID(), ModelAPI_AttributeBoolean::typeId());


  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), GROUP_NAME_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), COMPUTE_ID());
  data()->boolean(COMPUTE_ID())->setValue(true);
}

//=================================================================================================
AttributePtr FeaturesPlugin_DuplicatedFaces::attributIsCompute()
{
  return attribute(COMPUTE_ID());
}

//=================================================================================================
AttributePtr FeaturesPlugin_DuplicatedFaces::attributObject()
{
  return attribute(OBJECT_ID());
}

//=================================================================================================
AttributePtr FeaturesPlugin_DuplicatedFaces::attributListFaces()
{
  return attribute(LIST_FACES_ID());
}

//=================================================================================================
AttributePtr FeaturesPlugin_DuplicatedFaces::attributNumberFaces()
{
  return attribute(NUMBER_FACES_ID());
}

//=================================================================================================
AttributePtr FeaturesPlugin_DuplicatedFaces::attributTolerance()
{
  return attribute(TOLERANCE_ID());
}


//=================================================================================================
void FeaturesPlugin_DuplicatedFaces::execute()
{
    if (boolean(CREATE_GROUP_ID())->value()) {

    if (string(GROUP_NAME_ID())->value() != ""
      && selectionList(LIST_FACES_ID())->isInitialized()) {

      if (lastResult().get()) {
        eraseResultFromList(lastResult());
      }

      if (!myCreateGroupFeature.get())
        createGroup();
      updateGroup();
    }

  } else {
    if (selectionList(LIST_FACES_ID())->isInitialized()) {

      if (myCreateGroupFeature.get()) {
        myCreateGroupFeature->eraseResults();
        SessionPtr aSession = ModelAPI_Session::get();
        DocumentPtr aDoc =  aSession->activeDocument();
        aDoc->removeFeature(myCreateGroupFeature);
        myCreateGroupFeature.reset();
      }

      if (lastResult().get())
        eraseResultFromList(lastResult());
      setFacesGroup(L"Group_DuplicatedFaces");
    }
  }

  if (selection(OBJECT_ID())->isInitialized()) {
    AttributeSelectionPtr aCompSolidAttr = selection(OBJECT_ID());
    ResultPtr aResult = aCompSolidAttr->context();

    double aTranparency = integer(TRANSPARENCY_ID())->value()/100.0;
    ModelAPI_Tools::setTransparency(aResult, aTranparency);

    ResultBodyPtr aResultBody = std::dynamic_pointer_cast<ModelAPI_ResultBody>(aResult);
    std::list<ResultPtr> allRes;
    ModelAPI_Tools::allSubs(aResultBody, allRes);
    std::list<ResultPtr>::iterator aRes;
    for (aRes = allRes.begin(); aRes != allRes.end(); aRes++) {
      ModelAPI_Tools::setTransparency(*aRes, aTranparency);
    }
  }
}

//=================================================================================================
void FeaturesPlugin_DuplicatedFaces::attributeChanged(const std::string& theID)
{
  if (theID == OBJECT_ID()
     || theID == TOLERANCE_ID()) {
    updateFaces();
    if (myCreateGroupFeature.get())
      updateGroup();
  }
}

//=================================================================================================
void FeaturesPlugin_DuplicatedFaces::createGroup()
{
  SessionPtr aSession = ModelAPI_Session::get();

  DocumentPtr aDoc =  aSession->activeDocument();

  if (aDoc.get()) {
    myCreateGroupFeature = aDoc->addFeature(FeaturesPlugin_GroupDuplicatedFaces::ID());
  }
}

//=================================================================================================
void FeaturesPlugin_DuplicatedFaces::updateGroup()
{
    myCreateGroupFeature->
                      boolean(FeaturesPlugin_GroupDuplicatedFaces::COMPUTE_ID())->setValue(false);
    myCreateGroupFeature->string(FeaturesPlugin_GroupDuplicatedFaces::GROUP_NAME_ID())
                        ->setValue(string(GROUP_NAME_ID())->value());

    myCreateGroupFeature->selection(FeaturesPlugin_GroupDuplicatedFaces::OBJECT_ID())
                        ->setValue(selection(OBJECT_ID())->context(),
                                   selection(OBJECT_ID())->value());

    AttributeSelectionListPtr aFacesFeatures =
      std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>
                               (myCreateGroupFeature->attribute(LIST_FACES_ID()));
    AttributeSelectionListPtr aFaces =
      std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(attribute(LIST_FACES_ID()));

    aFaces->copyTo(aFacesFeatures);

    myCreateGroupFeature->integer(FeaturesPlugin_GroupDuplicatedFaces::TRANSPARENCY_ID())
                          ->setValue(integer(TRANSPARENCY_ID())->value());

    myCreateGroupFeature->real(FeaturesPlugin_GroupDuplicatedFaces::TOLERANCE_ID())
                          ->setValue(real(TOLERANCE_ID())->value());

    myCreateGroupFeature->execute();
    myCreateGroupFeature
                      ->boolean(FeaturesPlugin_GroupDuplicatedFaces::COMPUTE_ID())->setValue(true);
}
