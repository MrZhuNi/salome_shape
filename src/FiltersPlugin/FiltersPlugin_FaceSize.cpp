// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#include "FiltersPlugin_FaceSize.h"

#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeDouble.h>

#include <ModelAPI_Tools.h>

#include <GeomAPI_Edge.h>
#include <GeomAPI_Shape.h>
#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAPI_Wire.h>
#include <GeomAlgoAPI_BasicProperties.h>

#include <map>
#include <iostream>


bool FiltersPlugin_FaceSize::isSupported(GeomAPI_Shape::ShapeType theType) const
{
  return theType == GeomAPI_Shape::FACE;
}

bool FiltersPlugin_FaceSize::isOk(const GeomShapePtr& theShape, const ResultPtr&,
                                  const ModelAPI_FiltersArgs& theArgs) const
{
  AttributePtr anAttr = theArgs.argument("value");
  AttributeDoublePtr aValue = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(anAttr);
  if (!aValue.get()|| !anAttr->isInitialized() )
    return false;
  double aVal = aValue->value();

  anAttr = theArgs.argument("valueMin");
  aValue = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(anAttr);
  if (!aValue.get()|| !anAttr->isInitialized() )
    return false;
  double aValMin = aValue->value();

  anAttr = theArgs.argument("valueMax");
  aValue = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(anAttr);
  if (!aValue.get()|| !anAttr->isInitialized() )
    return false;
  double aValMax = aValue->value();

  double aTolerance = 0.0001;
  double aLength;
  double aSurfArea;
  double aVolume;
  std::string aError;
  if( !GetBasicProperties(theShape,
                          aTolerance,
                          aLength,
                          aSurfArea,
                          aVolume,
                          aError) )
      return false;

  anAttr = theArgs.argument("comparatorType");
  AttributeStringPtr aCompAttr = std::dynamic_pointer_cast<ModelAPI_AttributeString>(anAttr);
  if (!aCompAttr)
    return false;
  std::string aCompString = aCompAttr->value();

  bool isOK = false;
  if (aCompString == "inf")
    isOK = aSurfArea < aVal;
  else if (aCompString == "infEq")
    isOK = aSurfArea <= aVal;
  else if (aCompString == "sup")
    isOK = aSurfArea > aVal;
  else if (aCompString == "supEq")
    isOK = aSurfArea >= aVal;
  else if (aCompString == "isBetween")
    isOK = (aSurfArea >= aValMin) && (aSurfArea <= aValMax);
  else if (aCompString == "isStrictlyBetween")
    isOK = (aSurfArea > aValMin) && (aSurfArea < aValMax);
  return isOK;
}

std::string FiltersPlugin_FaceSize::xmlRepresentation() const
{
  return xmlFromFile("filter-FaceSize.xml");
}

void FiltersPlugin_FaceSize::initAttributes(ModelAPI_FiltersArgs& theArguments)
{
  theArguments.initAttribute("comparatorType", ModelAPI_AttributeString::typeId());
  theArguments.initAttribute("value", ModelAPI_AttributeDouble::typeId());
  theArguments.initAttribute("valueMin", ModelAPI_AttributeDouble::typeId());
  theArguments.initAttribute("valueMax", ModelAPI_AttributeDouble::typeId());
}

