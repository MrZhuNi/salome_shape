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

#include "GeomAlgoAPI_ContinuousFaces.h"

#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>
#include <LocalAnalysis_SurfaceContinuity.hxx>
#include <ShapeAnalysis_Surface.hxx>
#include <gp_Pnt.hxx>

#include <Standard_ErrorHandler.hxx>

const double PI = 3.141592653589793238463;

//=======================================================================
bool isContinuousFaces(const GeomShapePtr& theFace1,
                       const GeomShapePtr& theFace2,
                       const GeomPointPtr& thePoint,
                       const double & theAngle,
                       std::string& theError)
{

  #ifdef _DEBUG
  std::cout << "isContinuousFaces " << std::endl;
  #endif

  if (!thePoint.get()) {
      theError = "isContinuousFaces : An invalid argument";
      return false;
  }
  const gp_Pnt& aPoint = thePoint->impl<gp_Pnt>();

  // Getting base shape.
  if (!theFace1.get()) {
    theError = "isContinuousFaces : An invalid argument";
    return false;
  }

  TopoDS_Shape aShape1 = theFace1->impl<TopoDS_Shape>();

  if (aShape1.IsNull()) {
    theError = "isContinuousFaces : An invalid argument";
    return false;
  }

  // Getting base shape.
  if (!theFace2.get()) {
    theError = "isContinuousFaces : An invalid argument";
    return false;
  }

  TopoDS_Shape aShape2 = theFace2->impl<TopoDS_Shape>();

  if (aShape2.IsNull()) {
    theError = "isContinuousFaces : An invalid argument";
    return false;
  }

  TopoDS_Face aFace1 = TopoDS::Face(aShape1);
  if (aFace1.IsNull()) {
    theError = "isContinuousFaces : An invalid argument";
    return false;
  }

  Handle(Geom_Surface) aSurf1 = BRep_Tool::Surface(aFace1);
  if (aSurf1.IsNull()) {
    theError = "isContinuousFaces : An invalid surface";
    return false;
  }

  ShapeAnalysis_Surface aSAS1(aSurf1);
  gp_Pnt2d aPointOnFace1 = aSAS1.ValueOfUV(aPoint, Precision::Confusion());

  TopoDS_Face aFace2 = TopoDS::Face(aShape2);
  if (aFace2.IsNull()) {
    theError = "isContinuousFaces : An invalid argument";
    return false;
  }

  Handle(Geom_Surface) aSurf2 = BRep_Tool::Surface(aFace2);
  if (aSurf2.IsNull()) {
    theError = "isContinuousFaces : An invalid surface";
    return false;
  }

  ShapeAnalysis_Surface aSAS2(aSurf2);
  gp_Pnt2d aPointOnFace2= aSAS2.ValueOfUV(aPoint, Precision::Confusion());

  bool aRes = false;
  try {
    OCC_CATCH_SIGNALS;
    LocalAnalysis_SurfaceContinuity aLocAnal(aSurf1,
                                             aPointOnFace1.X(),
                                             aPointOnFace1.Y(),
                                             aSurf2,
                                             aPointOnFace2.X(),
                                             aPointOnFace2.Y(),
                                             GeomAbs_Shape::GeomAbs_G1, // Order
                                             0.001, // EpsNul
                                             0.001, // EpsC0
                                             0.001, // EpsC1
                                             0.001, // EpsC2
                                             theAngle * PI/ 180.0); //EpsG1
    aRes = aLocAnal.IsG1();
  }
  catch (Standard_Failure const& anException) {
    theError = "LocalAnalysis_SurfaceContinuity error :";
    theError += anException.GetMessageString();
  }

  return aRes;
}
