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

#ifndef GEOMALGOAPI_GLUEDETECTOR_H_
#define GEOMALGOAPI_GLUEDETECTOR_H_

#include <GeomAlgoAPI.h>
#include <Standard.hxx>
#include <Standard_Macro.hxx>

#include <TopAbs_ShapeEnum.hxx>

#include <TopTools_ListOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>

#include <GeomAlgoAPI_GluerAlgo.h>
#include <GeomAlgoAPI_Algo.h>
#include <GEOMAlgo_PassKeyShape.hxx>

//=======================================================================
//function : GEOMAlgoAPI_GlueDetector
//purpose  :
//=======================================================================
class GeomAlgoAPI_GlueDetector  : public GeomAlgoAPI_GluerAlgo,
                                  public GeomAlgoAPI_Algo
{
public:
  GEOMALGOAPI_EXPORT
    GeomAlgoAPI_GlueDetector();

  GEOMALGOAPI_EXPORT virtual
    ~GeomAlgoAPI_GlueDetector();

  GEOMALGOAPI_EXPORT virtual
    void Perform() ;

  GEOMALGOAPI_EXPORT
    const TopTools_IndexedDataMapOfShapeListOfShape& StickedShapes();

protected:
  GEOMALGOAPI_EXPORT
    void DetectVertices() ;

  GEOMALGOAPI_EXPORT
    void DetectEdges() ;

  GEOMALGOAPI_EXPORT
    void DetectFaces() ;

  GEOMALGOAPI_EXPORT
    void DetectShapes(const TopAbs_ShapeEnum aType) ;

  GEOMALGOAPI_EXPORT
    void EdgePassKey(const TopoDS_Edge& aE,
                     GEOMAlgo_PassKeyShape& aPK) ;

  GEOMALGOAPI_EXPORT
    void FacePassKey(const TopoDS_Face& aF,
                     GEOMAlgo_PassKeyShape& aPK) ;

  GEOMALGOAPI_EXPORT
    void CheckDetected();
  //
  GEOMALGOAPI_EXPORT
    void CheckDetected
      (const TopTools_ListOfShape& aLVSD,
       const TopTools_IndexedDataMapOfShapeListOfShape& aMVE,
       const TopTools_IndexedDataMapOfShapeListOfShape& aMEV);


 protected:
  TopTools_IndexedDataMapOfShapeListOfShape myStickedShapes;

};
#endif
