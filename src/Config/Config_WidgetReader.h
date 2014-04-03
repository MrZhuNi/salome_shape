/*
 * Config_WidgetReader.h
 *
 *  Created on: Apr 2, 2014
 *      Author: sbh
 */

#ifndef CONFIG_WIDGETREADER_H_
#define CONFIG_WIDGETREADER_H_

#include <Config.h>
#include <Config_XMLReader.h>

#include <map>
#include <string>


class CONFIG_EXPORT Config_WidgetReader: public Config_XMLReader
{
public:
  Config_WidgetReader(const std::string& theXmlFile);
  virtual ~Config_WidgetReader();

  std::string featureWidgetCfg(std::string theFeatureName);

protected:
  void processNode(xmlNodePtr theNode);
  bool processChildren(xmlNodePtr theNode);

private:
  std::map<std::string, std::string> myWidgetCache;

};

#endif /* CONFIG_WIDGETREADER_H_ */
