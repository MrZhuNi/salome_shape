// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef FeaturesPlugin_WidgetFilletMultiRadiuses_H
#define FeaturesPlugin_WidgetFilletMultiRadiuses_H

#include "FeaturesPlugin.h"

#include <ModuleBase_WidgetSelector.h>
#include <ModuleBase_ViewerPrs.h>
#include <ModelAPI_AttributeTables.h>

#include <QList>
#include <QStringList>
#include <QStyledItemDelegate>

class QWidget;
class QLabel;
class QTableWidget;
class QPushButton;
class QTableWidgetItem;
class QLineEdit;

class DataArrayItemDelegate : public QStyledItemDelegate
{
 Q_OBJECT
public:
  DataArrayItemDelegate(bool theTypeMethod);

  virtual QWidget* createEditor(QWidget* theParent,
                                const QStyleOptionViewItem & theOption,
                                const QModelIndex& theIndex) const;

private slots:
  void onEditItem(const QString& theText);

private:
  bool myTypeMethodPoint;
};


/*!
 * \ingroup GUI
 * Represent a content of the property panel to show/modify parameters of a Field feature.
 */
class FeaturesPlugin_WidgetFilletMultiRadiuses : public ModuleBase_WidgetSelector
{
 Q_OBJECT
public:
  FeaturesPlugin_WidgetFilletMultiRadiuses(QWidget* theParent,
                                           ModuleBase_IWorkshop* theWorkshop,
                                           const Config_WidgetAPI* theData,
                                           bool TypeMethodBypoint);

  virtual ~FeaturesPlugin_WidgetFilletMultiRadiuses() {}

  /// Returns true if the event is processed.
  virtual bool processEnter();

  /// The methiod called when widget is deactivated
  virtual void deactivate();

  /// Checks the widget validity. By default, it returns true.
  /// \param thePrs a selected presentation in the view
  /// \return a boolean value
  virtual bool isValidSelectionCustom(const std::shared_ptr<ModuleBase_ViewerPrs>& theValue);

  /// Returns values which should be highlighted when the whidget is active
  /// \param theValues a list of presentations
  virtual void getHighlighted(QList<std::shared_ptr<ModuleBase_ViewerPrs>>& theValues);

protected:
  /// The methiod called when widget is activated
  virtual void activateCustom();


  /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValueCustom();

  /// Restore value from attribute data to the widget's control
  virtual bool restoreValueCustom();

  /// Redefinition of virtual function
  /// \param theObject an object for the event
  /// \param theEvent an event
  virtual bool eventFilter(QObject* theObject, QEvent* theEvent);

  //virtual void showEvent(QShowEvent* theEvent);

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

  /// Set the given wrapped value to the current widget
  /// This value should be processed in the widget according to the needs
  /// \param theValues the wrapped selection values
  /// \param theToValidate a validation of the values flag
  virtual bool setSelection(QList<std::shared_ptr<ModuleBase_ViewerPrs>>& theValues,
                            const bool theToValidate);

    /// Return the attribute values wrapped in a list of viewer presentations
  /// \return a list of viewer presentations, which contains an attribute result and
  /// a shape. If the attribute do not uses the shape, it is empty
  virtual QList<std::shared_ptr<ModuleBase_ViewerPrs>> getAttributeSelection() const;

   /// Returns attribute indices selected in the widget selection list
  /// \param theIndices a list of indices
  void getSelectedAttributeIndices(std::set<int>& theIndices);

  /// Gets the feature attribute and fill a list of viewer presentation for the attribute
  /// indices. If the the container of indices is empty, it returns all objects.
  /// \param theAttributeIds indices in attribute list to be returned
  /// \param theValues the result presentations, filled with object and shape of an attribute item
  void convertIndicesToViewerSelection(std::set<int> theAttributeIds,
                            QList<std::shared_ptr<ModuleBase_ViewerPrs>>& theValues) const;
private slots:

  /// Slot called on add a row
  void onAdd();

  /// Slot called on remove a row
  void onRemove();

  /// Retunrs a list of possible shape types
  /// \return a list of shapes
  virtual QIntList shapeTypes() const;

  /// Slot called on editing of a table cell
  /// \param theRow a row of the cell
  /// \param theCol a column of the cell
  void onTableEdited(int theRow, int theCol);

  /// Slot is called on selection of list of selected items
  void onListSelection();

private:

  /// Return a value from the string
  /// \param theStrVal a string
  ModelAPI_AttributeTables::Value getValue(QString theStrVal) const;

  /// Return a QString from the ModelAPI_AttributeTables
  /// \param theStrVal a string
  QString getValueText(ModelAPI_AttributeTables::Value& theVal) const;

  /// Return a radii value from the name
  /// \param theStrVal a string
  QString findRadius(QString thename) const;

  /// tables
  QTableWidget* myDataTbl;

  /// Remove button
  QPushButton* myRemoveBtn;

  /// Editor for table header
  QLineEdit* myHeaderEditor;

  // Choice of the method
  bool myTypeMethodBypoint;

  //Sort the selection
  bool mySetSelection;

  bool mySortList;

  DataArrayItemDelegate* myDelegate;

  // Label for first and last row
  std::vector<QString> myfirstRowValue;
  std::vector<QString> myLastRowValue;
  // map to sort
  std::map<double,std::pair<QString,QString>> myValuesSort;

};

#endif