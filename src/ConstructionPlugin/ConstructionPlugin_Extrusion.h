// File:        ConstructionPlugin_Extrusion.h
// Created:     30 May 2014
// Author:      Vitaly SMETANNIKOV

#ifndef ConstructionPlugin_Extrusion_HeaderFile
#define ConstructionPlugin_Extrusion_HeaderFile

#include "ConstructionPlugin.h"
#include <ModelAPI_Feature.h>

/// Extrusion kind
const std::string CONSTRUCTION_EXTRUSION_KIND("Extrusion");

/// attribute name of referenced face
const std::string EXTRUSION_FACE = "extrusion_face";

/// attribute name of extrusion size
const std::string EXTRUSION_SIZE = "extrusion_size";

/// attribute name of reverce direction
const std::string EXTRUSION_REVERSE = "extrusion_reverse";


class ConstructionPlugin_Extrusion: public ModelAPI_Feature
{
public:
  /// Returns the kind of a feature
  CONSTRUCTIONPLUGIN_EXPORT virtual const std::string& getKind() 
  { static std::string MY_KIND = CONSTRUCTION_EXTRUSION_KIND; return MY_KIND; }

  /// Returns to which group in the document must be added feature
  CONSTRUCTIONPLUGIN_EXPORT virtual const std::string& getGroup() 
  { static std::string MY_GROUP = "Construction";  return MY_GROUP; }

  /// Creates a new part document if needed
  CONSTRUCTIONPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  CONSTRUCTIONPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation
  ConstructionPlugin_Extrusion();
};

#endif
