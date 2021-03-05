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

#include "SketchPlugin_Polyline.h"

#include "SketchPlugin_Line.h"
#include "SketchPlugin_Sketch.h"

#include "SketchPlugin_ConstraintCoincidence.h"

#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Session.h>

#include <GeomDataAPI_Point2DArray.h>

#include <GeomDataAPI_Point2D.h>
#include <GeomAlgoAPI_PointBuilder.h>
#include <GeomAlgoAPI_EdgeBuilder.h>
#include <GeomAlgoAPI_CompoundBuilder.h>
#include <SketchPlugin_Tools.h>

#include <iostream>

SketchPlugin_Polyline::SketchPlugin_Polyline()
    : SketchPlugin_SketchEntity()
{}

void SketchPlugin_Polyline::initAttributes()
{
  SketchPlugin_SketchEntity::initAttributes();
}

void SketchPlugin_Polyline::initDerivedClassAttributes()
{

  data()->addAttribute(POINTS_ID(), GeomDataAPI_Point2DArray::typeId());

}

void SketchPlugin_Polyline::execute()
{
  createLineFeature();
}

void  SketchPlugin_Polyline::createLineFeature()
{
  AttributePoint2DArrayPtr aPointsArray =
      std::dynamic_pointer_cast<GeomDataAPI_Point2DArray>(attribute(POINTS_ID()));

  getAISObject(AISObjectPtr());

  if (aPointsArray->isInitialized())
  {
    FeaturePtr aLastline;
    FeaturePtr aFirstline;
    // collect points
    for (int anIndex = 1; anIndex < aPointsArray->size(); ++anIndex) {

      FeaturePtr aLine = sketch()->addFeature(SketchPlugin_Line::ID());
      if (anIndex ==1) {
        aFirstline = aLine;
      }
      std::shared_ptr<GeomDataAPI_Point2D> aStartA = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aLine->attribute(SketchPlugin_Line::START_ID()));

      aStartA->setValue(aPointsArray->pnt(anIndex-1));

      std::shared_ptr<GeomDataAPI_Point2D> aEndA = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        aLine->attribute(SketchPlugin_Line::END_ID()));

      aEndA->setValue(aPointsArray->pnt(anIndex));

      aLine->boolean(SketchPlugin_SketchEntity::AUXILIARY_ID())->setValue(
          boolean(AUXILIARY_ID())->value());

      if (aLastline) {
        // Initialize new line with first point equal to end of previous
        std::shared_ptr<ModelAPI_Data> aSFData = aLastline->data();
        std::shared_ptr<GeomDataAPI_Point2D> aSPoint =
                                    std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                                                aSFData->attribute(SketchPlugin_Line::END_ID()));
        std::shared_ptr<ModelAPI_Data> aNFData = aLine->data();
        std::shared_ptr<GeomDataAPI_Point2D> aNPoint =
                                    std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                                              aNFData->attribute(SketchPlugin_Line::START_ID()));
        aNPoint->setValue(aSPoint->x(), aSPoint->y());
        SketchPlugin_ConstraintCoincidence::createCoincidenceFeature(sketch(), aSPoint, aNPoint);
      }

      aLine->execute();
      aLastline = aLine;
    }
    // Initialize new line with first point equal to end of previous
    std::shared_ptr<ModelAPI_Data> aSFData = aLastline->data();
    std::shared_ptr<GeomDataAPI_Point2D> aSPoint =
                                           std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                                                aSFData->attribute(SketchPlugin_Line::END_ID()));
    std::shared_ptr<ModelAPI_Data> aNFData = aFirstline->data();
    std::shared_ptr<GeomDataAPI_Point2D> aNPoint =
                                           std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                                              aNFData->attribute(SketchPlugin_Line::START_ID()));

    double aDistance = aSPoint->pnt()->distance(aNPoint->pnt());
    if (aDistance < 1) {
      aNPoint->setValue(aSPoint->x(), aSPoint->y());
      SketchPlugin_ConstraintCoincidence::createCoincidenceFeature(sketch(), aSPoint, aNPoint);
    }
  }
}

const std::string& SketchPlugin_Polyline::getKind()
{
  static std::string MY_KIND = SketchPlugin_Polyline::ID();
  return MY_KIND;
}

void SketchPlugin_Polyline::attributeChanged(const std::string& theID) {
}

AISObjectPtr SketchPlugin_Polyline::getAISObject(AISObjectPtr thePrevious)
{

  SketchPlugin_Sketch* aSketch = sketch();
  if (!aSketch)
    return AISObjectPtr();

  AttributePoint2DArrayPtr aPointsArray =
      std::dynamic_pointer_cast<GeomDataAPI_Point2DArray>(attribute(POINTS_ID()));

  if (aPointsArray->size() < 2)
    return AISObjectPtr();

  std::list<GeomShapePtr> aShapes;

  // convert points to vertices
  std::list<GeomPointPtr> aPoles3D;
  for (int anIndex = 0; anIndex < aPointsArray->size()-1; ++anIndex) {

    GeomPnt2dPtr aPoint1 = aPointsArray->pnt(anIndex);
    GeomPnt2dPtr aPoint2 = aPointsArray->pnt(anIndex + 1);
    GeomPointPtr aPole3D1 = aSketch->to3D(aPoint1->x(), aPoint1->y());
    GeomPointPtr aPole3D2 = aSketch->to3D(aPoint2->x(), aPoint2->y());
    aShapes.push_back(GeomAlgoAPI_PointBuilder::vertex(aPole3D1));

    GeomShapePtr anEdge =
      GeomAlgoAPI_EdgeBuilder::line(aPole3D1, aPole3D2);
    if (anEdge)
      aShapes.push_back(anEdge);

  }

  GeomShapePtr aCompound = GeomAlgoAPI_CompoundBuilder::compound(aShapes);

  AISObjectPtr anAIS = thePrevious;
  if (!anAIS)
    anAIS.reset(new GeomAPI_AISObject());
  anAIS->createShape(aCompound);

  // Modify attributes
  SketchPlugin_Tools::customizeFeaturePrs(anAIS, boolean(AUXILIARY_ID())->value());

  return anAIS;
}