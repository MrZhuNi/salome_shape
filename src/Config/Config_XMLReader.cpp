/*
 * Config_XMLReader.cpp
 *
 *  Created on: Mar 14, 2014
 *      Author: sbh
 */

#include <Config_XMLReader.h>

#include <Event_Loop.h>
#include <libxml\parser.h>
#include <libxml\tree.h>

/*
#ifdef WIN32
//For GetModuleFileNameW
#include <windows.h>
#endif
*/

#ifdef _DEBUG
#include <iostream>
#endif

Config_XMLReader::Config_XMLReader(const std::string& theXmlFileName)
{
  std::string prefix;
  /* the problem: application may be launched using python execuable, to use environment variable
                  (at least for the current moment)
  //Get path to *.xml files (typically ./bin/../plugins/)
#ifdef WIN32
  HMODULE hModule = GetModuleHandleW(NULL);
  WCHAR wchar_path[MAX_PATH];
  GetModuleFileNameW(hModule, wchar_path, MAX_PATH);
  char char_path[MAX_PATH];
  char DefChar = ' ';
  WideCharToMultiByte(CP_ACP, 0, wchar_path, -1, char_path, MAX_PATH, &DefChar, NULL);
  prefix = std::string(char_path);
  //chop "bin\XGUI.exe"
  unsigned found = prefix.rfind("bin");
  if (found != std::string::npos)
    prefix.replace(found, prefix.length(), "plugins\\");
#else
  //TODO(sbh): Find full path to binary on linux
  prefix = "../plugins/";
#endif
  */
  char* anEnv = getenv("NEW_GEOM_CONFIG_FILE");
  if (anEnv) {
    prefix = std::string(anEnv) + "/";
  }

  myDocumentPath = prefix + theXmlFileName;
}

Config_XMLReader::~Config_XMLReader()
{
}

/*
 * Read all nodes (recursively if processChildren() is true
 * for a node). For each read node the processNode will be
 * called.
 */
void Config_XMLReader::readAll()
{
  xmlNodePtr aRoot = findRoot();
  readRecursively(aRoot);
}

/*
 * Allows to customize reader's behavior for a node. Virtual.
 * The default implementation does nothing. (In debug mode prints
 * some info)
 */
void Config_XMLReader::processNode(xmlNodePtr aNode)
{
#ifdef _DEBUG
  std::cout << "Config_XMLReader::processNode: "
  << aNode->name << " content: "
  << aNode->content << std::endl;
#endif
}

/*
 * Defines which nodes should be processed recursively. Virtual.
 * The default implementation to read all nodes.
 */
bool Config_XMLReader::processChildren(xmlNodePtr aNode)
{
  return true;
}

xmlNodePtr Config_XMLReader::findRoot()
{
  xmlDocPtr aDoc;
  aDoc = xmlParseFile(myDocumentPath.c_str());
  if (aDoc == NULL) {
#ifdef _DEBUG
    std::cout << "Config_XMLReader::import: " << "Document " << myDocumentPath
    << " is not parsed successfully." << std::endl;
#endif
    return NULL;
  }
  xmlNodePtr aRoot = xmlDocGetRootElement(aDoc);
#ifdef _DEBUG
  if(aRoot == NULL) {
    std::cout << "Config_XMLReader::import: " << "Error: empty document";
  }
#endif
  return aRoot;
}

/*
 * Calls processNode() for each child (for some - recursively)
 * of the given node.
 * \sa ReadAll()
 */
void Config_XMLReader::readRecursively(xmlNodePtr theParent)
{
  if (!theParent)
    return;
  xmlNodePtr aNode = theParent->xmlChildrenNode;
  for(; aNode; aNode = aNode->next) {
    processNode(aNode);
    if (processChildren(aNode)) {
      readRecursively(aNode);
    }
  }
}

/*
 * void* -> xmlNodePtr
 */
xmlNodePtr Config_XMLReader::node(void* theNode)
{
  return static_cast<xmlNodePtr>(theNode);
}

/*
 * Returns named property for a given node as std::string.
 */
std::string Config_XMLReader::getProperty(xmlNodePtr theNode, const char* name)
{
  std::string result = "";
  char* aPropChars = (char*) xmlGetProp(theNode, BAD_CAST name);
  if (!aPropChars || aPropChars[0] == 0)
    return result;
  result = std::string(aPropChars);
  return result;
}

/*
 * Returns true if theNode is XML node with a given name.
 */
bool Config_XMLReader::isNode(xmlNodePtr theNode, const char* theNodeName, ...)
{
  bool result = false;
  const xmlChar* aName = theNode->name;
  if (!aName || theNode->type != XML_ELEMENT_NODE)
    return false;

  if (!xmlStrcmp(aName, (const xmlChar *) theNodeName))
    return true;

  va_list args; // define argument list variable
  va_start(args, theNodeName); // init list; point to last defined argument
  while(true) {
    char *anArg = va_arg (args, char *); // get next argument
    if (anArg == NULL)
      break;
    if (!xmlStrcmp(aName, (const xmlChar *) anArg)) {
      va_end(args); // cleanup the system stack
      return true;
    }
  }
  va_end(args); // cleanup the system stack
  return false;
}
