// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Edge.hxx
// Created:     24 Jul 2014
// Author:      Artem ZHIDKOV

#ifndef GeomAPI_Edge_H_
#define GeomAPI_Edge_H_

#include <GeomAPI_Shape.h>

class GeomAPI_Pln;
class GeomAPI_Pnt;
class GeomAPI_Circ;
class GeomAPI_Lin;

/**\class GeomAPI_Edge
* \ingroup DataModel
 * \brief Interface to the edge object
 */

class GeomAPI_Edge : public GeomAPI_Shape
{
public:
   /// Makes an undefined Edge (no geometry).
  GEOMAPI_EXPORT 
   GeomAPI_Edge();

   /// Creation of edge by the edge-shape
  GEOMAPI_EXPORT 
   GeomAPI_Edge(const std::shared_ptr<GeomAPI_Shape>& theShape);

  /// Verifies that the edge is a line
  GEOMAPI_EXPORT 
  bool isLine() const;

  /// Verifies that the edge is a circle
  GEOMAPI_EXPORT 
  bool isCircle() const;

  /// Verifies that the edge is an arc of circle
  GEOMAPI_EXPORT 
  bool isArc() const;

  /// Returns the first vertex coordinates of the edge 
  GEOMAPI_EXPORT 
  std::shared_ptr<GeomAPI_Pnt> firstPoint();

  /// Returns the Last vertex coordinates of the edge 
  GEOMAPI_EXPORT 
  std::shared_ptr<GeomAPI_Pnt> lastPoint();

  /// Returns a circle if edge is based on the circle curve
  GEOMAPI_EXPORT 
  std::shared_ptr<GeomAPI_Circ> circle();

  /// Returns a line if edge is based on the linear curve
  GEOMAPI_EXPORT 
  std::shared_ptr<GeomAPI_Lin> line();

  /// Returns true if the current edge is geometrically equal to the given edge
  GEOMAPI_EXPORT 
  bool isEqual(const std::shared_ptr<GeomAPI_Shape> theEdge) const;

  /// Returns range of parameter on the curve
  GEOMAPI_EXPORT 
  void getRange(double& theFirst, double& theLast) const;

  /// Returns true, if the edge is fully placed in the specified plane
  GEOMAPI_EXPORT
  bool isInPlane(const std::shared_ptr<GeomAPI_Pln> thePlane) const;
};

#endif

