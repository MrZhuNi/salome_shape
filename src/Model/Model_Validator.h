// File:        Model_Validator.hxx
// Created:     2 Jul 2014
// Author:      Mikhail PONIKAROV

#ifndef Model_Validator_HeaderFile
#define Model_Validator_HeaderFile

#include <Model.h>
#include <ModelAPI_Validator.h>
#include <map>
#include <set>

/**\class Model_ValidatorsFactory
 * \ingroup DataModel
 * \breif Manages the registered validators
 *
 * Allows to get a validator by the feature identifier and 
 * the attribute identifier (if attribute is validated).
 * All accessible validators mustbe registered by the ID string first.
 * The instance of this factory can be get in the PluginManager.
 * Keeps the validator objects alive and just returns one of it by request.
 * All the needed information is provided to the validator as an argument,
 * this allows to work with them independently from the feature specific object.
 */
class Model_ValidatorsFactory: public ModelAPI_ValidatorsFactory
{
private:
  std::map<std::string, ModelAPI_Validator*> myIDs; ///< map from ID to registered validator
  /// validators IDs by feature ID
  std::map<std::string, std::set<std::string> > myFeatures; 
  /// set of pairs: validators IDs, list of arguments
  typedef std::set<std::pair<std::string, std::list<std::string> > > AttrValidators;
  /// validators IDs and arguments by feature and attribute IDs
  std::map<std::string, std::map<std::string, AttrValidators> > myAttrs;
public:
  /// Registers the instance of the validator by the ID
  MODEL_EXPORT virtual void registerValidator(
    const std::string& theID, ModelAPI_Validator* theValidator);

  /// Assigns validator to the feature
  MODEL_EXPORT virtual void assignValidator(
    const std::string& theID, const std::string& theFeatureID);

  /// Assigns validator to the attribute of the feature
  MODEL_EXPORT virtual void assignValidator(const std::string& theID, 
    const std::string& theFeatureID, const std::string& theAttrID,
    const std::list<std::string>& theArguments);

  /// Provides a validator for the feature, returns NULL if no validator
  MODEL_EXPORT virtual void validators(const std::string& theFeatureID, 
    std::list<ModelAPI_Validator*>& theResult) const;
  /// Provides a validator for the attribute, returns NULL if no validator
  MODEL_EXPORT virtual void validators(
    const std::string& theFeatureID, const std::string& theAttrID,
    std::list<ModelAPI_Validator*>& theValidators, 
    std::list<std::list<std::string> >& theArguments) const;

  /// Returns the result of "validate" method for attribute of validator.
  /// If validator is not exists, returns true: everything is valid by default.
  //MODEL_EXPORT virtual bool validate(
  //  const boost::shared_ptr<ModelAPI_Feature>& theFeature, const std::string& theAttrID) const;

protected:
  /// Get instance from PluginManager
  Model_ValidatorsFactory();

  friend class Model_PluginManager;
};

#endif
