// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include <GeomAPI_Shape.h>

#include "ModelGeomAlgo_Shape.h"

#include <ModelAPI_Feature.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_ResultConstruction.h>

#include <GeomAPI_PlanarEdges.h>
#include <GeomAPI_Pnt.h>


#ifdef WIN32
#pragma warning(disable : 4996) // for sprintf
#endif

namespace ModelGeomAlgo_Shape
{
  void shapesOfType(const FeaturePtr& theFeature,
                    const GeomAPI_Shape::ShapeType& theType,
                    std::set<ResultPtr>& theShapeResults)
  {
    theShapeResults.clear();
    std::list<ResultPtr> aResults = theFeature->results();
    std::list<ResultPtr>::const_iterator aRIter = aResults.cbegin();
    for (; aRIter != aResults.cend(); aRIter++) {
      ResultPtr aResult = *aRIter;
      GeomShapePtr aShape = aResult->shape();
      if (aShape.get() && aShape->shapeType() == theType)
        theShapeResults.insert(aResult);
    }
  }

  // Check the point is within shape's bounding box
  static bool isPointWithinBB(const GeomPointPtr& thePoint,
                              const GeomShapePtr& theShape,
                              const double theTolerance)
  {
    double aXMin, aXMax, aYMin, aYMax, aZMin, aZMax;
    theShape->computeSize(aXMin, aYMin, aZMin, aXMax, aYMax, aZMax);
    return thePoint->x() >= aXMin - theTolerance && thePoint->x() <= aXMax + theTolerance &&
           thePoint->y() >= aYMin - theTolerance && thePoint->y() <= aYMax + theTolerance &&
           thePoint->z() >= aZMin - theTolerance && thePoint->z() <= aZMax + theTolerance;
  }

  // Select sub-shape of the given type, which contains the given point
  static GeomShapePtr findSubShape(const GeomShapePtr& theShape,
                                   const GeomAPI_Shape::ShapeType& theType,
                                   const GeomPointPtr& thePoint,
                                   const double theTolerance)
  {
    std::list<GeomShapePtr> aSubs = theShape->subShapes(theType);
    for (std::list<GeomShapePtr>::const_iterator aSubIt = aSubs.begin();
      aSubIt != aSubs.end(); ++aSubIt) {
      if ((*aSubIt)->middlePoint()->distance(thePoint) < theTolerance)
        return *aSubIt;
    }

    // not found
    return GeomShapePtr();
  }

  bool findSubshapeByPoint(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                           const std::shared_ptr<GeomAPI_Pnt>& thePoint,
                           const GeomAPI_Shape::ShapeType& theShapeType,
                           std::shared_ptr<ModelAPI_Result>& theResult,
                           std::shared_ptr<GeomAPI_Shape>& theSubshape)
  {
    static const double TOLERANCE = 1.e-7;

    theResult = ResultPtr();
    const std::list<ResultPtr>& aResults = theFeature->results();
    for (std::list<ResultPtr>::const_iterator aResIt = aResults.begin();
         aResIt != aResults.end(); ++aResIt) {
      GeomShapePtr aCurShape = (*aResIt)->shape();
      // first of all, check the point is within bounding box of the result
      if (!aCurShape || !isPointWithinBB(thePoint, aCurShape, TOLERANCE))
        continue;
      // now, process all sub-shapes of the given type and check their inner points,
      // but skip the case the selected type is COMPOUND and the shape is a list of sketch edges
      // (it will be processed later)
      std::shared_ptr<GeomAPI_PlanarEdges> aSketchEdges =
          std::dynamic_pointer_cast<GeomAPI_PlanarEdges>(aCurShape);
      if (theShapeType != GeomAPI_Shape::COMPOUND || !aSketchEdges)
        theSubshape = findSubShape(aCurShape, theShapeType, thePoint, TOLERANCE);
      if (theSubshape) {
        theResult = *aResIt;
        break;
      }

      // special case for ResultConstruction if the FACE is selected
      ResultConstructionPtr aResConstr =
          std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(*aResIt);
      if (aResConstr && theShapeType >= GeomAPI_Shape::FACE) {
        int aNbFaces = aResConstr->facesNum();
        for (int aFaceInd = 0; aFaceInd < aNbFaces; ++aFaceInd) {
          GeomFacePtr aCurFace = aResConstr->face(aFaceInd);
          // check the point is within bounding box of the face
          if (!isPointWithinBB(thePoint, aCurFace, TOLERANCE))
            continue;
          theSubshape = findSubShape(aCurFace, theShapeType, thePoint, TOLERANCE);
          if (theSubshape) {
            theResult = *aResIt;
            break;
          }
        }
      }
      if (theResult)
        break;

      // next special case: the full sketch is selected
      // the selection type is a COMPOUND
      if (aSketchEdges &&
          aSketchEdges->middlePoint()->distance(thePoint) < TOLERANCE) {
        // select whole result
        theResult = *aResIt;
        theSubshape = GeomShapePtr();
        break;
      }
    }

    return (bool)theResult;
  }
} // namespace ModelGeomAlgo_Shape
