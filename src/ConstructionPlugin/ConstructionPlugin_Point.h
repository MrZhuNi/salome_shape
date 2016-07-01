// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ConstructionPlugin_Point.h
// Created:     3 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef ConstructionPlugin_Point_H_
#define ConstructionPlugin_Point_H_

#include "ConstructionPlugin.h"

#include <GeomAPI_ICustomPrs.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Result.h>

/// \class ConstructionPlugin_Point
/// \ingroup Plugins
/// \brief Feature for creation of the new part in PartSet.
class ConstructionPlugin_Point: public ModelAPI_Feature, public GeomAPI_ICustomPrs
{
public:
  /// Returns the kind of a feature.
  CONSTRUCTIONPLUGIN_EXPORT virtual const std::string& getKind();

  /// Point kind.
  inline static const std::string& ID()
  {
    static const std::string CONSTRUCTION_POINT_KIND("Point");
    return CONSTRUCTION_POINT_KIND;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD()
  {
    static const std::string MY_CREATION_METHOD_ID("creation_method");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD_BY_XYZ()
  {
    static const std::string MY_CREATION_METHOD_ID("by_xyz");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name for creation method.
  inline static const std::string& CREATION_METHOD_BY_DISTANCE_ON_EDGE()
  {
    static const std::string MY_CREATION_METHOD_ID("by_distance_on_edge");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name for X coordinate.
  inline static const std::string& X()
  {
    static const std::string POINT_ATTR_X("x");
    return POINT_ATTR_X;
  }

  /// Attribute name for Y coordinate.
  inline static const std::string& Y()
  {
    static const std::string POINT_ATTR_Y("y");
    return POINT_ATTR_Y;
  }

  /// Attribute name for Z coordinate.
  inline static const std::string& Z()
  {
    static const std::string POINT_ATTR_Z("z");
    return POINT_ATTR_Z;
  }

  /// Attribute name for seleted edge.
  inline static const std::string& EDGE()
  {
    static const std::string ATTR_ID("edge");
    return ATTR_ID;
  }

  /// Attribute name for distance.
  inline static const std::string& DISTANCE_VALUE()
  {
    static const std::string ATTR_ID("value");
    return ATTR_ID;
  }

  /// Attribute name for percent flag.
  inline static const std::string& DISTANCE_PERCENT()
  {
    static const std::string ATTR_ID("percent");
    return ATTR_ID;
  }

  /// Attribute name for reverse flag.
  inline static const std::string& REVERSE()
  {
    static const std::string ATTR_ID("reverse");
    return ATTR_ID;
  }

  /// Creates a new part document if needed.
  CONSTRUCTIONPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes.
  CONSTRUCTIONPLUGIN_EXPORT virtual void initAttributes();

  /// Construction result is allways recomuted on the fly.
  CONSTRUCTIONPLUGIN_EXPORT virtual bool isPersistentResult() {return false;}

  /// Use plugin manager for features creation
  ConstructionPlugin_Point();

  /// Customize presentation of the feature
  virtual bool customisePresentation(ResultPtr theResult, AISObjectPtr thePrs,
                                     std::shared_ptr<GeomAPI_ICustomPrs> theDefaultPrs);

private:
  std::shared_ptr<GeomAPI_Shape> createByXYZ();
  std::shared_ptr<GeomAPI_Shape> createByDistanceOnEdge();

};

#endif
