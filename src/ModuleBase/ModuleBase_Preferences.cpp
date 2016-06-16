// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_Preferences.cpp
// Created:     07 Aug 2014
// Author:      Vitaly SMETANNIKOV

#include "ModuleBase_Preferences.h"
//#include "ModuleBase_Constants.h"

#include <Config_PropManager.h>

#include <TopAbs_ShapeEnum.hxx>

#include <SUIT_ResourceMgr.h>
#include <SUIT_PreferenceMgr.h>
#include <Qtx.h>

#include <QLayout>
#include <QApplication>
#include <QDialogButtonBox>
#include <QPushButton>

const QString ModuleBase_Preferences::VIEWER_SECTION = "Viewer";
const QString ModuleBase_Preferences::MENU_SECTION = "Menu";

SUIT_ResourceMgr* ModuleBase_Preferences::myResourceMgr = 0;

SUIT_ResourceMgr* ModuleBase_Preferences::resourceMgr()
{
  if (!myResourceMgr) {
    myResourceMgr = new SUIT_ResourceMgr("SHAPER");
    myResourceMgr->setCurrentFormat("xml");
  }
  return myResourceMgr;
}

bool ModuleBase_Preferences::editPreferences(ModuleBase_Prefs& theModified)
{
  ModuleBase_PreferencesDlg aDlg(resourceMgr(), QApplication::activeWindow());
  aDlg.exec();
  if (aDlg.isChanged()) {
    aDlg.modified(theModified);
    resourceMgr()->save();
    return true;
  }
  return false;
}

void ModuleBase_Preferences::updateConfigByResources()
{
  Config_Properties aProps = Config_PropManager::getProperties();
  Config_Properties::iterator aIt;
  for (aIt = aProps.begin(); aIt != aProps.end(); ++aIt) {
    Config_Prop* aProp = (*aIt);
    QString aVal = myResourceMgr->stringValue(QString(aProp->section().c_str()),
                                              QString(aProp->name().c_str()));
    if (!aVal.isEmpty()) {
      aProp->setValue(aVal.toStdString());
    }
  }
}

void ModuleBase_Preferences::updateResourcesByConfig()
{
  Config_Properties aProps = Config_PropManager::getProperties();
  Config_Properties::iterator aIt;
  for (aIt = aProps.begin(); aIt != aProps.end(); ++aIt) {
    Config_Prop* aProp = (*aIt);
    myResourceMgr->setValue(QString(aProp->section().c_str()), QString(aProp->name().c_str()),
                            QString(aProp->value().c_str()));
  }
}

void ModuleBase_Preferences::resetConfig()
{
  Config_Properties aProps = Config_PropManager::getProperties();
  Config_Properties::iterator aIt;
  for (aIt = aProps.begin(); aIt != aProps.end(); ++aIt) {
    Config_Prop* aProp = (*aIt);
    aProp->setValue(aProp->defaultValue());
  }
}

void ModuleBase_Preferences::loadCustomProps()
{
  if(!myResourceMgr)
    return;
  QStringList aSections = myResourceMgr->sections();
  foreach (QString aSection, aSections)
  {
    QStringList aParams = myResourceMgr->parameters(aSection);
    foreach (QString aParam, aParams)
    {
      Config_Prop* aProp = Config_PropManager::registerProp(aSection.toStdString(),
                                       aParam.toStdString(), "", Config_Prop::Disabled);
      aProp->setValue(myResourceMgr->stringValue(aSection, aParam).toStdString());
    }
  }
}


void ModuleBase_Preferences::createEditContent(ModuleBase_IPrefMgr* thePref, int thePage)
{
  thePref->prefMgr()->setItemIcon(thePage, QIcon(":pictures/module.png"));
  createCustomPage(thePref, thePage);
}

void ModuleBase_Preferences::resetResourcePreferences(SUIT_PreferenceMgr* thePref)
{
  if (!thePref)
    return;

  QtxResourceMgr::WorkingMode aPrev =
    thePref->resourceMgr()->setWorkingMode(QtxResourceMgr::IgnoreUserValues);
  thePref->retrieve();
  thePref->resourceMgr()->setWorkingMode(aPrev);
}

void ModuleBase_Preferences::resetConfigPropPreferences(SUIT_PreferenceMgr* thePref)
{
  resetConfig();
  updateResourcesByConfig();

  // retrieve the reset resource values to the preferences items
  Config_Properties aProps = Config_PropManager::getProperties();
  Config_Properties::iterator aIt;
  QStringList aValues;
  QStringList aSections;
  for (aIt = aProps.begin(); aIt != aProps.end(); ++aIt) {
    Config_Prop* aProp = (*aIt);
    aValues.append(QString(aProp->name().c_str()));
    if (!aSections.contains(aProp->section().c_str()))
      aSections.append(aProp->section().c_str());
    QtxPreferenceItem* anItem = thePref->findItem(QString(aProp->title().c_str()), true);
    if (anItem)
      anItem->retrieve();
  }
}

void ModuleBase_Preferences::createCustomPage(ModuleBase_IPrefMgr* thePref, int thePageId)
{
  SUIT_ResourceMgr* aResMgr = ModuleBase_Preferences::resourceMgr();
  bool isResModified = false;

  // Make a Tab from each section
  std::list<std::string> aSections = Config_PropManager::getSections();
  std::list<std::string>::const_iterator it;
  for (it = aSections.cbegin(); it != aSections.cend(); ++it) {
    Config_Properties aProps = Config_PropManager::getProperties(*it);
    int aTab = thePref->prefMgr()->addItem(QString((*it).c_str()), thePageId);
    thePref->prefMgr()->setItemProperty("columns", 2, aTab);

    Config_Properties::const_iterator aIt;
    for (aIt = aProps.cbegin(); aIt != aProps.cend(); ++aIt) {
      Config_Prop* aProp = (*aIt);
      // check that the property is defined
      QString aSection(aProp->section().c_str());
      QString aName(aProp->name().c_str());
      if (!aResMgr->hasValue(aSection, aName)) {
        aResMgr->setValue(aSection, aName, QString(aProp->value().c_str()));
        isResModified = true;
      }
      // Add item
      if (aProp->type() != Config_Prop::Disabled) {
        SUIT_PreferenceMgr::PrefItemType aPrefType = SUIT_PreferenceMgr::Auto;
        if (aProp->type() == Config_Prop::Directory) {
          aPrefType = SUIT_PreferenceMgr::File;
        } else {
          aPrefType = (SUIT_PreferenceMgr::PrefItemType) aProp->type();
        }
        int anId = thePref->addPreference(QObject::tr(aProp->title().c_str()), aTab, aPrefType,
                                          QString::fromStdString(aProp->section()),
                                          QString::fromStdString(aProp->name()));
        if(aProp->type() == Config_Prop::Directory) {
          thePref->setItemProperty("path_type", Qtx::PT_Directory, anId);
        }
      }
    }
  }
}

/**
* Implementation of preferences manager interface
*/
class ModuleBase_PrefMgr: public ModuleBase_IPrefMgr
{
public:
  /// Constructor
  /// \param theMgr a preferences manager
  ModuleBase_PrefMgr(ModuleBase_PreferencesMgr* theMgr):myMgr(theMgr) {}

  virtual int addPreference(const QString& theLbl, int pId, 
                            SUIT_PreferenceMgr::PrefItemType theType,
                            const QString& theSection, const QString& theName )
  {
    return myMgr->addItem(theLbl, pId, theType, theSection, theName);
  }

  virtual void setItemProperty( const QString& thePropName, const QVariant& theValue,
                               const int theId = -1) {
    myMgr->setItemProperty(thePropName, theValue, theId);
  }

  virtual SUIT_PreferenceMgr* prefMgr() const { return myMgr; }

private:
  ModuleBase_PreferencesMgr* myMgr;
};

//**********************************************************
//**********************************************************
//**********************************************************
ModuleBase_PreferencesDlg::ModuleBase_PreferencesDlg(SUIT_ResourceMgr* theResurces, QWidget* theParent)
    : QDialog(theParent),
      myIsChanged(false)
{
  setWindowTitle(tr("Edit preferences"));

  QVBoxLayout* main = new QVBoxLayout(this);
  main->setMargin(5);
  main->setSpacing(5);

  myPreferences = new ModuleBase_PreferencesMgr(theResurces, this);
  main->addWidget(myPreferences);

  setFocusProxy(myPreferences);
  myPreferences->setFrameStyle(QFrame::Box | QFrame::Sunken);

  QDialogButtonBox* aBtnBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel |
                                                   QDialogButtonBox::Reset,
                                                   Qt::Horizontal, this);
  QPushButton* aDefaultButton = aBtnBox->button(QDialogButtonBox::Reset);
  aDefaultButton->setText(tr("Default"));
  connect(aDefaultButton, SIGNAL(clicked()), this, SLOT(onDefault()));

  main->addWidget(aBtnBox);
  connect(aBtnBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(aBtnBox, SIGNAL(rejected()), this, SLOT(reject()));
  createEditors();

  myPreferences->retrieve();
  setMinimumSize(800, 220);
}

ModuleBase_PreferencesDlg::~ModuleBase_PreferencesDlg()
{
}

void ModuleBase_PreferencesDlg::createEditors()
{
  int aPage = myPreferences->addItem(tr("Desktop"));
  myPreferences->setItemIcon(aPage, QIcon(":pictures/view_prefs.png"));

  createMenuPage(aPage);
  createViewerPage(aPage);

  aPage = myPreferences->addItem(tr("Module"));
  myPreferences->setItemIcon(aPage, QIcon(":pictures/module.png"));

  ModuleBase_PrefMgr aMgr(myPreferences);
  ModuleBase_Preferences::createEditContent(&aMgr, aPage);
}

void ModuleBase_PreferencesDlg::createViewerPage(int thePageId)
{
  int viewTab = myPreferences->addItem(tr("Viewer"), thePageId);

  QStringList gradList;
  gradList << tr("Horizontal gradient") << tr("Vertical gradient") << tr("First diagonal gradient")
           << tr("Second diagonal gradient") << tr("First corner gradient")
           << tr("Second corner gradient") << tr("Third corner gradient")
           << tr("Fourth corner gradient");

  QList<QVariant> idList;
  for (int i = 0; i < gradList.size(); i++)
    idList << i;

  int bgGroup = myPreferences->addItem(tr("Background"), viewTab);

  QString aImgFiles("Image files (*.bmp *.gif *.pix *.xwd *.rgb *.rs)");

  int bgId = myPreferences->addItem(tr("Viewer 3d"), bgGroup, SUIT_PreferenceMgr::Background,
                                    ModuleBase_Preferences::VIEWER_SECTION, "background");
  myPreferences->setItemProperty("gradient_names", gradList, bgId);
  myPreferences->setItemProperty("gradient_ids", idList, bgId);
  myPreferences->setItemProperty("texture_enabled", true, bgId);
  myPreferences->setItemProperty("texture_center_enabled", true, bgId);
  myPreferences->setItemProperty("texture_tile_enabled", true, bgId);
  myPreferences->setItemProperty("texture_stretch_enabled", true, bgId);
  myPreferences->setItemProperty("custom_enabled", false, bgId);
  myPreferences->setItemProperty("image_formats", aImgFiles, bgId);

  // Create other parameters group in viewer tab
  int otherGroup = myPreferences->addItem(tr("Default selection"), viewTab);
  myPreferences->setItemProperty("columns", 3, otherGroup);
  myPreferences->addItem(tr("Faces"), otherGroup, 
                         SUIT_PreferenceMgr::Bool,
                         ModuleBase_Preferences::VIEWER_SECTION, "face-selection");
  myPreferences->addItem(tr("Edges"), otherGroup, 
                         SUIT_PreferenceMgr::Bool,
                         ModuleBase_Preferences::VIEWER_SECTION, "edge-selection");
  myPreferences->addItem(tr("Vertices"), otherGroup, 
                         SUIT_PreferenceMgr::Bool,
                         ModuleBase_Preferences::VIEWER_SECTION, "vertex-selection");

  myPreferences->addItem(tr("Vertex selection sensitivity"), otherGroup, SUIT_PreferenceMgr::Double,
                         ModuleBase_Preferences::VIEWER_SECTION, "point-selection-sensitivity");
}

void ModuleBase_PreferencesDlg::createMenuPage(int thePageId)
{
  int aMenuTab = myPreferences->addItem(tr("Main menu"), thePageId);

  int aSizeGroup = myPreferences->addItem(tr("Size"), aMenuTab);
  myPreferences->setItemProperty("columns", 1, aSizeGroup);

  int aRowsNb = myPreferences->addItem(tr("Number of rows"), aSizeGroup,
                                       SUIT_PreferenceMgr::IntSpin, ModuleBase_Preferences::MENU_SECTION,
                                       "rows_number");
  myPreferences->setItemProperty("min", 1, aRowsNb);
  myPreferences->setItemProperty("max", 6, aRowsNb);

  myPreferences->addItem(tr("Show Status Bar"), aSizeGroup,
                         SUIT_PreferenceMgr::Bool, ModuleBase_Preferences::MENU_SECTION,
                         "status_bar");
}

void ModuleBase_PreferencesDlg::accept()
{
  myPreferences->store();
  myIsChanged = true;

  // Save custom properties
  ModuleBase_Preferences::updateConfigByResources();
  QDialog::accept();
}

void ModuleBase_PreferencesDlg::modified(ModuleBase_Prefs& theModified) const
{
  theModified = myPreferences->modified();
}

void ModuleBase_PreferencesDlg::onDefault()
{
  // reset main resources. It throwns all resource manager items to the
  // initial/default state. If there is no a default state of the item,
  // it will be filled with an empty value. It concernerned to plugin
  // config items, like visualization color. The main xml do not contains
  // default values for them. So, it is important to reset the config
  // properties after reseting the resources preferences.
  ModuleBase_Preferences::resetResourcePreferences(myPreferences);
  // reset plugin's resources. It fills the config resources with the default
  // values, stores result in the resource manager and retrieve the preferences
  // items with these values.
  ModuleBase_Preferences::resetConfigPropPreferences(myPreferences);
}

//**********************************************************
//**********************************************************
//**********************************************************
void ModuleBase_PreferencesMgr::changedResources(const ResourceMap& theMap)
{
  myModified.clear();
  ResourceMap::ConstIterator it;
  QString sec, param;
  for (it = theMap.begin(); it != theMap.end(); ++it) {
    ModuleBase_Pref aPref;
    it.key()->resource(aPref.first, aPref.second);
    myModified.append(aPref);
  }
}
