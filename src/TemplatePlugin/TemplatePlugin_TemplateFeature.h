// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        TemplatePlugin_TemplateFeature.h
// Created:     30 May 2014
// Author:      Vitaly SMETANNIKOV

#ifndef TemplatePlugin_TemplateFeature_H_
#define TemplatePlugin_TemplateFeature_H_

#include "TemplatePlugin.h"
#include <ModelAPI_Feature.h>
#include <ModelAPI_ResultBody.h>
#include <GeomAlgoAPI_Extrusion.h>
#include <GeomAPI_Shape.h>


class TemplatePlugin_TemplateFeature : public ModelAPI_Feature
{
 public:
  /// Extrusion kind
  inline static const std::string& ID()
  {
    static const std::string MY_ID("TemplateFeature");
    return MY_ID;
  }
  /// attribute name of referenced face
  inline static const std::string& FACE_ID()
  {
    static const std::string MY_FACE_ID("extrusion_face");
    return MY_FACE_ID;
  }
  /// attribute name of extrusion size
  inline static const std::string& SIZE_ID()
  {
    static const std::string MY_SIZE_ID("extrusion_size");
    return MY_SIZE_ID;
  }
  /// attribute name of reverse direction
  inline static const std::string& REVERSE_ID()
  {
    static const std::string MY_REVERSE_ID("extrusion_reverse");
    return MY_REVERSE_ID;
  }

  /// Returns the kind of a feature
  TEMPLATEPLUGIN_EXPORT virtual const std::string& getKind()
  {
    return myId;
  }

  /// Creates a new part document if needed
  TEMPLATEPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  TEMPLATEPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation
  TemplatePlugin_TemplateFeature(const std::string& theId);
private:
  /// Load Naming data structure of the feature to the document
  void LoadNamingDS(GeomAlgoAPI_Extrusion& theFeature, std::shared_ptr<ModelAPI_ResultBody> theResultBody,
	                std::shared_ptr<GeomAPI_Shape> theBasis,
	                std::shared_ptr<GeomAPI_Shape> theContext);

  std::string myId;
};

#endif
