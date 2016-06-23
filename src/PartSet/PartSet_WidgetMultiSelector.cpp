// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_WidgetMultiSelector.cpp
// Created:     15 Apr 2015
// Author:      Natalia Ermolaeva

#include "PartSet_WidgetMultiSelector.h"

#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <ModuleBase_Definitions.h>
#include <ModuleBase_Tools.h>
#include <ModuleBase_IWorkshop.h>
#include <ModuleBase_ISelection.h>
#include <ModuleBase_ViewerPrs.h>

#include <Config_WidgetAPI.h>

#include <PartSet_Tools.h>
#include <PartSet_ExternalObjectsMgr.h>
#include <SketchPlugin_Feature.h>

#include <SketchPlugin_ConstraintRigid.h>

#include <XGUI_Workshop.h>

#include <QComboBox>

PartSet_WidgetMultiSelector::PartSet_WidgetMultiSelector(QWidget* theParent,
                                                         ModuleBase_IWorkshop* theWorkshop,
                                                         const Config_WidgetAPI* theData)
: ModuleBase_WidgetMultiSelector(theParent, theWorkshop, theData)
{
  myExternalObjectMgr = new PartSet_ExternalObjectsMgr(theData->getProperty("use_external"), false);
}

PartSet_WidgetMultiSelector::~PartSet_WidgetMultiSelector()
{
  delete myExternalObjectMgr;
}

//********************************************************************
bool PartSet_WidgetMultiSelector::isValidSelectionCustom(const ModuleBase_ViewerPrsPtr& thePrs)
{
  bool aValid = ModuleBase_WidgetMultiSelector::isValidSelectionCustom(thePrs);
  if (aValid) {
    ObjectPtr anObject = myWorkshop->selection()->getResult(thePrs);
    aValid = myExternalObjectMgr->isValidObject(anObject);
  }
  return aValid;
}

//********************************************************************
void PartSet_WidgetMultiSelector::restoreAttributeValue(const AttributePtr& theAttribute,
                                                        const bool theValid)
{
  ModuleBase_WidgetMultiSelector::restoreAttributeValue(theAttribute, theValid);

  myExternalObjectMgr->removeExternal(sketch(), myFeature, myWorkshop, true);
}

void PartSet_WidgetMultiSelector::getGeomSelection(const ModuleBase_ViewerPrsPtr& thePrs,
                                                   ObjectPtr& theObject,
                                                   GeomShapePtr& theShape)
{
  ModuleBase_WidgetMultiSelector::getGeomSelection(thePrs, theObject, theShape);

  FeaturePtr aSelectedFeature = ModelAPI_Feature::feature(theObject);
  std::shared_ptr<SketchPlugin_Feature> aSPFeature = 
          std::dynamic_pointer_cast<SketchPlugin_Feature>(aSelectedFeature);
  // there is no a sketch feature is selected, but the shape exists, try to create an exernal object
  // TODO: unite with the same functionality in PartSet_WidgetShapeSelector
  if (aSPFeature.get() == NULL)
    theObject = NULL;
  if (myExternalObjectMgr->useExternal()) {
    GeomShapePtr aShape = theShape;
    if (!aShape.get()) {
      ResultPtr aResult = myWorkshop->selection()->getResult(thePrs);
      if (aResult.get())
        aShape = aResult->shape();
    }
    if (aShape.get() != NULL && !aShape->isNull())
      theObject = myExternalObjectMgr->externalObject(theObject, aShape, sketch(), myIsInValidate);
  }
}
