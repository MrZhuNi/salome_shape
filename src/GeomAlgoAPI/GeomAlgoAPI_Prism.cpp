// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Prism.cpp
// Created:     5 May 2015
// Author:      Dmitry Bobylev

#include "GeomAlgoAPI_Prism.h"

#include <GeomAPI_Face.h>
#include <GeomAPI_Pln.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_ShapeExplorer.h>
#include <GeomAPI_XYZ.h>
#include <GeomAlgoAPI_DFLoader.h>
#include <GeomAlgoAPI_FaceBuilder.h>
#include <GeomAlgoAPI_ShapeTools.h>

#include <Bnd_Box.hxx>
#include <BRep_Builder.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepBndLib.hxx>
#include <BRepBuilderAPI_FindPlane.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepTools.hxx>
#include <Geom_Curve.hxx>
#include <Geom2d_Curve.hxx>
#include <BRepLib_CheckCurveOnSurface.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <gp_Pln.hxx>
#include <IntAna_IntConicQuad.hxx>
#include <IntAna_Quadric.hxx>
#include <IntTools_Context.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

//=================================================================================================
GeomAlgoAPI_Prism::GeomAlgoAPI_Prism(const GeomShapePtr theBaseShape,
                                     const double       theToSize,
                                     const double       theFromSize)
{
  build(theBaseShape, std::shared_ptr<GeomAPI_Dir>(), GeomShapePtr(), theToSize, GeomShapePtr(), theFromSize);
}

//=================================================================================================
GeomAlgoAPI_Prism::GeomAlgoAPI_Prism(const GeomShapePtr                 theBaseShape,
                                     const std::shared_ptr<GeomAPI_Dir> theDirection,
                                     const double                       theToSize,
                                     const double                       theFromSize)
{
  build(theBaseShape, theDirection, GeomShapePtr(), theToSize, GeomShapePtr(), theFromSize);
}

//=================================================================================================
GeomAlgoAPI_Prism::GeomAlgoAPI_Prism(const GeomShapePtr theBaseShape,
                                     const GeomShapePtr theToShape,
                                     const double       theToSize,
                                     const GeomShapePtr theFromShape,
                                     const double       theFromSize)
{
  build(theBaseShape, std::shared_ptr<GeomAPI_Dir>(), theToShape, theToSize, theFromShape, theFromSize);
}

//=================================================================================================
GeomAlgoAPI_Prism::GeomAlgoAPI_Prism(const GeomShapePtr                 theBaseShape,
                                     const std::shared_ptr<GeomAPI_Dir> theDirection,
                                     const GeomShapePtr                 theToShape,
                                     const double                       theToSize,
                                     const GeomShapePtr                 theFromShape,
                                     const double                       theFromSize)
{
  build(theBaseShape, theDirection, theToShape, theToSize, theFromShape, theFromSize);
}

//=================================================================================================
void GeomAlgoAPI_Prism::build(const GeomShapePtr&                theBaseShape,
                              const std::shared_ptr<GeomAPI_Dir> theDirection,
                              const GeomShapePtr&                theToShape,
                              const double                       theToSize,
                              const GeomShapePtr&                theFromShape,
                              const double                       theFromSize)
{
  if(!theBaseShape.get() ||
    (((!theFromShape.get() && !theToShape.get()) || (theFromShape.get() && theToShape.get() && theFromShape->isEqual(theToShape)))
    && (theFromSize == -theToSize))) {
    return;
  }

  // Getting base shape.
  const TopoDS_Shape& aBaseShape = theBaseShape->impl<TopoDS_Shape>();
  TopAbs_ShapeEnum aShapeTypeToExp;
  switch(aBaseShape.ShapeType()) {
    case TopAbs_VERTEX:
      aShapeTypeToExp = TopAbs_VERTEX;
      break;
    case TopAbs_EDGE:
    case TopAbs_WIRE:
      aShapeTypeToExp = TopAbs_EDGE;
      break;
    case TopAbs_FACE:
    case TopAbs_SHELL:
      aShapeTypeToExp = TopAbs_FACE;
      break;
    default:
      return;
  }

  // Getting direction.
  gp_Vec aDirVec;
  std::shared_ptr<GeomAPI_Pnt> aBaseLoc;
  std::shared_ptr<GeomAPI_Dir> aBaseDir;
  GeomShapePtr aBasePlane;
  const bool isBoundingShapesSet = theFromShape.get() || theToShape.get();
  BRepBuilderAPI_FindPlane aFindPlane(aBaseShape);
  if(theDirection.get()) {
    aBaseDir = theDirection;
    aDirVec = theDirection->impl<gp_Dir>();
  } else {
    if(aBaseShape.ShapeType() == TopAbs_VERTEX
        || aBaseShape.ShapeType() == TopAbs_EDGE
        || aFindPlane.Found() == Standard_False) {
      return;
    }

    Handle(Geom_Plane) aPlane;
    if(aBaseShape.ShapeType() == TopAbs_FACE || aBaseShape.ShapeType() == TopAbs_SHELL) {
      TopExp_Explorer anExp(aBaseShape, TopAbs_FACE);
      const TopoDS_Shape& aFace = anExp.Current();
      Handle(Geom_Surface) aSurface = BRep_Tool::Surface(TopoDS::Face(aFace));
      if(aSurface->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
        Handle(Geom_RectangularTrimmedSurface) aTrimSurface =
          Handle(Geom_RectangularTrimmedSurface)::DownCast(aSurface);
        aSurface = aTrimSurface->BasisSurface();
      }
      if(aSurface->DynamicType() != STANDARD_TYPE(Geom_Plane)) {
        return;
      }
      aPlane = Handle(Geom_Plane)::DownCast(aSurface);
    } else {
      aPlane = aFindPlane.Plane();
    }
    gp_Pnt aLoc = aPlane->Axis().Location();
    aDirVec = aPlane->Axis().Direction();
    aBaseLoc.reset(new GeomAPI_Pnt(aLoc.X(), aLoc.Y(), aLoc.Z()));
    aBaseDir.reset(new GeomAPI_Dir(aDirVec.X(), aDirVec.Y(), aDirVec.Z()));
  }
  if(!aBaseLoc.get()) {
    gp_Pnt aLoc;
    gp_XYZ aDirXYZ = aDirVec.XYZ();
    Standard_Real aMinParam = Precision::Infinite();
    for(TopExp_Explorer anExp(aBaseShape, TopAbs_VERTEX); anExp.More(); anExp.Next()) {
      const TopoDS_Shape& aVertex = anExp.Current();
      gp_Pnt aPnt = BRep_Tool::Pnt(TopoDS::Vertex(aVertex));
      double aParam = aDirXYZ.Dot(aPnt.XYZ());
      if(aParam < aMinParam) {
        aMinParam = aParam;
        aLoc = aPnt;
      }
    }
    aBaseLoc.reset(new GeomAPI_Pnt(aLoc.X(), aLoc.Y(), aLoc.Z()));
  }
  aBasePlane = GeomAlgoAPI_FaceBuilder::planarFace(aBaseLoc, aBaseDir);

  TopoDS_Shape aResult;
  if(!isBoundingShapesSet) {
    // Moving base shape.
    gp_Trsf aTrsf;
    aTrsf.SetTranslation(aDirVec * -theFromSize);
    BRepBuilderAPI_Transform* aTransformBuilder = new BRepBuilderAPI_Transform(aBaseShape, aTrsf);
    if(!aTransformBuilder) {
      return;
    }
    this->appendAlgo(std::shared_ptr<GeomAlgoAPI_MakeShape>(new GeomAlgoAPI_MakeShape(aTransformBuilder)));
    if(!aTransformBuilder->IsDone()) {
      return;
    }
    TopoDS_Shape aMovedBase = aTransformBuilder->Shape();

    // Making prism.
    BRepPrimAPI_MakePrism* aPrismBuilder = new BRepPrimAPI_MakePrism(aMovedBase, aDirVec * (theFromSize + theToSize));
    if(!aPrismBuilder) {
      return;
    }
    this->appendAlgo(std::shared_ptr<GeomAlgoAPI_MakeShape>(new GeomAlgoAPI_MakeShape(aPrismBuilder)));
    if(!aPrismBuilder->IsDone()) {
      return;
    }
    aResult = aPrismBuilder->Shape();

    // Setting naming.
    for(TopExp_Explorer anExp(aMovedBase, aShapeTypeToExp); anExp.More(); anExp.Next()) {
      const TopoDS_Shape& aShape = anExp.Current();
      GeomShapePtr aFromShape(new GeomAPI_Shape), aToShape(new GeomAPI_Shape);
      aFromShape->setImpl(new TopoDS_Shape(aPrismBuilder->FirstShape(aShape)));
      aToShape->setImpl(new TopoDS_Shape(aPrismBuilder->LastShape(aShape)));
      this->addFromShape(aFromShape);
      this->addToShape(aToShape);
    }
  } else {
    GeomShapePtr aBoundingFromShape = theFromShape ? theFromShape : aBasePlane;
    GeomShapePtr aBoundingToShape   = theToShape   ? theToShape   : aBasePlane;

    // Moving prism bounding faces according to "from" and "to" sizes.
    std::shared_ptr<GeomAPI_Pln> aFromPln = GeomAPI_Face(aBoundingFromShape).getPlane();
    std::shared_ptr<GeomAPI_Pnt> aFromLoc = aFromPln->location();
    std::shared_ptr<GeomAPI_Dir> aFromDir = aFromPln->direction();

    std::shared_ptr<GeomAPI_Pln> aToPln = GeomAPI_Face(aBoundingToShape).getPlane();
    std::shared_ptr<GeomAPI_Pnt> aToLoc = aToPln->location();
    std::shared_ptr<GeomAPI_Dir> aToDir = aToPln->direction();

    bool aSign = aFromLoc->xyz()->dot(aBaseDir->xyz()) > aToLoc->xyz()->dot(aBaseDir->xyz());

    std::shared_ptr<GeomAPI_Pnt> aFromPnt(new GeomAPI_Pnt(aFromLoc->xyz()->added(aBaseDir->xyz()->multiplied(
                                                          aSign ? theFromSize : -theFromSize))));
    aBoundingFromShape = GeomAlgoAPI_FaceBuilder::planarFace(aFromPnt, aFromDir);

    std::shared_ptr<GeomAPI_Pnt> aToPnt(new GeomAPI_Pnt(aToLoc->xyz()->added(aBaseDir->xyz()->multiplied(
                                                        aSign ? -theToSize : theToSize))));
    aBoundingToShape = GeomAlgoAPI_FaceBuilder::planarFace(aToPnt, aToDir);

    // Getting bounding box for base shape.
    Bnd_Box aBndBox;
    BRepBndLib::Add(aBaseShape, aBndBox);
    Standard_Real aXArr[2] = {aBndBox.CornerMin().X(), aBndBox.CornerMax().X()};
    Standard_Real aYArr[2] = {aBndBox.CornerMin().Y(), aBndBox.CornerMax().Y()};
    Standard_Real aZArr[2] = {aBndBox.CornerMin().Z(), aBndBox.CornerMax().Z()};
    gp_Pnt aPoints[8];
    int aNum = 0;
    for(int i = 0; i < 2; i++) {
      for(int j = 0; j < 2; j++) {
        for(int k = 0; k < 2; k++) {
          aPoints[aNum] = gp_Pnt(aXArr[i], aYArr[j], aZArr[k]);
          aNum++;
        }
      }
    }

    // Project points to bounding planes. Search max distance to them.
    IntAna_Quadric aBndToQuadric(gp_Pln(aToPnt->impl<gp_Pnt>(), aToDir->impl<gp_Dir>()));
    IntAna_Quadric aBndFromQuadric(gp_Pln(aFromPnt->impl<gp_Pnt>(), aFromDir->impl<gp_Dir>()));
    Standard_Real aMaxToDist = 0, aMaxFromDist = 0;
    for(int i = 0; i < 8; i++) {
      gp_Lin aLine(aPoints[i], aDirVec);
      IntAna_IntConicQuad aToIntAna(aLine, aBndToQuadric);
      IntAna_IntConicQuad aFromIntAna(aLine, aBndFromQuadric);
      if(aToIntAna.NbPoints() == 0 || aFromIntAna.NbPoints() == 0) {
        return;
      }
      const gp_Pnt& aPntOnToFace = aToIntAna.Point(1);
      const gp_Pnt& aPntOnFromFace = aFromIntAna.Point(1);
      if(aPoints[i].Distance(aPntOnToFace) > aMaxToDist) {
        aMaxToDist = aPoints[i].Distance(aPntOnToFace);
      }
      if(aPoints[i].Distance(aPntOnFromFace) > aMaxFromDist) {
        aMaxFromDist = aPoints[i].Distance(aPntOnFromFace);
      }
    }

    // We added 1 just to be sure that prism is long enough for boolean operation.
    double aPrismLength = aMaxToDist + aMaxFromDist + 1;

    // Moving base shape.
    gp_Trsf aTrsf;
    aTrsf.SetTranslation(aDirVec * -aPrismLength);
    BRepBuilderAPI_Transform* aTransformBuilder = new BRepBuilderAPI_Transform(aBaseShape, aTrsf);
    if(!aTransformBuilder) {
      return;
    }
    this->appendAlgo(std::shared_ptr<GeomAlgoAPI_MakeShape>(new GeomAlgoAPI_MakeShape(aTransformBuilder)));
    if(!aTransformBuilder->IsDone()) {
      return;
    }
    TopoDS_Shape aMovedBase = aTransformBuilder->Shape();

    // Making prism.
    BRepPrimAPI_MakePrism* aPrismBuilder = new BRepPrimAPI_MakePrism(aMovedBase, aDirVec * 2 * aPrismLength);
    if(!aPrismBuilder) {
      return;
    }
    this->appendAlgo(std::shared_ptr<GeomAlgoAPI_MakeShape>(new GeomAlgoAPI_MakeShape(aPrismBuilder)));
    if(!aPrismBuilder->IsDone()) {
      return;
    }
    aResult = aPrismBuilder->Shape();

    // Orienting bounding planes.
    std::shared_ptr<GeomAPI_Pnt> aCentreOfMass = GeomAlgoAPI_ShapeTools::centreOfMass(theBaseShape);
    const gp_Pnt& aCentrePnt = aCentreOfMass->impl<gp_Pnt>();
    gp_Lin aLine(aCentrePnt, aDirVec);
    IntAna_IntConicQuad aToIntAna(aLine, aBndToQuadric);
    IntAna_IntConicQuad aFromIntAna(aLine, aBndFromQuadric);
    Standard_Real aToParameter = aToIntAna.ParamOnConic(1);
    Standard_Real aFromParameter = aFromIntAna.ParamOnConic(1);
    if(aToParameter > aFromParameter) {
      gp_Vec aVec = aToDir->impl<gp_Dir>();
      if((aVec * aDirVec) > 0) {
        aToDir->setImpl(new gp_Dir(aVec.Reversed()));
        aBoundingToShape = GeomAlgoAPI_FaceBuilder::planarFace(aToPnt, aToDir);
      }
      aVec = aFromDir->impl<gp_Dir>();
      if((aVec * aDirVec) < 0) {
        aFromDir->setImpl(new gp_Dir(aVec.Reversed()));
        aBoundingFromShape = GeomAlgoAPI_FaceBuilder::planarFace(aFromPnt, aFromDir);
      }
    } else {
      gp_Vec aVec = aToDir->impl<gp_Dir>();
      if((aVec * aDirVec) < 0) {
        aToDir->setImpl(new gp_Dir(aVec.Reversed()));
        aBoundingToShape = GeomAlgoAPI_FaceBuilder::planarFace(aToPnt, aToDir);
      }
      aVec = aFromDir->impl<gp_Dir>();
      if((aVec * aDirVec) > 0) {
        aFromDir->setImpl(new gp_Dir(aVec.Reversed()));
        aBoundingFromShape = GeomAlgoAPI_FaceBuilder::planarFace(aFromPnt, aFromDir);
      }
    }

    // Making solids from bounding planes.
    TopoDS_Shell aToShell, aFromShell;
    TopoDS_Solid aToSolid, aFromSolid;
    const TopoDS_Shape& aToShape   = aBoundingToShape->impl<TopoDS_Shape>();
    const TopoDS_Shape& aFromShape = aBoundingFromShape->impl<TopoDS_Shape>();
    TopoDS_Face aToFace   = TopoDS::Face(aToShape);
    TopoDS_Face aFromFace = TopoDS::Face(aFromShape);
    BRep_Builder aBoundingBuilder;
    aBoundingBuilder.MakeShell(aToShell);
    aBoundingBuilder.Add(aToShell, aToShape);
    aBoundingBuilder.MakeShell(aFromShell);
    aBoundingBuilder.Add(aFromShell, aFromShape);
    aBoundingBuilder.MakeSolid(aToSolid);
    aBoundingBuilder.Add(aToSolid, aToShell);
    aBoundingBuilder.MakeSolid(aFromSolid);
    aBoundingBuilder.Add(aFromSolid, aFromShell);

    // Cutting with to plane.
    BRepAlgoAPI_Cut* aToCutBuilder = new BRepAlgoAPI_Cut(aResult, aToSolid);
    aToCutBuilder->Build();
    if(!aToCutBuilder->IsDone()) {
      return;
    }
    this->appendAlgo(std::shared_ptr<GeomAlgoAPI_MakeShape>(new GeomAlgoAPI_MakeShape(aToCutBuilder)));
    aResult = aToCutBuilder->Shape();
    if(aResult.ShapeType() == TopAbs_COMPOUND) {
      aResult = GeomAlgoAPI_DFLoader::refineResult(aResult);
    }
    if(aShapeTypeToExp == TopAbs_FACE) {
      const TopTools_ListOfShape& aToShapes = aToCutBuilder->Modified(aToShape);
      for(TopTools_ListIteratorOfListOfShape anIt(aToShapes); anIt.More(); anIt.Next()) {
        GeomShapePtr aGeomSh(new GeomAPI_Shape());
        aGeomSh->setImpl(new TopoDS_Shape(anIt.Value()));
        this->addToShape(aGeomSh);
      }
    }

    // Cutting with from plane.
    BRepAlgoAPI_Cut* aFromCutBuilder = new BRepAlgoAPI_Cut(aResult, aFromSolid);
    aFromCutBuilder->Build();
    if(!aFromCutBuilder->IsDone()) {
      return;
    }
    this->appendAlgo(std::shared_ptr<GeomAlgoAPI_MakeShape>(new GeomAlgoAPI_MakeShape(aFromCutBuilder)));
    aResult = aFromCutBuilder->Shape();
    TopoDS_Iterator aCheckIt(aResult);
    if(!aCheckIt.More()) {
      return;
    }
    if(aResult.ShapeType() == TopAbs_COMPOUND) {
      aResult = GeomAlgoAPI_DFLoader::refineResult(aResult);
    }
    if(aShapeTypeToExp == TopAbs_FACE) {
      const TopTools_ListOfShape& aFromShapes = aFromCutBuilder->Modified(aFromShape);
      for(TopTools_ListIteratorOfListOfShape anIt(aFromShapes); anIt.More(); anIt.Next()) {
        GeomShapePtr aGeomSh(new GeomAPI_Shape());
        aGeomSh->setImpl(new TopoDS_Shape(anIt.Value()));
        this->addFromShape(aGeomSh);
      }
    }

    // Naming for extrusion from vertex, edge.
    for(TopExp_Explorer anExp(aResult, aShapeTypeToExp); anExp.More(); anExp.Next()) {
      const TopoDS_Shape& aShape = anExp.Current();
      GeomShapePtr aGeomSh(new GeomAPI_Shape());
      if(aShapeTypeToExp == TopAbs_VERTEX) {
        gp_Pnt aPnt = BRep_Tool::Pnt(TopoDS::Vertex(aShape));
        IntTools_Context anIntTools;
        if(anIntTools.IsValidPointForFace(aPnt, aToFace, Precision::Confusion()) == Standard_True) {
          aGeomSh->setImpl(new TopoDS_Shape(aShape));
          this->addToShape(aGeomSh);
        }
        if(anIntTools.IsValidPointForFace(aPnt, aFromFace, Precision::Confusion()) == Standard_True) {
          aGeomSh->setImpl(new TopoDS_Shape(aShape));
          this->addFromShape(aGeomSh);
        }
      } else if(aShapeTypeToExp == TopAbs_EDGE) {
        TopoDS_Edge anEdge = TopoDS::Edge(aShape);
        BRepLib_CheckCurveOnSurface anEdgeCheck(anEdge, aToFace);
        anEdgeCheck.Perform();
        if(anEdgeCheck.MaxDistance() < Precision::Confusion()) {
          aGeomSh->setImpl(new TopoDS_Shape(aShape));
          this->addToShape(aGeomSh);
        }
        anEdgeCheck.Init(anEdge, aFromFace);
        anEdgeCheck.Perform();
        if(anEdgeCheck.MaxDistance() < Precision::Confusion()) {
          aGeomSh->setImpl(new TopoDS_Shape(aShape));
          this->addFromShape(aGeomSh);
        }
      } else {
        break;
      }
    }

    if(aResult.ShapeType() == TopAbs_COMPOUND) {
      std::shared_ptr<GeomAPI_Shape> aGeomShape(new GeomAPI_Shape);
      aGeomShape->setImpl(new TopoDS_Shape(aResult));
      ListOfShape aCompSolids, aFreeSolids;
      aGeomShape = GeomAlgoAPI_ShapeTools::combineShapes(aGeomShape,
                                                         GeomAPI_Shape::COMPSOLID,
                                                         aCompSolids,
                                                         aFreeSolids);
      aResult = aGeomShape->impl<TopoDS_Shape>();
    }
  }

  // Setting result.
  if(aResult.IsNull()) {
    return;
  }
  aResult = GeomAlgoAPI_DFLoader::refineResult(aResult);
  GeomShapePtr aGeomSh(new GeomAPI_Shape());
  aGeomSh->setImpl(new TopoDS_Shape(aResult));
  this->setShape(aGeomSh);
  this->setDone(true);
}
