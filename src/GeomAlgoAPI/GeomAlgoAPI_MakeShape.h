// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_MakeShape.h
// Created:     17 Oct 2014
// Author:      Sergey ZARITCHNY
#ifndef GeomAlgoAPI_MakeShape_H_
#define GeomAlgoAPI_MakeShape_H_

#include <GeomAPI_Shape.h>
#include <GeomAlgoAPI.h>

#include <list>
#include <memory>

/**\class GeomAlgoAPI_MakeShape
 * \ingroup DataAlgo
 * \brief Interface to the root class of all topological shapes constructions
 */
class GeomAlgoAPI_MakeShape : public GeomAPI_Interface
{
public:
  /// Algo type enum
  enum AlgoType {
    MakeShape,
    BOPAlgoBuilder
  };

public:
  /// Constructor by the already stored builder in the interface
  GEOMALGOAPI_EXPORT GeomAlgoAPI_MakeShape(void* theBuilder, const AlgoType theAlgoType = MakeShape);

  /// Returns a shape built by the shape construction algorithm
  GEOMALGOAPI_EXPORT virtual const std::shared_ptr<GeomAPI_Shape> shape() const;

  /// Returns the list of shapes generated from the shape \a theShape
  GEOMALGOAPI_EXPORT virtual void generated(
    const std::shared_ptr<GeomAPI_Shape> theShape, ListOfShape& theHistory);

  /// Returns the  list of shapes modified from the shape \a theShape
  GEOMALGOAPI_EXPORT virtual void modified(
    const std::shared_ptr<GeomAPI_Shape> theShape, ListOfShape& theHistory);

  /// Returns whether the shape is deleted
  GEOMALGOAPI_EXPORT virtual bool isDeleted(const std::shared_ptr<GeomAPI_Shape> theShape);

protected:
  GEOMALGOAPI_EXPORT GeomAlgoAPI_MakeShape(){};

protected:
  GeomAlgoAPI_MakeShape::AlgoType myAlgoType; ///< Type of make shape algo.
  std::shared_ptr<GeomAPI_Shape> myShape; ///< Resulting shape.
  std::shared_ptr<GeomAPI_Shape> myWire; ///< Wire for pipe algo.
  std::shared_ptr<GeomAPI_Shape> myBaseShape; ///< Base shape of algo.
};

typedef std::list<std::shared_ptr<GeomAlgoAPI_MakeShape> > ListOfMakeShape;

#endif
