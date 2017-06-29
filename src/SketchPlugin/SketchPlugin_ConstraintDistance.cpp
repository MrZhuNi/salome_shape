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

#include "SketchPlugin_ConstraintDistance.h"
#include <SketchPlugin_Point.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Line.h>

#include <SketcherPrs_Tools.h>
#include <SketcherPrs_Factory.h>

#include <GeomAPI_Dir2d.h>
#include <GeomAPI_Lin2d.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_XY.h>
#include <GeomDataAPI_Point2D.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Data.h>

#include <Config_PropManager.h>

#include <math.h>

const double tolerance = 1e-7;


SketchPlugin_ConstraintDistance::SketchPlugin_ConstraintDistance()
{
  myFlyoutUpdate = false;
}

//*************************************************************************************
void SketchPlugin_ConstraintDistance::initAttributes()
{
  data()->addAttribute(SketchPlugin_Constraint::VALUE(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT(), GeomDataAPI_Point2D::typeId());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::typeId());
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_B(), ModelAPI_AttributeRefAttr::typeId());
  data()->addAttribute(SIGNED(), ModelAPI_AttributeBoolean::typeId());
}

void SketchPlugin_ConstraintDistance::colorConfigInfo(std::string& theSection, std::string& theName,
                                                      std::string& theDefault)
{
  theSection = "Visualization";
  theName = "sketch_dimension_color";
  theDefault = SKETCH_DIMENSION_COLOR;
}

//*************************************************************************************
void SketchPlugin_ConstraintDistance::execute()
{
  std::shared_ptr<ModelAPI_Data> aData = data();
  AttributeDoublePtr anAttrValue = std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(
      aData->attribute(SketchPlugin_Constraint::VALUE()));

  if(anAttrValue->isInitialized())
    return;

  double aDistance = calculateCurrentDistance();
  if(aDistance >= 0)
    anAttrValue->setValue(aDistance);
}

//*************************************************************************************
AISObjectPtr SketchPlugin_ConstraintDistance::getAISObject(AISObjectPtr thePrevious)
{
  if (!sketch())
    return thePrevious;

  AISObjectPtr anAIS = SketcherPrs_Factory::lengthDimensionConstraint(this,
                                                                      sketch()->coordinatePlane(),
                                                                      thePrevious);
  return anAIS;
}

//*************************************************************************************
void SketchPlugin_ConstraintDistance::move(double theDeltaX, double theDeltaY)
{
  std::shared_ptr<ModelAPI_Data> aData = data();
  if (!aData->isValid())
    return;

  // Recalculate a shift of flyout point in terms of local coordinates
  std::shared_ptr<GeomAPI_XY> aDir(new GeomAPI_XY(theDeltaX, theDeltaY));
  std::shared_ptr<GeomAPI_Ax3> aPlane = SketchPlugin_Sketch::plane(sketch());
  std::shared_ptr<GeomDataAPI_Point2D> aPointA = SketcherPrs_Tools::getFeaturePoint(
      data(), SketchPlugin_Constraint::ENTITY_A(), aPlane);
  std::shared_ptr<GeomDataAPI_Point2D> aPointB = SketcherPrs_Tools::getFeaturePoint(
      data(), SketchPlugin_Constraint::ENTITY_B(), aPlane);

  std::shared_ptr<GeomAPI_XY> aStartPnt;
  std::shared_ptr<GeomAPI_XY> aEndPnt;
  if (aPointA && aPointB) {
    aStartPnt = aPointA->pnt()->xy();
    aEndPnt = aPointB->pnt()->xy();
  } else if (aPointA) {
    FeaturePtr aLine = SketcherPrs_Tools::getFeatureLine(data(),
        SketchPlugin_Constraint::ENTITY_B());
    if (!aLine)
      return;
    std::shared_ptr<GeomAPI_Pnt2d> aPoint = aPointA->pnt();
    aStartPnt = aPoint->xy();
    aEndPnt = SketcherPrs_Tools::getProjectionPoint(aLine, aPoint)->xy();
  } else if (aPointB) {
    FeaturePtr aLine = SketcherPrs_Tools::getFeatureLine(data(),
        SketchPlugin_Constraint::ENTITY_A());
    if (!aLine)
      return;
    std::shared_ptr<GeomAPI_Pnt2d> aPoint = aPointB->pnt();
    aStartPnt = SketcherPrs_Tools::getProjectionPoint(aLine, aPoint)->xy();
    aEndPnt = aPoint->xy();
  } else
    return;

  std::shared_ptr<GeomAPI_Dir2d> aLineDir(new GeomAPI_Dir2d(aEndPnt->decreased(aStartPnt)));
  double dX = aDir->dot(aLineDir->xy());
  double dY = -aDir->cross(aLineDir->xy());

  std::shared_ptr<GeomDataAPI_Point2D> aPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT()));
  myFlyoutUpdate = true;
  if (aPoint->isInitialized()) {
    aPoint->setValue(aPoint->x() + dX, aPoint->y() + dY);
  } else {
    aPoint->setValue(dX, dY);
  }
  myFlyoutUpdate = false;
}

double SketchPlugin_ConstraintDistance::calculateCurrentDistance()
{
  double aDistance = -1.;

  std::shared_ptr<ModelAPI_Data> aData = data();
  std::shared_ptr<GeomAPI_Ax3> aPlane = SketchPlugin_Sketch::plane(sketch());
  std::shared_ptr<GeomDataAPI_Point2D> aPointA =
    SketcherPrs_Tools::getFeaturePoint(aData, SketchPlugin_Constraint::ENTITY_A(), aPlane);
  std::shared_ptr<GeomDataAPI_Point2D> aPointB =
      SketcherPrs_Tools::getFeaturePoint(aData, SketchPlugin_Constraint::ENTITY_B(), aPlane);

  if (aPointA.get() && aPointB.get()) {  // both points
    aDistance = aPointA->pnt()->distance(aPointB->pnt());
  } else {
    FeaturePtr aLineFeature;
    std::shared_ptr<SketchPlugin_Line> aLine;
    if (!aPointA.get() && aPointB.get()) {  //Line and point
      aLineFeature = SketcherPrs_Tools::getFeatureLine(aData, SketchPlugin_Constraint::ENTITY_A());
      aLine = std::dynamic_pointer_cast<SketchPlugin_Line>(aLineFeature);
      if (aLine.get()) {
        aDistance = aLine->distanceToPoint(aPointB->pnt());
      }
    } else if (aPointA.get() && !aPointB.get()) {  // Point and line
      aLineFeature = SketcherPrs_Tools::getFeatureLine(aData, SketchPlugin_Constraint::ENTITY_B());
      aLine = std::dynamic_pointer_cast<SketchPlugin_Line>(aLineFeature);
      if (aLine.get()) {
        aDistance = aLine->distanceToPoint(aPointA->pnt());
      }
    }
  }
  return aDistance;
}

bool SketchPlugin_ConstraintDistance::areAttributesInitialized()
{
  std::shared_ptr<ModelAPI_Data> aData = data();
  std::shared_ptr<GeomAPI_Ax3> aPlane = SketchPlugin_Sketch::plane(sketch());
  std::shared_ptr<GeomDataAPI_Point2D> aPointA =
      SketcherPrs_Tools::getFeaturePoint(aData, SketchPlugin_Constraint::ENTITY_A(), aPlane);
  std::shared_ptr<GeomDataAPI_Point2D> aPointB =
      SketcherPrs_Tools::getFeaturePoint(aData, SketchPlugin_Constraint::ENTITY_B(), aPlane);

  if (!aPointA && !aPointB)
    return false;
  else if (aPointA || aPointB) {
    FeaturePtr aLine;
    if (!aPointA)
      aLine = SketcherPrs_Tools::getFeatureLine(aData, SketchPlugin_Constraint::ENTITY_A());
    else if (!aPointB)
      aLine = SketcherPrs_Tools::getFeatureLine(aData, SketchPlugin_Constraint::ENTITY_B());
    else // both points are initialized
      return true;

    if (!aLine || aLine->getKind() != SketchPlugin_Line::ID())
      return false;
  }
  return true;
}

void SketchPlugin_ConstraintDistance::attributeChanged(const std::string& theID)
{
  if (theID == SketchPlugin_Constraint::ENTITY_A() ||
      theID == SketchPlugin_Constraint::ENTITY_B())
  {
    std::shared_ptr<ModelAPI_AttributeDouble> aValueAttr = std::dynamic_pointer_cast<
        ModelAPI_AttributeDouble>(data()->attribute(SketchPlugin_Constraint::VALUE()));
    if (!aValueAttr->isInitialized()) {
      // only if it is not initialized, try to compute the current value
      double aDistance = calculateCurrentDistance();
      if (aDistance > 0) { // set as value the length of updated references
        aValueAttr->setValue(aDistance);
      }
    }
  } else if (theID == SketchPlugin_Constraint::FLYOUT_VALUE_PNT() && !myFlyoutUpdate) {
    myFlyoutUpdate = true;
    // Recalculate flyout point in local coordinates of the distance constraint:
    // the X coordinate is a length of projection of the flyout point on the
    //                  line binding two distanced points
    //                  or a line of projection of the distanced point onto the distanced segment
    // the Y coordinate is a distance from the flyout point to the line
    std::shared_ptr<GeomDataAPI_Point2D> aFlyoutAttr =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
        attribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT()));
    std::shared_ptr<GeomAPI_Pnt2d> aFlyoutPnt = aFlyoutAttr->pnt();

    std::shared_ptr<GeomAPI_Ax3> aPlane = SketchPlugin_Sketch::plane(sketch());
    std::shared_ptr<GeomDataAPI_Point2D> aPointA = SketcherPrs_Tools::getFeaturePoint(
        data(), SketchPlugin_Constraint::ENTITY_A(), aPlane);
    std::shared_ptr<GeomDataAPI_Point2D> aPointB = SketcherPrs_Tools::getFeaturePoint(
        data(), SketchPlugin_Constraint::ENTITY_B(), aPlane);

    std::shared_ptr<GeomAPI_XY> aStartPnt;
    std::shared_ptr<GeomAPI_XY> aEndPnt;
    if (aPointA && aPointB) {
      aStartPnt = aPointA->pnt()->xy();
      aEndPnt = aPointB->pnt()->xy();
    } else if (aPointA) {
      FeaturePtr aLine = SketcherPrs_Tools::getFeatureLine(data(),
          SketchPlugin_Constraint::ENTITY_B());
      if (!aLine)
        return;
      std::shared_ptr<GeomAPI_Pnt2d> aPoint = aPointA->pnt();
      aStartPnt = aPoint->xy();
      aEndPnt = SketcherPrs_Tools::getProjectionPoint(aLine, aPoint)->xy();
    } else if (aPointB) {
      FeaturePtr aLine = SketcherPrs_Tools::getFeatureLine(data(),
          SketchPlugin_Constraint::ENTITY_A());
      if (!aLine)
        return;
      std::shared_ptr<GeomAPI_Pnt2d> aPoint = aPointB->pnt();
      aStartPnt = SketcherPrs_Tools::getProjectionPoint(aLine, aPoint)->xy();
      aEndPnt = aPoint->xy();
    } else
      return;

    if (aEndPnt->distance(aStartPnt) < tolerance)
      return;

    std::shared_ptr<GeomAPI_Dir2d> aLineDir(new GeomAPI_Dir2d(aEndPnt->decreased(aStartPnt)));
    std::shared_ptr<GeomAPI_XY> aFlyoutDir = aFlyoutPnt->xy()->decreased(aStartPnt);

    double X = aFlyoutDir->dot(aLineDir->xy());
    double Y = -aFlyoutDir->cross(aLineDir->xy());
    aFlyoutAttr->setValue(X, Y);
    myFlyoutUpdate = false;
  }
}

