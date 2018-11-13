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

#include <QWidget>

class QLabel;
class QComboBox;
class QGroupBox;
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


class ModuleBase_WidgetSelectionFilter : public ModuleBase_ModelWidget
{
  Q_OBJECT
public:
  ModuleBase_WidgetSelectionFilter(QWidget* theParent, ModuleBase_IWorkshop* theWorkshop,
    const Config_WidgetAPI* theData);

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
  void onAddFilter();

private:
  ModuleBase_IWorkshop* myWorkshop;

  QComboBox* myFiltersCombo;
  QGroupBox* myFiltersGroup;

  int mySelectionType;
};

#endif