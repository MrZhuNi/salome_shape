// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

#include <TemplatePlugin_Plugin.h>
#include <TemplatePlugin_TemplateFeature.h>
#include <ModelAPI_Session.h>

#include <string>
#include <memory>

// the only created instance of this plugin
static TemplatePlugin_Plugin* MY_FEATURES_INSTANCE = new TemplatePlugin_Plugin();

TemplatePlugin_Plugin::TemplatePlugin_Plugin()
{
  // register this plugin
  ModelAPI_Session::get()->registerPlugin(this);
}

FeaturePtr TemplatePlugin_Plugin::createFeature(std::string theFeatureID)
{
  return FeaturePtr(new TemplatePlugin_TemplateFeature(theFeatureID));
}
