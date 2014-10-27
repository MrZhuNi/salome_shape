// File:        XGUI_Preferences.h
// Created:     07 Aug 2014
// Author:      Vitaly SMETANNIKOV

#ifndef XGUI_Preferences_H
#define XGUI_Preferences_H

#include "XGUI.h"
#include "XGUI_IPrefMgr.h"

#include <SUIT_PreferenceMgr.h>
#include <QDialog>

class SUIT_ResourceMgr;
class QWidget;

// Pair of values: section name, value name
typedef QPair<QString, QString> XGUI_Pref;
typedef QList<XGUI_Pref> XGUI_Prefs;

//***********************************************************************
/// Class for manipulation  with preferences in the application
class XGUI_EXPORT XGUI_Preferences
{
 public:
  static const QString VIEWER_SECTION;
  static const QString MENU_SECTION;

  static bool editPreferences(XGUI_Prefs& theModified);

  /// Returns currently installed resource manager
  static SUIT_ResourceMgr* resourceMgr();

  /// Sets a resource manager
  /// It is used in case of necessity to define external resource manager (not NewGeom)
  static void setResourceMgr(SUIT_ResourceMgr* theMgr) { myResourceMgr = theMgr; }

  /// Updates Config_PropManager properties by module from SUIT_ResourceMgr
  static void updateConfigByResources();

  /// Updates SUIT_ResourceMgr values by Config_PropManager properties
  /// \param theUpdateOnlyInvalid flag to update only invalid values, if it is false, all are updated
  static void updateResourcesByConfig();

  /// Set default values to the Config_PropManager properties
  static void resetConfig();

  /// Loads properties defined by module to Config_PropManager
  static void loadCustomProps();

  /// 
  static void createEditContent(XGUI_IPrefMgr* thePref, int thePage);

private:
  /// Creates content of preferences editing widget
  static void createCustomPage(XGUI_IPrefMgr* thePref, int thePageId);

  static SUIT_ResourceMgr* myResourceMgr;
};

//***********************************************************************
/// Manager of preferences
class XGUI_EXPORT XGUI_PreferencesMgr : public SUIT_PreferenceMgr
{
Q_OBJECT
 public:
  XGUI_PreferencesMgr(QtxResourceMgr* theResource, QWidget* theParent)
      : SUIT_PreferenceMgr(theResource, theParent)
  {
  }

  virtual ~XGUI_PreferencesMgr()
  {
  }

  XGUI_Prefs modified() const
  {
    return myModified;
  }

 protected:
  virtual void changedResources(const ResourceMap& theMap);

 private:
  XGUI_Prefs myModified;
};

//***********************************************************************
/// Dialog box for preferences editing
class XGUI_EXPORT XGUI_PreferencesDlg : public QDialog
{
Q_OBJECT
 public:
  XGUI_PreferencesDlg(SUIT_ResourceMgr* theResurces, QWidget* theParent = 0);
  virtual ~XGUI_PreferencesDlg();

  bool isChanged() const
  {
    return myIsChanged;
  }

  void modified(XGUI_Prefs& theModified) const;

 public slots:
  virtual void accept();

protected slots:
  void onDefault();

 private:
   /// Create editors for aplication properties
   void createEditors();

   /// Create a viewer page in dialog box
   void createViewerPage(int thePageId);
   
   /// Create menu properties page in the dialog box
   void createMenuPage(int thePageId);

  XGUI_PreferencesMgr* myPreferences;
  bool myIsChanged;
};

#endif
