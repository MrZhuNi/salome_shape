// Copyright (C) 2017-2021  CEA/DEN, EDF R&D
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

// File:        GeomAlgoAPI_Sphere.h
// Created:     16 Mar 2017
// Author:      Clarisse Genrault (CEA)

#ifndef GEOMALGOAPI_SPHERE_H_
#define GEOMALGOAPI_SPHERE_H_

#include <GeomAlgoAPI_MakeShape.h>

#include <GeomAPI_Pnt.h>

/**\class GeomAlgoAPI_Sphere
 * \ingroup DataAlgo
 * \brief Allows to create Sphere Primitives
 */
class GeomAlgoAPI_Sphere : public GeomAlgoAPI_MakeShape
{
 public:
  /// Creates a sphere.
  /// \param theCenterPoint The center point of the sphere
  /// \param theRadius The radius of the sphere
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Sphere(std::shared_ptr<GeomAPI_Pnt> theCenterPoint,
                                        const double theRadius);
  /// Creates a sphere.
  /// \param theRMin The inner radius of the sphere
  /// \param theRMax The outer radius of the sphere
  /// \param thePhiMin The lower phi limit
  /// \param thePhiMax The higher phi limit
  /// \param theThetaMin The lower theta limit
  /// \param theThetaMax The higher theta limit
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Sphere(const double theRMin, const double theRMax,
                                        const double thePhiMin, const double thePhiMax,
                                        const double theThetaMin, const double theThetaMax);

  /// Checks if data for the sphere construction is OK.
  GEOMALGOAPI_EXPORT bool check();

  /// Builds the sphere.
  GEOMALGOAPI_EXPORT void build();

  /// Builds the sphere.
  GEOMALGOAPI_EXPORT void buildSphere();

  /// Builds the section of sphere.
  GEOMALGOAPI_EXPORT void buildSectionSphere();

 private:
  bool isSectionSphere; // Boolean to define if the second mode is used.
  std::shared_ptr<GeomAPI_Pnt> myCenterPoint; /// Center of the sphere for the first mode.
  double myRadius; /// Radius of the sphere for the first mode.
  double myRMin, myRMax; /// Inner and outer radius for the second mode.
  double myPhiMin, myPhiMax; /// Lower and higher phi limit for the second mode.
  double myThetaMin, myThetaMax; /// Lower and higher theta limit for the second mode.
};

#endif // GEOMALGOAPI_SPHERE_H_
