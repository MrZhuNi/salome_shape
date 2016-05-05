// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_WidgetAction.cpp
// Created:     15 Apr 2016
// Author:      Natalia Ermolaeva

#include <ModuleBase_WidgetAction.h>
#include <ModuleBase_Tools.h>
#include <ModuleBase_IconFactory.h>

#include <Config_WidgetAPI.h>

#include <QWidget>
#include <QHBoxLayout>
#include <QToolButton>

#define DEFAULT_TOOL_TIP_WIDTH 300

ModuleBase_WidgetAction::ModuleBase_WidgetAction(QWidget* theParent,
                                                 const Config_WidgetAPI* theData)
: ModuleBase_ModelWidget(theParent, theData)
{
  QHBoxLayout* aControlLay = new QHBoxLayout(this);
  ModuleBase_Tools::adjustMargins(aControlLay);

  myActionID = attributeID();
  setAttributeID("");

  myButton = new QToolButton(this);
  QString aText = QString::fromStdString(theData->widgetLabel());
  QString aToolTip = ModuleBase_Tools::wrapTextByWords(
              QString::fromStdString(theData->widgetTooltip()), myButton, DEFAULT_TOOL_TIP_WIDTH);

  myButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  aControlLay->addWidget(myButton);

  QString aLabelIcon = QString::fromStdString(theData->widgetIcon());
  if (!aLabelIcon.isEmpty())
    myButton->setIcon(ModuleBase_IconFactory::loadPixmap(aLabelIcon));
  else
    myButton->setText(aText);
  myButton->setToolTip(aToolTip);

  connect(myButton, SIGNAL(clicked(bool)), this, SLOT(onActionClicked()));
}

ModuleBase_WidgetAction::~ModuleBase_WidgetAction()
{
}

bool ModuleBase_WidgetAction::focusTo()
{
  return false;
}

QList<QWidget*> ModuleBase_WidgetAction::getControls() const
{
  QList<QWidget*> aList;
  aList.append(myButton);
  return aList;
}

bool ModuleBase_WidgetAction::storeValueCustom()
{
  return true;
}

bool ModuleBase_WidgetAction::restoreValueCustom()
{
  return true;
}

void ModuleBase_WidgetAction::onActionClicked()
{
  if (myFeature->customAction(myActionID))
    updateObject(myFeature);
}
