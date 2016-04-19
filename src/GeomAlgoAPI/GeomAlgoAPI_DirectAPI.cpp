// Copyright (C) 2014-2016 CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_DirectAPI.cpp
// Created:     17 Mar 2016
// Author:      Clarisse Genrault (CEA)

#include "GeomAlgoAPI_DirectAPI.h"
#include <GeomAlgoAPI_Box.h>
#include <GeomAlgoAPI_BoxPoints.h>

#include <iostream>

namespace GeomAlgoAPI_DirectAPI
{
  //=========================================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_DirectAPI::MakeBox(const double theDx, const double theDy, 
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
    return aBoxAlgo.shape();
  }
  
  //=========================================================================================================
  std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_DirectAPI::MakeBox(std::shared_ptr<GeomAPI_Pnt> theFirstPoint,
                                                                std::shared_ptr<GeomAPI_Pnt> theSecondPoint) throw (GeomAlgoAPI_Exception)
  {
    GeomAlgoAPI_BoxPoints aBoxAlgo(theFirstPoint, theSecondPoint);
    
    if (!aBoxAlgo.check()) {
      throw GeomAlgoAPI_Exception(aBoxAlgo.getError());
    }
    
    aBoxAlgo.build();
    
    if(!aBoxAlgo.isDone()) {
      throw GeomAlgoAPI_Exception(aBoxAlgo.getError());
    }
    return aBoxAlgo.shape();
  }
}