// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#include <GeomAlgoAPI_Tube.h>

#include <BRepAlgo_FaceRestrictor.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>

#include <gp_Circ.hxx>

#include <Precision.hxx>

#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>

//=================================================================================================
GeomAlgoAPI_Tube::GeomAlgoAPI_Tube()
{
}


//=================================================================================================
GeomAlgoAPI_Tube::GeomAlgoAPI_Tube(const double theRMin, const double theRMax, const double theZ)
{
  isSimple = true;
  
  myRMin = theRMin;
  myRMax = theRMax;
  myZ = theZ;
}

//=================================================================================================
GeomAlgoAPI_Tube::GeomAlgoAPI_Tube(const double theRMin, const double theRMax, const double theZ,
                                   const double theStartPhi, const double theDeltaPhi)
{
  isSimple = false;
  
  myRMin = theRMin;
  myRMax = theRMax;
  myZ = theZ;
  myStartPhi = theStartPhi;
  myDeltaPhi = theDeltaPhi;
}

//=================================================================================================
bool GeomAlgoAPI_Tube::check()
{
   if ((myRMax - myRMin) < Precision::Confusion()){
    myError = "Tube builder :: rmin is greater than or equal to rmax.";
    return false;
  } else if (myRMin < 0.){
    myError = "Tube builder :: rmin is negative.";
    return false;
  } else if (myRMax < Precision::Confusion()){
    myError = "Tube builder :: rmax is negative or null.";
    return false;
  } else if (myZ < Precision::Confusion()){
    myError = "Tube builder :: z is negative or null.";
    return false;
  } 
  
  if (!isSimple) {
    if (myDeltaPhi < ANGULAR_PRECISION){
      myError = "Tube builder :: deltaphi is negative or null.";
      return false;
    } else if (myDeltaPhi > 360.){
      myError = "Tube builder :: deltaphi is greater than 360 degrees.";
      return false;
    }
  }
    
  return true;
}

//=================================================================================================
void GeomAlgoAPI_Tube::build()
{
  myCreatedFaces.clear();

  std::shared_ptr<GeomAPI_Shape> aResultShape;
  if (isSimple) 
    aResultShape = buildSimpleTube();
  else
    aResultShape = buildSegmentTube();

  // Test on the shapes
  if (!(aResultShape).get() || aResultShape->isNull()) {
    myError = "";
    return;
  }

  setShape(aResultShape);
  setDone(true);
}

//=================================================================================================
std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_Tube::buildSimpleTube()
{
  // Construct the inner and outer circles
  gp_Pnt anOrigin(0., 0., 0.);
  gp_Dir aNormal(0., 0., 1.);
  gp_Circ anInnerCircle(gp_Ax2(anOrigin, aNormal), myRMin);
  gp_Circ anOuterCircle(gp_Ax2(anOrigin, aNormal), myRMax);
    
  // Construct the outer wire
  BRepBuilderAPI_MakeEdge anOuterCircleBuilder(anOuterCircle);
  anOuterCircleBuilder.Build();
  BRepBuilderAPI_MakeWire anOuterWireBuilder;
  anOuterWireBuilder.Add(anOuterCircleBuilder.Edge());
  anOuterWireBuilder.Build();
  TopoDS_Wire anOuterWire(anOuterWireBuilder.Wire());
  
  // Construct the face with the outer wire
  BRepBuilderAPI_MakeFace aFaceBuilder(anOuterWireBuilder.Wire());
  aFaceBuilder.Build();
  TopoDS_Face aFace(aFaceBuilder.Face());
  
  // Construct the inner wire
  BRepBuilderAPI_MakeEdge anInnerCircleBuilder(anInnerCircle);
  anInnerCircleBuilder.Build();
  BRepBuilderAPI_MakeWire anInnerWireBuilder;
  anInnerWireBuilder.Add(anInnerCircleBuilder.Edge());
  anInnerWireBuilder.Build();
  TopoDS_Wire anInnerWire(anInnerWireBuilder.Wire()); 
  
  // Construct the hole face
  BRepAlgo_FaceRestrictor aFRestrictor;
  aFRestrictor.Init(aFace, Standard_False, Standard_True);
  aFRestrictor.Add(anInnerWire);
  aFRestrictor.Add(anOuterWire);
  aFRestrictor.Perform();
  aFace = TopoDS_Face(aFRestrictor.Current());
  
  // Construct the tube
  gp_Vec aVec(aNormal);
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(aVec * -myZ/2);
  BRepBuilderAPI_Transform* aTransformBuilder =
      new BRepBuilderAPI_Transform(aFace, aTrsf);
  if (!aTransformBuilder || !aTransformBuilder->IsDone()) {
    return;
  }
//   this->appendAlgo(std::shared_ptr<GeomAlgoAPI_MakeShape>(
//       new GeomAlgoAPI_MakeShape(aTransformBuilder)));
  TopoDS_Shape aMovedBase = aTransformBuilder->Shape();
  BRepPrimAPI_MakePrism* aPrismBuilder = new BRepPrimAPI_MakePrism(aMovedBase, aVec * myZ);
  
  setImpl(aPrismBuilder);
  setBuilderType(OCCT_BRepBuilderAPI_MakeShape);
 
  std::shared_ptr<GeomAPI_Shape> aResultShape =
    std::shared_ptr<GeomAPI_Shape>(new GeomAPI_Shape());
  aResultShape->setImpl(new TopoDS_Shape(aPrismBuilder->Shape()));
  
  return aResultShape;
}

//=================================================================================================
std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_Tube::buildSegmentTube()
{
  const double aStartPhiRad = myStartPhi * M_PI / 180.;
  BRepBuilderAPI_MakeWire aWireBuilder;

  // Build the 4 points defining the section's vertices (it's a simple rectangular section)
  gp_Pnt aTopOuter(myRMax*cos(aStartPhiRad), myRMax*sin(aStartPhiRad),myZ/2.);
  gp_Pnt aBaseOuter(myRMax*cos(aStartPhiRad), myRMax*sin(aStartPhiRad),-myZ/2.);
  gp_Pnt aTopInner(myRMin*cos(aStartPhiRad), myRMin*sin(aStartPhiRad),myZ/2.);
  gp_Pnt aBaseInner(myRMin*cos(aStartPhiRad), myRMin*sin(aStartPhiRad),-myZ/2.);

  // Build the edges of the section's outline, add them to a wire
  BRepBuilderAPI_MakeEdge anEdgeBuilderTop(aTopOuter, aTopInner);
  anEdgeBuilderTop.Build();
  aWireBuilder.Add(anEdgeBuilderTop.Edge());
  BRepBuilderAPI_MakeEdge anEdgeBuilderInner(aTopInner, aBaseInner);
  anEdgeBuilderInner.Build();
  aWireBuilder.Add(anEdgeBuilderInner.Edge());
  BRepBuilderAPI_MakeEdge anEdgeBuilderBase(aBaseInner, aBaseOuter);
  anEdgeBuilderBase.Build();
  aWireBuilder.Add(anEdgeBuilderBase.Edge());
  BRepBuilderAPI_MakeEdge anEdgeBuilderOuter(aBaseOuter, aTopOuter);
  anEdgeBuilderOuter.Build();
  aWireBuilder.Add(anEdgeBuilderOuter.Edge());

  // Build the section (face) from the wire
  aWireBuilder.Build();
  BRepBuilderAPI_MakeFace aFaceBuilder(aWireBuilder.Wire());
  aFaceBuilder.Build();

  if (!aFaceBuilder.IsDone()){
    myError = "Tube Segment builder :: section is not valid";
    return;
  }

  // Create the mathematical tools needed to perform the revolution
  gp_Dir aZDir(0., 0., 1.);
  gp_Pnt anOrigin(0., 0., 0.);
  gp_Ax1 aZAxis(anOrigin, aZDir);

  // Perform the revolution using the section
  BRepPrimAPI_MakeRevol* aRevolBuilder = new BRepPrimAPI_MakeRevol(aFaceBuilder.Face(), aZAxis, myDeltaPhi * M_PI / 180., Standard_True);
  if(!aRevolBuilder) {
    return;
    myError = "Tube Segment builder :: section revolution did not succeed";
  }
  if(!aRevolBuilder->IsDone()) {
    myError = "Tube Segment builder :: section revolution did not succeed";
    return;
  }

  // Store the result and publish it
  std::shared_ptr<GeomAPI_Shape> aResultShape =  std::shared_ptr<GeomAPI_Shape>(new GeomAPI_Shape()) ;
  aResultShape->setImpl(new TopoDS_Shape(aRevolBuilder->Shape()));
  
  return aResultShape;
}
