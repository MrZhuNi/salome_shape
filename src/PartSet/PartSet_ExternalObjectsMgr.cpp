// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_ExternalObjectsMgr.cpp
// Created:     15 Apr 2015
// Author:      Natalia Ermolaeva

#include "PartSet_ExternalObjectsMgr.h"
#include "PartSet_Tools.h"

#include <XGUI_Workshop.h>

#include <QString>

PartSet_ExternalObjectsMgr::PartSet_ExternalObjectsMgr(const std::string& theExternal, const bool theDefaultValue)
: myUseExternal(theDefaultValue)
{
  QString aIsExternal(theExternal.c_str());
  if (!aIsExternal.isEmpty()) {
    QString aStr = aIsExternal.toUpper();
    myUseExternal = (aStr == "TRUE") || (aStr == "YES"); 
  }
}

ObjectPtr PartSet_ExternalObjectsMgr::externalObject(const ObjectPtr& theSelectedObject,
                                                     const GeomShapePtr& theShape,
                                                     const CompositeFeaturePtr& theSketch)
{
  ObjectPtr aSelectedObject = PartSet_Tools::findFixedObjectByExternal(theShape->impl<TopoDS_Shape>(),
                                                             theSelectedObject, theSketch);
  if (!aSelectedObject.get()) {
    // Processing of external (non-sketch) object
    aSelectedObject = PartSet_Tools::createFixedObjectByExternal(theShape->impl<TopoDS_Shape>(),
                                                                 theSelectedObject, theSketch);
    if (aSelectedObject.get())
      myExternalObjects.append(aSelectedObject);
  }
  return aSelectedObject;
}

//********************************************************************
void PartSet_ExternalObjectsMgr::removeExternal(const CompositeFeaturePtr& theSketch,
                                                const FeaturePtr& theFeature)
{
  QObjectPtrList::const_iterator anIt = myExternalObjects.begin(), aLast = myExternalObjects.end();
  for (; anIt != aLast; anIt++) {
    ObjectPtr anObject = *anIt;
    if (anObject.get()) {
      DocumentPtr aDoc = anObject->document();
      FeaturePtr aFeature = ModelAPI_Feature::feature(anObject);
      if (aFeature.get() != NULL) {
        QObjectPtrList anObjects;
        anObjects.append(aFeature);
        // the external feature should be removed with all references, sketch feature should be ignored
        std::set<FeaturePtr> anIgnoredFeatures;
        anIgnoredFeatures.insert(theSketch);
        // the current feature should be ignored, because it can use the external feature in the
        // attributes and, therefore have a references to it. So, the delete functionality tries
        // to delete this feature. Test case is creation of a constraint on external point,
        // use in this control after an external point, the point of the sketch.
        anIgnoredFeatures.insert(theFeature);
        XGUI_Workshop::deleteFeatures(anObjects, anIgnoredFeatures);
      }
    }
  }
  myExternalObjects.clear();
}
