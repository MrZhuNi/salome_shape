// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        PrimitivesPlugin_Plugin.hxx
// Created:     07 July 2014
// Author:      Vitaly SMETANNIKOV

#ifndef PrimitivesPlugin_Plugin_H_
#define PrimitivesPlugin_Plugin_H_

#include "PrimitivesPlugin.h"
#include <ModelAPI_Plugin.h>
#include <ModelAPI_Feature.h>

/**\class PrimitivesPlugin_Plugin
 * \ingroup Plugins
 * \brief The main class managing Primitive Features as plugins.
 */
class PRIMITIVESPLUGIN_EXPORT PrimitivesPlugin_Plugin : public ModelAPI_Plugin
{
 public:
  /// Creates the feature object of this plugin by the feature string ID
  virtual FeaturePtr createFeature(std::string theFeatureID);

 public:
  /// Default constructor
  PrimitivesPlugin_Plugin();
};

#endif
