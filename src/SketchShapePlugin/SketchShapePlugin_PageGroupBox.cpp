// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchShapePlugin_PageGroupBox.h
// Created:     25 Nov 2015
// Author:      Natalia ERMOLAEVA

#include <SketchShapePlugin_PageGroupBox.h>

#include <QListWidget>
#include <QList>

SketchShapePlugin_PageGroupBox::SketchShapePlugin_PageGroupBox(QWidget* theParent)
: ModuleBase_PageGroupBox(theParent)
{
}

void SketchShapePlugin_PageGroupBox::setHighlightedGroupBox(bool isHighlighted)
{
  QList<QListWidget*> aListWidgets = findChildren<QListWidget*>();

  QList<QListWidget*>::const_iterator anIt = aListWidgets.begin(), aLast = aListWidgets.end();
  for (; anIt != aLast; anIt++) {
    QListWidget* aListWidget = *anIt;
    ModuleBase_Tools::setShadowEffect(aListWidget, isHighlighted);
  }
}

