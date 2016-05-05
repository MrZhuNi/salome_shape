// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Shape.hxx
// Created:     23 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef GeomAPI_Shape_H_
#define GeomAPI_Shape_H_

#include <GeomAPI_Interface.h>
#include <memory>
#include <list>

/**\class GeomAPI_Shape
 * \ingroup DataModel
 * \brief Interface to the topological shape object
 */
class GeomAPI_Shape : public GeomAPI_Interface
{
public:
  /// Shape type enum
  enum ShapeType {
    COMPOUND, COMPSOLID, SOLID, SHELL,
    FACE, WIRE, EDGE, VERTEX,
    SHAPE
  };

  /// Shape orientation
  enum Orientation {
    FORWARD,
    REVERSED,
    INTERNAL,
    EXTERNAL
  };

 public:
  /// Creation of empty (null) shape
  GEOMAPI_EXPORT 
  GeomAPI_Shape();

  /// \return a new Shape with the same Orientation and Location and a new TShape with the same
  ///         geometry and no sub-shapes.
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Shape> emptyCopied() const;

  /// Returns true if the underlied shape is null
  GEOMAPI_EXPORT 
  bool isNull() const;

  /// Returns whether the shapes are equal
  GEOMAPI_EXPORT 
  virtual bool isEqual(const std::shared_ptr<GeomAPI_Shape> theShape) const;

  /// Returns whether the shape is a vertex
  GEOMAPI_EXPORT 
  virtual bool isVertex() const;

  /// Returns whether the shape is an edge
  GEOMAPI_EXPORT 
  virtual bool isEdge() const;

  /// Returns whether the shape is a face
  GEOMAPI_EXPORT 
  virtual bool isFace() const;

  /// Returns whether the shape is a compound
  GEOMAPI_EXPORT 
  virtual bool isCompound() const;

  /// Returns whether the shape is a compound of solids
  GEOMAPI_EXPORT 
  virtual bool isCompoundOfSolids() const;

  /// Returns whether the shape is a compound where all elements are topologically connected
  GEOMAPI_EXPORT 
  virtual bool isConnectedTopology() const;

  /// Returns whether the shape is a solid
  GEOMAPI_EXPORT 
  virtual bool isSolid() const;

  /// Returns whether the shape is a compsolid
  GEOMAPI_EXPORT
  virtual bool isCompSolid() const;

  /// Returns whether the shape is planar
  GEOMAPI_EXPORT
  virtual bool isPlanar() const;

  /// Returns the shape type
  GEOMAPI_EXPORT
  virtual ShapeType shapeType() const;

  /// \return the shape type as string.
  GEOMAPI_EXPORT
  virtual std::string shapeTypeStr() const;

  /// \return the shape orientation.
  GEOMAPI_EXPORT virtual Orientation orientation() const;

  /// Sets the shape orientation.
  GEOMAPI_EXPORT virtual void setOrientation(const Orientation theOrientation);

  /// \return true if passed shape is a sub-shape of this shape.
  /// \param theShape shape to search.
  GEOMAPI_EXPORT virtual bool isSubShape(const std::shared_ptr<GeomAPI_Shape> theShape) const;

  /// Computes boundary dimensions of the shape
  /// Returns False if it is not possible
  GEOMAPI_EXPORT 
  bool computeSize(double& theXmin, double& theYmin, double& theZmin,
                   double& theXmax, double& theYmax, double& theZmax) const;

  /// Returns the shape as BRep stream
  GEOMAPI_EXPORT 
  std::string getShapeStream() const;
};

//! Pointer on list of shapes
typedef std::list<std::shared_ptr<GeomAPI_Shape> > ListOfShape;

//! Pointer on attribute object
typedef std::shared_ptr<GeomAPI_Shape> GeomShapePtr;

#endif
