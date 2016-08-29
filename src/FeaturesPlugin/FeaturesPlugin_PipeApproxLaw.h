// Copyright

// File:        FeaturesPlugin_PipeApproxLaw.h


#ifndef FeaturesPlugin_PipeApproxLaw_H_
#define FeaturesPlugin_PipeApproxLaw_H_

#include <FeaturesPlugin.h>

#include <GeomAlgoAPI_PipeApprox.h>
#include <GeomAlgoAPI_ParameterLaw.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_ResultParameter.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_Attribute.h>
#include <ModelAPI_ResultBody.h>

#include <memory>
#include <list>
#include <vector>
#include <map>
#include <string> 

class GeomAPI_Shape;
class ModelAPI_ResultBody;

/** \class FeaturesPlugin_Pipe
 *  \ingroup Plugins
 *  \brief Feature for creation of Pipe from the planar face.
 *  Pipe creates the lateral faces based on edges of the base face and
 *  the start and end faces and/or start and end angles.
 */
class FeaturesPlugin_PipeApproxLaw : public ModelAPI_Feature
{
 public:
  /// Pipe kind.
  inline static const std::string& ID()
  {
    static const std::string MY_PipeApproxLaw_ID("PipeApproxLaw");
    return MY_PipeApproxLaw_ID;
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

  /// Attribute name of ParmLaws
  inline static const std::string& VARIABLE_ID()
  {
    static const std::string MY_VARIABLE_ID("Parameter_laws");
    return MY_VARIABLE_ID;
  }

  /// Attribute name of nb cross-sections
  inline static const std::string& NBSECT_ID()
  {
    static const std::string MY_NBSECT_ID("nb_sect");
    return MY_NBSECT_ID;
  }

  /// attribute name of flag of minimizetorsion
  inline static const std::string& FRENET_ID()
  {
    static const std::string MY_FRENET_ID("use_Frenet");
    return MY_FRENET_ID;
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
    static std::string MY_KIND = FeaturesPlugin_PipeApproxLaw::ID();
    return MY_KIND;
  }

  /// Creates a new part document if needed.
  FEATURESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes.
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation.
  FeaturesPlugin_PipeApproxLaw();

private:
  /// Load Naming data structure of the feature to the document.
  void loadNamingDS(GeomAlgoAPI_PipeApprox& thePipeApproxLawAlgo,
                    std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                    std::shared_ptr<GeomAPI_Shape> theBasis);

  /// does the update of sketch that will call "execution" of this feature
  void performSketchIteration();

  bool checkParamLaws();
  void FeaturesPlugin_PipeApproxLaw::getParamLaws(GeomAlgoAPI_ParameterLaw& aParameterLawAlgo);

  AttributeSelectionListPtr mySelection;
  AttributePtr myParameter;

  /// fields needed for iteration of updated sketch
  std::map<int, std::vector<double>> myIterationMOI; ///< empty means iteration is not started

  std::vector<ResultParameterPtr> myVectResultParameterPtr;
  
  AttributeSelectionListPtr myIterationUpdatedFacesSelectionList;
  ListOfShape myIterationLOS;
  
  int myCPT;
  int myNbLaws;
  int myNbSections;
};
#endif
