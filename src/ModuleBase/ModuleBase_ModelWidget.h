// File:        ModuleBase_ModelWidget.h
// Created:     25 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef ModuleBase_ModelWidget_H
#define ModuleBase_ModelWidget_H

#include <ModuleBase.h>

#include <ModelAPI_Feature.h>

#include <QObject>

#include <boost/shared_ptr.hpp>

class Config_WidgetAPI;
class ModelAPI_Feature;
class QKeyEvent;

/**\class ModuleBase_ModelWidget
 * \brief An abstract custom widget class. This class realization is assumed to create some controls.
 * The controls values modification should send signal about values change.
 *
 * Common interface for widgets in the property panel.
 * Every widget are able to save/restore data from the model and/or to contain other widgets.
 *
 */
class MODULEBASE_EXPORT ModuleBase_ModelWidget : public QObject
{
  Q_OBJECT
public:
  /// Constructor
  /// \theParent the parent object
  /// \theData the widget configuation. The attribute of the model widget is obtained from
  ModuleBase_ModelWidget(QObject* theParent, const Config_WidgetAPI* theData);
  /// Destructor
  virtual ~ModuleBase_ModelWidget() {};

  /// Saves the internal parameters to the given feature
  /// \param theFeature a model feature to be changed
  virtual bool storeValue(FeaturePtr theFeature) const = 0;

  virtual bool restoreValue(FeaturePtr theFeature) = 0;

  /// Returns whether the widget can accept focus, or if it corresponds to the given attribute
  /// \param theAttribute name
  bool canFocusTo(const std::string& theAttributeName) const;

  /// Set focus to the first control of the current widget. The focus policy of the control is checked.
  /// If the widget has the NonFocus focus policy, it is skipped.
  virtual void focusTo();

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const = 0;

  /// Returns whether the control has a default value
  /// \return a boolean value
  bool hasDefaultValue() const { return myHasDefaultValue; }

  /// Returns the attribute name
  /// \returns the string value
  std::string attributeID() const;

signals:
  /// The signal about widget values changed
  void valuesChanged();
  /// The signal about key release on the control, that corresponds to the attribute
  /// \param theAttributeName a name of the attribute
  /// \param theEvent key release event
  void keyReleased(const std::string& theAttributeName, QKeyEvent* theEvent);

protected:
  bool myHasDefaultValue; /// the boolean state whether the control has a default value

private:
  std::string myAttributeID; /// the attribute name of the model feature
};

#endif
