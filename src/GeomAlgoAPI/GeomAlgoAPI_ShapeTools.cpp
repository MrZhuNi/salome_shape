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

#include "GeomAlgoAPI_ShapeTools.h"

#include "GeomAlgoAPI_SketchBuilder.h"

#include <GeomAPI_Ax1.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Dir.h>
#include <GeomAPI_Face.h>
#include <GeomAPI_Pln.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Wire.h>

#include <Bnd_Box.hxx>
#include <BRep_Builder.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAlgo.hxx>
#include <BRepAlgo_FaceRestrictor.hxx>
#include <BRepBndLib.hxx>
#include <BRepBuilderAPI_FindPlane.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepExtrema_ExtCF.hxx>
#include <BRepGProp.hxx>
#include <BRepTools.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Curve.hxx>
#include <BRepLib_CheckCurveOnSurface.hxx>
#include <BRep_Tool.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomLib_IsPlanarSurface.hxx>
#include <GeomLib_Tool.hxx>
#include <GeomAPI_IntCS.hxx>
#include <gp_Pln.hxx>
#include <GProp_GProps.hxx>
#include <IntAna_IntConicQuad.hxx>
#include <IntAna_Quadric.hxx>
#include <NCollection_Vector.hxx>
#include <ShapeAnalysis.hxx>
#include <ShapeAnalysis_Surface.hxx>
#include <TopoDS_Builder.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>


#include <BOPAlgo_Builder.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <TopoDS_Edge.hxx>

//==================================================================================================
double GeomAlgoAPI_ShapeTools::volume(const std::shared_ptr<GeomAPI_Shape> theShape)
{
  GProp_GProps aGProps;
  if(!theShape.get()) {
    return 0.0;
  }
  const TopoDS_Shape& aShape = theShape->impl<TopoDS_Shape>();
  if(aShape.IsNull()) {
    return 0.0;
  }
  const Standard_Real anEps = 1.e-6;
  if (aShape.ShapeType() <= TopAbs_SOLID)
    BRepGProp::VolumeProperties(aShape, aGProps, anEps);
  else
    BRepGProp::SurfaceProperties(aShape, aGProps, anEps);
  return aGProps.Mass();
}

//==================================================================================================
double GeomAlgoAPI_ShapeTools::area (const std::shared_ptr<GeomAPI_Shape> theShape)
{
  GProp_GProps aGProps;
  if(!theShape.get()) {
    return 0.0;
  }
  const TopoDS_Shape& aShape = theShape->impl<TopoDS_Shape>();
  if(aShape.IsNull()) {
    return 0.0;
  }
  const Standard_Real anEps = 1.e-6;

  BRepGProp::SurfaceProperties(aShape, aGProps, anEps);
  return aGProps.Mass();
}

//==================================================================================================
std::shared_ptr<GeomAPI_Pnt>
  GeomAlgoAPI_ShapeTools::centreOfMass(const std::shared_ptr<GeomAPI_Shape> theShape)
{
  GProp_GProps aGProps;
  if(!theShape) {
    return std::shared_ptr<GeomAPI_Pnt>();
  }
  const TopoDS_Shape& aShape = theShape->impl<TopoDS_Shape>();
  if(aShape.IsNull()) {
    return std::shared_ptr<GeomAPI_Pnt>();
  }
  gp_Pnt aCentre;
  if(aShape.ShapeType() == TopAbs_VERTEX) {
    aCentre = BRep_Tool::Pnt(TopoDS::Vertex(aShape));
  } else if(aShape.ShapeType() == TopAbs_EDGE || aShape.ShapeType() == TopAbs_WIRE) {
    BRepGProp::LinearProperties(aShape, aGProps);
    aCentre = aGProps.CentreOfMass();
  } else {
    const Standard_Real anEps = 1.e-6;
    BRepGProp::SurfaceProperties(aShape, aGProps, anEps);
    aCentre = aGProps.CentreOfMass();
  }
  return std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(aCentre.X(), aCentre.Y(), aCentre.Z()));
}

//==================================================================================================
std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeTools::combineShapes(
  const std::shared_ptr<GeomAPI_Shape> theCompound,
  const GeomAPI_Shape::ShapeType theType,
  ListOfShape& theCombinedShapes,
  ListOfShape& theFreeShapes)
{
  GeomShapePtr aResult = theCompound;

  if(!theCompound.get()) {
    return aResult;
  }

  if(theType != GeomAPI_Shape::SHELL && theType != GeomAPI_Shape::COMPSOLID) {
    return aResult;
  }

  TopAbs_ShapeEnum aTS = TopAbs_EDGE;
  TopAbs_ShapeEnum aTA = TopAbs_FACE;
  if(theType == GeomAPI_Shape::COMPSOLID) {
    aTS = TopAbs_FACE;
    aTA = TopAbs_SOLID;
  }

  theCombinedShapes.clear();
  theFreeShapes.clear();

  // Get free shapes.
  const TopoDS_Shape& aShapesComp = theCompound->impl<TopoDS_Shape>();
  for(TopoDS_Iterator anIter(aShapesComp); anIter.More(); anIter.Next() ) {
    const TopoDS_Shape& aShape = anIter.Value();
    if(aShape.ShapeType() > aTA) {
      std::shared_ptr<GeomAPI_Shape> aGeomShape(new GeomAPI_Shape);
      aGeomShape->setImpl<TopoDS_Shape>(new TopoDS_Shape(aShape));
      theFreeShapes.push_back(aGeomShape);
    }
  }

  // Map subshapes and shapes.
  TopTools_IndexedDataMapOfShapeListOfShape aMapSA;
  TopExp::MapShapesAndAncestors(aShapesComp, aTS, aTA, aMapSA);
  if(aMapSA.IsEmpty()) {
    return aResult;
  }

  // Get all shapes with common subshapes and free shapes.
  NCollection_Map<TopoDS_Shape> aFreeShapes;
  NCollection_Vector<NCollection_Map<TopoDS_Shape>> aShapesWithCommonSubshapes;
  for(TopTools_IndexedDataMapOfShapeListOfShape::Iterator
      anIter(aMapSA); anIter.More(); anIter.Next()) {
    const TopoDS_Shape& aShape = anIter.Key();
    TopTools_ListOfShape& aListOfShape = anIter.ChangeValue();
    if(aListOfShape.IsEmpty()) {
      continue;
    }
    else if(aListOfShape.Size() == 1) {
      const TopoDS_Shape& aF = aListOfShape.First();
      aFreeShapes.Add(aF);
      aListOfShape.Clear();
    } else {
      NCollection_List<TopoDS_Shape> aTempList;
      NCollection_Map<TopoDS_Shape> aTempMap;
      const TopoDS_Shape& aF = aListOfShape.First();
      const TopoDS_Shape& aL = aListOfShape.Last();
      aTempList.Append(aF);
      aTempList.Append(aL);
      aTempMap.Add(aF);
      aTempMap.Add(aL);
      aFreeShapes.Remove(aF);
      aFreeShapes.Remove(aL);
      aListOfShape.Clear();
      for(NCollection_List<TopoDS_Shape>::Iterator
          aTempIter(aTempList); aTempIter.More(); aTempIter.Next()) {
        const TopoDS_Shape& aTempShape = aTempIter.Value();
        for(TopTools_IndexedDataMapOfShapeListOfShape::Iterator
            anIter(aMapSA); anIter.More(); anIter.Next()) {
          TopTools_ListOfShape& aTempListOfShape = anIter.ChangeValue();
          if(aTempListOfShape.IsEmpty()) {
            continue;
          } else if(aTempListOfShape.Size() == 1 && aTempListOfShape.First() == aTempShape) {
            aTempListOfShape.Clear();
          } else if(aTempListOfShape.Size() > 1) {
            if(aTempListOfShape.First() == aTempShape) {
              const TopoDS_Shape& aTL = aTempListOfShape.Last();
              if(aTempMap.Add(aTL)) {
                aTempList.Append(aTL);
                aFreeShapes.Remove(aTL);
              }
              aTempListOfShape.Clear();
            } else if(aTempListOfShape.Last() == aTempShape) {
              const TopoDS_Shape& aTF = aTempListOfShape.First();
              if(aTempMap.Add(aTF)) {
                aTempList.Append(aTF);
                aFreeShapes.Remove(aTF);
              }
              aTempListOfShape.Clear();
            }
          }
        }
      }
      aShapesWithCommonSubshapes.Append(aTempMap);
    }
  }

  // Combine shapes with common subshapes.
  for(NCollection_Vector<NCollection_Map<TopoDS_Shape>>::Iterator
      anIter(aShapesWithCommonSubshapes); anIter.More(); anIter.Next()) {
    TopoDS_Shell aShell;
    TopoDS_CompSolid aCSolid;
    TopoDS_Builder aBuilder;
    theType ==
      GeomAPI_Shape::COMPSOLID ? aBuilder.MakeCompSolid(aCSolid) : aBuilder.MakeShell(aShell);
    NCollection_Map<TopoDS_Shape>& aShapesMap = anIter.ChangeValue();
    for(TopExp_Explorer anExp(aShapesComp, aTA); anExp.More(); anExp.Next()) {
      const TopoDS_Shape& aShape = anExp.Current();
      if(aShapesMap.Contains(aShape)) {
        theType ==
          GeomAPI_Shape::COMPSOLID ? aBuilder.Add(aCSolid, aShape) : aBuilder.Add(aShell, aShape);
        aShapesMap.Remove(aShape);
      }
    }
    std::shared_ptr<GeomAPI_Shape> aGeomShape(new GeomAPI_Shape);
    TopoDS_Shape* aSh = theType == GeomAPI_Shape::COMPSOLID ? new TopoDS_Shape(aCSolid) :
                                                              new TopoDS_Shape(aShell);
    aGeomShape->setImpl<TopoDS_Shape>(aSh);
    theCombinedShapes.push_back(aGeomShape);
  }

  // Adding free shapes.
  for(TopExp_Explorer anExp(aShapesComp, aTA); anExp.More(); anExp.Next()) {
    const TopoDS_Shape& aShape = anExp.Current();
    if(aFreeShapes.Contains(aShape)) {
      std::shared_ptr<GeomAPI_Shape> aGeomShape(new GeomAPI_Shape);
      aGeomShape->setImpl<TopoDS_Shape>(new TopoDS_Shape(aShape));
      theFreeShapes.push_back(aGeomShape);
    }
  }

  if(theCombinedShapes.size() == 1 && theFreeShapes.size() == 0) {
    aResult = theCombinedShapes.front();
  } else if(theCombinedShapes.size() == 0 && theFreeShapes.size() == 1) {
    aResult = theFreeShapes.front();
  } else {
    TopoDS_Compound aResultComp;
    TopoDS_Builder aBuilder;
    aBuilder.MakeCompound(aResultComp);
    for(ListOfShape::const_iterator anIter = theCombinedShapes.cbegin();
        anIter != theCombinedShapes.cend(); anIter++) {
      aBuilder.Add(aResultComp, (*anIter)->impl<TopoDS_Shape>());
    }
    for(ListOfShape::const_iterator anIter = theFreeShapes.cbegin();
        anIter != theFreeShapes.cend(); anIter++) {
      aBuilder.Add(aResultComp, (*anIter)->impl<TopoDS_Shape>());
    }
    aResult->setImpl(new TopoDS_Shape(aResultComp));
  }

  return aResult;
}

//==================================================================================================
static void addSimpleShapeToList(const TopoDS_Shape& theShape,
                                 NCollection_List<TopoDS_Shape>& theList)
{
  if(theShape.IsNull()) {
    return;
  }

  if(theShape.ShapeType() == TopAbs_COMPOUND) {
    for(TopoDS_Iterator anIt(theShape); anIt.More(); anIt.Next()) {
      addSimpleShapeToList(anIt.Value(), theList);
    }
  } else {
    theList.Append(theShape);
  }
}

//==================================================================================================
static TopoDS_Compound makeCompound(const NCollection_List<TopoDS_Shape> theShapes)
{
  TopoDS_Compound aCompound;

  BRep_Builder aBuilder;
  aBuilder.MakeCompound(aCompound);

  for(NCollection_List<TopoDS_Shape>::Iterator anIt(theShapes); anIt.More(); anIt.Next()) {
    aBuilder.Add(aCompound, anIt.Value());
  }

  return aCompound;
}

//==================================================================================================
std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeTools::groupSharedTopology(
  const std::shared_ptr<GeomAPI_Shape> theCompound)
{
  GeomShapePtr aResult = theCompound;

  if(!theCompound.get()) {
    return aResult;
  }

  TopoDS_Shape anInShape = aResult->impl<TopoDS_Shape>();
  NCollection_List<TopoDS_Shape> anUngroupedShapes, aStillUngroupedShapes;
  addSimpleShapeToList(anInShape, anUngroupedShapes);

  // Iterate over all shapes and find shapes with shared vertices.
  TopTools_ListOfShape aMapOrder;
  TopTools_DataMapOfShapeListOfShape aVertexShapesMap;
  for(NCollection_List<TopoDS_Shape>::Iterator aShapesIt(anUngroupedShapes);
      aShapesIt.More();
      aShapesIt.Next()) {
    const TopoDS_Shape& aShape = aShapesIt.Value();
    for(TopExp_Explorer aShapeExp(aShape, TopAbs_VERTEX);
        aShapeExp.More();
        aShapeExp.Next()) {
      const TopoDS_Shape& aVertex = aShapeExp.Current();
      if (!aVertexShapesMap.IsBound(aVertex)) {
        NCollection_List<TopoDS_Shape> aList;
        aList.Append(aShape);
        aMapOrder.Append(aVertex);
        aVertexShapesMap.Bind(aVertex, aList);
      } else {
        if(!aVertexShapesMap.ChangeFind(aVertex).Contains(aShape)) {
          aVertexShapesMap.ChangeFind(aVertex).Append(aShape);
        }
      }
    }
  }

  // Iterate over the map and group shapes.
  NCollection_Vector<TopTools_ListOfShape> aGroups;
  while (!aMapOrder.IsEmpty()) {
    // Get first group of shapes in map, and then unbind it.
    const TopoDS_Shape& aKey = aMapOrder.First();
    TopTools_ListOfShape aGroupedShapes = aVertexShapesMap.Find(aKey);
    aVertexShapesMap.UnBind(aKey);
    aMapOrder.Remove(aKey);
    // Iterate over shapes in this group and add to it shapes from groups in map.
    for(TopTools_ListOfShape::Iterator aGroupIt(aGroupedShapes);
        aGroupIt.More(); aGroupIt.Next()) {
      const TopoDS_Shape& aGroupedShape = aGroupIt.Value();
      TopTools_ListOfShape aKeysToUnbind;
      for(TopTools_ListOfShape::Iterator aKeysIt(aMapOrder);
          aKeysIt.More();
          aKeysIt.Next()) {
        const TopTools_ListOfShape& aGroupInMap = aVertexShapesMap(aKeysIt.Value());
        if(!aGroupInMap.Contains(aGroupedShape)) {
          // Group in map does not containt shape from our group, so go to the next group in map.
          continue;
        }
        // Iterate over shape in group in map, and add new shapes into our group.
        for(TopTools_ListOfShape::Iterator aGroupInMapIt(aGroupInMap);
            aGroupInMapIt.More();
            aGroupInMapIt.Next()) {
          const TopoDS_Shape& aShape = aGroupInMapIt.Value();
          if (!aGroupedShapes.Contains(aShape)) {
            aGroupedShapes.Append(aShape);
          }
        }
        // Save key to unbind from this map.
        aKeysToUnbind.Append(aKeysIt.Value());
      }
      // Unbind groups from map that we added to our group.
      for(TopTools_ListOfShape::Iterator aKeysIt(aKeysToUnbind);
          aKeysIt.More();
          aKeysIt.Next()) {
        aVertexShapesMap.UnBind(aKeysIt.Value());
        aMapOrder.Remove(aKeysIt.Value());
      }
    }
    // Sort shapes.
    TopTools_ListOfShape aSortedGroup;
    for(int aST = TopAbs_COMPOUND; aST <= TopAbs_SHAPE; ++aST) {
      TopTools_ListOfShape::Iterator anIt(aGroupedShapes);
      while (anIt.More()) {
        if(anIt.Value().ShapeType() == aST) {
          aSortedGroup.Append(anIt.Value());
          aGroupedShapes.Remove(anIt);
        } else {
          anIt.Next();
        }
      }
    }
    aGroups.Append(aSortedGroup);
  }

  TopoDS_Compound aCompound;
  BRep_Builder aBuilder;
  aBuilder.MakeCompound(aCompound);
  ListOfShape aCompSolids, aFreeSolids;
  for(NCollection_Vector<NCollection_List<TopoDS_Shape>>::Iterator
      anIt(aGroups); anIt.More(); anIt.Next()) {
    NCollection_List<TopoDS_Shape> aGroup = anIt.Value();
    GeomShapePtr aGeomShape(new GeomAPI_Shape());
    if(aGroup.Size() == 1) {
      aGeomShape->setImpl(new TopoDS_Shape(aGroup.First()));
    } else {
      aGeomShape->setImpl(new TopoDS_Shape(makeCompound(aGroup)));
      aGeomShape = GeomAlgoAPI_ShapeTools::combineShapes(aGeomShape,
                                                         GeomAPI_Shape::COMPSOLID,
                                                         aCompSolids,
                                                         aFreeSolids);
    }
    aBuilder.Add(aCompound, aGeomShape->impl<TopoDS_Shape>());
  }

  if(!aCompound.IsNull()) {
    aResult->setImpl(new TopoDS_Shape(aCompound));
  }

  return aResult;
}

//==================================================================================================
std::list<std::shared_ptr<GeomAPI_Pnt> >
  GeomAlgoAPI_ShapeTools::getBoundingBox(const ListOfShape& theShapes, const double theEnlarge)
{
  // Bounding box of all objects.
  Bnd_Box aBndBox;

  // Getting box.
  for (ListOfShape::const_iterator
    anObjectsIt = theShapes.begin(); anObjectsIt != theShapes.end(); anObjectsIt++) {
    const TopoDS_Shape& aShape = (*anObjectsIt)->impl<TopoDS_Shape>();
    BRepBndLib::Add(aShape, aBndBox);
  }

  if(theEnlarge != 0.0) {
    // We enlarge bounding box just to be sure that plane will be large enough to cut all objects.
    aBndBox.Enlarge(theEnlarge);
  }

  Standard_Real aXArr[2] = {aBndBox.CornerMin().X(), aBndBox.CornerMax().X()};
  Standard_Real aYArr[2] = {aBndBox.CornerMin().Y(), aBndBox.CornerMax().Y()};
  Standard_Real aZArr[2] = {aBndBox.CornerMin().Z(), aBndBox.CornerMax().Z()};
  std::list<std::shared_ptr<GeomAPI_Pnt> > aResultPoints;
  int aNum = 0;
  for(int i = 0; i < 2; i++) {
    for(int j = 0; j < 2; j++) {
      for(int k = 0; k < 2; k++) {
        std::shared_ptr<GeomAPI_Pnt> aPnt(new GeomAPI_Pnt(aXArr[i], aYArr[j], aZArr[k]));
        aResultPoints.push_back(aPnt);
      }
    }
  }

  return aResultPoints;
}

//==================================================================================================
std::shared_ptr<GeomAPI_Shape>
  GeomAlgoAPI_ShapeTools::faceToInfinitePlane(const std::shared_ptr<GeomAPI_Shape> theFace)
{
  if (!theFace.get())
    return std::shared_ptr<GeomAPI_Shape>();

  TopoDS_Face aPlaneFace = TopoDS::Face(theFace->impl<TopoDS_Shape>());
  if (aPlaneFace.IsNull())
    return std::shared_ptr<GeomAPI_Shape>();

  Handle(Geom_Plane) aPlane = Handle(Geom_Plane)::DownCast(BRep_Tool::Surface(aPlaneFace));
  if (aPlane.IsNull())
    return std::shared_ptr<GeomAPI_Shape>();

  // make an infinity face on the plane
  TopoDS_Shape anInfiniteFace = BRepBuilderAPI_MakeFace(aPlane->Pln()).Shape();

  std::shared_ptr<GeomAPI_Shape> aResult(new GeomAPI_Shape);
  aResult->setImpl(new TopoDS_Shape(anInfiniteFace));
  return aResult;
}

//==================================================================================================
std::shared_ptr<GeomAPI_Face> GeomAlgoAPI_ShapeTools::fitPlaneToBox(
  const std::shared_ptr<GeomAPI_Shape> thePlane,
  const std::list<std::shared_ptr<GeomAPI_Pnt> >& thePoints)
{
  std::shared_ptr<GeomAPI_Face> aResultFace;

  if(!thePlane.get()) {
    return aResultFace;
  }

  const TopoDS_Shape& aShape = thePlane->impl<TopoDS_Shape>();
  if(aShape.ShapeType() != TopAbs_FACE) {
    return aResultFace;
  }

  TopoDS_Face aFace = TopoDS::Face(aShape);
  Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace);
  if(aSurf.IsNull()) {
    return aResultFace;
  }

  GeomLib_IsPlanarSurface isPlanar(aSurf);
  if(!isPlanar.IsPlanar()) {
    return aResultFace;
  }

  if(thePoints.size() != 8) {
    return aResultFace;
  }

  const gp_Pln& aFacePln = isPlanar.Plan();
  Handle(Geom_Plane) aFacePlane = new Geom_Plane(aFacePln);
  IntAna_Quadric aQuadric(aFacePln);
  Standard_Real UMin, UMax, VMin, VMax;
  UMin = UMax = VMin = VMax = 0;
  for (std::list<std::shared_ptr<GeomAPI_Pnt> >::const_iterator
       aPointsIt = thePoints.begin(); aPointsIt != thePoints.end(); aPointsIt++) {
    const gp_Pnt& aPnt = (*aPointsIt)->impl<gp_Pnt>();
    gp_Lin aLin(aPnt, aFacePln.Axis().Direction());
    IntAna_IntConicQuad anIntAna(aLin, aQuadric);
    const gp_Pnt& aPntOnFace = anIntAna.Point(1);
    Standard_Real aPntU(0), aPntV(0);
    GeomLib_Tool::Parameters(aFacePlane, aPntOnFace, Precision::Confusion(), aPntU, aPntV);
    if(aPntU < UMin) UMin = aPntU;
    if(aPntU > UMax) UMax = aPntU;
    if(aPntV < VMin) VMin = aPntV;
    if(aPntV > VMax) VMax = aPntV;
  }
  aResultFace.reset(new GeomAPI_Face());
  aResultFace->setImpl(new TopoDS_Face(BRepLib_MakeFace(aFacePln, UMin, UMax, VMin, VMax).Face()));

  return aResultFace;
}

//==================================================================================================
void GeomAlgoAPI_ShapeTools::findBounds(const std::shared_ptr<GeomAPI_Shape> theShape,
                                        std::shared_ptr<GeomAPI_Vertex>& theV1,
                                        std::shared_ptr<GeomAPI_Vertex>& theV2)
{
  if(!theShape.get()) {
    std::shared_ptr<GeomAPI_Vertex> aVertex(new GeomAPI_Vertex);
    aVertex->setImpl(new TopoDS_Vertex());
    theV1 = aVertex;
    theV2 = aVertex;
    return;
  }

  const TopoDS_Shape& aShape = theShape->impl<TopoDS_Shape>();
  TopoDS_Vertex aV1, aV2;
  ShapeAnalysis::FindBounds(aShape, aV1, aV2);

  std::shared_ptr<GeomAPI_Vertex> aGeomV1(new GeomAPI_Vertex()), aGeomV2(new GeomAPI_Vertex());
  aGeomV1->setImpl(new TopoDS_Vertex(aV1));
  aGeomV2->setImpl(new TopoDS_Vertex(aV2));
  theV1 = aGeomV1;
  theV2 = aGeomV2;
}

//==================================================================================================
void GeomAlgoAPI_ShapeTools::makeFacesWithHoles(const std::shared_ptr<GeomAPI_Pnt> theOrigin,
                                                const std::shared_ptr<GeomAPI_Dir> theDirection,
                                                const ListOfShape& theWires,
                                                ListOfShape& theFaces)
{
  BRepBuilderAPI_MakeFace aMKFace(gp_Pln(theOrigin->impl<gp_Pnt>(),
                                          theDirection->impl<gp_Dir>()));
  TopoDS_Face aFace = aMKFace.Face();

  BRepAlgo_FaceRestrictor aFRestrictor;
  aFRestrictor.Init(aFace, Standard_False, Standard_True);
  for(ListOfShape::const_iterator anIt = theWires.cbegin();
      anIt != theWires.cend();
      ++anIt) {
    TopoDS_Wire aWire = TopoDS::Wire((*anIt)->impl<TopoDS_Shape>());
    aFRestrictor.Add(aWire);
  }

  aFRestrictor.Perform();

  if(!aFRestrictor.IsDone()) {
    return;
  }

  for(; aFRestrictor.More(); aFRestrictor.Next()) {
    GeomShapePtr aShape(new GeomAPI_Shape());
    aShape->setImpl(new TopoDS_Shape(aFRestrictor.Current()));
    theFaces.push_back(aShape);
  }
}

//==================================================================================================
std::shared_ptr<GeomAPI_Pln> GeomAlgoAPI_ShapeTools::findPlane(const ListOfShape& theShapes)
{
  TopoDS_Compound aCompound;
  BRep_Builder aBuilder;
  aBuilder.MakeCompound(aCompound);

  for(ListOfShape::const_iterator anIt = theShapes.cbegin(); anIt != theShapes.cend(); ++anIt) {
    aBuilder.Add(aCompound, (*anIt)->impl<TopoDS_Shape>());
  }
  BRepBuilderAPI_FindPlane aFindPlane(aCompound);

  if(aFindPlane.Found() != Standard_True) {
    return std::shared_ptr<GeomAPI_Pln>();
  }

  Handle(Geom_Plane) aPlane = aFindPlane.Plane();
  gp_Pnt aLoc = aPlane->Location();
  gp_Dir aDir = aPlane->Axis().Direction();

  std::shared_ptr<GeomAPI_Pnt> aGeomPnt(new GeomAPI_Pnt(aLoc.X(), aLoc.Y(), aLoc.Z()));
  std::shared_ptr<GeomAPI_Dir> aGeomDir(new GeomAPI_Dir(aDir.X(), aDir.Y(), aDir.Z()));

  std::shared_ptr<GeomAPI_Pln> aPln(new GeomAPI_Pln(aGeomPnt, aGeomDir));

  return aPln;
}

//==================================================================================================
bool GeomAlgoAPI_ShapeTools::isSubShapeInsideShape(
  const std::shared_ptr<GeomAPI_Shape> theSubShape,
  const std::shared_ptr<GeomAPI_Shape> theBaseShape)
{
  if(!theSubShape.get() || !theBaseShape.get()) {
    return false;
  }

  const TopoDS_Shape& aSubShape = theSubShape->impl<TopoDS_Shape>();
  const TopoDS_Shape& aBaseShape = theBaseShape->impl<TopoDS_Shape>();

  if(aSubShape.ShapeType() == TopAbs_VERTEX) {
    // If sub-shape is a vertex check distance to shape. If it is <= Precision::Confusion() then OK.
    BRepExtrema_DistShapeShape aDist(aBaseShape, aSubShape);
    aDist.Perform();
    if(!aDist.IsDone() || aDist.Value() > Precision::Confusion()) {
      return false;
    }
  } else if (aSubShape.ShapeType() == TopAbs_EDGE) {
    if(aBaseShape.ShapeType() == TopAbs_FACE) {
      // Check that edge is on face surface.
      TopoDS_Face aFace = TopoDS::Face(aBaseShape);
      TopoDS_Edge anEdge = TopoDS::Edge(aSubShape);
      BRepLib_CheckCurveOnSurface aCheck(anEdge, aFace);
      aCheck.Perform();
      if(!aCheck.IsDone() || aCheck.MaxDistance() > Precision::Confusion()) {
        return false;
      }

      // Check intersections.
      TopoDS_Vertex aV1, aV2;
      ShapeAnalysis::FindBounds(anEdge, aV1, aV2);
      gp_Pnt aPnt1 = BRep_Tool::Pnt(aV1);
      gp_Pnt aPnt2 = BRep_Tool::Pnt(aV2);
      for(TopExp_Explorer anExp(aBaseShape, TopAbs_EDGE); anExp.More(); anExp.Next()) {
        const TopoDS_Shape& anEdgeOnFace = anExp.Current();
        BRepExtrema_DistShapeShape aDist(anEdgeOnFace, anEdge);
        aDist.Perform();
        if(aDist.IsDone() && aDist.Value() <= Precision::Confusion()) {
          // Edge intersect face bound. Check that it is not on edge begin or end.
          for(Standard_Integer anIndex = 1; anIndex <= aDist.NbSolution(); ++anIndex) {
            gp_Pnt aPntOnSubShape = aDist.PointOnShape2(anIndex);
            if(aPntOnSubShape.Distance(aPnt1) > Precision::Confusion()
                && aPntOnSubShape.Distance(aPnt2) > Precision::Confusion()) {
              return false;
            }
          }
        }
      }

      // No intersections found. Edge is inside or outside face. Check it.
      BRepAdaptor_Curve aCurveAdaptor(anEdge);
      gp_Pnt aPointToCheck =
        aCurveAdaptor.Value((aCurveAdaptor.FirstParameter() +
                              aCurveAdaptor.LastParameter()) / 2.0);
      Handle(Geom_Surface) aSurface = BRep_Tool::Surface(aFace);
      ShapeAnalysis_Surface aSAS(aSurface);
      gp_Pnt2d aPointOnFace = aSAS.ValueOfUV(aPointToCheck, Precision::Confusion());
      BRepTopAdaptor_FClass2d aFClass2d(aFace, Precision::Confusion());
      if(aFClass2d.Perform(aPointOnFace) == TopAbs_OUT) {
        return false;
      }

    } else {
      return false;
    }
  } else {
    return false;
  }

  return true;
}

//==================================================================================================
bool GeomAlgoAPI_ShapeTools::isShapeValid(const std::shared_ptr<GeomAPI_Shape> theShape)
{
  if(!theShape.get()) {
    return false;
  }

  BRepCheck_Analyzer aChecker(theShape->impl<TopoDS_Shape>());
  return (aChecker.IsValid() == Standard_True);
}

//==================================================================================================
std::shared_ptr<GeomAPI_Shape>
  GeomAlgoAPI_ShapeTools::getFaceOuterWire(const std::shared_ptr<GeomAPI_Shape> theFace)
{
  GeomShapePtr anOuterWire;

  if(!theFace.get() || !theFace->isFace()) {
    return anOuterWire;
  }

  TopoDS_Face aFace = TopoDS::Face(theFace->impl<TopoDS_Shape>());
  TopoDS_Wire aWire = BRepTools::OuterWire(aFace);

  anOuterWire.reset(new GeomAPI_Shape());
  anOuterWire->setImpl(new TopoDS_Shape(aWire));

  return anOuterWire;
}

//==================================================================================================
bool GeomAlgoAPI_ShapeTools::isParallel(const std::shared_ptr<GeomAPI_Edge> theEdge,
                                        const std::shared_ptr<GeomAPI_Face> theFace)
{
  if(!theEdge.get() || !theFace.get()) {
    return false;
  }

  TopoDS_Edge anEdge = TopoDS::Edge(theEdge->impl<TopoDS_Shape>());
  TopoDS_Face aFace  = TopoDS::Face(theFace->impl<TopoDS_Shape>());

  BRepExtrema_ExtCF anExt(anEdge, aFace);
  return anExt.IsParallel() == Standard_True;
}

//==================================================================================================
std::list<std::shared_ptr<GeomAPI_Vertex> > GeomAlgoAPI_ShapeTools::intersect(
  const std::shared_ptr<GeomAPI_Edge> theEdge, const std::shared_ptr<GeomAPI_Face> theFace,
  const bool thePointsOutsideFace)
{
  std::list<std::shared_ptr<GeomAPI_Vertex> > aResult;
  if(!theEdge.get() || !theFace.get()) {
    return aResult;
  }

  TopoDS_Edge anEdge = TopoDS::Edge(theEdge->impl<TopoDS_Shape>());
  double aFirstOnCurve, aLastOnCurve;
  Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, aFirstOnCurve, aLastOnCurve);

  TopoDS_Face aFace  = TopoDS::Face(theFace->impl<TopoDS_Shape>());
  Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace);

  GeomAPI_IntCS anIntAlgo(aCurve, aSurf);
  if (!anIntAlgo.IsDone())
    return aResult;
  // searching for points-intersection
  for(int anIntNum = 1; anIntNum <= anIntAlgo.NbPoints() + anIntAlgo.NbSegments(); anIntNum++) {
    gp_Pnt anInt;
    if (anIntNum <= anIntAlgo.NbPoints()) {
      anInt = anIntAlgo.Point(anIntNum);
    } else { // take the middle point on the segment of the intersection
      Handle(Geom_Curve) anIntCurve = anIntAlgo.Segment(anIntNum - anIntAlgo.NbPoints());
      anIntCurve->D0((anIntCurve->FirstParameter() + anIntCurve->LastParameter()) / 2., anInt);
    }
    aResult.push_back(std::shared_ptr<GeomAPI_Vertex>(
      new GeomAPI_Vertex(anInt.X(), anInt.Y(), anInt.Z())));
  }
  return aResult;
}

//==================================================================================================
void GeomAlgoAPI_ShapeTools::splitShape(const std::shared_ptr<GeomAPI_Shape>& theBaseShape,
                                      const GeomAlgoAPI_ShapeTools::PointToRefsMap& thePointsInfo,
                                      std::set<std::shared_ptr<GeomAPI_Shape> >& theShapes)
{
  // to split shape at least one point should be presented in the points container
  if (thePointsInfo.empty())
    return;

    // General Fuse to split edge by vertices
  BOPAlgo_Builder aBOP;
  TopoDS_Edge aBaseEdge = theBaseShape->impl<TopoDS_Edge>();
  // Rebuild closed edge to place vertex to one of split points.
  // This will prevent edge to be split on same vertex.
  if (BRep_Tool::IsClosed(aBaseEdge))
  {
    Standard_Real aFirst, aLast;
    Handle(Geom_Curve) aCurve = BRep_Tool::Curve(aBaseEdge, aFirst, aLast);

    PointToRefsMap::const_iterator aPIt = thePointsInfo.begin();
    std::shared_ptr<GeomAPI_Pnt> aPnt = aPIt->first;
    gp_Pnt aPoint(aPnt->x(), aPnt->y(), aPnt->z());

    TopAbs_Orientation anOrientation = aBaseEdge.Orientation();
    aBaseEdge = BRepBuilderAPI_MakeEdge(aCurve, aPoint, aPoint).Edge();
    aBaseEdge.Orientation(anOrientation);
  }
  aBOP.AddArgument(aBaseEdge);

  PointToRefsMap::const_iterator aPIt = thePointsInfo.begin();
  for (; aPIt != thePointsInfo.end(); ++aPIt) {
    std::shared_ptr<GeomAPI_Pnt> aPnt = aPIt->first;
    TopoDS_Vertex aV = BRepBuilderAPI_MakeVertex(gp_Pnt(aPnt->x(), aPnt->y(), aPnt->z()));
    aBOP.AddArgument(aV);
  }

  aBOP.Perform();
  if (aBOP.HasErrors())
    return;

  // Collect splits
  const TopTools_ListOfShape& aSplits = aBOP.Modified(aBaseEdge);
  TopTools_ListIteratorOfListOfShape anIt(aSplits);
  for (; anIt.More(); anIt.Next()) {
    std::shared_ptr<GeomAPI_Shape> anEdge(new GeomAPI_Shape);
    anEdge->setImpl(new TopoDS_Shape(anIt.Value()));
    theShapes.insert(anEdge);
  }
}

//==================================================================================================
void GeomAlgoAPI_ShapeTools::splitShape_p(const std::shared_ptr<GeomAPI_Shape>& theBaseShape,
                                          const std::list<std::shared_ptr<GeomAPI_Pnt> >& thePoints,
                                          std::set<std::shared_ptr<GeomAPI_Shape> >& theShapes)
{
  // General Fuse to split edge by vertices
  BOPAlgo_Builder aBOP;
  TopoDS_Edge aBaseEdge = theBaseShape->impl<TopoDS_Edge>();
  // Rebuild closed edge to place vertex to one of split points.
  // This will prevent edge to be split on seam vertex.
  if (BRep_Tool::IsClosed(aBaseEdge))
  {
    Standard_Real aFirst, aLast;
    Handle(Geom_Curve) aCurve = BRep_Tool::Curve(aBaseEdge, aFirst, aLast);

    std::list<std::shared_ptr<GeomAPI_Pnt> >::const_iterator aPIt = thePoints.begin();
    gp_Pnt aPoint((*aPIt)->x(), (*aPIt)->y(), (*aPIt)->z());

    TopAbs_Orientation anOrientation = aBaseEdge.Orientation();
    aBaseEdge = BRepBuilderAPI_MakeEdge(aCurve, aPoint, aPoint).Edge();
    aBaseEdge.Orientation(anOrientation);
  }
  aBOP.AddArgument(aBaseEdge);

  std::list<std::shared_ptr<GeomAPI_Pnt> >::const_iterator aPtIt = thePoints.begin();
  for (; aPtIt != thePoints.end(); ++aPtIt) {
    std::shared_ptr<GeomAPI_Pnt> aPnt = *aPtIt;
    TopoDS_Vertex aV = BRepBuilderAPI_MakeVertex(gp_Pnt(aPnt->x(), aPnt->y(), aPnt->z()));
    aBOP.AddArgument(aV);
  }

  aBOP.Perform();
  if (aBOP.HasErrors())
    return;

  // Collect splits
  const TopTools_ListOfShape& aSplits = aBOP.Modified(aBaseEdge);
  TopTools_ListIteratorOfListOfShape anIt(aSplits);
  for (; anIt.More(); anIt.Next()) {
    std::shared_ptr<GeomAPI_Shape> anEdge(new GeomAPI_Shape);
    anEdge->setImpl(new TopoDS_Shape(anIt.Value()));
    theShapes.insert(anEdge);
  }
}

//==================================================================================================
std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeTools::findShape(
                                  const std::list<std::shared_ptr<GeomAPI_Pnt> >& thePoints,
                                  const std::set<std::shared_ptr<GeomAPI_Shape> >& theShapes)
{
  std::shared_ptr<GeomAPI_Shape> aResultShape;

  if (thePoints.size() == 2) {
    std::list<std::shared_ptr<GeomAPI_Pnt> >::const_iterator aPntIt = thePoints.begin();
    std::shared_ptr<GeomAPI_Pnt> aFirstPoint = *aPntIt;
    aPntIt++;
    std::shared_ptr<GeomAPI_Pnt> aLastPoint = *aPntIt;

    std::set<std::shared_ptr<GeomAPI_Shape> >::const_iterator anIt = theShapes.begin(),
                                                              aLast = theShapes.end();
    for (; anIt != aLast; anIt++) {
      GeomShapePtr aShape = *anIt;
      std::shared_ptr<GeomAPI_Edge> anEdge(new GeomAPI_Edge(aShape));
      if (anEdge.get()) {
        std::shared_ptr<GeomAPI_Pnt> anEdgeFirstPoint = anEdge->firstPoint();
        std::shared_ptr<GeomAPI_Pnt> anEdgeLastPoint = anEdge->lastPoint();
        if (anEdgeFirstPoint->isEqual(aFirstPoint) &&
            anEdgeLastPoint->isEqual(aLastPoint))
            aResultShape = aShape;
      }
    }
  }

  return aResultShape;
}

//==================================================================================================
std::shared_ptr<GeomAPI_Dir> GeomAlgoAPI_ShapeTools::buildDirFromAxisAndShape(
                                    const std::shared_ptr<GeomAPI_Shape> theBaseShape,
                                    const std::shared_ptr<GeomAPI_Ax1> theAxis)
{
  gp_Pnt aCentreOfMassPoint =
    GeomAlgoAPI_ShapeTools::centreOfMass(theBaseShape)->impl<gp_Pnt>();
  Handle(Geom_Line) aLine = new Geom_Line(theAxis->impl<gp_Ax1>());
  GeomAPI_ProjectPointOnCurve aPrjTool(aCentreOfMassPoint, aLine);
  gp_Pnt aPoint = aPrjTool.NearestPoint();

  std::shared_ptr<GeomAPI_Dir> aDir(new GeomAPI_Dir(aCentreOfMassPoint.X()-aPoint.X(),
                                                    aCentreOfMassPoint.Y()-aPoint.Y(),
                                                    aCentreOfMassPoint.Z()-aPoint.Z()));
  return aDir;
}

//==================================================================================================
static TopoDS_Wire fixParametricGaps(const TopoDS_Wire& theWire)
{
  TopoDS_Wire aFixedWire;
  Handle(Geom_Curve) aPrevCurve;
  double aPrevLastParam = 0.0;

  BRep_Builder aBuilder;
  aBuilder.MakeWire(aFixedWire);

  BRepTools_WireExplorer aWExp(theWire);
  for (; aWExp.More(); aWExp.Next()) {
    TopoDS_Edge anEdge = aWExp.Current();
    double aFirst, aLast;
    Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, aFirst, aLast);
    if (aCurve == aPrevCurve) {
      // if parametric gap occurs, create new edge based on the copied curve
      aCurve = Handle(Geom_Curve)::DownCast(aCurve->Copy());
      TopoDS_Vertex aV1, aV2;
      TopExp::Vertices(anEdge, aV1, aV2);
      anEdge = TopoDS::Edge(anEdge.EmptyCopied());
      aBuilder.UpdateEdge(anEdge, aCurve, BRep_Tool::Tolerance(anEdge));
      aBuilder.Add(anEdge, aV1);
      aBuilder.Add(anEdge, aV2);
    }

    aBuilder.Add(aFixedWire, anEdge);

    aPrevCurve = aCurve;
    aPrevLastParam = aLast;
  }

  return aFixedWire;
}

std::shared_ptr<GeomAPI_Edge> GeomAlgoAPI_ShapeTools::wireToEdge(
      const std::shared_ptr<GeomAPI_Wire>& theWire)
{
  GeomEdgePtr anEdge;
  if (theWire) {
    TopoDS_Wire aWire = theWire->impl<TopoDS_Wire>();
    // Workaround: when concatenate a wire consisting of two edges based on the same B-spline curve
    // (non-periodic, but having equal start and end points), first of which is placed at the end
    // on the curve and second is placed at the start, this workaround copies second curve to avoid
    // treating these edges as a single curve by setting trim parameters.
    aWire = fixParametricGaps(aWire);
    TopoDS_Edge aNewEdge = BRepAlgo::ConcatenateWireC0(aWire);
    anEdge = GeomEdgePtr(new GeomAPI_Edge);
    anEdge->setImpl(new TopoDS_Edge(aNewEdge));
  }
  return anEdge;
}
