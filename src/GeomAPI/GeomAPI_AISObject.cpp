// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_AISObject.cpp
// Created:     25 Jun 2014
// Author:      Artem ZHIDKOV

#include <GeomAPI_AISObject.h>

#include <GeomAPI_Circ.h>
#include <GeomAPI_Dir.h>
#include <GeomAPI_Lin.h>
#include <GeomAPI_Pln.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Shape.h>
#include <GeomAPI_XYZ.h>

#include <Geom_Plane.hxx>
#include <TopoDS_Shape.hxx>
#include <Quantity_NameOfColor.hxx>
#include <BRepBndLib.hxx>

#include <AIS_InteractiveObject.hxx>
#include <AIS_InteractiveContext.hxx>
#include <AIS_LengthDimension.hxx>
#include <AIS_ParallelRelation.hxx>
#include <AIS_PerpendicularRelation.hxx>
#include <AIS_RadiusDimension.hxx>
#include <AIS_Shape.hxx>
#include <AIS_FixRelation.hxx>
#include <Prs3d_PointAspect.hxx>

const double tolerance = 1e-7;

const int CONSTRAINT_TEXT_HEIGHT = 28;  /// the text height of the constraint
const int CONSTRAINT_TEXT_SELECTION_TOLERANCE = 20;  /// the text selection tolerance

GeomAPI_AISObject::GeomAPI_AISObject()
    : GeomAPI_Interface(new Handle(AIS_InteractiveObject)())
{
}

void GeomAPI_AISObject::createShape(std::shared_ptr<GeomAPI_Shape> theShape)
{
  const TopoDS_Shape& aTDS =
      (theShape && theShape->implPtr<TopoDS_Shape>()) ?
          theShape->impl<TopoDS_Shape>() : TopoDS_Shape();

  Handle(AIS_InteractiveObject) anAIS = impl<Handle(AIS_InteractiveObject)>();
  if (!anAIS.IsNull()) {
    Handle(AIS_Shape) aShapeAIS = Handle(AIS_Shape)::DownCast(anAIS);
    if (aShapeAIS) {
      // if the AIS object is displayed in the opened local context in some mode, additional
      // AIS sub objects are created there. They should be rebuild for correct selecting.
      // It is possible to correct it by closing local context before the shape set and opening
      // after. Another workaround to thrown down the selection and reselecting the AIS.
      // If there was a problem here, try the first solution with close/open local context.

      aShapeAIS->Set(aTDS);
      aShapeAIS->Redisplay(Standard_True);
    }
  } else
    setImpl(new Handle(AIS_InteractiveObject)(new AIS_Shape(aTDS)));
}

void GeomAPI_AISObject::createDistance(std::shared_ptr<GeomAPI_Pnt> theStartPoint,
                                       std::shared_ptr<GeomAPI_Pnt> theEndPoint,
                                       std::shared_ptr<GeomAPI_Pnt> theFlyoutPoint,
                                       std::shared_ptr<GeomAPI_Pln> thePlane, double theDistance)
{
  double aFlyout = 0;
  if (theFlyoutPoint) {
    double aDist = 0.0;
    if (theStartPoint->distance(theEndPoint) < tolerance)
      aDist = theStartPoint->distance(theFlyoutPoint);
    else {
      std::shared_ptr<GeomAPI_Lin> aLine = std::shared_ptr<GeomAPI_Lin>(
          new GeomAPI_Lin(theStartPoint, theEndPoint));
      aDist = aLine->distance(theFlyoutPoint);
    }

    std::shared_ptr<GeomAPI_XYZ> aLineDir = theEndPoint->xyz()->decreased(theStartPoint->xyz());
    std::shared_ptr<GeomAPI_XYZ> aFOutDir = theFlyoutPoint->xyz()->decreased(
        theStartPoint->xyz());
    std::shared_ptr<GeomAPI_XYZ> aNorm = thePlane->direction()->xyz();
    if (aLineDir->cross(aFOutDir)->dot(aNorm) < 0)
      aDist = -aDist;
    aFlyout = aDist;
  }

  Handle(AIS_InteractiveObject) anAIS = impl<Handle(AIS_InteractiveObject)>();
  if (anAIS.IsNull()) {
    Handle(AIS_LengthDimension) aDimAIS = new AIS_LengthDimension(theStartPoint->impl<gp_Pnt>(),
                                                                  theEndPoint->impl<gp_Pnt>(),
                                                                  thePlane->impl<gp_Pln>());
    aDimAIS->SetCustomValue(theDistance);

    Handle(Prs3d_DimensionAspect) anAspect = new Prs3d_DimensionAspect();
    anAspect->MakeArrows3d(Standard_False);
    anAspect->MakeText3d(false);
    anAspect->TextAspect()->SetHeight(CONSTRAINT_TEXT_HEIGHT);
    anAspect->MakeTextShaded(false);
    anAspect->ArrowAspect()->SetLength(theDistance / 10.);
    aDimAIS->DimensionAspect()->MakeUnitsDisplayed(false);
    aDimAIS->SetDimensionAspect(anAspect);
    aDimAIS->SetSelToleranceForText2d(CONSTRAINT_TEXT_SELECTION_TOLERANCE);
    aDimAIS->SetFlyout(aFlyout);

    setImpl(new Handle(AIS_InteractiveObject)(aDimAIS));
  } else {
    // update presentation
    Handle(AIS_LengthDimension) aDimAIS = Handle(AIS_LengthDimension)::DownCast(anAIS);
    if (!aDimAIS.IsNull()) {
      aDimAIS->SetMeasuredGeometry(theStartPoint->impl<gp_Pnt>(), theEndPoint->impl<gp_Pnt>(),
                                   thePlane->impl<gp_Pln>());
      aDimAIS->SetCustomValue(theDistance);
      aDimAIS->SetFlyout(aFlyout);

      aDimAIS->Redisplay(Standard_True);
    }
  }
}

void GeomAPI_AISObject::createRadius(std::shared_ptr<GeomAPI_Circ> theCircle,
                                     std::shared_ptr<GeomAPI_Pnt> theFlyoutPoint,
                                     double theRadius)
{
  std::shared_ptr<GeomAPI_Pnt> aCenter = theCircle->center();

  // TODO: a bug in AIS_RadiusDimension:
  // The anchor point can't be myCirc.Location() - an exception is raised.
  // But we need exactly this case...
  // We want to show a radius dimension starting from the circle centre and 
  // ending at the user-defined point.
  // Also, if anchor point coincides with myP2, the radius dimension is not displayed at all.
  std::shared_ptr<GeomAPI_Pnt> anAnchor = theCircle->project(theFlyoutPoint);
  std::shared_ptr<GeomAPI_XYZ> anAnchorXYZ = anAnchor->xyz();
  anAnchorXYZ = anAnchorXYZ->decreased(aCenter->xyz());
  std::shared_ptr<GeomAPI_Dir> aDeltaDir(new GeomAPI_Dir(anAnchorXYZ));
  const double aDelta = 1e-3;
  anAnchor->setX(anAnchor->x() + aDelta * aDeltaDir->x());
  anAnchor->setY(anAnchor->y() + aDelta * aDeltaDir->y());
  anAnchor->setZ(anAnchor->z() + aDelta * aDeltaDir->z());

  Handle(AIS_InteractiveObject) anAIS = impl<Handle(AIS_InteractiveObject)>();
  if (anAIS.IsNull()) {
    Handle(AIS_RadiusDimension) aDimAIS = new AIS_RadiusDimension(theCircle->impl<gp_Circ>(),
                                                                  anAnchor->impl<gp_Pnt>());
    aDimAIS->SetCustomValue(theRadius);

    Handle(Prs3d_DimensionAspect) anAspect = new Prs3d_DimensionAspect();
    anAspect->MakeArrows3d(Standard_False);
    anAspect->MakeText3d(false);
    anAspect->TextAspect()->SetHeight(CONSTRAINT_TEXT_HEIGHT);
    anAspect->ArrowAspect()->SetLength(theRadius / 5.);
    anAspect->MakeTextShaded(false);
    aDimAIS->DimensionAspect()->MakeUnitsDisplayed(false);
    aDimAIS->SetDimensionAspect(anAspect);
    aDimAIS->SetSelToleranceForText2d(CONSTRAINT_TEXT_SELECTION_TOLERANCE);

    setImpl(new Handle(AIS_InteractiveObject)(aDimAIS));
  } else {
    // update presentation
    Handle(AIS_RadiusDimension) aDimAIS = Handle(AIS_RadiusDimension)::DownCast(anAIS);
    if (!aDimAIS.IsNull()) {
      aDimAIS->SetMeasuredGeometry(theCircle->impl<gp_Circ>(), anAnchor->impl<gp_Pnt>());
      aDimAIS->SetCustomValue(theRadius);
      aDimAIS->Redisplay(Standard_True);
    }
  }
}

void GeomAPI_AISObject::createParallel(std::shared_ptr<GeomAPI_Shape> theLine1,
                                       std::shared_ptr<GeomAPI_Shape> theLine2,
                                       std::shared_ptr<GeomAPI_Pnt> theFlyoutPoint,
                                       std::shared_ptr<GeomAPI_Pln> thePlane)
{
  Handle(Geom_Plane) aPlane = new Geom_Plane(thePlane->impl<gp_Pln>());
  Handle(AIS_InteractiveObject) anAIS = impl<Handle(AIS_InteractiveObject)>();
  if (anAIS.IsNull()) {
    Handle(AIS_ParallelRelation) aParallel = new AIS_ParallelRelation(
        theLine1->impl<TopoDS_Shape>(), theLine2->impl<TopoDS_Shape>(), aPlane);
    if (theFlyoutPoint)
      aParallel->SetPosition(theFlyoutPoint->impl<gp_Pnt>());

    setImpl(new Handle(AIS_InteractiveObject)(aParallel));
  } else {
    Handle(AIS_ParallelRelation) aParallel = Handle(AIS_ParallelRelation)::DownCast(anAIS);
    if (!aParallel.IsNull()) {
      aParallel->SetFirstShape(theLine1->impl<TopoDS_Shape>());
      aParallel->SetSecondShape(theLine2->impl<TopoDS_Shape>());
      aParallel->SetPlane(aPlane);
      if (theFlyoutPoint)
        aParallel->SetPosition(theFlyoutPoint->impl<gp_Pnt>());
      aParallel->Redisplay(Standard_True);
    }
  }
}

void GeomAPI_AISObject::createPerpendicular(std::shared_ptr<GeomAPI_Shape> theLine1,
                                            std::shared_ptr<GeomAPI_Shape> theLine2,
                                            std::shared_ptr<GeomAPI_Pln> thePlane)
{
  Handle(Geom_Plane) aPlane = new Geom_Plane(thePlane->impl<gp_Pln>());
  Handle(AIS_InteractiveObject) anAIS = impl<Handle(AIS_InteractiveObject)>();
  if (anAIS.IsNull()) {
    Handle(AIS_PerpendicularRelation) aPerpendicular = new AIS_PerpendicularRelation(
        theLine1->impl<TopoDS_Shape>(), theLine2->impl<TopoDS_Shape>(), aPlane);

    setImpl(new Handle(AIS_InteractiveObject)(aPerpendicular));
  } else {
    Handle(AIS_PerpendicularRelation) aPerpendicular = Handle(AIS_PerpendicularRelation)::DownCast(
        anAIS);
    if (!aPerpendicular.IsNull()) {
      aPerpendicular->SetFirstShape(theLine1->impl<TopoDS_Shape>());
      aPerpendicular->SetSecondShape(theLine2->impl<TopoDS_Shape>());
      aPerpendicular->SetPlane(aPlane);
      aPerpendicular->Redisplay(Standard_True);
    }
  }
}


void GeomAPI_AISObject::createFixed(std::shared_ptr<GeomAPI_Shape> theShape,
                                    std::shared_ptr<GeomAPI_Pln> thePlane)
{
  Handle(Geom_Plane) aPlane = new Geom_Plane(thePlane->impl<gp_Pln>());
  Handle(AIS_InteractiveObject) anAIS = impl<Handle(AIS_InteractiveObject)>();
  TopoDS_Shape aShape = theShape->impl<TopoDS_Shape>();
  Handle(AIS_FixRelation) aFixPrs;
  if (anAIS.IsNull()) {
    aFixPrs = new AIS_FixRelation(aShape, aPlane);

    setImpl(new Handle(AIS_InteractiveObject)(aFixPrs));
  } else {
    aFixPrs = Handle(AIS_FixRelation)::DownCast(anAIS);
    if (!aFixPrs.IsNull()) {
      aFixPrs->SetFirstShape(aShape);
      aFixPrs->SetPlane(aPlane);
      aFixPrs->Redisplay(Standard_True);
    }
  }
  if (!aFixPrs.IsNull()) {
    Bnd_Box aBox;
    BRepBndLib::Add(aShape, aBox);
    double aXmin, aXmax, aYmin, aYmax, aZmin, aZmax;
    aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
    gp_Pnt aXYZ1(aXmin, aXmax, aYmin);
    gp_Pnt aXYZ2(aXmax, aYmax, aZmax);
    double aDist = aXYZ1.Distance(aXYZ2);
    if (aDist > Precision::Confusion()) {
      aFixPrs->SetArrowSize(aDist/8.);
    }
  }
}



void GeomAPI_AISObject::setColor(const int& theColor)
{
  Handle(AIS_InteractiveObject) anAIS = impl<Handle(AIS_InteractiveObject)>();
  if (anAIS.IsNull())
    return;
  Quantity_Color aColor((Quantity_NameOfColor) theColor);
  anAIS->SetColor(aColor);
  Handle(AIS_Dimension) aDimAIS = Handle(AIS_Dimension)::DownCast(anAIS);
  if (!aDimAIS.IsNull()) {
    aDimAIS->DimensionAspect()->SetCommonColor(aColor);
  }
}

void GeomAPI_AISObject::setWidth(const double& theWidth)
{
  Handle(AIS_InteractiveObject) anAIS = impl<Handle(AIS_InteractiveObject)>();
  if (anAIS.IsNull())
    return;
  anAIS->SetWidth(theWidth);
  anAIS->Redisplay();
}

void GeomAPI_AISObject::setColor(int theR, int theG, int theB)
{
  Handle(AIS_InteractiveObject) anAIS = impl<Handle(AIS_InteractiveObject)>();
  if (anAIS.IsNull())
    return;
  Quantity_Color aColor(theR / 255., theG / 255., theB / 255., Quantity_TOC_RGB);
  anAIS->SetColor(aColor);
  Handle(AIS_Dimension) aDimAIS = Handle(AIS_Dimension)::DownCast(anAIS);
  if (!aDimAIS.IsNull()) {
    aDimAIS->DimensionAspect()->SetCommonColor(aColor);
  }
}

bool GeomAPI_AISObject::empty() const
{
  Handle(AIS_InteractiveObject) anAIS = const_cast<GeomAPI_AISObject*>(this)
      ->impl<Handle(AIS_InteractiveObject)>();
  if (anAIS.IsNull())
    return true;
  return false;
}

int GeomAPI_AISObject::getShapeType() const
{
  Handle(AIS_InteractiveObject) anAIS = const_cast<GeomAPI_AISObject*>(this)
      ->impl<Handle(AIS_InteractiveObject)>();
  if (!anAIS.IsNull()) {
    Handle(AIS_Shape) aAISShape = Handle(AIS_Shape)::DownCast(anAIS);
    if (!aAISShape.IsNull()) {
      return aAISShape->Shape().ShapeType();
    }
  }
  return -1;
}

void GeomAPI_AISObject::setPointMarker(int theType, double theScale)
{
  Handle(AIS_InteractiveObject) anAIS = impl<Handle(AIS_InteractiveObject)>();
  if (!anAIS.IsNull()) {
    Handle(AIS_Drawer) aDrawer = anAIS->Attributes();
    if (aDrawer->HasPointAspect()) {
      Handle(Prs3d_PointAspect) aPA = aDrawer->PointAspect();
      aPA->SetTypeOfMarker((Aspect_TypeOfMarker)theType);
      aPA->SetScale(theScale);
    } else {
      Quantity_NameOfColor aCol = Quantity_NOC_YELLOW;
      aDrawer->SetPointAspect(new Prs3d_PointAspect((Aspect_TypeOfMarker)theType, aCol, theScale));
    }
  }
}


void GeomAPI_AISObject::setLineStyle(int theStyle)
{
  Handle(AIS_InteractiveObject) anAIS = impl<Handle(AIS_InteractiveObject)>();
  if (!anAIS.IsNull()) {
    Handle(AIS_Drawer) aDrawer = anAIS->Attributes();
    if (aDrawer->HasLineAspect())
      aDrawer->LineAspect()->SetTypeOfLine((Aspect_TypeOfLine)theStyle);
    if (aDrawer->HasWireAspect())
      aDrawer->WireAspect()->SetTypeOfLine((Aspect_TypeOfLine)theStyle);
  }
}


void GeomAPI_AISObject::setTransparensy(double theVal)
{
  Handle(AIS_InteractiveObject) anAIS = impl<Handle(AIS_InteractiveObject)>();
  if (!anAIS.IsNull()) {
    Handle(AIS_InteractiveContext) aContext = anAIS->GetContext();
    if (!aContext.IsNull())
      aContext->SetTransparency(anAIS, theVal, false);
  }
}
