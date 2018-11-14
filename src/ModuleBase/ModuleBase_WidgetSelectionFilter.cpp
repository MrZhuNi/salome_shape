// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include "ModuleBase_WidgetSelectionFilter.h"
#include "ModuleBase_Tools.h"
#include "ModuleBase_IWorkshop.h"
#include "ModuleBase_IModule.h"
#include "ModuleBase_IPropertyPanel.h"
#include "ModuleBase_PageWidget.h"
#include "ModuleBase_WidgetSelector.h"

#include <ModelAPI_Session.h>

#include <QLayout>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QGroupBox>
#include <QDialog>
#include <QToolButton>

static int SelectionType = 0;

ModuleBase_FilterStarter::ModuleBase_FilterStarter(const std::string& theFeature,
  QWidget* theParent, ModuleBase_IWorkshop* theWorkshop)
  : QWidget(theParent),
  myFeatureName(theFeature),
  myWorkshop(theWorkshop)
{
  QHBoxLayout* aMainLayout = new QHBoxLayout(this);
  ModuleBase_Tools::adjustMargins(aMainLayout);

  aMainLayout->addStretch(1);
  QPushButton* aLaunchBtn = new QPushButton(tr("Selection by filters"), this);
  connect(aLaunchBtn, SIGNAL(clicked()), SLOT(onFiltersLaunch()));
  aMainLayout->addWidget(aLaunchBtn);

  myFilterLbl = new QLabel(this);
  myFilterLbl->setPixmap(QPixmap(":pictures/filter.png"));
  aMainLayout->addWidget(myFilterLbl);

  myModifyLbl = new QLabel(this);
  myModifyLbl->setPixmap(QPixmap(":pictures/plus_minus.png"));
  aMainLayout->addWidget(myModifyLbl);
  aMainLayout->addStretch(1);

  myFilterLbl->hide();
  myModifyLbl->hide();
}

void ModuleBase_FilterStarter::onFiltersLaunch()
{
  SelectionType = myShapeType;
  ModuleBase_WidgetSelector* aSelector = dynamic_cast<ModuleBase_WidgetSelector*>(parent());
  aSelector->storeValue(); // Store values defined by user
  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
    (myWorkshop->module()->createOperation(myFeatureName));
  myWorkshop->processLaunchOperation(aFOperation);
}

void ModuleBase_FilterStarter::setSelectionType(const QString& theType)
{
  QString aType = theType.toUpper();
  if ((aType == "VERTEX") || (aType == "VERTICES"))
    myShapeType = GeomAPI_Shape::VERTEX;
  else if ((aType == "EDGE") || (aType == "EDGES"))
    myShapeType = GeomAPI_Shape::EDGE;
  else if ((aType == "WIRE") || (aType == "WIRES"))
    myShapeType = GeomAPI_Shape::WIRE;
  else if ((aType == "FACE") || (aType == "FACES"))
    myShapeType = GeomAPI_Shape::FACE;
  else if ((aType == "SHELL") || (aType == "SHELLS"))
    myShapeType = GeomAPI_Shape::SHELL;
  else if ((aType == "SOLID") || (aType == "SOLIDS"))
    myShapeType = GeomAPI_Shape::SOLID;
  else if ((aType == "COMPSOLID") || (aType == "COMPSOLIDS"))
    myShapeType = GeomAPI_Shape::COMPSOLID;
  else if ((aType == "COMPOUND") || (aType == "COMPOUNDS"))
    myShapeType = GeomAPI_Shape::COMPOUND;
  else
    myShapeType = GeomAPI_Shape::SHAPE;
}

//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
ModuleBase_FilterItem::ModuleBase_FilterItem(const FilterPtr& theFilter, QWidget* theParent)
  : QWidget(theParent), myFilter(theFilter)
{
  QHBoxLayout* aLayout = new QHBoxLayout(this);
  aLayout->setContentsMargins(0, 0, 0, 0);

  // Reverse filter button
  myRevBtn = new QToolButton(this);
  myRevBtn->setCheckable(true);
  myRevBtn->setChecked(false);
  myRevBtn->setAutoRaise(true);
  myRevBtn->setIcon(QIcon(":pictures/accept.png"));
  connect(myRevBtn, SIGNAL(toggled(bool)), SLOT(onReverse(bool)));
  aLayout->addWidget(myRevBtn);

  aLayout->addWidget(new QLabel(myFilter->name().c_str(), this), 1);

  QToolButton* aDelBtn = new QToolButton(this);
  aDelBtn->setIcon(QIcon(":pictures/button_cancel.png"));
  aDelBtn->setAutoRaise(true);
  connect(aDelBtn, SIGNAL(clicked(bool)), SLOT(onDelete()));
  aLayout->addWidget(aDelBtn);
}


void ModuleBase_FilterItem::onReverse(bool theCheck)
{
  if (theCheck)
    myRevBtn->setIcon(QIcon(":pictures/stop.png"));
  else
    myRevBtn->setIcon(QIcon(":pictures/accept.png"));
}

void ModuleBase_FilterItem::onDelete()
{
  emit deleteItem(this);
}


//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
ModuleBase_WidgetSelectionFilter::ModuleBase_WidgetSelectionFilter(QWidget* theParent,
  ModuleBase_IWorkshop* theWorkshop, const Config_WidgetAPI* theData)
  : ModuleBase_ModelWidget(theParent, theData),
  myWorkshop(theWorkshop), mySelectionType(SelectionType)
{
  QVBoxLayout* aMainLayout = new QVBoxLayout(this);
  ModuleBase_Tools::adjustMargins(aMainLayout);

  myFiltersGroup = new QGroupBox(tr("Dynamic Filters"), this);
  myGroupLayout = new QVBoxLayout(myFiltersGroup);
  myGroupLayout->setContentsMargins(0, 0, 0, 0);
  myGroupLayout->setSpacing(0);

  QWidget* aFiltersWgt = new QWidget(myFiltersGroup);
  QHBoxLayout* aFiltersLay = new QHBoxLayout(aFiltersWgt);
  ModuleBase_Tools::adjustMargins(aFiltersLay);

  QLabel* aFilterLbl = new QLabel(aFiltersWgt);
  aFilterLbl->setPixmap(QPixmap(":pictures/filter.png"));

  myFiltersCombo = new QComboBox(aFiltersWgt);
  SessionPtr aSession = ModelAPI_Session::get();
  myFilters =
    aSession->filters()->filtersForShapeType((GeomAPI_Shape::ShapeType) mySelectionType);
  QStringList aItems;
  std::list<FilterPtr>::const_iterator aIt;
  for (aIt = myFilters.cbegin(); aIt != myFilters.cend(); aIt++)
    aItems.push_back((*aIt)->name().c_str());
  myFiltersCombo->addItems(aItems);

  QToolButton* aAddBtn = new QToolButton(aFiltersWgt);
  aAddBtn->setIcon(QIcon(":pictures/add.png"));
  aAddBtn->setAutoRaise(true);
  aAddBtn->setToolTip(tr("Add current filter"));
  connect(aAddBtn, SIGNAL(clicked()), SLOT(onAddItem()));

  aFiltersLay->addWidget(aFilterLbl);
  aFiltersLay->addWidget(myFiltersCombo, 1);
  aFiltersLay->addWidget(aAddBtn);

  myGroupLayout->addWidget(aFiltersWgt);

  aMainLayout->addWidget(myFiltersGroup);

  QWidget* aBtnWgt = new QWidget(this);
  QHBoxLayout* aBtnLayout = new QHBoxLayout(aBtnWgt);
  ModuleBase_Tools::adjustMargins(aBtnLayout);

  aBtnLayout->addStretch(1);

  QPushButton* aSelectBtn = new QPushButton(tr("Select"), aBtnWgt);
  connect(aSelectBtn, SIGNAL(clicked()), SLOT(onSelect()));
  aBtnLayout->addWidget(aSelectBtn);

  aMainLayout->addWidget(aBtnWgt);

  aMainLayout->addStretch(1);
}

void ModuleBase_WidgetSelectionFilter::onAddItem()
{
  int aId = myFiltersCombo->currentIndex();
  myFiltersCombo->removeItem(aId);

  std::list<FilterPtr>::iterator aIt;
  int i;
  FilterPtr aFilter;
  for (aIt = myFilters.begin(), i = 0; aIt != myFilters.cend(); i++, aIt++) {
    if (i == aId) {
      aFilter = (*aIt);
      myFilters.erase(aIt);
      break;
    }
  }
  if (aFilter.get()) {
    myUseFilters.push_back(aFilter);
    ModuleBase_FilterItem* aItem = new ModuleBase_FilterItem(aFilter, myFiltersGroup);
    connect(aItem, SIGNAL(deleteItem(ModuleBase_FilterItem*)),
      SLOT(onDeleteItem(ModuleBase_FilterItem*)));
    myGroupLayout->addWidget(aItem);
  }
}

void ModuleBase_WidgetSelectionFilter::onDeleteItem(ModuleBase_FilterItem* theItem)
{
  FilterPtr aFilter = theItem->filter();
  myGroupLayout->removeWidget(theItem);
  theItem->deleteLater();

  myUseFilters.remove(aFilter);
  myFilters.push_back(aFilter);
  myFiltersCombo->addItem(aFilter->name().c_str());
}

void ModuleBase_WidgetSelectionFilter::onSelect()
{

}

QList<QWidget*> ModuleBase_WidgetSelectionFilter::getControls() const
{
  return QList<QWidget*>();
}
