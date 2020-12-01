// Copyright (C) 2018-2020  CEA/DEN, EDF R&D
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

#include "FeaturesPlugin_SharedFaces.h"

#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_Attribute.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>
#include <Locale_Convert.h>

#include <GeomAlgoAPI_CompoundBuilder.h>

#include <Config_PropManager.h>
#include <ModelAPI_ResultBody.h>
#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAlgoAPI_SharedFaces.h>
#include <GeomAPI_ShapeIterator.h>
#include <ModelAPI_Tools.h>
#include <iomanip>
#include <sstream>

//=================================================================================================
FeaturesPlugin_SharedFaces::FeaturesPlugin_SharedFaces()
{
}

//=================================================================================================
void FeaturesPlugin_SharedFaces::initAttributes()
{
  // attribute for object selected
  data()->addAttribute(OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
  AttributeSelectionListPtr aList = std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(
    data()->addAttribute(LIST_FACES_ID(), ModelAPI_AttributeSelectionList::typeId()));

  data()->addAttribute(NUMBER_FACES_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(TRANSPARENCY_ID(), ModelAPI_AttributeInteger::typeId());
  data()->addAttribute(CREATE_GROUP_ID(), ModelAPI_AttributeBoolean::typeId());
  data()->addAttribute(GROUP_NAME_ID(), ModelAPI_AttributeString::typeId());

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), GROUP_NAME_ID());
  boolean(CREATE_GROUP_ID())->setValue(false);
}

void explodeCompound(const GeomShapePtr& theCompound, ListOfShape& theSubs)
{
  if (theCompound->isCompound() || theCompound->isCompSolid()) {
    GeomAPI_ShapeIterator anIt(theCompound);
    for (; anIt.more(); anIt.next())
      explodeCompound(anIt.current(), theSubs);
  } else
    theSubs.push_back(theCompound);
}

//=================================================================================================
void FeaturesPlugin_SharedFaces::execute()
{
  if (boolean(CREATE_GROUP_ID())->value()
    && selectionList(LIST_FACES_ID())->isInitialized()
    && string(GROUP_NAME_ID())->value() != "") {
    AttributeStringPtr aNameAtt = string( GROUP_NAME_ID() ) ;
    std::wstring aNameFace = aNameAtt->isUValue() ?
                            Locale::Convert::toWString(aNameAtt->valueU()) :
                            Locale::Convert::toWString(aNameAtt->value());

    if (myGroup.get())
      eraseResults();
    setFacesGroup(aNameFace);

  } else {
    if (myGroup.get()) {
      eraseResults();
      myGroup.reset();
    }

  }
  if (selection(OBJECT_ID())->isInitialized()) {
    AttributeSelectionPtr ancompSolidAttr = selection(OBJECT_ID());
    ResultPtr aResult = ancompSolidAttr->context();

    double aTranparency = integer(TRANSPARENCY_ID())->value()/100.0;
    ModelAPI_Tools::setTransparency(aResult, aTranparency);

    ResultBodyPtr aResultBody = std::dynamic_pointer_cast<ModelAPI_ResultBody>(aResult);
    std::list<ResultPtr> allRes;
    ModelAPI_Tools::allSubs(aResultBody, allRes);
    std::list<ResultPtr>::iterator aRes;
    for(aRes = allRes.begin(); aRes != allRes.end(); aRes++) {
      ModelAPI_Tools::setTransparency(*aRes, aTranparency);
    }
  }
}

//=================================================================================================
void FeaturesPlugin_SharedFaces::attributeChanged(const std::string& theID)
{
  if (theID == OBJECT_ID()) {
    AttributeSelectionPtr ancompSolidAttr = selection(OBJECT_ID());

    GeomShapePtr aShape = ancompSolidAttr->value();
    if (aShape.get() && ancompSolidAttr->context().get()) {

      aShape = ancompSolidAttr->context()->shape();
      if (aShape) {
        std::string anError;
        ListOfShape aFaces;
        ListOfShape theSubs;
        explodeCompound(aShape, theSubs);
        if( !GetSharedredFaces( theSubs,
                                aFaces,
                                true,
                                anError))
          setError("Error in GetSharedredFaces calculation :" +  anError);

        AttributeSelectionListPtr aFacesListAttr =
                    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>
                                                        (attribute(LIST_FACES_ID()));

        if (aFacesListAttr->isInitialized())
          aFacesListAttr->clear();

        aFacesListAttr->setSelectionType("face");

        ListOfShape::const_iterator anIt = aFaces.cbegin();
        for(; anIt != aFaces.cend(); ++anIt) {

          GeomShapePtr aFacePtr = *anIt;

          if (!aFacePtr.get()) {
            setError("GetSharedredFaces : An invalid face found " +  anError);
          }
          aFacesListAttr->append( ancompSolidAttr->context(), aFacePtr);
        }
        std::stringstream alabel;
        alabel << "Number of shared faces : " << aFacesListAttr->size();
        string(NUMBER_FACES_ID() )->setValue( alabel.str() );

      }
    }
  }
}

//=================================================================================================
void FeaturesPlugin_SharedFaces::setFacesGroup(const std::wstring& theName )
{
  std::vector<int> aColor;
  myGroup = document()->createGroup(data());
  // clean the result of the operation
  myGroup->data()->setName(theName);
  myGroup->store(GeomShapePtr());

  // shapes containing in group
  ListOfShape aFaces;
  AttributeSelectionListPtr aFacesListAttr =
                    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>
                                                        (attribute(LIST_FACES_ID()));

  for (int anI =0; anI< aFacesListAttr->size(); anI++) {
    AttributeSelectionPtr aAtt = aFacesListAttr->value(anI);
    aFaces.push_back( aAtt->value() );
  }
  GeomShapePtr aCompound = GeomAlgoAPI_CompoundBuilder::compound(aFaces);
  myGroup->store(aCompound);
  aColor = {255,0,0};
  setResult(myGroup);
  ModelAPI_Tools::setColor( lastResult(),aColor);

}

