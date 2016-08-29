// Copyright

// File:        FeaturesPlugin_ParameterLaw.h


#ifndef FeaturesPlugin_ParameterLaw_H_
#define FeaturesPlugin_ParameterLaw_H_

#include <FeaturesPlugin.h>

#include <GeomAlgoAPI_ParameterLaw.h>
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
class FeaturesPlugin_ParameterLaw : public ModelAPI_Feature
{
 public:
  /// Pipe kind.
  inline static const std::string& ID()
  {
    static const std::string MY_ParameterLaw_ID("ParameterLaw");
    return MY_ParameterLaw_ID;
  }

      /// attribute name of law reference curve
  inline static const std::string& LAW_REFERENCE_ID()
  {
    static const std::string MY_LAW_REFERENCE_ID("law_reference");
    return MY_LAW_REFERENCE_ID;
  }

      /// attribute name of law definition curve
  inline static const std::string& LAW_DEFINITION_ID()
  {
    static const std::string MY_LAW_DEFINITION_ID("law_definition");
    return MY_LAW_DEFINITION_ID;
  }

      /// attribute name of parameter name
  inline static const std::string& VARIABLE_ID()
  {
    static const std::string MY_VARIABLE_ID("variable");
    return MY_VARIABLE_ID;
  }

    /// attribute name of flag of reverse direction
  inline static const std::string& REVERSE_ID()
  {
    static const std::string MY_REVERSE_ID("reverse_direction");
    return MY_REVERSE_ID;
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
    static std::string MY_KIND = FeaturesPlugin_ParameterLaw::ID();
    return MY_KIND;
  }

  /// Creates a new part document if needed.
  FEATURESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes.
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation.
  FeaturesPlugin_ParameterLaw();

private:
  /// Load Naming data structure of the feature to the document.
  void loadNamingDS(GeomAlgoAPI_ParameterLaw& theParameterLawAlgo,
                    std::shared_ptr<ModelAPI_ResultBody> theResultBody);
};
#endif
