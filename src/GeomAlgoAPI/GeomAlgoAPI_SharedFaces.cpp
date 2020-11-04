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

#include "GeomAlgoAPI_SharedFaces.h"

#include <TopTools_SequenceOfShape.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Compound.hxx>
#include <BRep_Builder.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <GeomAlgoAPI_ShapeBuilder.h>
#include <vector>
#include <TopExp.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopExp_Explorer.hxx>


//=======================================================================
//function : GetSharedShapes
//purpose  :
//
// NOTE on the implementation
//
// 1) Resulting sub-shapes are published as a children of the 1st input shape
//    from theShapes list. Due to this reason only direct sub-shapes of the 1st
//    shape can be contained in the result of the operation (i.e. shares between
//    2nd/3rd, etc couples cannot be retrieved.
// 2) An exception from above case is when a single compound is specified as an
//    input. In this case we search shares between its top-level content, so we
//    are able to search shares between all possible couples of shapes.
// 3) Parameter theMultiShare controls what types of shares to search:
//    - True: get sub-shapes that are shared between ALL input shapes;
//    - False: get shares between couples of input sub-shapes (see points 1 and 2).
//
// Thus, we have the following cases:
// [1] theShapes = N shapes (N>1), theMultiShare = True
//     Result: sub-shapes that are shared by all theShapes
// [2] theShapes = N shapes (N>1), theMultiShare = False
//     Result: sub-shapes of 1st shape from theShapes that are shared with any shape
//     from theShapes
// [3] theShapes = 1 shape, theMultiShare = True
//     Result: sub-shapes that are shared by all top-level sub-objects of theShapes[0]
// [4] theShapes = 1 shape, theMultiShare = False
//     Result: sub-shapes of all possible couples of all top-level sub-objects of
//     theShapes[0].
//=======================================================================
//=================================================================================================
bool GetSharedredFaces( const ListOfShape& theShapes,
                        ListOfShape & theFaces,
                        const bool  theMultiShare,
                        std::string& theError)
{

  #ifdef _DEBUG
  std::cout << "GetSharedredFaces " << std::endl;
  #endif
  int aLen = theShapes.size();
  if (aLen < 1) return NULL;

  TopAbs_ShapeEnum aShapeType = TopAbs_FACE;

  TopoDS_Shape aShape;
  TopTools_SequenceOfShape aShapesSeq;

  ListOfShape::const_iterator anIt = theShapes.cbegin();

  for(; anIt != theShapes.cend(); ++anIt) {

    GeomShapePtr aShapePtr = *anIt;

    if (!aShapePtr.get()) {
      theError = "GetSharedredFaces : An invalid argument";
      return false;
    }
    aShape = aShapePtr->impl<TopoDS_Shape>();
    aShapesSeq.Append( aShape );
  }

  // if only single shape is specified as input
  // collect all ites top-level sub-shapes for processing
  if ( aShapesSeq.Length() == 1 )
  {
    aShape = aShapesSeq.First();
    aShapesSeq.Clear();
    for ( TopoDS_Iterator it( aShape ); it.More(); it.Next() )
      aShapesSeq.Append( it.Value() );
  }

  TopTools_MapOfShape mapShape;

  // find shared shapes

  // number of iterations
  int nbIters  =  theMultiShare || theShapes.size() > 1 ? 1 : aShapesSeq.Length()-1;
  // numShares factor to search (i.e. by what nb of shapes each found sub-shape should be shared)
  int nbShares =  theMultiShare ? aShapesSeq.Length()-1 : 1;

  for ( int iter = 1; iter <= nbIters; iter++) {
    for ( int ind = iter+1; ind <= aShapesSeq.Length(); ind++) {
      if ( ind-1+nbShares > aShapesSeq.Length() ) break;
      TopoDS_Compound aCurrSelection;
      TopoDS_Shape aShape1 = aShapesSeq.Value( iter );
      TopTools_IndexedMapOfShape mapSelected;
      TopExp::MapShapes(aShape1, aShapeType, mapSelected);
      for ( int s = 0; s < nbShares; s++ ) {
        BRep_Builder B;
        TopoDS_Compound aCompound;
        B.MakeCompound(aCompound);
        const TopoDS_Shape& aShape2 = aShapesSeq.Value( ind+s );
        TopTools_MapOfShape mapShape2;
        TopExp_Explorer exp (aShape2, aShapeType);
        for (; exp.More(); exp.Next()) {
          const TopoDS_Shape& aSS = exp.Current();
          if (mapShape2.Add(aSS) && mapSelected.Contains(aSS)) {
            B.Add(aCompound, aSS);
          }
        }
        mapSelected.Clear();
        aCurrSelection = aCompound;
        TopExp::MapShapes(aCurrSelection, aShapeType, mapSelected);
      }
      TopoDS_Iterator itSel(aCurrSelection, Standard_True, Standard_True);
      for (; itSel.More(); itSel.Next()) {
        const TopoDS_Shape& aSS = itSel.Value();
        GeomShapePtr aS(new GeomAPI_Shape);
        aS->setImpl<TopoDS_Shape>(new TopoDS_Shape(aSS));
        if (mapShape.Add(aSS) )
          theFaces.push_back(aS);
      }
    }
  }
  return true;
}

