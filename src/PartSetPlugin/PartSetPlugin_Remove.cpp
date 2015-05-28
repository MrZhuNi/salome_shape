// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSetPlugin_Remove.cxx
// Created:     20 May 2014
// Author:      Mikhail PONIKAROV

#include "PartSetPlugin_Remove.h"
#include "PartSetPlugin_Part.h"
#include <ModelAPI_Document.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeDocRef.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Feature.h>

void PartSetPlugin_Remove::execute()
{
  std::shared_ptr<ModelAPI_Session> aPManager = ModelAPI_Session::get();
  std::shared_ptr<ModelAPI_Document> aRoot = aPManager->moduleDocument();
  std::shared_ptr<PartSetPlugin_Part> a;
  for (int a = aRoot->size(ModelAPI_ResultPart::group()) - 1; a >= 0; a--) {
    ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(
        aRoot->object(ModelAPI_ResultPart::group(), a));
    if (aPart && aPart->data()->document(ModelAPI_ResultPart::DOC_REF())->value() == document()) {
      FeaturePtr aFeature = aRoot->feature(aPart);
      if (aFeature) {
        // do remove
        aPart->data()->document(ModelAPI_ResultPart::DOC_REF())->value()->close();
        std::set<std::shared_ptr<ModelAPI_Feature> > aRefFeatures;
        aRoot->refsToFeature(aFeature, aRefFeatures);
        if (aRefFeatures.empty())
          aRoot->removeFeature(aFeature);
      }
    }
  }
}
