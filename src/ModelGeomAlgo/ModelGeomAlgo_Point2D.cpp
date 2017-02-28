// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_Tools.cpp
// Created:     20 Jul 2016
// Author:      Natalia ERMOLAEVA

#include "ModelGeomAlgo_Point2D.h"

#include <ModelAPI_Feature.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_Tools.h>

#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomDataAPI_Point2D.h>

#include <GeomAPI_Pnt.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_Vertex.h>
#include <GeomAPI_Dir.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Lin.h>
#include <GeomAPI_Circ.h>

#ifdef WIN32
#pragma warning(disable : 4996) // for sprintf
#endif

namespace ModelGeomAlgo_Point2D {
  std::shared_ptr<GeomDataAPI_Point2D> getPointOfRefAttr(ModelAPI_Feature* theFeature,
                                                  const std::string& theAttribute,
                                                  const std::string& theObjectFeatureKind,
                                                  const std::string& theObjectFeatureAttribute)
  {
    std::shared_ptr<GeomDataAPI_Point2D> aPointAttr;

    /// essential check as it is called in openGl thread
    if (!theFeature || !theFeature->data().get() || !theFeature->data()->isValid())
      return std::shared_ptr<GeomDataAPI_Point2D>();

    FeaturePtr aFeature;
    std::shared_ptr<ModelAPI_AttributeRefAttr> anAttr = std::dynamic_pointer_cast<
        ModelAPI_AttributeRefAttr>(theFeature->data()->attribute(theAttribute));
    if(anAttr.get() && anAttr->isInitialized()) {
      aFeature = ModelAPI_Feature::feature(anAttr->object());
      if (aFeature.get()) {
        bool aFeatureOfObjectKind = !theObjectFeatureKind.empty() &&
                                    !theObjectFeatureAttribute.empty() &&
                                    aFeature->getKind() == theObjectFeatureKind;
        if(aFeatureOfObjectKind)
            aPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                                    aFeature->data()->attribute(theObjectFeatureAttribute));
        else if (anAttr->attr())
          aPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(anAttr->attr());
      }
    }
    return aPointAttr;
  }

  void getPointsOfReference(const std::shared_ptr<ModelAPI_Object>& theObject,
                            const std::string& theReferenceFeatureKind,
                            std::set<std::shared_ptr<GeomDataAPI_Point2D> >& theAttributes,
                            const std::string& theObjectFeatureKind,
                            const std::string& theObjectFeatureAttribute,
                            const bool isSkipFeatureAttributes)
  {
    // find by feature
    FeaturePtr aSourceFeature = ModelAPI_Feature::feature(theObject);

    const std::set<AttributePtr>& aRefsList = theObject->data()->refsToMe();
    std::set<AttributePtr>::const_iterator aIt;
    for (aIt = aRefsList.cbegin(); aIt != aRefsList.cend(); ++aIt) {
      std::shared_ptr<ModelAPI_Attribute> aAttr = (*aIt);
      FeaturePtr aRefFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aAttr->owner());
      if (aRefFeature->getKind() == theReferenceFeatureKind) {
        std::list<AttributePtr> anAttributes =
                         aRefFeature->data()->attributes(ModelAPI_AttributeRefAttr::typeId());
        std::list<AttributePtr>::iterator anIter = anAttributes.begin(), aLast = anAttributes.end();
        bool isSkippedAttribute = false;
        if (isSkipFeatureAttributes) {
          for(anIter = anAttributes.begin(); anIter != aLast && !isSkippedAttribute; anIter++) {
            AttributeRefAttrPtr aRefAttribute =
              std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*anIter);
            if (aRefAttribute.get() && !aRefAttribute->isObject()) {
              std::shared_ptr<GeomDataAPI_Point2D> aPointAttr =
                             std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aRefAttribute->attr());
              FeaturePtr anAttributeFeature = ModelAPI_Feature::feature(aPointAttr->owner());
              isSkippedAttribute = aSourceFeature == anAttributeFeature;
            }
          }
        }
        if (isSkippedAttribute)
          continue;

        // it searches the first point of AttributeRefAtt
        std::shared_ptr<GeomDataAPI_Point2D> aPointAttr;
        for(anIter = anAttributes.begin(); anIter != aLast && !aPointAttr.get(); anIter++) {
          AttributeRefAttrPtr aRefAttribute =
            std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*anIter);
          if (aRefAttribute.get()) {
            aPointAttr = getPointOfRefAttr(aRefFeature.get(), aRefAttribute->id(),
                         theObjectFeatureKind, theObjectFeatureAttribute);
          }
        }
        if (aPointAttr.get()) {
          theAttributes.insert(aPointAttr);
        }
      }
    }
    // find by results
    FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theObject);
    if (aFeature.get()) {
      const std::list<std::shared_ptr<ModelAPI_Result> > aResults = aFeature->results();
      std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aRIter = aResults.begin();
      for (; aRIter != aResults.cend(); aRIter++) {
        ResultPtr aResult = *aRIter;
        getPointsOfReference(aResult, theReferenceFeatureKind, theAttributes, theObjectFeatureKind,
                             theObjectFeatureAttribute);
      }
    }
  }

  void getPointsIntersectedShape(const std::shared_ptr<ModelAPI_Feature>& theBaseFeature,
                                 const std::list<std::shared_ptr<ModelAPI_Feature> >& theFeatures,
                                 std::list<std::shared_ptr<GeomAPI_Pnt> >& thePoints,
                                 std::map<std::shared_ptr<ModelAPI_Object>,
                                               std::shared_ptr<GeomAPI_Pnt> >& theObjectToPoint)
  {
    GeomShapePtr aFeatureShape;
    {
      std::set<ResultPtr> anEdgeShapes;
      ModelAPI_Tools::shapesOfType(theBaseFeature, GeomAPI_Shape::EDGE, anEdgeShapes);
      if (anEdgeShapes.empty())
        return;
      aFeatureShape = (*anEdgeShapes.begin())->shape();
    }

    std::list<std::shared_ptr<ModelAPI_Feature> >::const_iterator anIt = theFeatures.begin(),
                                                                  aLast = theFeatures.end();
    for (; anIt != aLast; anIt++) {
      FeaturePtr aFeature = *anIt;
      if (aFeature.get() == theBaseFeature.get())
        continue;
      if (aFeature.get()) {
        std::set<ResultPtr> anEdgeShapes;
        ModelAPI_Tools::shapesOfType(aFeature, GeomAPI_Shape::EDGE, anEdgeShapes);
        if (anEdgeShapes.empty())
          continue;
        ResultPtr aResult = *anEdgeShapes.begin();
        GeomShapePtr aShape = aResult->shape();

        GeomShapePtr aShapeOfIntersection = aFeatureShape->intersect(aShape);
        if (!aShapeOfIntersection.get())
          continue;
        switch (aShapeOfIntersection->shapeType()) {
          case GeomAPI_Shape::VERTEX: {
            std::shared_ptr<GeomAPI_Vertex> aVertex =
              std::shared_ptr<GeomAPI_Vertex>(new GeomAPI_Vertex(aShapeOfIntersection));
            std::shared_ptr<GeomAPI_Pnt> aPnt = aVertex->point();
            thePoints.push_back(aPnt);
            theObjectToPoint[aResult] = aPnt;
          }
          break;
          case GeomAPI_Shape::EDGE: {
            /*std::shared_ptr<GeomAPI_Edge> anEdge =
              std::shared_ptr<GeomAPI_Edge>(new GeomAPI_Vertex(aShapeOfIntersection));
            std::shared_ptr<GeomAPI_Pnt> aPnt = aVertex->point();
            thePoints.push_back(aPnt);
            theObjectToPoint[aResult] = aPnt;*/
          }
          break;
          case GeomAPI_Shape::COMPOUND: {
          }
          break;
          default: break;
        }
      }
    }
  }

  void getPointsInsideShape(const std::shared_ptr<GeomAPI_Shape> theBaseShape,
                            const std::set<std::shared_ptr<GeomDataAPI_Point2D> >& theAttributes,
                            const std::shared_ptr<GeomAPI_Pnt>& theOrigin,
                            const std::shared_ptr<GeomAPI_Dir>& theDirX,
                            const std::shared_ptr<GeomAPI_Dir>& theDirY,
                            std::list<std::shared_ptr<GeomAPI_Pnt> >& thePoints,
                            std::map<std::shared_ptr<GeomDataAPI_Point2D>,
                                     std::shared_ptr<GeomAPI_Pnt> >& theAttributeToPoint)
  {
    std::set<std::shared_ptr<GeomDataAPI_Point2D> >::const_iterator anIt = theAttributes.begin(),
                                                            aLast = theAttributes.end();
    for (; anIt != aLast; anIt++) {
      std::shared_ptr<GeomDataAPI_Point2D> anAttribute = *anIt;
      std::shared_ptr<GeomAPI_Pnt2d> aPnt2d = anAttribute->pnt();
      std::shared_ptr<GeomAPI_Pnt> aPoint = aPnt2d->to3D(theOrigin, theDirX, theDirY);
      std::shared_ptr<GeomAPI_Pnt> aProjectedPoint;
      if (isPointOnEdge(theBaseShape, aPoint, aProjectedPoint)) {
        thePoints.push_back(aProjectedPoint);
        theAttributeToPoint[anAttribute] = aProjectedPoint;
      }
    }
  }

  bool isPointOnEdge(const std::shared_ptr<GeomAPI_Shape> theBaseShape,
                     const std::shared_ptr<GeomAPI_Pnt>& thePoint,
                     std::shared_ptr<GeomAPI_Pnt>& theProjectedPoint)
  {
    bool isInside = false;
    if (theBaseShape->shapeType() == GeomAPI_Shape::EDGE) {
      std::shared_ptr<GeomAPI_Edge> anEdge(new GeomAPI_Edge(theBaseShape));
      if (anEdge->isLine()) {
        std::shared_ptr<GeomAPI_Lin> aLine = anEdge->line();
        theProjectedPoint = aLine->project(thePoint);
      }
      else if (anEdge->isCircle() || anEdge->isArc()) {
        std::shared_ptr<GeomAPI_Circ> aCircle = anEdge->circle();
        theProjectedPoint = aCircle->project(thePoint);
      }
      if (theProjectedPoint.get()) {
        std::shared_ptr<GeomAPI_Vertex> aVertexShape(new GeomAPI_Vertex(theProjectedPoint->x(),
                                                  theProjectedPoint->y(), theProjectedPoint->z()));
        isInside = GeomAlgoAPI_ShapeTools::isSubShapeInsideShape(aVertexShape, theBaseShape);
      }
    }
    return isInside;
  }

  std::string doubleToString(double theValue)
  {
    std::string aValueStr;
    char aBuf[50];
    int n = sprintf(aBuf, "%g", theValue);
    aValueStr = std::string(aBuf);
    return aValueStr;
  }

#ifdef _DEBUG
  std::string getPontAttributesInfo(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                           const std::set<std::shared_ptr<ModelAPI_Attribute> >& theAttributesOnly)
  {
    std::string anInfo;

    std::list<AttributePtr> anAttrs = theFeature->data()->attributes(GeomDataAPI_Point2D::typeId());
    std::list<AttributePtr>::const_iterator anIt = anAttrs.begin(), aLast = anAttrs.end();

    for(; anIt != aLast; anIt++) {
      AttributePtr anAttribute = *anIt;
      if (anAttribute.get() && (theAttributesOnly.empty() ||
          theAttributesOnly.find(anAttribute) != theAttributesOnly.end())) {
      if (!anInfo.empty()) {
        anInfo.append(", ");
        anInfo.append("\n");
      }
      anInfo.append("    " + getPointAttributeInfo(anAttribute));
      }
    }
    return anInfo;
  }

  std::string getPointAttributeInfo(const std::shared_ptr<ModelAPI_Attribute>& theAttribute)
  {
    std::string anInfo;
    std::string aValue = "not defined";
    std::string aType = theAttribute->attributeType();
    if (aType == GeomDataAPI_Point2D::typeId()) {
      std::shared_ptr<GeomDataAPI_Point2D> aPoint =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(theAttribute);
      if (aPoint.get() && aPoint->isInitialized()) {
        aValue = std::string("(" + doubleToString(aPoint->x()) + ", "+
                             doubleToString(aPoint->y()) + ")");
      }
    }
    anInfo.append(theAttribute->id() + ": " + aValue);

    return anInfo;
  }
#endif
} // namespace ModelGeomAlgo_Point2D
