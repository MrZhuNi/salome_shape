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

#include "FeaturesPlugin_CommonSharedFaces.h"

#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_Attribute.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_Validator.h>

#include <GeomAPI_ShapeIterator.h>

#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAlgoAPI_SharedFaces.h>

#include <sstream>

//=================================================================================================
void FeaturesPlugin_CommonSharedFaces::updateFaces()
{
  AttributeSelectionPtr aCompSolidAttr =
              std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(attributObject());
  AttributeSelectionListPtr aFacesListAttr =
              std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList> (attributListFaces());

  GeomShapePtr aShape = aCompSolidAttr->value();
  AttributeBooleanPtr anIsCompute =
            std::dynamic_pointer_cast<ModelAPI_AttributeBoolean>(attributIsCompute());
  if (!anIsCompute->value()) {
    myShape = aShape;
    anIsCompute->setValue(true);
  }
  if (aShape.get() && aCompSolidAttr->context().get() && !aShape->isEqual(myShape)) {
    if (aFacesListAttr->isInitialized())
        aFacesListAttr->clear();
    aShape = aCompSolidAttr->context()->shape();
    if (aShape) {
      std::string anError;
      ListOfShape aFaces;
      ListOfShape theShapes;
      theShapes.push_back(aShape);

      if (!GetSharedFaces(theShapes, aFaces, false, anError))
        setError("Error in GetSharedFaces calculation :" +  anError);
      myShape = aShape;
      aFacesListAttr->setSelectionType("face");
      ListOfShape::const_iterator anIt = aFaces.cbegin();
      for(; anIt != aFaces.cend(); ++anIt) {
        GeomShapePtr aFacePtr = *anIt;
        if (!aFacePtr.get())
          setError("GetSharedFaces : An invalid face found " +  anError);
        aFacesListAttr->append( aCompSolidAttr->context(), aFacePtr);
      }
      std::stringstream aLabel;
      aLabel << aFacesListAttr->size();
      AttributeStringPtr aNumberFacesAttr =
              std::dynamic_pointer_cast<ModelAPI_AttributeString>
                                                      (attributNumberFaces());
      aNumberFacesAttr->setValue( aLabel.str() );
    }
  }
}

//=================================================================================================
void FeaturesPlugin_CommonSharedFaces::setFacesGroup(const std::wstring& theName )
{
  std::vector<int> aColor;
  ResultGroupPtr aGroup = document()->createGroup(data());
  // clean the result of the operation
  aGroup->data()->setName(theName);
  aGroup->store(GeomShapePtr());

  // shapes containing in group
  ListOfShape aFaces;
  AttributeSelectionListPtr aFacesListAttr =
              std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList> (attributListFaces());

  for (int anI =0; anI< aFacesListAttr->size(); anI++) {
    AttributeSelectionPtr aAtt = aFacesListAttr->value(anI);
    aFaces.push_back( aAtt->value() );
  }

  GeomShapePtr aCompound = GeomAlgoAPI_CompoundBuilder::compound(aFaces);
  aGroup->store(aCompound);
  aColor = {255,0,0};
  setResult(aGroup);
  ModelAPI_Tools::setColor( lastResult(),aColor);
}

