// File:        ModuleBase_Widgets.h
// Created:     04 June 2014
// Author:      Vitaly Smetannikov

#include <ModuleBase_WidgetDoubleValue.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Data.h>

#include <Config_Keywords.h>
#include <Config_WidgetAPI.h>

#include <Events_Loop.h>
#include <Model_Events.h>

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QDoubleSpinBox>


ModuleBase_WidgetDoubleValue::ModuleBase_WidgetDoubleValue(QWidget* theParent, const Config_WidgetAPI* theData)
  : ModuleBase_ModelWidget(theParent, theData)
{
  myContainer = new QWidget(theParent);
  QHBoxLayout* aControlLay = new QHBoxLayout(myContainer);
  aControlLay->setContentsMargins(0, 0, 0, 0);

  QString aLabelText = QString::fromStdString(theData->widgetLabel());
  QString aLabelIcon = QString::fromStdString(theData->widgetIcon());
  myLabel = new QLabel(aLabelText, myContainer);
  myLabel->setPixmap(QPixmap(aLabelIcon));
  aControlLay->addWidget(myLabel);

  mySpinBox = new QDoubleSpinBox(myContainer);
  QString anObjName = QString::fromStdString(attributeID());
  mySpinBox->setObjectName(anObjName);

  bool isOk = false;
  std::string aProp = theData->getProperty(DOUBLE_WDG_MIN);
  double aMinVal = QString::fromStdString(aProp).toDouble(&isOk);
  if (isOk) {
    mySpinBox->setMinimum(aMinVal);
  } else {
    mySpinBox->setMinimum(-DBL_MAX);
  }

  aProp = theData->getProperty(DOUBLE_WDG_MAX);
  double aMaxVal = QString::fromStdString(aProp).toDouble(&isOk);
  if (isOk) {
    mySpinBox->setMaximum(aMaxVal);
  } else {
    mySpinBox->setMaximum(DBL_MAX);
  }

  aProp = theData->getProperty(DOUBLE_WDG_STEP);
  double aStepVal = QString::fromStdString(aProp).toDouble(&isOk);
  if (isOk) {
    mySpinBox->setSingleStep(aStepVal);
  }

  aProp = theData->getProperty(DOUBLE_WDG_DFLT);
  double aDefVal = QString::fromStdString(aProp).toDouble(&isOk);
  if (isOk) {
    mySpinBox->setValue(aDefVal);
  }

  QString aTTip = QString::fromStdString(theData->widgetTooltip());
  mySpinBox->setToolTip(aTTip);

  aControlLay->addWidget(mySpinBox);
  aControlLay->setStretch(1, 1);

  connect(mySpinBox, SIGNAL(valueChanged(double)), this, SIGNAL(valuesChanged()));
}

ModuleBase_WidgetDoubleValue::~ModuleBase_WidgetDoubleValue()
{
}

bool ModuleBase_WidgetDoubleValue::storeValue(FeaturePtr theFeature) const
{
  DataPtr aData = theFeature->data();
  boost::shared_ptr<ModelAPI_AttributeDouble> aReal = aData->real(attributeID());
  if (aReal->value() != mySpinBox->value()) {
    aReal->setValue(mySpinBox->value());
    Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_FEATURE_UPDATED));
  }
  return true;
}

bool ModuleBase_WidgetDoubleValue::restoreValue(FeaturePtr theFeature)
{
  DataPtr aData = theFeature->data();
  boost::shared_ptr<ModelAPI_AttributeDouble> aRef = aData->real(attributeID());

  bool isBlocked = mySpinBox->blockSignals(true);
  mySpinBox->setValue(aRef->value());
  mySpinBox->blockSignals(isBlocked);

  return true;
}

QList<QWidget*> ModuleBase_WidgetDoubleValue::getControls() const
{
  QList<QWidget*> aList;
  aList.append(myLabel);
  aList.append(mySpinBox);
  return aList;
}
