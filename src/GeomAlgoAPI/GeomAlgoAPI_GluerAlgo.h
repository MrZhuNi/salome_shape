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

#ifndef GEOMALGOAPI_GLUERALGO_H_
#define GEOMALGOAPI_GLUERALGO_H_

#include <GeomAlgoAPI.h>
#include <Standard_Macro.hxx>
#include <TopoDS_Shape.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <IntTools_Context.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>

//=======================================================================
//class    : GEOMAlgo_GluerAlgo
//purpose  :
//=======================================================================
class GeomAlgoAPI_GluerAlgo {
public:

  GEOMALGOAPI_EXPORT
    GeomAlgoAPI_GluerAlgo();

  GEOMALGOAPI_EXPORT
    virtual ~GeomAlgoAPI_GluerAlgo();

  GEOMALGOAPI_EXPORT
    virtual void SetArgument(const TopoDS_Shape& theShape) ;

  GEOMALGOAPI_EXPORT
    const TopoDS_Shape& Argument() const;

  GEOMALGOAPI_EXPORT
    void SetTolerance(const Standard_Real aT) ;

  GEOMALGOAPI_EXPORT
    Standard_Real Tolerance() const;

  GEOMALGOAPI_EXPORT
    void SetCheckGeometry(const Standard_Boolean aFlag) ;

  GEOMALGOAPI_EXPORT
    Standard_Boolean CheckGeometry() const;

  GEOMALGOAPI_EXPORT
    virtual  void Perform() ;

  GEOMALGOAPI_EXPORT
    virtual  void Clear() ;

  GEOMALGOAPI_EXPORT
    void SetContext(const Handle(IntTools_Context)&) ;

  GEOMALGOAPI_EXPORT
    const Handle(IntTools_Context)& Context() ;

  GEOMALGOAPI_EXPORT
    const TopTools_DataMapOfShapeListOfShape& Images() const;

  GEOMALGOAPI_EXPORT
    const TopTools_DataMapOfShapeShape& Origins() const;

protected:
  TopoDS_Shape myArgument;
  Standard_Real myTolerance;
  Standard_Boolean myCheckGeometry;
  Handle(IntTools_Context) myContext;
  TopTools_DataMapOfShapeListOfShape myImages;
  TopTools_DataMapOfShapeShape myOrigins;

private:
};
#endif
