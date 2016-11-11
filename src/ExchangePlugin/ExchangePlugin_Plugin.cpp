// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    ExchangePlugin_Plugin.cpp
// Created: Aug 28, 2014
// Author:  Sergey BELASH

#include <ExchangePlugin_Plugin.h>
#include <ExchangePlugin_Dump.h>
#include <ExchangePlugin_ImportFeature.h>
#include <ExchangePlugin_ExportFeature.h>
#include <ExchangePlugin_Validators.h>

#include <Config_PropManager.h>

#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <memory>

// the only created instance of this plugin
static ExchangePlugin_Plugin* MY_EXCHANGE_INSTANCE = new ExchangePlugin_Plugin();

ExchangePlugin_Plugin::ExchangePlugin_Plugin()
{
  // register this plugin
  SessionPtr aSession = ModelAPI_Session::get();
  aSession->registerPlugin(this);
  ModelAPI_ValidatorsFactory* aFactory = aSession->validators();
  aFactory->registerValidator("ExchangePlugin_ImportFormat",
                              new ExchangePlugin_ImportFormatValidator);
  aFactory->registerValidator("ExchangePlugin_ExportFormat",
                              new ExchangePlugin_ExportFormatValidator);
}

FeaturePtr ExchangePlugin_Plugin::createFeature(std::string theFeatureID)
{
  if (theFeatureID == ExchangePlugin_ImportFeature::ID()) {
    return FeaturePtr(new ExchangePlugin_ImportFeature);
  } else
  if (theFeatureID == ExchangePlugin_ExportFeature::ID()) {
    return FeaturePtr(new ExchangePlugin_ExportFeature);
  } else
  if (theFeatureID == ExchangePlugin_Dump::ID()) {
    return FeaturePtr(new ExchangePlugin_Dump);
  }
  // feature of such kind is not found
  return FeaturePtr();
}
