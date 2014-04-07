// File:        Model_Document.h
// Created:     28 Feb 2014
// Author:      Mikhail PONIKAROV

#ifndef Model_Document_HeaderFile
#define Model_Document_HeaderFile

#include <Model.h>
#include <ModelAPI_Document.h>
#include <Event_Message.h>

#include <TDocStd_Document.hxx>
#include <map>

class Handle_Model_Document;

/**\class Model_Document
 * \ingroup DataModel
 * \brief Document for internal data structure of any object storage. Corresponds to the SALOME study.
 * Document contains all data of te SALOME Study specific to this module
 * that must be written into the HDF file.
 * Also it provides acces to this data: open/save, transactions management etc.
 * to provide access to all stored data.
 */

class Model_Document: public ModelAPI_Document
{
public:

  //! Loads the OCAF document from the file.
  //! \param theFileName full name of the file to load
  //! \param theStudyID identifier of the SALOME study to associate with loaded file
  //! \returns true if file was loaded successfully
  MODEL_EXPORT bool load(const char* theFileName);

  //! Saves the OCAF document to the file.
  //! \param theFileName full name of the file to store
  //! \returns true if file was stored successfully
  MODEL_EXPORT bool save(const char* theFileName);

  //! Removes document data
  MODEL_EXPORT void close();

  //! Starts a new operation (opens a tansaction)
  MODEL_EXPORT void startOperation();
  //! Finishes the previously started operation (closes the transaction)
  MODEL_EXPORT void finishOperation();
  //! Aborts the operation 
  MODEL_EXPORT void abortOperation();
  //! Returns true if operation has been started, but not yet finished or aborted
  MODEL_EXPORT bool isOperation();
  //! Returns true if document was modified (since creation/opening)
  MODEL_EXPORT bool isModified();

  //! Returns True if there are available Undos
  MODEL_EXPORT bool canUndo();
  //! Undoes last operation
  MODEL_EXPORT void undo();
  //! Returns True if there are available Redos
  MODEL_EXPORT bool canRedo();
  //! Redoes last operation
  MODEL_EXPORT void redo();

  //! Adds to the document the new feature of the given feature id
  //! \param creates feature and puts it in the document
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_Feature> addFeature(std::string theID);

  //! Returns the existing feature by the label
  //! \param theLabel base label of the feature
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_Feature> feature(TDF_Label& theLabel);

  //! Adds a new sub-document by the identifier, or returns existing one if it is already exist
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_Document> subDocument(std::string theDocID);

  //! Creates an iterator of the features by the specific groups
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_Iterator> featuresIterator(
    const std::string theGroup);

  MODEL_EXPORT virtual const std::string& id() const {return myID;}

  //! Returns the feature in the group by the index (started from zero)
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_Feature> 
    feature(const std::string& theGroupID, const int theIndex);

  ///! Returns the vector of groups already added to the document
  MODEL_EXPORT virtual const std::vector<std::string>& getGroups() const;

  //! Returns the index of feature in the group (zero based)
  //! \retruns -1 if not found
  MODEL_EXPORT virtual int featureIndex(std::shared_ptr<ModelAPI_Feature> theFeature);

protected:

  //! Returns (creates if needed) the group label
  TDF_Label groupLabel(const std::string theGroup);

  //! Initializes feature with a unique name in this group (unique name is generated as 
  //! feature type + "_" + index
  void setUniqueName(std::shared_ptr<ModelAPI_Feature> theFeature);

  //! Adds to the document the new feature
  void addFeature(const std::shared_ptr<ModelAPI_Feature> theFeature);

  //! Creates new document with binary file format
  Model_Document(const std::string theID);

  friend class Model_Application;

private:
  std::string myID; ///< identifier of the document in the application
  Handle_TDocStd_Document myDoc; ///< OCAF document
  int myTransactionsAfterSave; ///< number of transactions after the last "save" call, used for "IsModified" method
  std::map<std::string, TDF_Label> myGroups; ///< root labels of the features groups identified by names
  std::vector<std::string> myGroupsNames; ///< names of added groups to the document
};

/// Event ID that model is updated
static const char * EVENT_FEATURE_UPDATED = "FeatureUpdated";

/// Message that feature was changed (used for Object Browser update)
class ModelAPI_FeatureUpdatedMessage : public Event_Message {
  std::shared_ptr<ModelAPI_Feature> myFeature; ///< which feature is changed
public:
  /// sender is not important, all information is located in the feature
  ModelAPI_FeatureUpdatedMessage(std::shared_ptr<ModelAPI_Feature> theFeature);

  /// Returns the ID of this message
  static const Event_ID messageId();

  /// Returns the feature that has been updated
  std::shared_ptr<ModelAPI_Feature> feature();
};

#endif
