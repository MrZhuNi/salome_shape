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

#ifndef ModuleBase_WidgetSelectionFilter_H
#define ModuleBase_WidgetSelectionFilter_H

#include "ModuleBase.h"
#include "ModuleBase_ModelWidget.h"

#include <ModelAPI_Filter.h>

#include <SelectMgr_IndexedMapOfOwner.hxx>
#include <AIS_Shape.hxx>
#include <AIS_ListOfInteractive.hxx>

#include <QWidget>

class QLabel;
class QComboBox;
class QGroupBox;
class QToolButton;
class QVBoxLayout;
class QPushButton;

class ModuleBase_IWorkshop;

class MODULEBASE_EXPORT ModuleBase_FilterStarter: public QWidget
{
  Q_OBJECT
public:
  ModuleBase_FilterStarter(const std::string& theFeature, QWidget* theParent,
    ModuleBase_IWorkshop* theWorkshop);

  ~ModuleBase_FilterStarter() {}

  void setSelectionType(const QString& theType);

private slots:
  void onFiltersLaunch();

private:
  std::string myFeatureName;
  ModuleBase_IWorkshop* myWorkshop;

  QLabel* myFilterLbl;
  QLabel* myModifyLbl;
  int myShapeType;
};


class ModuleBase_FilterItem : public QWidget
{
  Q_OBJECT
public:
  ModuleBase_FilterItem(const FilterPtr& theFilter, QWidget* theParent);

  FilterPtr filter() const { return myFilter; }

signals:
  void deleteItem(ModuleBase_FilterItem* theItem);

  private slots:
  void onReverse(bool theCheck);
  void onDelete();

private:
  FilterPtr myFilter;
  QToolButton* myRevBtn;
};

class ModuleBase_WidgetSelectionFilter : public ModuleBase_ModelWidget
{
  Q_OBJECT
public:
  ModuleBase_WidgetSelectionFilter(QWidget* theParent, ModuleBase_IWorkshop* theWorkshop,
    const Config_WidgetAPI* theData);

  ~ModuleBase_WidgetSelectionFilter();

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

protected:
  /// Saves the internal parameters to the given feature (not ussed for this widget)
  /// \return True in success
  virtual bool storeValueCustom() { return true;  }

  /// Restore value from attribute data to the widget's control (not ussed for this widget)
  virtual bool restoreValueCustom() { return true; }

private slots:
  void onAddItem();
  void onDeleteItem(ModuleBase_FilterItem* theItem);
  void onSelect();
  void onShowOnly(bool theErase);

private:
  void updateSelectBtn();
  void updateNumberSelected();
  void clearCurrentSelection(bool toUpdate = false);
  void updatePreview(const TopoDS_Shape& theShape);

private:
  ModuleBase_IWorkshop* myWorkshop;

  QComboBox* myFiltersCombo;
  QGroupBox* myFiltersGroup;
  QVBoxLayout* myGroupLayout;
  QPushButton* mySelectBtn;
  QLabel* myNbLbl;
  QPushButton* myShowBtn;

  int mySelectionType;
  std::list<FilterPtr> myFilters;
  std::list<FilterPtr> myUseFilters;

  Handle(SelectMgr_IndexedMapOfOwner) myOwners;
  Handle(AIS_Shape) myPreview;

  AIS_ListOfInteractive myListIO;
};


#endif