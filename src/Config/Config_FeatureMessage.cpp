/*
 *
 */
#include "Config_FeatureMessage.h"

Config_FeatureMessage::Config_FeatureMessage(const Events_ID theId, const void* theParent)
    : Events_Message(theId, theParent)
{
  myId = "";
  myText = "";
  myTooltip = "";
  myIcon = "";
  myKeysequence = "";

  myGroupId = "";
  myWorkbenchId = "";
}

const std::string& Config_FeatureMessage::icon() const
{
  return myIcon;
}

void Config_FeatureMessage::setIcon(const std::string& icon)
{
  myIcon = icon;
}

const std::string& Config_FeatureMessage::id() const
{
  return myId;
}

void Config_FeatureMessage::setId(const std::string& id)
{
  myId = id;
}

const std::string& Config_FeatureMessage::keysequence() const
{
  return myKeysequence;
}

void Config_FeatureMessage::setKeysequence(const std::string& keysequence)
{
  myKeysequence = keysequence;
}

const std::string& Config_FeatureMessage::text() const
{
  return myText;
}

void Config_FeatureMessage::setText(const std::string& text)
{
  myText = text;
}

const std::string& Config_FeatureMessage::tooltip() const
{
  return myTooltip;
}

const std::string& Config_FeatureMessage::groupId() const
{
  return myGroupId;
}

void Config_FeatureMessage::setGroupId(const std::string& groupId)
{
  myGroupId = groupId;
}

const std::string& Config_FeatureMessage::workbenchId() const
{
  return myWorkbenchId;
}

void Config_FeatureMessage::setWorkbenchId(const std::string& workbenchId)
{
  myWorkbenchId = workbenchId;
}

void Config_FeatureMessage::setTooltip(const std::string& tooltip)
{
  myTooltip = tooltip;
}

const std::string& Config_FeatureMessage::pluginLibrary() const
{
  return myPluginLibrary;
}

void Config_FeatureMessage::setPluginLibrary(const std::string& myPluginLibrary)
{
  this->myPluginLibrary = myPluginLibrary;
}

bool Config_FeatureMessage::isUseInput() const
{
  return myUseInput;
}

void Config_FeatureMessage::setUseInput(bool isUseInput)
{
  myUseInput = isUseInput;
}

const std::string& Config_FeatureMessage::nestedFeatures() const
{
  return myNestedFeatures;
}

void Config_FeatureMessage::setNestedFeatures(const std::string& theNestedFeatures)
{
  myNestedFeatures = theNestedFeatures;
}
