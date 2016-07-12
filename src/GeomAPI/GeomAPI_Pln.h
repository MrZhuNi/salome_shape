// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Pln.hxx
// Created:     23 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef GeomAPI_Pln_H_
#define GeomAPI_Pln_H_

#include <memory>
#include <GeomAPI_Interface.h>

class GeomAPI_Ax3;
class GeomAPI_Pnt;
class GeomAPI_Dir;
class GeomAPI_Lin;

/**\class GeomAPI_Pln
 * \ingroup DataModel
 * \brief 3D point defined by three coordinates
 */

class GeomAPI_Pln : public GeomAPI_Interface
{
 public:
  /// Creation of plane by the axis placement
  GEOMAPI_EXPORT 
  GeomAPI_Pln(const std::shared_ptr<GeomAPI_Ax3>& theAxis);

  /// Creation of plane by the point and normal
  GEOMAPI_EXPORT 
  GeomAPI_Pln(const std::shared_ptr<GeomAPI_Pnt>& thePoint,
              const std::shared_ptr<GeomAPI_Dir>& theNormal);

  /// Creation of plane by coefficients (Ax+By+Cz+D=0)
  GEOMAPI_EXPORT 
  GeomAPI_Pln(const double theA, const double theB, const double theC, const double theD);

  /// Returns a point of this plane
  GEOMAPI_EXPORT 
  std::shared_ptr<GeomAPI_Pnt> location() const;

  /// Returns a plane normal
  GEOMAPI_EXPORT 
  std::shared_ptr<GeomAPI_Dir> direction() const;

  /// Returns a plane x direction
  GEOMAPI_EXPORT 
  std::shared_ptr<GeomAPI_Dir> xDirection() const;

  /// Returns the plane coefficients (Ax+By+Cz+D=0)
  GEOMAPI_EXPORT 
  void coefficients(double& theA, double& theB, double& theC, double& theD);

  /// Returns true if planes are coincident.
  GEOMAPI_EXPORT
  bool isCoincident(const std::shared_ptr<GeomAPI_Pln> thePlane, const double theTolerance = 1.e-7);

  /// Returns true if plane is parallel to theLine.
  GEOMAPI_EXPORT
  bool isParallel(const std::shared_ptr<GeomAPI_Lin> theLine);

  /// Returns intersection point or empty if no intersections
  GEOMAPI_EXPORT
  std::shared_ptr<GeomAPI_Pnt> intersect(const std::shared_ptr<GeomAPI_Lin>& theLine) const;

  /// Returns projection of the given point onto the plane
  GEOMAPI_EXPORT
  std::shared_ptr<GeomAPI_Pnt> project(const std::shared_ptr<GeomAPI_Pnt>& thePoint) const;
};

#endif

