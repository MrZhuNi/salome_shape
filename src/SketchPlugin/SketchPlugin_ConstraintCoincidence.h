// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:    SketchPlugin_ConstraintCoincidence.h
// Created: 08 May 2014
// Author:  Artem ZHIDKOV

#ifndef SketchPlugin_ConstraintCoincidence_H_
#define SketchPlugin_ConstraintCoincidence_H_

#include "SketchPlugin.h"
#include "SketchPlugin_ConstraintBase.h"
#include <SketchPlugin_Sketch.h>
#include <list>

class GeomDataAPI_Point2D;

/** \class SketchPlugin_ConstraintCoincidence
 *  \ingroup Plugins
 *  \brief Feature for creation of a new constraint which defines equivalence of two points
 *
 *  This constraint has two attributes:
 *  SketchPlugin_Constraint::ENTITY_A() and SketchPlugin_Constraint::ENTITY_B()
 */
class SketchPlugin_ConstraintCoincidence : public SketchPlugin_ConstraintBase
{
 public:
  /// Coincidence constraint kind
  inline static const std::string& ID()
  {
    static const std::string MY_CONSTRAINT_COINCIDENCE_ID("SketchConstraintCoincidence");
    return MY_CONSTRAINT_COINCIDENCE_ID;
  }
  /// \brief Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = SketchPlugin_ConstraintCoincidence::ID();
    return MY_KIND;
  }

  /// Returns the AIS preview
  SKETCHPLUGIN_EXPORT virtual AISObjectPtr getAISObject(AISObjectPtr thePrevious);

  /// \brief Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// \brief Request for initialization of data model of the feature: adding all attributes
  SKETCHPLUGIN_EXPORT virtual void initAttributes();

  /// Returns coincident feature if there is a coincidence built on the given features
  /// \param theFeature1 the first feature
  /// \param theFeature2 the second feature
  static FeaturePtr findCoincidenceFeature(const FeaturePtr& theFeature1,
                                           const FeaturePtr& theFeature2);

  /// Returns point of coincidence feature
  /// \param theFeature a coincidence feature
  /// \return point 2d attribute. Coincidence always has at least one point 2d attribute
  static std::shared_ptr<GeomDataAPI_Point2D> getPoint(const FeaturePtr& theFeature);

  /// \brief Use plugin manager for features creation
  SketchPlugin_ConstraintCoincidence();
};

#endif
