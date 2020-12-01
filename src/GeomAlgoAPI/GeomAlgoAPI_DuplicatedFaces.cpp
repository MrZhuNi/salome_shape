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

#include "GeomAlgoAPI_DuplicatedFaces.h"

#include <TopTools_SequenceOfShape.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Compound.hxx>
#include <BRep_Builder.hxx>
#include <GeomAlgoAPI_GlueDetector.h>
#include <TopTools_IndexedMapOfShape.hxx>
#include <GeomAlgoAPI_ShapeBuilder.h>
#include <vector>
#include <GeomAlgoAPI_SortListOfShapes.h>
#include <TopExp.hxx>

//=================================================================================================
bool GetDuplicatedFaces(const ListOfShape& theShapes,
                        const Standard_Real& theTolerance,
                        ListOfShape & theFaces,
                        std::string& theError)
{
  #ifdef _DEBUG
  std::cout << "GetDuplicatedFaces " << std::endl;
  #endif

  TopoDS_Shape aShape;
  TopTools_SequenceOfShape aShapesSeq;

  ListOfShape::const_iterator anIt = theShapes.cbegin();

  for (; anIt != theShapes.cend(); ++anIt) {

    GeomShapePtr aShapePtr = *anIt;

    if (!aShapePtr.get()) {
      theError = "GetDuplicatedFaces : An invalid argument";
      return false;
    }
    aShape = aShapePtr->impl<TopoDS_Shape>();
    aShapesSeq.Append( aShape );
  }

  if (aShapesSeq.Length() > 1){
    TopoDS_Compound aCompound;
    BRep_Builder aBuilder;
    aBuilder.MakeCompound( aCompound );
    for ( int i = 1; i <= aShapesSeq.Length(); ++i )
      aBuilder.Add( aCompound, aShapesSeq( i ) );

    aShape = aCompound;
  }

  GeomAlgoAPI_GlueDetector aGluer;
  aGluer.SetArgument(aShape);
  aGluer.SetTolerance(theTolerance);
  aGluer.Perform();
  Standard_Integer iErr = aGluer.ErrorStatus();
  if (iErr) return NULL;

  std::vector< TopTools_IndexedMapOfShape* > anIndices( aShapesSeq.Length(), NULL );
  Handle(TColStd_HArray1OfInteger) anArray;
  GeomShapePtr anObj;

  ListOfShape listOnePerSet;

  const TopTools_DataMapOfShapeListOfShape& aImages = aGluer.Images();
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape aItDMSLS (aImages);
  for (int index = 1; aItDMSLS.More(); aItDMSLS.Next(), ++index) {
    // some key shape

    // list of shapes of the argument that can be glued
    const TopTools_ListOfShape& aLSD = aItDMSLS.Value();

    //listShape.Append(aLSD.First());
    TopoDS_Shape aValue = aLSD.First();

    if (aValue.ShapeType() == TopAbs_FACE) {
      GeomShapePtr aS(new GeomAPI_Shape);
      aS->setImpl<TopoDS_Shape>(new TopoDS_Shape(aValue));
      listOnePerSet.push_back(aS);
    }
  }

  // for stable order of returned entities
  GeomAlgoAPI_SortListOfShapes::sort(listOnePerSet);

  //TopTools_ListIteratorOfListOfShape aListIt (listOnePerSet);
  ListOfShape::const_iterator aListIt = listOnePerSet.cbegin();
  for (; aListIt != listOnePerSet.cend(); ++aListIt) {
    TopoDS_Shape aValue = (*aListIt)->impl<TopoDS_Shape>();
    // find a shape to add aValue as a sub-shape
    anObj.reset();

    anIt = theShapes.cbegin();
    GeomShapePtr aShapePtr;
    for (int i = 0; i < theShapes.size(); ++i, ++anIt) {
      aShapePtr = *anIt;
      if (!anIndices[i]) {
        anIndices[i] = new TopTools_IndexedMapOfShape;
        aShape = aShapePtr->impl<TopoDS_Shape>();
        TopExp::MapShapes( aShape, *anIndices[i]);
      }
      if (int index = anIndices[i]->FindIndex( aValue )) {

        const TopoDS_Shape& aSelShape = anIndices[i]->FindKey(index);
        GeomShapePtr aS(new GeomAPI_Shape);
        aS->setImpl<TopoDS_Shape>(new TopoDS_Shape(aSelShape));
        // GeomAlgoAPI_ShapeBuilder::add(aShapePtr,aS);
        if (aS.get())
            theFaces.push_back(aS);
        break;
      }
    }
  }
  for (size_t i = 0 ; i < anIndices.size(); ++i)
    delete anIndices[i];

  return true;
}

