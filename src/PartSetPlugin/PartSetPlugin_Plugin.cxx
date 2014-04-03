#include "PartSetPlugin_Plugin.h"
#include "PartSetPlugin_Part.h"
#include "PartSetPlugin_Point.h"
#include <ModelAPI_PluginManager.h>
#include <ModelAPI_Document.h>

using namespace std;

// the only created instance of this plugin
static PartSetPlugin_Plugin* MY_INSTANCE = new PartSetPlugin_Plugin();

PartSetPlugin_Plugin::PartSetPlugin_Plugin() 
{
  // register this plugin
  ModelAPI_PluginManager::get()->registerPlugin(this);
}

std::shared_ptr<ModelAPI_Feature> PartSetPlugin_Plugin::createFeature(string theFeatureID)
{
  std::shared_ptr<ModelAPI_Feature> aCreated;
  bool isCurrent = true; // to create a feature in the current document
  if (theFeatureID == "Part") {
    aCreated = std::shared_ptr<ModelAPI_Feature>(new PartSetPlugin_Part);
    isCurrent = false; // allways create in the root document
  } else if (theFeatureID == "Point") {
    aCreated = std::shared_ptr<ModelAPI_Feature>(new PartSetPlugin_Point);
  }

  // add to a root document for the current moment
  if (aCreated) {
    shared_ptr<ModelAPI_Document> aDoc = isCurrent ? 
      ModelAPI_PluginManager::get()->currentDocument() :
      ModelAPI_PluginManager::get()->rootDocument();
    aDoc->addFeature(aCreated, PARTS_GROUP);
  }
  // feature of such kind is not found
  return aCreated;
}
