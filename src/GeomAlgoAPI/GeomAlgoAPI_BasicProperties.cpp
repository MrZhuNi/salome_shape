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

#include "GeomAlgoAPI_BasicProperties.h"

#include <GProp_GProps.hxx>
#include <TopoDS_Shape.hxx>
#include <BRepGProp.hxx>
#include <TopExp_Explorer.hxx>
#include <Standard_ErrorHandler.hxx>

//=================================================================================================
bool GetBasicProperties(  const std::shared_ptr<GeomAPI_Shape>& theShape,
                          const double theTolerance,
                          Standard_Real& theLength,
                          Standard_Real& theSurfArea,
                          Standard_Real& theVolume,
                          std::string& theError)
{

  #ifdef _DEBUG
  std::cout << "GetBasicProperties " << std::endl;
  #endif

  if (!theShape.get()) {
    theError = "GetBasicProperties : An invalid argument";
    return false;
  }

  TopoDS_Shape aShape = theShape->impl<TopoDS_Shape>();

  //Compute the parameters
  GProp_GProps LProps, SProps;
  Standard_Real anEps = theTolerance >= 0 ? theTolerance : 1.e-6;
  try {
    OCC_CATCH_SIGNALS;
    BRepGProp::LinearProperties(aShape, LProps, Standard_True);
    theLength = LProps.Mass();

    BRepGProp::SurfaceProperties(aShape, SProps, anEps, Standard_True);
    theSurfArea = SProps.Mass();

    theVolume = 0.0;
    if (aShape.ShapeType() < TopAbs_SHELL) {
      for (TopExp_Explorer Exp (aShape, TopAbs_SOLID); Exp.More(); Exp.Next()) {
        GProp_GProps VProps;
        BRepGProp::VolumeProperties(Exp.Current(), VProps, anEps, Standard_True);
        theVolume += VProps.Mass();
      }
    }
  }
  catch (Standard_Failure& aFail) {
    theError = aFail.GetMessageString();
    return false;
  }

  return true;

}