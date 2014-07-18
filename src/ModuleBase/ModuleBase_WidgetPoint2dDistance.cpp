// File:        ModuleBase_WidgetPoint2dDistance.h
// Created:     23 June 2014
// Author:      Vitaly Smetannikov

#include "ModuleBase_WidgetPoint2dDistance.h"
#include "ModuleBase_WidgetValueFeature.h"

#include <GeomAPI_Pnt2d.h>
#include <Config_WidgetAPI.h>
#include <GeomDataAPI_Point2D.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeDouble.h>

#include <QDoubleSpinBox>

ModuleBase_WidgetPoint2dDistance::ModuleBase_WidgetPoint2dDistance(QWidget* theParent, const Config_WidgetAPI* theData)
  : ModuleBase_WidgetDoubleValue(theParent, theData)
{
  myFirstPntName = theData->getProperty("first_point");
}

ModuleBase_WidgetPoint2dDistance::~ModuleBase_WidgetPoint2dDistance()
{
}

bool ModuleBase_WidgetPoint2dDistance::setValue(ModuleBase_WidgetValue* theValue)
{
  bool isDone = false;

  if (theValue) {
    ModuleBase_WidgetValueFeature* aFeatureValue = 
                         dynamic_cast<ModuleBase_WidgetValueFeature*>(theValue);
    if (aFeatureValue) {
      boost::shared_ptr<GeomAPI_Pnt2d> aPnt = aFeatureValue->point();
      ObjectPtr aObject = aFeatureValue->object();
      FeaturePtr aFeature = boost::dynamic_pointer_cast<ModelAPI_Feature>(aObject);
      if (aFeature && aPnt) {
        setPoint(aFeature, aPnt);
        isDone = true;
      }
    }
  }
  return isDone;
}

void ModuleBase_WidgetPoint2dDistance::setPoint(FeaturePtr theFeature, const boost::shared_ptr<GeomAPI_Pnt2d>& thePnt)
{
  boost::shared_ptr<ModelAPI_Data> aData = theFeature->data();
  boost::shared_ptr<GeomDataAPI_Point2D> aPoint = boost::dynamic_pointer_cast<GeomDataAPI_Point2D>
                                                              (aData->attribute(myFirstPntName));
  if (!aPoint) return;

  double aRadius = thePnt->distance(aPoint->pnt());
  AttributeDoublePtr aReal = aData->real(attributeID());
  if (aReal && (aReal->value() != aRadius)) {
    aReal->setValue(aRadius);
    mySpinBox->setValue(aRadius);
  }
}