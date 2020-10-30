// Copyright (C) 2017-2020  CEA/DEN, EDF R&D
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

#include <GeomAlgoAPI_Fillet.h>
#include <GeomAlgoAPI_DFLoader.h>

#include <BRepFilletAPI_MakeFillet.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <gp_Pnt2d.hxx>

//=================================================================================================
GeomAlgoAPI_Fillet::GeomAlgoAPI_Fillet(const GeomShapePtr& theBaseSolid,
                                       const ListOfShape&  theFilletEdges,
                                       const double        theFilletRadius)
{
  build(theBaseSolid, theFilletEdges, theFilletRadius);
}

//=================================================================================================
GeomAlgoAPI_Fillet::GeomAlgoAPI_Fillet(const GeomShapePtr& theBaseSolid,
                                       const ListOfShape&  theFilletEdges,
                                       const double        theStartRadius,
                                       const double        theEndRadius)
{
  if (theEndRadius < 0.)
    return;
  build(theBaseSolid, theFilletEdges, theStartRadius, theEndRadius);
}

//=================================================================================================
GeomAlgoAPI_Fillet::GeomAlgoAPI_Fillet(const GeomShapePtr& theBaseSolid,
                                       const ListOfShape&  theFilletEdges,
                                       const std::list<double>& theCurvCoord,
                                       const std::list<double>& theRadiuses)
{
  if (theRadiuses.size()== 0 )
    return;
  build(theBaseSolid, theFilletEdges,theCurvCoord, theRadiuses);
}

//=================================================================================================
void GeomAlgoAPI_Fillet::build(const GeomShapePtr& theBaseSolid,
                               const ListOfShape&  theFilletEdges,
                               const double        theStartRadius,
                               const double        theEndRadius)
{
  if (!theBaseSolid || theFilletEdges.empty() || theStartRadius < 0.)
    return;

  // create fillet builder
  BRepFilletAPI_MakeFillet* aFilletBuilder =
      new BRepFilletAPI_MakeFillet(theBaseSolid->impl<TopoDS_Shape>());
  setImpl(aFilletBuilder);
  setBuilderType(OCCT_BRepBuilderAPI_MakeShape);

  // assign filleting edges
  for (ListOfShape::const_iterator anIt = theFilletEdges.begin();
       anIt != theFilletEdges.end(); ++anIt) {
    if ((*anIt)->isEdge())
      aFilletBuilder->Add( (*anIt)->impl<TopoDS_Edge>() );
  }

  // assign fillet radii for each contour of filleting edges
  bool isFixedRadius = theEndRadius < 0.;
  int aNbContours = aFilletBuilder->NbContours();
  for (int ind = 1; ind <= aNbContours; ++ind) {
    if (isFixedRadius)
      aFilletBuilder->SetRadius(theStartRadius, ind, 1);
    else
      aFilletBuilder->SetRadius(theStartRadius, theEndRadius, ind, 1);
  }

  // build and get result
  aFilletBuilder->Build();
  if (!aFilletBuilder->IsDone())
    return;
  TopoDS_Shape aResult = GeomAlgoAPI_DFLoader::refineResult(aFilletBuilder->Shape());

  std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(aResult));
  setShape(aShape);
  setDone(true);
}

//=================================================================================================
void GeomAlgoAPI_Fillet::build(const GeomShapePtr& theBaseSolid,
                               const ListOfShape&  theFilletEdges,
                               const std::list<double>& theCurvCoord,
                               const std::list<double>& theRadiuses)
{
  if (!theBaseSolid || theFilletEdges.empty() || theRadiuses.size() == 0)
    return;

  // create fillet builder
  BRepFilletAPI_MakeFillet* aFilletBuilder =
      new BRepFilletAPI_MakeFillet(theBaseSolid->impl<TopoDS_Shape>());
  setImpl(aFilletBuilder);
  setBuilderType(OCCT_BRepBuilderAPI_MakeShape);

  // assign filleting edges
  for (ListOfShape::const_iterator anIt = theFilletEdges.begin();
       anIt != theFilletEdges.end(); ++anIt) {
    if ((*anIt)->isEdge())
      aFilletBuilder->Add( (*anIt)->impl<TopoDS_Edge>() );
  }

  TColgp_Array1OfPnt2d array(1, theRadiuses.size());

  int i = 1;
  std::list<double>::const_iterator anItCurv = theCurvCoord.begin();
  std::list<double>::const_iterator anItRadius = theRadiuses.begin();

  for( ; anItCurv != theCurvCoord.end(); ++anItCurv, ++anItRadius )
  {
    array.SetValue(i, gp_Pnt2d( (*anItCurv) , (*anItRadius)));
    i++;
  }
  // assign fillet radii for each contour of filleting edges
  int aNbContours = aFilletBuilder->NbContours();
  for (int ind = 1; ind <= aNbContours; ++ind) {
    aFilletBuilder->SetRadius(array, ind, 1);
  }

  // build and get result
  aFilletBuilder->Build();
  if (!aFilletBuilder->IsDone())
    return;
  TopoDS_Shape aResult = GeomAlgoAPI_DFLoader::refineResult(aFilletBuilder->Shape());

  std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(aResult));
  setShape(aShape);
  setDone(true);
}