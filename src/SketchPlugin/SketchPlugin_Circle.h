// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchPlugin_Circle.h
// Created:     26 May 2014
// Author:      Artem ZHIDKOV

#ifndef SketchPlugin_Circle_H_
#define SketchPlugin_Circle_H_

#include "SketchPlugin.h"
#include <SketchPlugin_SketchEntity.h>
#include <SketchPlugin_Sketch.h>
#include <GeomAPI_IPresentable.h>

/**\class SketchPlugin_Circle
 * \ingroup Plugins
 * \brief Feature for creation of the new circle in PartSet.
 */
class SketchPlugin_Circle : public SketchPlugin_SketchEntity, public GeomAPI_IPresentable
{
 public:
  /// Circle feature kind
  inline static const std::string& ID()
  {
    static const std::string MY_CIRCLE_ID("SketchCircle");
    return MY_CIRCLE_ID;
  }

  inline static const std::string& CIRCLE_TYPE()
  {
    static const std::string TYPE("CircleType");
    return TYPE;
  }

  /// Creation method by center and radius.
  inline static const std::string& CIRCLE_TYPE_CENTER_AND_RADIUS()
  {
    static const std::string TYPE("CenterRadius");
    return TYPE;
  }

  /// Creation method by three points.
  inline static const std::string& CIRCLE_TYPE_THREE_POINTS()
  {
    static const std::string TYPE("ThreePoints");
    return TYPE;
  }

  /// 2D point - center of the circle
  inline static const std::string& CENTER_ID()
  {
    static const std::string MY_CIRCLE_CENTER_ID("CircleCenter");
    return MY_CIRCLE_CENTER_ID;
  }

  /// Radius of the circle
  inline static const std::string& RADIUS_ID()
  {
    static const std::string MY_CIRCLE_RADIUS_ID("CircleRadius");
    return MY_CIRCLE_RADIUS_ID;
  }

  /// First point id.
  inline static const std::string& FIRST_POINT_ID()
  {
    static const std::string FIRST_PNT("FirstPoint");
    return FIRST_PNT;
  }

  /// Second point id.
  inline static const std::string& SECOND_POINT_ID()
  {
    static const std::string SECOND_PNT("SecondPoint");
    return SECOND_PNT;
  }

  /// Third point id.
  inline static const std::string& THIRD_POINT_ID()
  {
    static const std::string THIRD_PNT("ThirdPoint");
    return THIRD_PNT;
  }

  /// Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_Circle::ID();
    return MY_KIND;
  }

  /// Returns true is sketch element is under the rigid constraint
  SKETCHPLUGIN_EXPORT virtual bool isFixed();

  /// Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// Adds sub-feature of the higher level feature (sub-element of the sketch)
  /// \param theFeature sub-feature
  SKETCHPLUGIN_EXPORT virtual const void addSub(const FeaturePtr& theFeature)
  {
  }

  /// Moves the feature
  /// \param theDeltaX the delta for X coordinate is moved
  /// \param theDeltaY the delta for Y coordinate is moved
  SKETCHPLUGIN_EXPORT virtual void move(const double theDeltaX, const double theDeltaY);

  /// Called on change of any argument-attribute of this object
  SKETCHPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// Returns the AIS preview
  virtual AISObjectPtr getAISObject(AISObjectPtr thePrevious);

  /// Use plugin manager for features creation
  SketchPlugin_Circle();

protected:
  /// \brief Initializes attributes of derived class.
  virtual void initDerivedClassAttributes();

private:
  /// Returns true if all obligatory attributes are initialized
  bool isFeatureValid();

  /// Update coordinates of representation by three points
  void adjustThreePoints();
};

#endif
