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

#include "GeomAlgoAPI_BoundingBox.h"

#include <BRepBuilderAPI_Copy.hxx>
#include <Bnd_Box.hxx>
#include <BRepTools.hxx>
#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>
#include <BRep_Builder.hxx>
#include <Geom_Circle.hxx>
#include <ShapeAnalysis.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS.hxx>
#include <gp_Pln.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <ShapeFix_Shape.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <Standard_ErrorHandler.hxx>
#include <TopExp_Explorer.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>

//=================================================================================================
bool GetBoundingBox(const std::shared_ptr<GeomAPI_Shape>& theShape,
                    Standard_Real& theXmin,Standard_Real& theXmax,
                    Standard_Real& theYmin,Standard_Real& theYmax,
                    Standard_Real& theZmin,Standard_Real& theZmax,
                    std::string& theError)
{
  #ifdef _DEBUG
  std::cout << "GetBoundingBox " << std::endl;
  #endif

  if (!theShape.get()) {
    theError = "GetBoundingBox : An invalid argument";
    return false;
  }

  TopoDS_Shape aShape = theShape->impl<TopoDS_Shape>();

  //Compute the parameters
  Bnd_Box B;
  try {
    OCC_CATCH_SIGNALS;
    BRepBuilderAPI_Copy aCopyTool (aShape);
    if (!aCopyTool.IsDone()) {
      theError = "GetBoundingBox Error: Bad shape detected";
      return false;
    }

    aShape = aCopyTool.Shape();

    // remove triangulation to obtain more exact boundaries
    BRepTools::Clean(aShape);

    BRepBndLib::AddOptimal(aShape, B);

    if (B.IsXThin(Precision::Confusion())
        || B.IsYThin(Precision::Confusion())
        || B.IsZThin(Precision::Confusion())) {
      theError = "GetBoundingBox Error: Bounding box cannot be precised";
        return false;
    }

    B.Get(theXmin, theYmin, theZmin, theXmax, theYmax, theZmax);
  }
  catch (Standard_Failure& aFail) {
    theError = aFail.GetMessageString();
    return false;
  }
  return true;
}

