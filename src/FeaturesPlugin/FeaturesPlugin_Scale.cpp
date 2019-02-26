// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#include <FeaturesPlugin_Scale.h>

#include <GeomAlgoAPI_PointBuilder.h>
#include <GeomAlgoAPI_Tools.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultPart.h>

#include <FeaturesPlugin_Tools.h>

//=================================================================================================
FeaturesPlugin_Scale::FeaturesPlugin_Scale()
{
}

//=================================================================================================
void FeaturesPlugin_Scale::initAttributes()
{
  data()->addAttribute(FeaturesPlugin_Scale::CREATION_METHOD(),
                       ModelAPI_AttributeString::typeId());

  AttributeSelectionListPtr aSelection =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(data()->addAttribute(
    FeaturesPlugin_Scale::OBJECTS_LIST_ID(), ModelAPI_AttributeSelectionList::typeId()));

  data()->addAttribute(FeaturesPlugin_Scale::CENTER_POINT_ID(),
                       ModelAPI_AttributeSelection::typeId());

  data()->addAttribute(FeaturesPlugin_Scale::SCALE_FACTOR_ID(),
                       ModelAPI_AttributeDouble::typeId());

  data()->addAttribute(FeaturesPlugin_Scale::SCALE_FACTOR_X_ID(),
                       ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(FeaturesPlugin_Scale::SCALE_FACTOR_Y_ID(),
                       ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(FeaturesPlugin_Scale::SCALE_FACTOR_Z_ID(),
                       ModelAPI_AttributeDouble::typeId());
}

//=================================================================================================
void FeaturesPlugin_Scale::execute()
{
  AttributeStringPtr aMethodTypeAttr = string(FeaturesPlugin_Scale::CREATION_METHOD());
  std::string aMethodType = aMethodTypeAttr->value();

  if (aMethodType == FeaturesPlugin_Scale::CREATION_METHOD_BY_FACTOR()) {
    performScaleByFactor();
  }

  if (aMethodType == FeaturesPlugin_Scale::CREATION_METHOD_BY_DIMENSIONS()) {
    performScaleByDimensions();
  }
}

//=================================================================================================
void FeaturesPlugin_Scale::performScaleByFactor()
{
  // Getting objects.
  ListOfShape anObjects;
  std::list<ResultPtr> aContextes;
  AttributeSelectionListPtr anObjectsSelList =
    selectionList(FeaturesPlugin_Scale::OBJECTS_LIST_ID());
  if (anObjectsSelList->size() == 0) {
    return;
  }
  for(int anObjectsIndex = 0; anObjectsIndex < anObjectsSelList->size(); anObjectsIndex++) {
    std::shared_ptr<ModelAPI_AttributeSelection> anObjectAttr =
      anObjectsSelList->value(anObjectsIndex);
    std::shared_ptr<GeomAPI_Shape> anObject = anObjectAttr->value();
    if(!anObject.get()) { // may be for not-activated parts
      return;
    }
    anObjects.push_back(anObject);
    aContextes.push_back(anObjectAttr->context());
  }

  // Getting the center point
  std::shared_ptr<GeomAPI_Pnt> aCenterPoint;
  std::shared_ptr<ModelAPI_AttributeSelection> anObjRef =
    selection(FeaturesPlugin_Scale::CENTER_POINT_ID());
  if (anObjRef.get() != NULL) {
    GeomShapePtr aShape = anObjRef->value();
    if (!aShape.get()) {
      aShape = anObjRef->context()->shape();
    }
    if (aShape) {
      aCenterPoint = GeomAlgoAPI_PointBuilder::point(aShape);
    }
  }

  // Getting scale factor
  double aScaleFactor = real(FeaturesPlugin_Scale::SCALE_FACTOR_ID())->value();

  // Moving each object.
  std::string anError;
  int aResultIndex = 0;
  std::list<ResultPtr>::iterator aContext = aContextes.begin();
  for(ListOfShape::iterator anObjectsIt = anObjects.begin(); anObjectsIt != anObjects.end();
        anObjectsIt++, aContext++) {
    std::shared_ptr<GeomAPI_Shape> aBaseShape = *anObjectsIt;
    std::shared_ptr<GeomAlgoAPI_Scale> aScaleAlgo(
      new GeomAlgoAPI_Scale(aBaseShape, aCenterPoint, aScaleFactor));

    if (!aScaleAlgo->check()) {
      setError(aScaleAlgo->getError());
      return;
    }

    aScaleAlgo->build();

    // Checking that the algorithm worked properly.
    if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aScaleAlgo, getKind(), anError)) {
      setError(anError);
      break;
    }

    ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);

    ListOfShape aShapes;
    aShapes.push_back(aBaseShape);
    FeaturesPlugin_Tools::loadModifiedShapes(aResultBody,
                                             aShapes,
                                             ListOfShape(),
                                             aScaleAlgo,
                                             aScaleAlgo->shape(),
                                             "Scaled");
    setResult(aResultBody, aResultIndex);
    aResultIndex++;
  }

  // Remove the rest results if there were produced in the previous pass.
  removeResults(aResultIndex);
}

//=================================================================================================
void FeaturesPlugin_Scale::performScaleByDimensions()
{
  // Getting objects.
  ListOfShape anObjects;
  std::list<ResultPtr> aContextes;
  AttributeSelectionListPtr anObjectsSelList =
    selectionList(FeaturesPlugin_Scale::OBJECTS_LIST_ID());
  if (anObjectsSelList->size() == 0) {
    return;
  }
  for(int anObjectsIndex = 0; anObjectsIndex < anObjectsSelList->size(); anObjectsIndex++) {
    std::shared_ptr<ModelAPI_AttributeSelection> anObjectAttr =
      anObjectsSelList->value(anObjectsIndex);
    std::shared_ptr<GeomAPI_Shape> anObject = anObjectAttr->value();
    if(!anObject.get()) { // may be for not-activated parts
      return;
    }
    anObjects.push_back(anObject);
    aContextes.push_back(anObjectAttr->context());
  }

  // Getting the center point
  std::shared_ptr<GeomAPI_Pnt> aCenterPoint;
  std::shared_ptr<ModelAPI_AttributeSelection> anObjRef =
    selection(FeaturesPlugin_Scale::CENTER_POINT_ID());
  if (anObjRef.get() != NULL) {
    GeomShapePtr aShape = anObjRef->value();
    if (!aShape.get()) {
      aShape = anObjRef->context()->shape();
    }
    if (aShape) {
      aCenterPoint = GeomAlgoAPI_PointBuilder::point(aShape);
    }
  }

  // Getting dimensions
  double aScaleFactorX = real(FeaturesPlugin_Scale::SCALE_FACTOR_X_ID())->value();
  double aScaleFactorY = real(FeaturesPlugin_Scale::SCALE_FACTOR_Y_ID())->value();
  double aScaleFactorZ = real(FeaturesPlugin_Scale::SCALE_FACTOR_Z_ID())->value();

  // Moving each object.
  std::string anError;
  int aResultIndex = 0;
  std::list<ResultPtr>::iterator aContext = aContextes.begin();
  for(ListOfShape::iterator anObjectsIt = anObjects.begin(); anObjectsIt != anObjects.end();
        anObjectsIt++, aContext++) {
    std::shared_ptr<GeomAPI_Shape> aBaseShape = *anObjectsIt;
    std::shared_ptr<GeomAlgoAPI_Scale> aScaleAlgo(new GeomAlgoAPI_Scale(aBaseShape,
                                                                        aCenterPoint,
                                                                        aScaleFactorX,
                                                                        aScaleFactorY,
                                                                        aScaleFactorZ));

    if (!aScaleAlgo->check()) {
      setError(aScaleAlgo->getError());
      return;
    }

    aScaleAlgo->build();

    // Checking that the algorithm worked properly.
    if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aScaleAlgo, getKind(), anError)) {
      setError(anError);
      break;
    }

    ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);

    ListOfShape aShapes;
    aShapes.push_back(aBaseShape);
    FeaturesPlugin_Tools::loadModifiedShapes(aResultBody,
                                             aShapes,
                                             ListOfShape(),
                                             aScaleAlgo,
                                             aScaleAlgo->shape(),
                                             "Scaled");
    setResult(aResultBody, aResultIndex);
    aResultIndex++;
  }

  // Remove the rest results if there were produced in the previous pass.
  removeResults(aResultIndex);
}
