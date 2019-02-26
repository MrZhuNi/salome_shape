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

#include <FeaturesPlugin_Symmetry.h>

#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_PointBuilder.h>
#include <GeomAlgoAPI_FaceBuilder.h>
#include <GeomAlgoAPI_Copy.h>
#include <GeomAlgoAPI_MakeShapeList.h>
#include <GeomAlgoAPI_Tools.h>

#include <GeomAPI_Edge.h>
#include <GeomAPI_Face.h>
#include <GeomAPI_Lin.h>
#include <GeomAPI_Pln.h>
#include <GeomAPI_ShapeIterator.h>
#include <GeomAPI_Trsf.h>

#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultPart.h>

#include <FeaturesPlugin_Tools.h>

//=================================================================================================
FeaturesPlugin_Symmetry::FeaturesPlugin_Symmetry()
{
}

//=================================================================================================
void FeaturesPlugin_Symmetry::initAttributes()
{
  data()->addAttribute(FeaturesPlugin_Symmetry::CREATION_METHOD(),
                       ModelAPI_AttributeString::typeId());

  AttributeSelectionListPtr aSelection =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(data()->addAttribute(
    FeaturesPlugin_Symmetry::OBJECTS_LIST_ID(), ModelAPI_AttributeSelectionList::typeId()));

  data()->addAttribute(FeaturesPlugin_Symmetry::POINT_OBJECT_ID(),
                       ModelAPI_AttributeSelection::typeId());

  data()->addAttribute(FeaturesPlugin_Symmetry::AXIS_OBJECT_ID(),
                       ModelAPI_AttributeSelection::typeId());

  data()->addAttribute(FeaturesPlugin_Symmetry::PLANE_OBJECT_ID(),
                       ModelAPI_AttributeSelection::typeId());

  data()->addAttribute(FeaturesPlugin_Symmetry::KEEP_ORIGINAL_RESULT(),
                       ModelAPI_AttributeBoolean::typeId());
}

//=================================================================================================
void FeaturesPlugin_Symmetry::execute()
{
  AttributeStringPtr aMethodTypeAttr = string(FeaturesPlugin_Symmetry::CREATION_METHOD());
  std::string aMethodType = aMethodTypeAttr->value();

  if (aMethodType == CREATION_METHOD_BY_POINT()) {
    performSymmetryByPoint();
  }

  if (aMethodType == CREATION_METHOD_BY_AXIS()) {
    performSymmetryByAxis();
  }

  if (aMethodType == CREATION_METHOD_BY_PLANE()) {
    performSymmetryByPlane();
  }
}

//=================================================================================================
bool FeaturesPlugin_Symmetry::collectSourceObjects(ListOfShape& theSourceShapes,
                                                   std::list<ResultPtr>& theSourceResults)
{
  AttributeSelectionListPtr anObjectsSelList =
    selectionList(FeaturesPlugin_Symmetry::OBJECTS_LIST_ID());
  if (anObjectsSelList->size() == 0) {
    return false;
  }
  for (int anObjectsIndex = 0; anObjectsIndex < anObjectsSelList->size(); anObjectsIndex++) {
    std::shared_ptr<ModelAPI_AttributeSelection> anObjectAttr =
      anObjectsSelList->value(anObjectsIndex);
    std::shared_ptr<GeomAPI_Shape> anObject = anObjectAttr->value();
    if (!anObject.get()) { // may be for not-activated parts
      return false;
    }
    theSourceShapes.push_back(anObject);
    theSourceResults.push_back(anObjectAttr->context());
  }
  return true;
}

//=================================================================================================
void FeaturesPlugin_Symmetry::performSymmetryByPoint()
{
  // Getting objects.
  ListOfShape anObjects;
  std::list<ResultPtr> aContextes;
  if (!collectSourceObjects(anObjects, aContextes))
    return;

  //Getting point.
  std::shared_ptr<GeomAPI_Pnt> aPoint;
  std::shared_ptr<ModelAPI_AttributeSelection> anObjRef =
    selection(FeaturesPlugin_Symmetry::POINT_OBJECT_ID());
  if (anObjRef.get() != NULL) {
    GeomShapePtr aShape1 = anObjRef->value();
    if (!aShape1.get()) {
      aShape1 = anObjRef->context()->shape();
    }
    if (aShape1) {
      aPoint = GeomAlgoAPI_PointBuilder::point(aShape1);
    }
  }

  // Moving each object.
  std::string anError;
  int aResultIndex = 0;
  std::list<ResultPtr>::iterator aContext = aContextes.begin();
  for(ListOfShape::iterator anObjectsIt = anObjects.begin(); anObjectsIt != anObjects.end();
        anObjectsIt++, aContext++) {
    std::shared_ptr<GeomAPI_Shape> aBaseShape = *anObjectsIt;
    bool isPart = aContext->get() && (*aContext)->groupName() == ModelAPI_ResultPart::group();

    // Setting result.
    if (isPart) {
      std::shared_ptr<GeomAPI_Trsf> aTrsf(new GeomAPI_Trsf());
      aTrsf->setSymmetry(aPoint);
      ResultPartPtr anOrigin = std::dynamic_pointer_cast<ModelAPI_ResultPart>(*aContext);
      buildResult(anOrigin, aTrsf, aResultIndex);
    }
    else {
      std::shared_ptr<GeomAlgoAPI_Symmetry> aSymmetryAlgo(
        new GeomAlgoAPI_Symmetry(aBaseShape, aPoint));

      if (!aSymmetryAlgo->check()) {
        setError(aSymmetryAlgo->getError());
        return;
      }

      aSymmetryAlgo->build();

      // Checking that the algorithm worked properly.
      if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aSymmetryAlgo, getKind(), anError)) {
        setError(anError);
        break;
      }

      buildResult(aSymmetryAlgo, aBaseShape, aResultIndex);
    }
    aResultIndex++;
  }

  // Remove the rest results if there were produced in the previous pass.
  removeResults(aResultIndex);
}

//=================================================================================================
void FeaturesPlugin_Symmetry::performSymmetryByAxis()
{
  // Getting objects.
  ListOfShape anObjects;
  std::list<ResultPtr> aContextes;
  if (!collectSourceObjects(anObjects, aContextes))
    return;

  //Getting axis.
  static const std::string aSelectionError = "Error: The axis shape selection is bad.";
  AttributeSelectionPtr anObjRef = selection(AXIS_OBJECT_ID());
  GeomShapePtr aShape = anObjRef->value();
  if (!aShape.get()) {
    if (anObjRef->context().get()) {
      aShape = anObjRef->context()->shape();
    }
  }
  if (!aShape.get()) {
    setError(aSelectionError);
    return;
  }

  GeomEdgePtr anEdge;
  if (aShape->isEdge())
  {
    anEdge = aShape->edge();
  }
  else if (aShape->isCompound())
  {
    GeomAPI_ShapeIterator anIt(aShape);
    anEdge = anIt.current()->edge();
  }

  if (!anEdge.get())
  {
    setError(aSelectionError);
    return;
  }

  std::shared_ptr<GeomAPI_Ax1> anAxis (new GeomAPI_Ax1(anEdge->line()->location(),
                                                       anEdge->line()->direction()));


  // Moving each object.
  std::string anError;
  int aResultIndex = 0;
  std::list<ResultPtr>::iterator aContext = aContextes.begin();
  for(ListOfShape::iterator anObjectsIt = anObjects.begin(); anObjectsIt != anObjects.end();
        anObjectsIt++, aContext++) {
    std::shared_ptr<GeomAPI_Shape> aBaseShape = *anObjectsIt;
    bool isPart = aContext->get() && (*aContext)->groupName() == ModelAPI_ResultPart::group();

    // Setting result.
    if (isPart) {
      std::shared_ptr<GeomAPI_Trsf> aTrsf(new GeomAPI_Trsf());
      aTrsf->setSymmetry(anAxis);
      ResultPartPtr anOrigin = std::dynamic_pointer_cast<ModelAPI_ResultPart>(*aContext);
      buildResult(anOrigin, aTrsf, aResultIndex);
    }
    else {
      std::shared_ptr<GeomAlgoAPI_Symmetry> aSymmetryAlgo(
        new GeomAlgoAPI_Symmetry(aBaseShape, anAxis));

      if (!aSymmetryAlgo->check()) {
        setError(aSymmetryAlgo->getError());
        return;
      }

      aSymmetryAlgo->build();

      // Checking that the algorithm worked properly.
      if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aSymmetryAlgo, getKind(), anError)) {
        setError(anError);
        break;
      }

      buildResult(aSymmetryAlgo, aBaseShape, aResultIndex);
    }
    aResultIndex++;
  }

  // Remove the rest results if there were produced in the previous pass.
  removeResults(aResultIndex);
}

//=================================================================================================
void FeaturesPlugin_Symmetry::performSymmetryByPlane()
{
  // Getting objects.
  ListOfShape anObjects;
  std::list<ResultPtr> aContextes;
  if (!collectSourceObjects(anObjects, aContextes))
    return;

  //Getting plane.
  static const std::string aSelectionError = "Error: The plane shape selection is bad.";
  AttributeSelectionPtr anObjRef = selection(PLANE_OBJECT_ID());
  GeomShapePtr aShape = anObjRef->value();
  if (!aShape.get()) {
    if (anObjRef->context().get()) {
      aShape = anObjRef->context()->shape();
    }
  }
  if (!aShape.get()) {
    setError(aSelectionError);
    return;
  }

  GeomFacePtr aFace;
  if (aShape->isFace())
  {
    aFace = aShape->face();
  }
  else if (aShape->isCompound())
  {
    GeomAPI_ShapeIterator anIt(aShape);
    aFace = anIt.current()->face();
  }

  if (!aFace.get())
  {
    setError(aSelectionError);
    return;
  }

  std::shared_ptr<GeomAPI_Ax2> aPlane(new GeomAPI_Ax2(aFace->getPlane()->location(),
                                                      aFace->getPlane()->direction()));


  // Moving each object.
  std::string anError;
  int aResultIndex = 0;
  std::list<ResultPtr>::iterator aContext = aContextes.begin();
  for(ListOfShape::iterator anObjectsIt = anObjects.begin(); anObjectsIt != anObjects.end();
        anObjectsIt++, aContext++) {
    std::shared_ptr<GeomAPI_Shape> aBaseShape = *anObjectsIt;
    bool isPart = aContext->get() && (*aContext)->groupName() == ModelAPI_ResultPart::group();

    // Setting result.
    if (isPart) {
      std::shared_ptr<GeomAPI_Trsf> aTrsf(new GeomAPI_Trsf());
      aTrsf->setSymmetry(aPlane);
      ResultPartPtr anOrigin = std::dynamic_pointer_cast<ModelAPI_ResultPart>(*aContext);
      buildResult(anOrigin, aTrsf, aResultIndex);
    } else {
      std::shared_ptr<GeomAlgoAPI_Symmetry> aSymmetryAlgo(
        new GeomAlgoAPI_Symmetry(aBaseShape, aPlane));

      if (!aSymmetryAlgo->check()) {
        setError(aSymmetryAlgo->getError());
        return;
      }

      aSymmetryAlgo->build();

      // Checking that the algorithm worked properly.
      if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aSymmetryAlgo, getKind(), anError)) {
        setError(anError);
        break;
      }

      buildResult(aSymmetryAlgo, aBaseShape, aResultIndex);
    }
    aResultIndex++;
  }

  // Remove the rest results if there were produced in the previous pass.
  removeResults(aResultIndex);
}

//=================================================================================================
void FeaturesPlugin_Symmetry::buildResult(
  std::shared_ptr<GeomAlgoAPI_Symmetry>& theSymmetryAlgo,
  std::shared_ptr<GeomAPI_Shape> theBaseShape, int theResultIndex)
{
  std::shared_ptr<GeomAlgoAPI_MakeShapeList> anAlgoList(new GeomAlgoAPI_MakeShapeList());
  anAlgoList->appendAlgo(theSymmetryAlgo);
  // Compose source shape and the result of symmetry.
  GeomShapePtr aCompound;
  if (boolean(KEEP_ORIGINAL_RESULT())->value()) {
    ListOfShape aShapes;
    // add a copy of a base shape otherwise selection of this base shape is bad (2592)
    std::shared_ptr<GeomAlgoAPI_Copy> aCopyAlgo(new GeomAlgoAPI_Copy(theBaseShape));
    aShapes.push_back(aCopyAlgo->shape());
    anAlgoList->appendAlgo(aCopyAlgo);

    aShapes.push_back(theSymmetryAlgo->shape());
    aCompound = GeomAlgoAPI_CompoundBuilder::compound(aShapes);
  } else
    aCompound = theSymmetryAlgo->shape();

  // Store and name the result.
  ResultBodyPtr aResultBody = document()->createBody(data(), theResultIndex);

  ListOfShape aBaseShapes;
  aBaseShapes.push_back(theBaseShape);
  FeaturesPlugin_Tools::loadModifiedShapes(aResultBody, aBaseShapes, ListOfShape(),
                                           anAlgoList, aCompound, "Symmetried");

  setResult(aResultBody, theResultIndex);
}

//=================================================================================================
void FeaturesPlugin_Symmetry::buildResult(ResultPartPtr theOriginal,
                                          std::shared_ptr<GeomAPI_Trsf> theTrsf,
                                          int& theResultIndex)
{
  if (boolean(KEEP_ORIGINAL_RESULT())->value()) {
    std::shared_ptr<GeomAPI_Trsf> anIdentity(new GeomAPI_Trsf());
    ResultPartPtr aCopy = document()->copyPart(theOriginal, data(), theResultIndex);
    aCopy->setTrsf(theOriginal, anIdentity);
    setResult(aCopy, theResultIndex);
    ++theResultIndex;
  }

  ResultPartPtr aResultPart = document()->copyPart(theOriginal, data(), theResultIndex);
  aResultPart->setTrsf(theOriginal, theTrsf);
  setResult(aResultPart, theResultIndex);
}
