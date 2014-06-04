// File:    SketchPlugin_ConstraintCoincidence.h
// Created: 08 May 2014
// Author:  Artem ZHIDKOV

#ifndef SketchPlugin_ConstraintCoincidence_HeaderFile
#define SketchPlugin_ConstraintCoincidence_HeaderFile

#include "SketchPlugin.h"
#include "SketchPlugin_Constraint.h"
#include <SketchPlugin_Sketch.h>
#include <list>

/// Coincidence constraint kind
const std::string SKETCH_CONSTRAINT_COINCIDENCE_KIND("SketchConstraintCoincidence");

/** \class SketchPlugin_ConstraintCoincidence
 *  \ingroup DataModel
 *  \brief Feature for creation of a new constraint which defines equivalence of two points
 *
 *  These constraint has two attributes:
 *  CONSTRAINT_ATTR_ENTITY_A and CONSTRAINT_ATTR_ENTITY_B
 */
class SketchPlugin_ConstraintCoincidence: public SketchPlugin_Constraint
{
public:
  /// \brief Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind() 
  {static std::string MY_KIND = SKETCH_CONSTRAINT_COINCIDENCE_KIND; return MY_KIND;}

  /// \brief Returns to which group in the document must be added feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getGroup() 
  {static std::string MY_GROUP = SKETCH_KIND; return MY_GROUP;}

  /// \brief Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// \brief Request for initialization of data model of the feature: adding all attributes
  SKETCHPLUGIN_EXPORT virtual void initAttributes();

  /// \brief Returns the sketch preview
  SKETCHPLUGIN_EXPORT virtual const boost::shared_ptr<GeomAPI_Shape>& preview();

  /// \brief Use plugin manager for features creation
  SketchPlugin_ConstraintCoincidence();
};

#endif
