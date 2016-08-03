// Name   : ConstructionAPI_Point.cpp
// Purpose: 
//
// History:
// 29/03/16 - Sergey POKHODENKO - Creation of the file

#include "ConstructionAPI_Point.h"

#include <GeomAPI_Shape.h>

#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
ConstructionAPI_Point::ConstructionAPI_Point(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
ConstructionAPI_Point::ConstructionAPI_Point(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                             const ModelHighAPI_Double& theX,
                                             const ModelHighAPI_Double& theY,
                                             const ModelHighAPI_Double& theZ)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    setByXYZ(theX, theY, theZ);
  }
}

/*//==================================================================================================
ConstructionAPI_Point::ConstructionAPI_Point(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                             const ModelHighAPI_Selection& theEdge,
                                             const ModelHighAPI_Double& theDistanceValue,
                                             const bool theDistancePercent,
                                             const bool theReverse)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    setByDistanceOnEdge(theEdge, theDistanceValue, theDistancePercent, theReverse);
  }
}

//==================================================================================================
ConstructionAPI_Point::ConstructionAPI_Point(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                             const ModelHighAPI_Selection& theObject1,
                                             const ModelHighAPI_Selection& theObject2)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    GeomAPI_Shape::ShapeType aType1 = getShapeType(theObject1);
    GeomAPI_Shape::ShapeType aType2 = getShapeType(theObject2);

    if(aType1 == GeomAPI_Shape::VERTEX && aType2 == GeomAPI_Shape::FACE) {
      // If first object is vertex and second object is face then set by projection.
      setByProjection(theObject1, theObject2);
    } else if(aType1 == GeomAPI_Shape::EDGE && aType2 == GeomAPI_Shape::EDGE) {
      // If both objects are edges then set by lines intersection.
      setByLinesIntersection(theObject1, theObject2);
    } else if(aType1 == GeomAPI_Shape::EDGE && aType2 == GeomAPI_Shape::FACE) {
      // If first object is edge and second object is face then set by line and plane intersection.
      setByLineAndPlaneIntersection(theObject1, theObject2);
    }
  }
}*/

//==================================================================================================
ConstructionAPI_Point::~ConstructionAPI_Point()
{

}

//==================================================================================================
void ConstructionAPI_Point::setByXYZ(const ModelHighAPI_Double& theX,
                                     const ModelHighAPI_Double& theY,
                                     const ModelHighAPI_Double& theZ)
{
  //fillAttribute(ConstructionPlugin_Point::CREATION_METHOD_BY_XYZ(), mycreationMethod);
  fillAttribute(theX, myx);
  fillAttribute(theY, myy);
  fillAttribute(theZ, myz);

  execute();
}

/*//==================================================================================================
void ConstructionAPI_Point::setByDistanceOnEdge(const ModelHighAPI_Selection& theEdge,
                                                const ModelHighAPI_Double& theDistanceValue,
                                                const bool theDistancePercent,
                                                const bool theReverse)
{
  fillAttribute(ConstructionPlugin_Point::CREATION_METHOD_BY_DISTANCE_ON_EDGE(), mycreationMethod);
  fillAttribute(theEdge, myedge);
  fillAttribute(theDistanceValue, mydistanceValue);
  fillAttribute(theDistancePercent, mydistancePercent);
  fillAttribute(theReverse, myreverse);

  execute();
}

//==================================================================================================
void ConstructionAPI_Point::setByProjection(const ModelHighAPI_Selection& theVertex,
                                            const ModelHighAPI_Selection& theFace)
{
  fillAttribute(ConstructionPlugin_Point::CREATION_METHOD_BY_PROJECTION(), mycreationMethod);
  fillAttribute(theVertex, mypoint);
  fillAttribute(theFace, myplane);

  execute();
}

//==================================================================================================
void ConstructionAPI_Point::setByLinesIntersection(const ModelHighAPI_Selection& theEdge1,
                                                   const ModelHighAPI_Selection& theEdge2)
{
  fillAttribute(ConstructionPlugin_Point::CREATION_METHOD_BY_LINES_INTERSECTION(), mycreationMethod);
  fillAttribute(theEdge1, myfirstLine);
  fillAttribute(theEdge2, mysecondLine);

  execute();
}

//==================================================================================================
void ConstructionAPI_Point::setByLineAndPlaneIntersection(const ModelHighAPI_Selection& theEdge,
                                                          const ModelHighAPI_Selection& theFace)
{
  fillAttribute(ConstructionPlugin_Point::CREATION_METHOD_BY_LINE_AND_PLANE_INTERSECTION(), mycreationMethod);
  fillAttribute(theEdge, myintersectionLine);
  fillAttribute(theFace, myintersectionPlane);

  execute();
}*/

//==================================================================================================
PointPtr addPoint(const std::shared_ptr<ModelAPI_Document>& thePart,
                  const ModelHighAPI_Double& theX,
                  const ModelHighAPI_Double& theY,
                  const ModelHighAPI_Double& theZ)
{
  // TODO(spo): check that thePart is not empty
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(ConstructionAPI_Point::ID());
  return PointPtr(new ConstructionAPI_Point(aFeature, theX, theY, theZ));
}

/*//==================================================================================================
PointPtr addPoint(const std::shared_ptr<ModelAPI_Document> & thePart,
                  const ModelHighAPI_Selection& theEdge,
                  const ModelHighAPI_Double& theDistanceValue,
                  const bool theDistancePercent,
                  const bool theReverse)
{
  // TODO(spo): check that thePart is not empty
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(ConstructionAPI_Point::ID());
  return PointPtr(new ConstructionAPI_Point(aFeature, theEdge, theDistanceValue, theDistancePercent, theReverse));
}

//==================================================================================================
PointPtr addPoint(const std::shared_ptr<ModelAPI_Document> & thePart,
                  const ModelHighAPI_Selection& theObject1,
                  const ModelHighAPI_Selection& theObject2)
{
  // TODO(spo): check that thePart is not empty
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(ConstructionAPI_Point::ID());
  return PointPtr(new ConstructionAPI_Point(aFeature, theObject1, theObject2));
}*/
