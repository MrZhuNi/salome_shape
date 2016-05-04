// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Shape.cpp
// Created:     23 Apr 2014
// Author:      Mikhail PONIKAROV

#include "GeomAPI_Shape.h"

#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>
#include <BRepBuilderAPI_FindPlane.hxx>
#include <BRepTools.hxx>
#include <Bnd_Box.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Conic.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>

#include <sstream>

#define MY_SHAPE implPtr<TopoDS_Shape>()

GeomAPI_Shape::GeomAPI_Shape()
    : GeomAPI_Interface(new TopoDS_Shape())
{
}

bool GeomAPI_Shape::isNull() const
{
  return MY_SHAPE->IsNull() == Standard_True;
}

bool GeomAPI_Shape::isEqual(const std::shared_ptr<GeomAPI_Shape> theShape) const
{
  if (!theShape.get())
    return false;
  if (isNull())
    return theShape->isNull();
  if (theShape->isNull())
    return false;

  return MY_SHAPE->IsEqual(theShape->impl<TopoDS_Shape>()) == Standard_True;
}

bool GeomAPI_Shape::isVertex() const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Shape*>(this)->impl<TopoDS_Shape>();
  return !aShape.IsNull() && aShape.ShapeType() == TopAbs_VERTEX;
}

bool GeomAPI_Shape::isEdge() const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Shape*>(this)->impl<TopoDS_Shape>();
  return !aShape.IsNull() && aShape.ShapeType() == TopAbs_EDGE;
}

bool GeomAPI_Shape::isFace() const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Shape*>(this)->impl<TopoDS_Shape>();
  return !aShape.IsNull() && aShape.ShapeType() == TopAbs_FACE;
}

bool GeomAPI_Shape::isCompound() const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Shape*>(this)->impl<TopoDS_Shape>();
  return !aShape.IsNull() && aShape.ShapeType() == TopAbs_COMPOUND;
}

bool GeomAPI_Shape::isCompoundOfSolids() const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Shape*>(this)->impl<TopoDS_Shape>();
  if (aShape.IsNull() || aShape.ShapeType() != TopAbs_COMPOUND)
    return false;
  bool isAtLeastOne = false;
  for(TopoDS_Iterator aSubs(aShape); aSubs.More(); aSubs.Next()) {
    if (aSubs.Value().IsNull() || aSubs.Value().ShapeType() != TopAbs_SOLID)
      return false;
    isAtLeastOne = true;
  }
  return isAtLeastOne;
}

// adds the nopt-compound elements recursively to the list
static void addSimpleToList(const TopoDS_Shape& theShape, NCollection_List<TopoDS_Shape>& theList)
{
  if (!theShape.IsNull()) {
    if (theShape.ShapeType() == TopAbs_COMPOUND) {
      for(TopoDS_Iterator aSubs(theShape); aSubs.More(); aSubs.Next()) {
        addSimpleToList(aSubs.Value(), theList);
      }
    } else {
      theList.Append(theShape);
    }
  }
}

bool GeomAPI_Shape::isConnectedTopology() const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Shape*>(this)->impl<TopoDS_Shape>();
  if (aShape.IsNull() || aShape.ShapeType() != TopAbs_COMPOUND)
    return false;
  NCollection_List<TopoDS_Shape> aNotConnected; // list of simple elements that are not detected in connection to others
  addSimpleToList(aShape, aNotConnected);
  if (aNotConnected.IsEmpty()) // an empty compound
    return false;

  // collect here the group of connected subs, starting with one first element
  NCollection_List<TopoDS_Shape> aNewConnected;
  aNewConnected.Append(aNotConnected.First());
  aNotConnected.RemoveFirst();
  // iterate until some new element become connected
  while(!aNewConnected.IsEmpty() && !aNotConnected.IsEmpty()) {
    NCollection_List<TopoDS_Shape> aNew; // very new connected to new connected
    NCollection_List<TopoDS_Shape>::Iterator aNotIter(aNotConnected);
    while(aNotIter.More()) {
      bool aConnected =  false;
      NCollection_List<TopoDS_Shape>::Iterator aNewIter(aNewConnected);
      for(; !aConnected && aNewIter.More(); aNewIter.Next()) {
        // checking topological connecion of aNotIter and aNewIter (if shapes are connected, vertices are connected for sure)
        TopExp_Explorer anExp1(aNotIter.Value(), TopAbs_VERTEX);
        for(; !aConnected && anExp1.More(); anExp1.Next()) {
          TopExp_Explorer anExp2(aNewIter.Value(), TopAbs_VERTEX);
          for(; anExp2.More(); anExp2.Next()) {
            if (anExp1.Current().IsSame(anExp2.Current())) {
              aConnected = true;
              break;
            }
          }
        }
      }
      if (aConnected) {
        aNew.Append(aNotIter.Value());
        aNotConnected.Remove(aNotIter);
      } else {
        aNotIter.Next();
      }
    }
    // remove all new connected and put to this list very new connected
    aNewConnected.Clear();
    aNewConnected.Append(aNew);
  }
  return aNotConnected.IsEmpty() == Standard_True;
}

bool GeomAPI_Shape::isSolid() const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Shape*>(this)->impl<TopoDS_Shape>();
  return !aShape.IsNull() && aShape.ShapeType() == TopAbs_SOLID;
}

bool GeomAPI_Shape::isCompSolid() const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Shape*>(this)->impl<TopoDS_Shape>();
  return !aShape.IsNull() && aShape.ShapeType() == TopAbs_COMPSOLID;
}

bool GeomAPI_Shape::isPlanar() const
{
  TopoDS_Shape aShape = impl<TopoDS_Shape>();

  if(aShape.IsNull()) {
    return false;
  }

  TopAbs_ShapeEnum aShapeType = aShape.ShapeType();
  if(aShapeType == TopAbs_COMPOUND) {
    TopoDS_Iterator anIt(aShape);
    int aShNum = 0;
    for(; anIt.More(); anIt.Next()) {
      ++aShNum;
    }
    if(aShNum == 1) {
      anIt.Initialize(aShape);
      aShape = anIt.Value();
    }
  }

  aShapeType = aShape.ShapeType();
  if(aShapeType == TopAbs_VERTEX) {
    return true;
  } else if(aShapeType == TopAbs_FACE) {
    const Handle(Geom_Surface)& aSurface = BRep_Tool::Surface(TopoDS::Face(aShape));
    Handle(Standard_Type) aType = aSurface->DynamicType();

    if(aType == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
      Handle(Geom_RectangularTrimmedSurface) aTrimSurface = Handle(Geom_RectangularTrimmedSurface)::DownCast(aSurface);
      aType = aTrimSurface->BasisSurface()->DynamicType();
    }
    return (aType == STANDARD_TYPE(Geom_Plane)) == Standard_True;
  } else {
    BRepBuilderAPI_FindPlane aFindPlane(aShape);
    bool isFound = aFindPlane.Found() == Standard_True;

    if(!isFound && aShapeType == TopAbs_EDGE) {
      Standard_Real aFirst, aLast;
      Handle(Geom_Curve) aCurve = BRep_Tool::Curve(TopoDS::Edge(aShape), aFirst, aLast);
      Handle(Standard_Type) aType = aCurve->DynamicType();

      if(aType == STANDARD_TYPE(Geom_TrimmedCurve)) {
        Handle(Geom_TrimmedCurve) aTrimCurve = Handle(Geom_TrimmedCurve)::DownCast(aCurve);
        aType = aTrimCurve->BasisCurve()->DynamicType();
      }

      if(aType == STANDARD_TYPE(Geom_Line)
          || aType == STANDARD_TYPE(Geom_Conic)
          || aType == STANDARD_TYPE(Geom_Circle)
          || aType == STANDARD_TYPE(Geom_Ellipse)
          || aType == STANDARD_TYPE(Geom_Hyperbola)
          || aType == STANDARD_TYPE(Geom_Parabola)) {
        isFound = true;
      }
    }

    return isFound;
  }

  return false;
}

GeomAPI_Shape::ShapeType GeomAPI_Shape::shapeType() const
{
  const TopoDS_Shape& aShape = impl<TopoDS_Shape>();

  ShapeType aST = GeomAPI_Shape::SHAPE;

  switch(aShape.ShapeType()) {
  case TopAbs_COMPOUND:
    aST = GeomAPI_Shape::COMPOUND;
    break;
  case TopAbs_COMPSOLID:
    aST = GeomAPI_Shape::COMPSOLID;
    break;
  case TopAbs_SOLID:
    aST = GeomAPI_Shape::SOLID;
    break;
  case TopAbs_SHELL:
    aST = GeomAPI_Shape::SHELL;
    break;
  case TopAbs_FACE:
    aST = GeomAPI_Shape::FACE;
    break;
  case TopAbs_WIRE:
    aST = GeomAPI_Shape::WIRE;
    break;
  case TopAbs_EDGE:
    aST = GeomAPI_Shape::EDGE;
    break;
  case TopAbs_VERTEX:
    aST = GeomAPI_Shape::VERTEX;
    break;
  case TopAbs_SHAPE:
    aST = GeomAPI_Shape::SHAPE;
    break;
  }

  return aST;
}

std::string GeomAPI_Shape::shapeTypeStr() const
{
  ShapeType aShapeType = shapeType();
  std::string aShapeTypeStr;

  switch(aShapeType) {
    case COMPOUND: {
      aShapeTypeStr = "Compound";
      break;
    }
    case COMPSOLID: {
      aShapeTypeStr = "CompSolid";
      break;
    }
    case SOLID: {
      aShapeTypeStr = "Solid";
      break;
    }
    case SHELL: {
      aShapeTypeStr = "Shell";
      break;
    }
    case FACE: {
      aShapeTypeStr = "Face";
      break;
    }
    case WIRE: {
      aShapeTypeStr = "Wire";
      break;
    }
    case EDGE: {
      aShapeTypeStr = "Edge";
      break;
    }
    case VERTEX: {
      aShapeTypeStr = "Vertex";
      break;
    }
    case SHAPE: {
      aShapeTypeStr = "Shape";
      break;
    }
  }

  return aShapeTypeStr;
}

bool GeomAPI_Shape::isSubShape(const std::shared_ptr<GeomAPI_Shape> theShape) const
{
  if(!theShape.get()) {
    return false;
  }

  const TopoDS_Shape& aShapeToSearch = theShape->impl<TopoDS_Shape>();
  if(aShapeToSearch.IsNull()) {
    return false;
  }

  for(TopExp_Explorer anExp(*MY_SHAPE, aShapeToSearch.ShapeType()); anExp.More(); anExp.Next()) {
    if(aShapeToSearch.IsEqual(anExp.Current())) {
      return true;
    }
  }

  return false;
}

bool GeomAPI_Shape::computeSize(double& theXmin, double& theYmin, double& theZmin,
                                double& theXmax, double& theYmax, double& theZmax) const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Shape*>(this)->impl<TopoDS_Shape>();
  if (aShape.IsNull())
    return false;
  Bnd_Box aBndBox;
  BRepBndLib::Add(aShape, aBndBox);
  aBndBox.Get(theXmin, theYmin, theZmin, theXmax, theYmax, theZmax);
  return true;
}

std::string GeomAPI_Shape::getShapeStream() const
{
  std::ostringstream aStream;
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Shape*>(this)->impl<TopoDS_Shape>();
  BRepTools::Write(aShape, aStream);
  return aStream.str();
}
