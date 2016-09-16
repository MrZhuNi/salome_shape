// Copyright (C) 2014-2016 CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Sphere.h
// Created:     29 Mar 2016
// Author:      CEA (delegation to Alyotech)

#ifndef GeomAlgoAPI_Sphere_H_
#define GeomAlgoAPI_Sphere_H_

#include <GeomAPI_Pnt.h>
#include <GeomAlgoAPI_MakeShape.h>

/**\class GeomAlgoAPI_Sphere
 * \ingroup DataAlgo
 * \brief Allows to create Sphere Primitives
 */
class GeomAlgoAPI_Sphere : public GeomAlgoAPI_MakeShape
{
 public: 
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Sphere();
  
  /// Creates a Sphere using a center and a radius
  /// \param theCenter the center
  /// \param theRadius the radius of the sphere
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Sphere(std::shared_ptr<GeomAPI_Pnt> theCenter, const double theRadius);
    
  /// Checks if the center myCenter and radius myRadius are OK. 
  GEOMALGOAPI_EXPORT bool check();
  
  /// Builds the sphere with the center myCenter and radius myRadius
  GEOMALGOAPI_EXPORT void build();

 private:
  std::shared_ptr<GeomAPI_Pnt> myCenter; /// attribute point defining the center of the sphere
  double myRadius; /// attribute radius of the sphere
};


#endif
