// Copyright (C) 2014-2016 CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Sphere.cpp
// Created:     29 Mar 2016
// Author:      CEA (delegation to Alyotech)

#include <GeomAlgoAPI_Sphere.h>

#include <BRepPrimAPI_MakeSphere.hxx>
#include <TopoDS_Shape.hxx>

//=================================================================================================
GeomAlgoAPI_Sphere::GeomAlgoAPI_Sphere()
{
}


//=================================================================================================
GeomAlgoAPI_Sphere::GeomAlgoAPI_Sphere(std::shared_ptr<GeomAPI_Pnt> theCenter, const double theRadius)
{
  myCenter = theCenter;
  myRadius = theRadius;
}

//=================================================================================================
bool GeomAlgoAPI_Sphere::check()
{
  if (myRadius < Precision::Confusion()) {
    myError = "Sphere builder with center and radius :: radius is less than or equal to zero.";
    return false;
  } 
  return true;
}

//=================================================================================================
void GeomAlgoAPI_Sphere::build()
{
  myCreatedFaces.clear();
  
  // Convert the point given as argument to OCCT object
  const gp_Pnt& aCenter = myCenter->impl<gp_Pnt>();

  // We need the OCCT sphere-making engine. And we need it as a regular pointer so we can add it to the list of MakeShape
  BRepPrimAPI_MakeSphere *aSphereMaker = new  BRepPrimAPI_MakeSphere(aCenter, myRadius);
  aSphereMaker->Build(); //Let the OCCT sphere-maker make the sphere

  // Test the algorithm
  if (!aSphereMaker->IsDone()) {
    return;
  }

  TopoDS_Shape aResult = aSphereMaker->Shape();
  std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(aResult));
  setShape(aShape);

// Test on the shapes
  if (!aShape.get() || aShape->isNull()) {
    myError = "Sphere builder with center and radius  :: resulting shape is null.";
    return;
  }
  
  setImpl(aSphereMaker);
  setBuilderType(OCCT_BRepBuilderAPI_MakeShape);
  
  setDone(true);
}
