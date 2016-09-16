// Copyright (C) 2014-2016 CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_ShapeAPI.cpp
// Created:     17 Mar 2016
// Author:      Clarisse Genrault (CEA)

#include "GeomAlgoAPI_ShapeAPI.h"
#include <GeomAlgoAPI_Box.h>
#include <GeomAlgoAPI_Sphere.h>

#include <GeomAPI_Pnt.h>
#include <GeomAPI_Edge.h>
#include <GeomAlgoAPI_EdgeBuilder.h>

#include <iostream>

namespace GeomAlgoAPI_ShapeAPI
{
  //=========================================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeBox(const double theDx, const double theDy, 
                                                               const double theDz) throw (GeomAlgoAPI_Exception)
  {
    GeomAlgoAPI_Box aBoxAlgo(theDx,theDy,theDz);
    
    if (!aBoxAlgo.check()) {
      throw GeomAlgoAPI_Exception(aBoxAlgo.getError());
    }
    
    aBoxAlgo.build();
    
    if(!aBoxAlgo.isDone()) {
      throw GeomAlgoAPI_Exception(aBoxAlgo.getError());
    }
    if (!aBoxAlgo.checkValid("Box builder with dimensions")) {
      throw GeomAlgoAPI_Exception(aBoxAlgo.getError());
    }
    return aBoxAlgo.shape();
  }
  
  //=========================================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeBox(std::shared_ptr<GeomAPI_Pnt> theFirstPoint,
                                                               std::shared_ptr<GeomAPI_Pnt> theSecondPoint) throw (GeomAlgoAPI_Exception)
  {
    GeomAlgoAPI_Box aBoxAlgo(theFirstPoint, theSecondPoint);
    
    if (!aBoxAlgo.check()) {
      throw GeomAlgoAPI_Exception(aBoxAlgo.getError());
    }
    
    aBoxAlgo.build();
    
    if(!aBoxAlgo.isDone()) {
      throw GeomAlgoAPI_Exception(aBoxAlgo.getError());
    }
    if (!aBoxAlgo.checkValid("Box builder with two points")) {
      throw GeomAlgoAPI_Exception(aBoxAlgo.getError());
    }
    return aBoxAlgo.shape();
  }
  
  //=========================================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeSphere(const double theRadius) throw (GeomAlgoAPI_Exception)
  {
    std::shared_ptr<GeomAPI_Pnt> anOrigin = std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(0., 0., 0.));
    GeomAlgoAPI_Sphere aSphereAlgo(anOrigin, theRadius);
    
    if (!aSphereAlgo.check()) {
      throw GeomAlgoAPI_Exception(aSphereAlgo.getError());
    }
    
    aSphereAlgo.build();
    
    if(!aSphereAlgo.isDone()) {
      throw GeomAlgoAPI_Exception(aSphereAlgo.getError());
    }
    if (!aSphereAlgo.checkValid("Sphere builder with radius at origin")) {
      throw GeomAlgoAPI_Exception(aSphereAlgo.getError());
    }
    return aSphereAlgo.shape();
  }
  
  //=========================================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeAPI::makeSphere(std::shared_ptr<GeomAPI_Pnt> theCenterPoint,
                                                                  const double theRadius) throw (GeomAlgoAPI_Exception)
  {
    GeomAlgoAPI_Sphere aSphereAlgo(theCenterPoint, theRadius);
    
    if (!aSphereAlgo.check()) {
      throw GeomAlgoAPI_Exception(aSphereAlgo.getError());
    }
    
    aSphereAlgo.build();
    
    if(!aSphereAlgo.isDone()) {
      throw GeomAlgoAPI_Exception(aSphereAlgo.getError());
    }
    if (!aSphereAlgo.checkValid("Sphere builder with center and radius")) {
      throw GeomAlgoAPI_Exception(aSphereAlgo.getError());
    }
    return aSphereAlgo.shape();
  }
}
