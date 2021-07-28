// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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
#include <OperaPlugin_Volume.h>

#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeIntArray.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_Tools.h>

#include <GeomAlgoAPI_Copy.h>
#include <GeomAlgoAPI_Tools.h>

#include <sstream>
#include <memory>
#include <iostream>

//=================================================================================================
OperaPlugin_Volume::OperaPlugin_Volume() // Nothing to do during instantiation
{
}

//=================================================================================================
static GeomShapePtr shapeOfSelection(AttributeSelectionPtr theSel) {
  GeomShapePtr aResult;
  FeaturePtr aSelFeature = theSel->contextFeature();
  if (aSelFeature.get()) {
    if (aSelFeature->results().empty()) // if selected feature has no results, make nothing
      return aResult;
    if (aSelFeature->results().size() == 1) { // for one sub-result don't make compound
      aResult = aSelFeature->firstResult()->shape();
    }
  }
  if (!aResult.get())
    aResult = theSel->value();
  if (!aResult.get()) {
    if (theSel->context().get())
      aResult = theSel->context()->shape();
  }
  return aResult;
}

//=================================================================================================
void OperaPlugin_Volume::initAttributes()
{
  //Get Medium
  data()->addAttribute(MEDIUM(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(VOLUME_LIST_ID(), ModelAPI_AttributeSelectionList::typeId());

  // // //Get Objects
  // AttributeSelectionListPtr aList = std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(
  //   data()->addAttribute(LIST_ID(), ModelAPI_AttributeSelectionList::typeId()));
  // aList->setWholeResultAllowed(true); // allow to select the whole result
}

//=================================================================================================
void OperaPlugin_Volume::execute()
{
  std::string aCopiesMedium = string(MEDIUM())->value();
  if (aCopiesMedium.empty())
  {
    setError("Error: Medium cannot be empty.");
    return;
  }

  int aResultIndex = 0;

  AttributeSelectionListPtr aList = selectionList(VOLUME_LIST_ID());

  for (int aSelIndex = 0; aSelIndex < aList->size(); aSelIndex++) {
    AttributeSelectionPtr aSel = aList->value(aSelIndex);
    std::shared_ptr<ModelAPI_Result> aResult = aSel->context();

    std::set<std::wstring> anExistingNames;
    std::wstring aBaseName = aResult->data()->name();
    std::wstring aName;
    int anInd = 0;
    do {
      anInd++;
      std::wostringstream aNameStr;
      aNameStr << "Volume_" << aBaseName;
      aName = aNameStr.str();
    } while (anExistingNames.count(aName));
    anExistingNames.insert(aName);

    ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);
    aResultBody->data()->setName(aName);
    aResultBody->store(aResult->shape());
    setResult(aResultBody, aResultIndex++);
    aResult->setDisabled(aResult, true);
  }
}
