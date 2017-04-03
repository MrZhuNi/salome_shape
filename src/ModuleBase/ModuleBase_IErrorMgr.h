// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_IErrorMgr.h
// Created:     22 July 2015
// Author:      Sergey POKHODENKO

#ifndef ModuleBase_IErrorMgr_H
#define ModuleBase_IErrorMgr_H

#include "ModuleBase.h"

#include <QObject>

#include <memory>

class ModuleBase_IPropertyPanel;
class ModelAPI_Feature;

/**
 * \class ModuleBase_IErrorMgr
 * \ingroup GUI
 * \brief An interface to Errors manager object
 */
class MODULEBASE_EXPORT ModuleBase_IErrorMgr : public QObject
{
  Q_OBJECT
public:
  /// Default constructor
  /// \param theParent a parent object
  ModuleBase_IErrorMgr(QObject* theParent = 0);
  /// Virtual destructor
  virtual ~ModuleBase_IErrorMgr();

  /// \brief Set property pane to the operation
  /// \param theProp a property panel instance
  virtual void setPropertyPanel(ModuleBase_IPropertyPanel* theProp);

  /// \return Currently installed property panel
  ModuleBase_IPropertyPanel* propertyPanel() const { return myPropertyPanel; }

  /// Update actions for the given feature
  /// \param theFeature a feature
  virtual void updateActions(const std::shared_ptr<ModelAPI_Feature>& theFeature) = 0;

protected slots:
  /// Process values changed event for processing feature attribute validation errors.
  virtual void onWidgetChanged() = 0;

protected:
  /// Access to property panel
  ModuleBase_IPropertyPanel* myPropertyPanel;
};

#endif // ModuleBase_IErrorMgr_H