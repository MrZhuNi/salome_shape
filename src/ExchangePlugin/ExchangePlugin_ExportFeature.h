// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    ExchangePlugin_ExportFeature.h
// Created: May 14, 2015
// Author:  Sergey POKHODENKO

#ifndef EXCHANGEPLUGIN_EXPORTFEATURE_H_
#define EXCHANGEPLUGIN_EXPORTFEATURE_H_

#include <ExchangePlugin.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Result.h>

#include <map>

/**
 * \class ExchangePlugin_ExportFeature
 * \ingroup Plugins
 * \brief Feature for export shapes to the external files in CAD formats.
 *
 * The list of supported formats is defined in the configuration file.
 */
class ExchangePlugin_ExportFeature : public ModelAPI_Feature
{
public:
  /// Feature kind
  inline static const std::string& ID()
  {
    static const std::string MY_EXPORT_ID("Export");
    return MY_EXPORT_ID;
  }
  /// attribute name for file format
  inline static const std::string& EXPORT_TYPE_ID()
  {
    static const std::string MY_EXPORT_TYPE_ID("ExportType");
    return MY_EXPORT_TYPE_ID;
  }
  /// attribute name of file path
  inline static const std::string& FILE_PATH_ID()
  {
    static const std::string MY_FILE_PATH_ID("file_path");
    return MY_FILE_PATH_ID;
  }
  /// attribute name of file format
  inline static const std::string& FILE_FORMAT_ID()
  {
    static const std::string MY_FILE_FORMAT_ID("file_format");
    return MY_FILE_FORMAT_ID;
  }
  /// attribute name of selection list
  inline static const std::string& SELECTION_LIST_ID()
  {
    static const std::string MY_SELECTION_LIST_ID("selection_list");
    return MY_SELECTION_LIST_ID;
  }
  /// attribute name of author for XAO format
  inline static const std::string& XAO_AUTHOR_ID()
  {
    static const std::string MY_XAO_AUTHOR_ID("xao_author");
    return MY_XAO_AUTHOR_ID;
  }
  /// Default constructor
  EXCHANGEPLUGIN_EXPORT ExchangePlugin_ExportFeature();
  /// Default destructor
  EXCHANGEPLUGIN_EXPORT virtual ~ExchangePlugin_ExportFeature();

  /// Returns the unique kind of a feature
  EXCHANGEPLUGIN_EXPORT virtual const std::string& getKind()
  {
    return ExchangePlugin_ExportFeature::ID();
  }

  /// Request for initialization of data model of the feature: adding all attributes
  EXCHANGEPLUGIN_EXPORT virtual void initAttributes();

  /// Computes or recomputes the results
  EXCHANGEPLUGIN_EXPORT virtual void execute();

  /// Reimplemented from ModelAPI_Feature::isMacro(). Returns true.
  EXCHANGEPLUGIN_EXPORT virtual bool isMacro() const { return true; }

  /// Reimplemented from ModelAPI_Feature::isPreviewNeeded(). Returns false.
  EXCHANGEPLUGIN_EXPORT virtual bool isPreviewNeeded() const { return false; }

protected:
  /// Performs export of the file
  EXCHANGEPLUGIN_EXPORT void exportFile(const std::string& theFileName,
                                        const std::string& theFormat);

  /// Performs export to XAO file
  EXCHANGEPLUGIN_EXPORT void exportXAO(const std::string& theFileName);
};

#endif /* EXPORT_EXPORTFEATURE_H_ */
