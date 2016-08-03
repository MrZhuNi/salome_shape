// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:      ModuleBase_DoubleSpinBox.h
// Author:    Sergey BELASH
//
#ifndef MODULEBASE_DOUBLESPINBOX_H_
#define MODULEBASE_DOUBLESPINBOX_H_

#include "ModuleBase.h"

#include <QDoubleSpinBox>
#include <QValidator>

#include <QColor>

class QKeyEvent;

/**
* \ingroup GUI
* An extension and customization of QDoubleSpinBox class
*/
class MODULEBASE_EXPORT ModuleBase_DoubleSpinBox : public QDoubleSpinBox
{
Q_OBJECT

 public:
  explicit ModuleBase_DoubleSpinBox( QWidget* theParent = 0, int thePrecision = -12 );
  virtual ~ModuleBase_DoubleSpinBox();

  /// Returns true if the control is clear
  bool isCleared() const;

  /// Set control clear
  virtual void setCleared(const bool);

  /// Returns current precision
  int getPrecision() const;

  /// Set precision
  void setPrecision(const int);

  /// Set step
  virtual void stepBy(int);

  /// Converts value from string to double
  virtual double valueFromText(const QString&) const;

  /// Convert value from double to string
  virtual QString textFromValue(double) const;

  /// Validate current value
  virtual QValidator::State validate(QString&, int&) const;

  /// Change enable/disable internal state to emit key press event
  /// \param theEnable if true, the signal is emitted
  /// \return the previous value
  bool enableKeyPressEvent(const bool& theEnable);

  /// Imitation of disable control value. If theEnable is false, the control becomes
  /// read only and base color is disabled. 
  void setValueEnabled(const bool& theEnable);

signals:
  /// The signal about key release on the control, that corresponds to the attribute
  void enterReleased();

 protected slots:
   /// Called on text changed
  virtual void onTextChanged(const QString&);

 protected:
   /// Removes extra trailing zero symbols
  QString removeTrailingZeroes(const QString&) const;
  /// Called on key press event
  virtual void keyReleaseEvent(QKeyEvent* theEvent);

  /// Called on key press event
  virtual void keyPressEvent(QKeyEvent* theEvent);

 private:
  // boolen flag whether the key event is emitted. The default value is false
  bool myIsEmitKeyPressEvent;

   /// Is clear flag
  bool myCleared;

  /// Precision value
  int myPrecision;

  /// Cashed color of active base palette
  QColor myEnabledBaseColor;
};

#endif
