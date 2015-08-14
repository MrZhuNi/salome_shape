/*
 * ModuleBase_WidgetToolbox.h
 *
 *  Created on: Feb 27, 2015
 *      Author: sbh
 */

#ifndef MODULEBASE_WIDGETTOOLBOX_H_
#define MODULEBASE_WIDGETTOOLBOX_H_

#include <ModuleBase.h>
#include <ModuleBase_PagedContainer.h>

class ModuleBase_PageBase;
class ModuleBase_ToolBox;

class MODULEBASE_EXPORT ModuleBase_WidgetToolbox : public ModuleBase_PagedContainer
{
  Q_OBJECT
 public:
  ModuleBase_WidgetToolbox(QWidget* theParent, const Config_WidgetAPI* theData,
                           const std::string& theParentId);
  virtual ~ModuleBase_WidgetToolbox();

  /// Defines if it is supported to set the value in this widget
  /// It returns false because this is an info widget
  virtual bool canSetValue() const { return false; };

  /// Overrides ModuleBase_PagedContainer
  virtual int addPage( ModuleBase_PageBase* theWidget,
                       const QString& theName,
                       const QString& theCaseId,
                       const QPixmap& theIcon );

 protected:
  /// Implements ModuleBase_PagedContainer
  virtual int currentPageIndex() const;
  /// Implements ModuleBase_PagedContainer
  virtual void setCurrentPageIndex(int);

 private:
  ModuleBase_ToolBox* myToolBox;
};

#endif /* MODULEBASE_WIDGETTOOLBOX_H_ */
