// Copyright

// File:        FeaturesPlugin_PipeApprox.h


#ifndef FeaturesPlugin_PipeApprox_H_
#define FeaturesPlugin_PipeApprox_H_

#include <FeaturesPlugin.h>

#include <GeomAlgoAPI_PipeApprox.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_Attribute.h>
#include <ModelAPI_ResultBody.h>

#include <memory>
#include <list>

class GeomAPI_Shape;
class ModelAPI_ResultBody;

/** \class FeaturesPlugin_Pipe
 *  \ingroup Plugins
 *  \brief Feature for creation of Pipe from the planar face.
 *  Pipe creates the lateral faces based on edges of the base face and
 *  the start and end faces and/or start and end angles.
 */
class FeaturesPlugin_PipeApprox : public ModelAPI_Feature
{
 public:
  /// Pipe kind.
  inline static const std::string& ID()
  {
    static const std::string MY_PipeApprox_ID("PipeApprox");
    return MY_PipeApprox_ID;
  }

  /// Attribute name of references sketch entities list, it should contain a sketch result or
  /// a pair a sketch result to sketch face.
  inline static const std::string& PROFILE_ID()
  {
    static const std::string MY_PROFILE_LIST_ID("profile");
    return MY_PROFILE_LIST_ID;
  }

  /// Attribute name of an Pipe path.
  inline static const std::string& PATH_OBJECT_ID()
  {
    static const std::string MY_PATH_ID("path_object");
    return MY_PATH_ID;
  }

  /// Attribute name of Bi-Normal.
  inline static const std::string& BINORMAL_ID()
  {
    static const std::string MY_BINORMAL_ID("binormal");
    return MY_BINORMAL_ID;
  }

  inline static const std::string& LAW_REFERENCE_ID()
  {
    static const std::string MY_LAW_REFERENCE_ID("law_reference");
    return MY_LAW_REFERENCE_ID;
  }

  inline static const std::string& LAW_DEFINITION_ID()
  {
    static const std::string MY_LAW_DEFINITION_ID("law_definition");
    return MY_LAW_DEFINITION_ID;
  }

  /// attribute name for creation method
  inline static const std::string& CREATION_METHOD()
  {
    static const std::string METHOD_ATTR("CreationMethod");
    return METHOD_ATTR;
  }


  /// \return the kind of a feature.
  FEATURESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = FeaturesPlugin_PipeApprox::ID();
    return MY_KIND;
  }

  /// Creates a new part document if needed.
  FEATURESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes.
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation.
  FeaturesPlugin_PipeApprox();

private:
  /// Load Naming data structure of the feature to the document.
  void loadNamingDS(GeomAlgoAPI_PipeApprox& thePipeApproxAlgo,
                    std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                    std::shared_ptr<GeomAPI_Shape> theBasis);

};
#endif
