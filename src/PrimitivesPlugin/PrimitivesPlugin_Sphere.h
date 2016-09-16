// Copyright (C) 2014-2016 CEA/DEN, EDF R&D

// File:        PrimitivesPlugin_Sphere.h
// Created:     29 Mar 2016
// Author:      CEA (delegation to Alyotech)

#ifndef PrimitivesPlugin_Sphere_H_
#define PrimitivesPlugin_Sphere_H_

#include <PrimitivesPlugin.h>
#include <ModelAPI_Feature.h>
#include <GeomAlgoAPI_Sphere.h>

class GeomAPI_Shape;
class ModelAPI_ResultBody;

/**\class PrimitivesPlugin_Sphere
 * \ingroup Plugins
 * \brief Feature for creation of a sphere.
 *
 * Sphere creates a sphere from a radius and a center point, defaulting to
 * the origin.
 */
class PrimitivesPlugin_Sphere : public ModelAPI_Feature
{
 public:
  /// Sphere kind
  inline static const std::string& ID()
  {
    static const std::string MY_SPHERE_ID("Sphere");
    return MY_SPHERE_ID;
  }

  /// attribute name of the center
  inline static const std::string& CENTER()
  {
    static const std::string MY_CENTER("center");
    return MY_CENTER;
  }

  /// attribute name of the radius
  inline static const std::string& RADIUS()
  {
    static const std::string MY_RADIUS("radius");
    return MY_RADIUS;
  }

  /// Returns the kind of a feature
  PRIMITIVESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = PrimitivesPlugin_Sphere::ID();
    return MY_KIND;
  }

  /// Creates a new part document if needed
  PRIMITIVESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  PRIMITIVESPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation
  PrimitivesPlugin_Sphere();

 private:
  /// Load Naming data structure of the feature to the document
  void loadNamingDS(std::shared_ptr<GeomAlgoAPI_Sphere> theSphereAlgo,
                    std::shared_ptr<ModelAPI_ResultBody> theResultSphere);

};

#endif
