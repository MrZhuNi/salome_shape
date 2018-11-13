// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include <Config_PropManager.h>

#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_FaceBuilder.h>

#include <GeomAPI_Dir.h>
#include <GeomAPI_PlanarEdges.h>
#include <GeomAPI_ShapeIterator.h>
#include <GeomAPI_Vertex.h>

#include <GeomDataAPI_Point2D.h>
#include <GeomAlgoAPI_PointBuilder.h>

#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Events.h>

#include <SketchPlugin_Sketch.h>
#include <SketchPlugin_Feature.h>
#include <SketchPlugin_IntersectionPoint.h>
#include <SketchPlugin_Projection.h>
#include <SketchPlugin_SketchEntity.h>
#include <SketchPlugin_Tools.h>

#include <Events_Loop.h>

#include <memory>

#include <math.h>
#include <vector>

SketchPlugin_Sketch::SketchPlugin_Sketch()
{
}

void SketchPlugin_Sketch::initAttributes()
{
  data()->addAttribute(SketchPlugin_Sketch::ORIGIN_ID(), GeomDataAPI_Point::typeId());
  data()->addAttribute(SketchPlugin_Sketch::DIRX_ID(), GeomDataAPI_Dir::typeId());
  data()->addAttribute(SketchPlugin_Sketch::NORM_ID(), GeomDataAPI_Dir::typeId());
  data()->addAttribute(SketchPlugin_Sketch::FEATURES_ID(), ModelAPI_AttributeRefList::typeId());
  // the selected face, base for the sketcher plane, not obligatory
  data()->addAttribute(SketchPlugin_SketchEntity::EXTERNAL_ID(),
    ModelAPI_AttributeSelection::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(
    getKind(), SketchPlugin_SketchEntity::EXTERNAL_ID());
  data()->addAttribute(SketchPlugin_Sketch::SOLVER_ERROR(), ModelAPI_AttributeString::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(
    getKind(), SketchPlugin_Sketch::SOLVER_ERROR());
  data()->addAttribute(SketchPlugin_Sketch::SOLVER_DOF(), ModelAPI_AttributeString::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(
    getKind(), SketchPlugin_Sketch::SOLVER_DOF());
}

void SketchPlugin_Sketch::execute()
{
  if (!data()->isValid())
    return;
  std::shared_ptr<ModelAPI_AttributeRefList> aRefList = std::dynamic_pointer_cast<
      ModelAPI_AttributeRefList>(data()->attribute(SketchPlugin_Sketch::FEATURES_ID()));

  std::shared_ptr<GeomDataAPI_Point> anOrigin = std::dynamic_pointer_cast<GeomDataAPI_Point>(
      data()->attribute(SketchPlugin_Sketch::ORIGIN_ID()));
  std::shared_ptr<GeomDataAPI_Dir> aDirX = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
      data()->attribute(SketchPlugin_Sketch::DIRX_ID()));
  std::shared_ptr<GeomDataAPI_Dir> aNorm = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
      data()->attribute(SketchPlugin_Sketch::NORM_ID()));

  std::list<ObjectPtr> aFeatures = aRefList->list();
  if (aFeatures.empty()) // actually, this must be avoided by the validators
    return;

  std::list<ObjectPtr>::const_iterator anIt = aFeatures.begin(), aLast = aFeatures.end();
  std::shared_ptr<SketchPlugin_Feature> aFeature;
  std::list<std::shared_ptr<GeomAPI_Shape> > aFeaturesPreview;
  for (; anIt != aLast; anIt++) {
    aFeature = std::dynamic_pointer_cast<SketchPlugin_Feature>(*anIt);
    if (aFeature) {
      if (!aFeature->sketch()) // on load document the back references are missed
        aFeature->setSketch(this);
      // do not include into the result the external edges with disabled flag "Include into result"
      if (aFeature->data()->attribute(SketchPlugin_SketchEntity::EXTERNAL_ID())) {
        if (aFeature->data()->selection(SketchPlugin_SketchEntity::EXTERNAL_ID())->context()) {
          const std::string& anAttrName =
              aFeature->getKind() == SketchPlugin_Projection::ID() ?
              SketchPlugin_Projection::INCLUDE_INTO_RESULT() :
              SketchPlugin_IntersectionPoint::INCLUDE_INTO_RESULT();
          AttributeBooleanPtr aKeepResult = aFeature->boolean(anAttrName);
          if (!aKeepResult || !aKeepResult->value())
            continue;
        }
      }
      // do not include the construction entities in the result
      if (aFeature->data()->attribute(SketchPlugin_SketchEntity::AUXILIARY_ID())) {
        if (aFeature->data()->boolean(SketchPlugin_SketchEntity::AUXILIARY_ID())->value())
          continue;
      }

      const std::list<std::shared_ptr<ModelAPI_Result> >& aRes = aFeature->results();
      std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aResIter = aRes.cbegin();
      for (; aResIter != aRes.cend(); aResIter++) {
        std::shared_ptr<ModelAPI_ResultConstruction> aConstr = std::dynamic_pointer_cast<
            ModelAPI_ResultConstruction>(*aResIter);
        if (aConstr) {
          std::shared_ptr<GeomAPI_Shape> aShape = aConstr->shape();
          if (aShape)
            aFeaturesPreview.push_back(aShape);
        }
      }
    }
  }

  // Collect all edges as one big wire
  std::shared_ptr<GeomAPI_PlanarEdges> aBigWire(new GeomAPI_PlanarEdges);
  std::list<std::shared_ptr<GeomAPI_Shape> >::const_iterator aShapeIt = aFeaturesPreview.begin();
  for (; aShapeIt != aFeaturesPreview.end(); ++aShapeIt) {
    aBigWire->addEdge(*aShapeIt);
  }
  aBigWire->setPlane(anOrigin->pnt(), aDirX->dir(), aNorm->dir());
  std::shared_ptr<ModelAPI_ResultConstruction> aConstr = document()->createConstruction(data());
  aConstr->setShape(aBigWire);
  setResult(aConstr);
}

std::shared_ptr<ModelAPI_Feature> SketchPlugin_Sketch::addFeature(std::string theID)
{
  // Set last feature of the sketch as current feature.
  // Reason: Changing of parameter through Python API may lead to creation of new features
  //         (e.g. changing number of copies in MultiRotation). If the sketch is not the last
  //         feature in the Object Browser, then new features will be added to the end feature.
  //         Therefore, setting any feature below the sketch as a current feature will disable
  //         these newly created features.
  std::shared_ptr<ModelAPI_AttributeRefList> aRefList = std::dynamic_pointer_cast<
      ModelAPI_AttributeRefList>(data()->attribute(SketchPlugin_Sketch::FEATURES_ID()));
  int aSize = aRefList->size(false);
  ObjectPtr aLastObject = aSize == 0 ? data()->owner() : aRefList->object(aSize - 1, false);
  FeaturePtr aLastFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aLastObject);
  document()->setCurrentFeature(aLastFeature, false);

  // add new feature
  std::shared_ptr<ModelAPI_Feature> aNew = document()->addFeature(theID, false);
  if (aNew) {
    // the sketch cannot be specified for the macro-features defined in python
    // like SketchRectangle, so we need to check the type of new feature
    std::shared_ptr<SketchPlugin_Feature> aSketchFeature =
        std::dynamic_pointer_cast<SketchPlugin_Feature>(aNew);
    if (aSketchFeature)
      aSketchFeature->setSketch(this);
    data()->reflist(SketchPlugin_Sketch::FEATURES_ID())->append(aNew);
  }
   // set as current also after it becomes sub to set correctly enabled for other sketch subs
  document()->setCurrentFeature(aNew, false);

  return aNew;
}

void SketchPlugin_Sketch::removeFeature(std::shared_ptr<ModelAPI_Feature> theFeature)
{
  if (!data()->isValid()) // sketch is already removed (case on undo of sketch), sync is not needed
    return;
  AttributeRefListPtr aList = reflist(SketchPlugin_Sketch::FEATURES_ID());
  // if the object is last, remove it from the list
  // (needed to skip empty transaction on edit of sketch feature)
  if (aList->object(aList->size(true) - 1, true) == theFeature) {
    aList->remove(theFeature);
  } else {
    // to keep the persistent sub-elements indexing, do not remove elements from list,
    // but substitute by nulls
    aList->substitute(theFeature, ObjectPtr());
  }
}

int SketchPlugin_Sketch::numberOfSubs(bool forTree) const
{
  if (forTree)
    return 0;
  return data()->reflist(FEATURES_ID())->size(false);
}

std::shared_ptr<ModelAPI_Feature> SketchPlugin_Sketch::subFeature(
  const int theIndex, bool forTree)
{
  if (forTree)
    return FeaturePtr();

  ObjectPtr anObj = data()->reflist(SketchPlugin_Sketch::FEATURES_ID())->object(theIndex, false);
  FeaturePtr aRes = std::dynamic_pointer_cast<ModelAPI_Feature>(anObj);
  return aRes;
}

int SketchPlugin_Sketch::subFeatureId(const int theIndex) const
{
  std::shared_ptr<ModelAPI_AttributeRefList> aRefList = std::dynamic_pointer_cast<
      ModelAPI_AttributeRefList>(data()->attribute(SketchPlugin_Sketch::FEATURES_ID()));
  std::list<ObjectPtr> aFeatures = aRefList->list();
  std::list<ObjectPtr>::const_iterator anIt = aFeatures.begin();
  int aResultIndex = 1; // number of the counted (created) features, started from 1
  int aFeatureIndex = -1; // number of the not-empty features in the list
  for (; anIt != aFeatures.end(); anIt++) {
    if (anIt->get())
      aFeatureIndex++;
    if (aFeatureIndex == theIndex)
      break;
    aResultIndex++;
  }
  return aResultIndex;
}

bool SketchPlugin_Sketch::isSub(ObjectPtr theObject) const
{
  // check is this feature of result
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theObject);
  if (!aFeature) {
    ResultPtr aRes = std::dynamic_pointer_cast<ModelAPI_Result>(theObject);
    if (aRes)
      aFeature = document()->feature(aRes);
  }
  if (aFeature) {
    return data()->reflist(SketchPlugin_Sketch::FEATURES_ID())->isInList(aFeature);
  }
  return false;
}


void SketchPlugin_Sketch::attributeChanged(const std::string& theID) {
  if (theID == SketchPlugin_SketchEntity::EXTERNAL_ID()) {
    AttributeSelectionPtr aSelAttr = selection(SketchPlugin_SketchEntity::EXTERNAL_ID());
    if (aSelAttr->context().get()) { // update arguments due to the selection value
      std::shared_ptr<GeomAPI_Shape> aSelection = aSelAttr->value();
      if (!aSelection.get()) aSelection = aSelAttr->context()->shape();
      // update the sketch plane
      std::shared_ptr<GeomAPI_Face> aFace;
      if (aSelection->isFace()) {
        aFace = aSelection->face();
      } else if (aSelection->isCompound()) {
        GeomAPI_ShapeIterator anIt(aSelection);
        aFace = anIt.current()->face();
      }
      if (aFace.get()) {
        std::shared_ptr<GeomAPI_Pln> aPlane = aFace->getPlane();
        if (aPlane.get()) {
          double anA, aB, aC, aD;
          aPlane->coefficients(anA, aB, aC, aD);

          // calculate attributes of the sketch
          std::shared_ptr<GeomAPI_Dir> aNormDir(new GeomAPI_Dir(anA, aB, aC));
          std::shared_ptr<GeomAPI_XYZ> aCoords = aNormDir->xyz();
          std::shared_ptr<GeomAPI_XYZ> aZero(new GeomAPI_XYZ(0, 0, 0));
          aCoords = aCoords->multiplied(-aD * aCoords->distance(aZero));
          std::shared_ptr<GeomAPI_Pnt> anOrigPnt(new GeomAPI_Pnt(aCoords));
          // X axis is preferable to be dirX on the sketch
          // here can not be very small value to avoid very close to X normal axis (issue 595)
          static const double tol = 0.1;
          bool isX = fabs(anA) - 1.0 < tol && fabs(aB) < tol && fabs(aC) < tol;
          std::shared_ptr<GeomAPI_Dir> aTempDir(
            isX ? new GeomAPI_Dir(0, 1, 0) : new GeomAPI_Dir(1, 0, 0));
          std::shared_ptr<GeomAPI_Dir> aYDir(new GeomAPI_Dir(aNormDir->cross(aTempDir)));
          std::shared_ptr<GeomAPI_Dir> aXDir(new GeomAPI_Dir(aYDir->cross(aNormDir)));

          // update position of the sketch
          std::shared_ptr<GeomDataAPI_Point> anOrigin = std::dynamic_pointer_cast
            <GeomDataAPI_Point>(data()->attribute(SketchPlugin_Sketch::ORIGIN_ID()));
          anOrigin->setValue(anOrigPnt);
          std::shared_ptr<GeomDataAPI_Dir> aNormal = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
            data()->attribute(SketchPlugin_Sketch::NORM_ID()));
          aNormal->setValue(aNormDir);
          std::shared_ptr<GeomDataAPI_Dir> aDirX = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
            data()->attribute(SketchPlugin_Sketch::DIRX_ID()));
          aDirX->setValue(aXDir);
          std::shared_ptr<GeomAPI_Dir> aDir = aPlane->direction();
        }
      }
    }
  } else if (theID == NORM_ID() || theID == DIRX_ID() || theID == ORIGIN_ID()) {
    // send all sub-elements are also updated: all entities become created on different plane
    static Events_ID anUpdateEvent = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);
    std::list<ObjectPtr> aSubs = data()->reflist(SketchPlugin_Sketch::FEATURES_ID())->list();
    std::list<ObjectPtr>::iterator aSub = aSubs.begin();
    for(; aSub != aSubs.end(); aSub++) {
      if (aSub->get())
        ModelAPI_EventCreator::get()->sendUpdated(*aSub, anUpdateEvent);
    }
  }
}

void SketchPlugin_Sketch::createPoint2DResult(ModelAPI_Feature* theFeature,
                                              SketchPlugin_Sketch* theSketch,
                                              const std::string& theAttributeID, const int theIndex)
{
  std::shared_ptr<GeomDataAPI_Point2D> aPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
    theFeature->attribute(theAttributeID));

  if (!aPoint || !aPoint->isInitialized())
    return;

  std::shared_ptr<GeomAPI_Pnt> aCenter(theSketch->to3D(aPoint->x(), aPoint->y()));
  //std::cout<<"Execute circle "<<aCenter->x()<<" "<<aCenter->y()<<" "<<aCenter->z()<<std::endl;
  // make a visible point
  std::shared_ptr<GeomAPI_Shape> aCenterPointShape = GeomAlgoAPI_PointBuilder::vertex(aCenter);
  std::shared_ptr<ModelAPI_ResultConstruction> aResult = theFeature->document()->createConstruction(
                     theFeature->data(), theIndex);
  aResult->setShape(aCenterPointShape);
  aResult->setIsInHistory(false);

  theFeature->setResult(aResult, theIndex);
}

FeaturePtr SketchPlugin_Sketch::addUniqueNamedCopiedFeature(FeaturePtr theFeature,
                                                            SketchPlugin_Sketch* theSketch,
                                                            const bool theIsCopy)
{
  FeaturePtr aNewFeature = theSketch->addFeature(theFeature->getKind());
  // addFeature generates a unique name for the feature, it caches the name
  std::string aUniqueFeatureName = aNewFeature->data()->name();
  // all attribute values are copied\pasted to the new feature, name is not an exception
  theFeature->data()->copyTo(aNewFeature->data());
  // external state should not be copied as a new object is an object of the current sketch
  if (theFeature->selection(SketchPlugin_SketchEntity::EXTERNAL_ID()).get())
    aNewFeature->selection(SketchPlugin_SketchEntity::EXTERNAL_ID())->setValue(ResultPtr(),
                                                                               GeomShapePtr());
  aNewFeature->data()->setName(aUniqueFeatureName);
  // text expressions could block setValue of some attributes
  SketchPlugin_Tools::clearExpressions(aNewFeature);
  // Set copy attribute
  AttributeBooleanPtr anAttr = aNewFeature->data()->boolean(SketchPlugin_SketchEntity::COPY_ID());
  if(anAttr.get()) {
    anAttr->setValue(theIsCopy);
  }

  return aNewFeature;
}

std::shared_ptr<GeomAPI_Ax3> SketchPlugin_Sketch::plane(SketchPlugin_Sketch* theSketch)
{
  std::shared_ptr<ModelAPI_Data> aData = theSketch->data();

  std::shared_ptr<GeomDataAPI_Point> anOrigin = std::dynamic_pointer_cast<GeomDataAPI_Point>(
      aData->attribute(SketchPlugin_Sketch::ORIGIN_ID()));
  std::shared_ptr<GeomDataAPI_Dir> aDirX = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
      aData->attribute(SketchPlugin_Sketch::DIRX_ID()));
  std::shared_ptr<GeomDataAPI_Dir> aNorm = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
      aData->attribute(SketchPlugin_Sketch::NORM_ID()));

  return std::shared_ptr<GeomAPI_Ax3>(new GeomAPI_Ax3(anOrigin->pnt(), aDirX->dir(), aNorm->dir()));
}

void SketchPlugin_Sketch::exchangeIDs(
  std::shared_ptr<ModelAPI_Feature> theFeature1, std::shared_ptr<ModelAPI_Feature> theFeature2)
{
  reflist(SketchPlugin_Sketch::FEATURES_ID())->exchange(theFeature1, theFeature2);
}
