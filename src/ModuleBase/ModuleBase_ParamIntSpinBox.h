
#ifndef ModuleBase_ParamIntSpinBox_H
#define ModuleBase_ParamIntSpinBox_H

#include "ModuleBase.h"

#include <ModuleBase_IntSpinBox.h>

#include <QValidator>

/**
* \ingroup GUI
* An extension of a double spin box which let to use parameters and expressions for value definition
*/
class MODULEBASE_EXPORT ModuleBase_ParamIntSpinBox : public ModuleBase_IntSpinBox
{
  Q_OBJECT

  enum State { Invalid = 0, NoVariable, Incompatible, Acceptable };

public:
  /*!
   \brief Constructor.

   Constructs a spin box with 0.0 as minimum value and 99.99 as maximum value,
   a step value of 1.0 and a precision of 2 decimal places.
   The value is initially set to 0.00.

   \param theParent a parent object
   */
  explicit ModuleBase_ParamIntSpinBox(QWidget* theParent = 0);
  virtual ~ModuleBase_ParamIntSpinBox();

  virtual void stepBy(int);

  /// Convert Int value from text string
  virtual int valueFromText(const QString&) const;

  /// Convert text string from int value
  virtual QString textFromValue(int) const;

  /// Validate the value
  virtual QValidator::State validate(QString&, int&) const;

  /// Set current int value
  virtual void setValue(int);

  /// Set current text value
  virtual void setText(const QString&);

  /// Set a flag about accepted variable
  void setAcceptVariables(const bool);

  /// Returns accepted variables flag
  bool isAcceptVariables() const;

  /// Returns True if the input value contains variable
  bool hasVariable() const;

 protected:
   /// Returns True if the given text contains variable
   /// \param theText a text string
  bool hasVariable(const QString& theText) const;

  /// Returns state of the control
  State isValid(const QString&, double&) const;

  /// Returns True if the given value is within min and max of the control
  bool checkRange(const double) const;

  /// Finds a variable by its name. Returns true in success
  /// \param theName a name of variable
  /// \param outValue an output value of the variable
  bool findVariable(const QString& theName, double& outValue) const;

 protected:
  virtual void showEvent(QShowEvent*);

 protected slots:
   /// A slot called on text change
  void onTextChanged(const QString&);

 private:
  void connectSignalsAndSlots();

 private:
  QString myTextValue;

  bool myAcceptVariables;
};

#endif
