// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        TemplatePlugin_Plugin.hxx
// Created:     07 July 2014
// Author:      Vitaly SMETANNIKOV

#ifndef TemplatePlugin_Plugin_H_
#define TemplatePlugin_Plugin_H_

#include "TemplatePlugin.h"
#include <ModelAPI_Plugin.h>
#include <ModelAPI_Feature.h>

class TEMPLATEPLUGIN_EXPORT TemplatePlugin_Plugin : public ModelAPI_Plugin
{
 public:
  /// Creates the feature object of this plugin by the feature string ID
  virtual FeaturePtr createFeature(std::string theFeatureID);

 public:
  /// Is needed for python wrapping by swig
  TemplatePlugin_Plugin();
};

#endif
