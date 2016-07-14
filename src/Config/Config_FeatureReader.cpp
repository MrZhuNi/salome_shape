// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * Config_FeatureReader.cpp
 *
 *  Created on: Mar 20, 2014
 *      Author: sbh
 */

#include <Config_Keywords.h>
#include <Config_Common.h>
#include <Config_FeatureMessage.h>
#include <Config_AttributeMessage.h>
#include <Config_FeatureReader.h>
#include <Events_Message.h>
#include <Events_Loop.h>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlstring.h>

#include <string>
#include <algorithm>
#include <list>

#ifdef _DEBUG
#include <iostream>
#endif

Config_FeatureReader::Config_FeatureReader(const std::string& theXmlFile,
                                           const std::string& theLibraryName,
                                           const char* theEventGenerated)
    : Config_XMLReader(theXmlFile),
      myLibraryName(theLibraryName),
      myEventGenerated(theEventGenerated ? theEventGenerated : Config_FeatureMessage::GUI_EVENT()),
      myIsProcessWidgets(theEventGenerated != NULL)
{
}

Config_FeatureReader::~Config_FeatureReader()
{
}

std::list<std::string> Config_FeatureReader::features() const
{
  return myFeatures;
}

void Config_FeatureReader::processNode(xmlNodePtr theNode)
{
  Events_ID aMenuItemEvent = Events_Loop::eventByName(myEventGenerated);
  if (isNode(theNode, NODE_FEATURE, NULL)) {
    storeAttribute(theNode, _ID);
    std::shared_ptr<Config_FeatureMessage> aMessage(new Config_FeatureMessage(aMenuItemEvent, this));
    fillFeature(theNode, aMessage);
    myFeatures.push_back(getProperty(theNode, _ID));
    //If a feature has xml definition for it's widget:
    aMessage->setUseInput(hasChild(theNode));
    Events_Loop::loop()->send(aMessage);
    //The m_last* variables always defined before fillFeature() call. XML is a tree.
  } else if (isNode(theNode, NODE_WORKBENCH, NODE_GROUP, NULL)) {
    storeAttribute(theNode, _ID);
    storeAttribute(theNode, WORKBENCH_DOC, true);
  } else if (myIsProcessWidgets) {
    // widgets, like shape_selector or containers, like toolbox
    if (isAttributeNode(theNode)) {
      std::shared_ptr<Config_AttributeMessage> aMessage(new Config_AttributeMessage(aMenuItemEvent, this));
      aMessage->setFeatureId(restoreAttribute(NODE_FEATURE, _ID));
      std::string anAttributeID = getProperty(theNode, _ID);
      if (!anAttributeID.empty()) {
        aMessage->setAttributeId(anAttributeID);
        aMessage->setObligatory(getBooleanAttribute(theNode, ATTR_OBLIGATORY, true));
        aMessage->setConcealment(getBooleanAttribute(theNode, ATTR_CONCEALMENT, false));

        std::list<std::pair<std::string, std::string> > aCases;
        xmlNodePtr aCaseNode = hasParentRecursive(theNode, WDG_SWITCH_CASE, WDG_TOOLBOX_BOX, WDG_CHECK_GROUP, NULL);
        while(aCaseNode) {
          std::string aCaseNodeID = getProperty(aCaseNode, _ID);
          std::string aSwitchNodeID = "";
          const xmlChar* aName = aCaseNode->name;
          xmlNodePtr aSwitchNode;
          if (!xmlStrcmp(aName, (const xmlChar *) WDG_SWITCH_CASE)) {
            aSwitchNode = hasParentRecursive(aCaseNode, WDG_SWITCH, NULL);
          }
          else if (!xmlStrcmp(aName, (const xmlChar *) WDG_TOOLBOX_BOX)) {
            aSwitchNode = hasParentRecursive(aCaseNode, WDG_TOOLBOX, NULL);
          }
          if (!xmlStrcmp(aName, (const xmlChar *) WDG_CHECK_GROUP)) {
            /// the box is optional, attribute is in case if the optional attribute value is not empty
            aSwitchNode = aCaseNode;
          }
          if (aSwitchNode)
            aSwitchNodeID = getProperty(aSwitchNode, _ID);

          aCases.push_back(std::make_pair(aSwitchNodeID, aCaseNodeID));
          aCaseNode = hasParentRecursive(aSwitchNode, WDG_SWITCH_CASE, WDG_TOOLBOX_BOX, WDG_CHECK_GROUP, NULL);
        }
        aMessage->setCases(aCases);
        Events_Loop::loop()->send(aMessage);
      }
    // container pages, like "case" or "box"
    } else if (isNode(theNode, WDG_CHECK_GROUP, WDG_SWITCH, WDG_SWITCH_CASE,
                      WDG_TOOLBOX, WDG_TOOLBOX_BOX, NULL)) {
      storeAttribute(theNode, _ID); // save case:caseId (or box:boxId)
    }
  }
  //Process SOURCE nodes.
  Config_XMLReader::processNode(theNode);
}

void Config_FeatureReader::cleanup(xmlNodePtr theNode)
{
  if (isNode(theNode, WDG_CHECK_GROUP, WDG_SWITCH, WDG_SWITCH_CASE,
             WDG_TOOLBOX, WDG_TOOLBOX_BOX, NULL)) {
    // cleanup id of cases when leave case node
    cleanupAttribute(theNode, _ID);
  }
}

bool Config_FeatureReader::processChildren(xmlNodePtr theNode)
{
  bool result = isNode(theNode, NODE_WORKBENCH, NODE_GROUP, NULL);
  if(!result && myIsProcessWidgets) {
    result = isNode(theNode, NODE_FEATURE, 
                             WDG_GROUP, WDG_CHECK_GROUP,
                             WDG_TOOLBOX, WDG_TOOLBOX_BOX,
                             WDG_SWITCH, WDG_SWITCH_CASE, NULL);
  }
  return result;
}

void Config_FeatureReader::fillFeature(xmlNodePtr theFeatureNode,
  const std::shared_ptr<Config_FeatureMessage>& outFeatureMessage)
{
  outFeatureMessage->setId(getProperty(theFeatureNode, _ID));
  outFeatureMessage->setPluginLibrary(myLibraryName);
  outFeatureMessage->setNestedFeatures(getProperty(theFeatureNode, FEATURE_NESTED));
  outFeatureMessage->setActionsWhenNested(getNormalizedProperty(theFeatureNode, FEATURE_WHEN_NESTED));
  outFeatureMessage->setModal(getBooleanAttribute(theFeatureNode, FEATURE_MODAL, false));

  bool isInternal = getBooleanAttribute(theFeatureNode, ATTR_INTERNAL, false);
  outFeatureMessage->setInternal(isInternal);
  if (isInternal) {
    //Internal feature has no visual representation.
    return;
  }
  
  outFeatureMessage->setText(getProperty(theFeatureNode, FEATURE_TEXT));
  outFeatureMessage->setTooltip(getProperty(theFeatureNode, FEATURE_TOOLTIP));
  outFeatureMessage->setIcon(getProperty(theFeatureNode, FEATURE_ICON));
  outFeatureMessage->setKeysequence(getProperty(theFeatureNode, FEATURE_KEYSEQUENCE));
  outFeatureMessage->setGroupId(restoreAttribute(NODE_GROUP, _ID));
  outFeatureMessage->setWorkbenchId(restoreAttribute(NODE_WORKBENCH, _ID));
  // Get document kind of a feature, if empty set workbench's kind (might be empty too)
  std::string aDocKind = getProperty(theFeatureNode, FEATURE_DOC);
  if(aDocKind.empty()) {
    aDocKind = restoreAttribute(NODE_WORKBENCH, WORKBENCH_DOC);
  }
  outFeatureMessage->setDocumentKind(aDocKind);
  bool isAutoPreview = getBooleanAttribute(theFeatureNode, FEATURE_AUTO_PREVIEW, true);
  outFeatureMessage->setAutoPreview(isAutoPreview);
}
