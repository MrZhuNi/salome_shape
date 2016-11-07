// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_WidgetEditor.h
// Created:     25 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef ModuleBase_WidgetEditor_H
#define ModuleBase_WidgetEditor_H

#include <ModuleBase.h>
#include "ModuleBase_WidgetDoubleValue.h"

#include <QObject>
#include <QStringList>

class ModelAPI_Feature;
class QLineEdit;

/**\class ModuleBase_WidgetEditor
 * \ingroup GUI
 * \brief Custom widget. An abstract class to be redefined to fill with some GUI controls
 */
class MODULEBASE_EXPORT ModuleBase_WidgetEditor : public ModuleBase_WidgetDoubleValue
{
Q_OBJECT
 public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  ModuleBase_WidgetEditor(QWidget* theParent, const Config_WidgetAPI* theData);
  /// Constructor
  /// \param theParent the parent object
  /// \param theAttribute The attribute of the model widget
  ModuleBase_WidgetEditor(QWidget* theParent, const std::string& theAttribute);

  /// Destructor
  virtual ~ModuleBase_WidgetEditor();

  /// Set focus to the first control of the current widget. 
  /// The focus policy of the control is checked.
  /// If the widget has the NonFocus focus policy, it is skipped.
  /// \return the state whether the widget can accept the focus
  virtual bool focusTo();

  /// Shous popup window under cursor for data editing
  /// \param theSendSignals a flag whether the signals should be sent or the value
  /// is to be applyed directly
  /// \return true if the editor value is accepted
  bool showPopupEditor(const bool theSendSignals = true);

  /// Set current cursor position
  /// \param theX the X coordinate
  /// \param theY the Y coordinate
  void setCursorPosition(const int theX, const int theY);

private:
  /// Show editor
  /// \param theOutValue a result value
  /// \param theOutText a result text
  /// \return true if the editor value is accepted
  bool editedValue(double& theOutValue, QString& theOutText);

 private:
   ///< the current widget feature
   FeaturePtr myFeature;  

   ///< the kinds of possible features
   QStringList myFeatureKinds;  

   int myXPosition, myYPosition;
};

#endif
