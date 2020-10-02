// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

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
  /// attribute name of xao file path
  inline static const std::string& XAO_FILE_PATH_ID()
  {
    static const std::string MY_XAO_FILE_PATH_ID("xao_file_path");
    return MY_XAO_FILE_PATH_ID;
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
  /// attribute name of geometry name for XAO format
  inline static const std::string& XAO_GEOMETRY_NAME_ID()
  {
    static const std::string MY_XAO_GEOMETRY_NAME_ID("xao_geometry_name");
    return MY_XAO_GEOMETRY_NAME_ID;
  }

  /// attribute name of root file path
  inline static const std::string& ROOT_FILE_PATH_ID()
  {
    static const std::string MY_ROOT_FILE_PATH_ID("root_file_path");
    return MY_ROOT_FILE_PATH_ID;
  }
  /// attribute name of author for XAO format
  inline static const std::string& ROOT_MANAGER_NAME_ID()
  {
    static const std::string MY_ROOT_MANAGER_NAME_ID("root_manager_name");
    return MY_ROOT_MANAGER_NAME_ID;
  }
  /// attribute name of geometry name for XAO format
  inline static const std::string& ROOT_MANAGER_TITLE_ID()
  {
    static const std::string MY_ROOT_MANAGER_TITLE_ID("root_manager_title");
    return MY_ROOT_MANAGER_TITLE_ID;
  }
  /// attribute name of file export 
  inline static const std::string& EXP_NAME_FILE_ID()
  {
    static const std::string MY_EXP_NAME_FILE_ID("root_name_file");
    return MY_EXP_NAME_FILE_ID;
  }
  /// Attribute name of the main solid.
  inline static const std::string& MAIN_OBJECT_ID()
  {
    static const std::string MY_MAIN_OBJECT_ID("root_main_object");
    return MY_MAIN_OBJECT_ID;
  }
  /// attribute name of materials file
  inline static const std::string& MAT_FILE_ID()
  {
    static const std::string MY_MAT_FILE_ID("mat_file");
    return MY_MAT_FILE_ID;
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

  /// Reimplemented from ModelAPI_Feature::attributeChanged()
  EXCHANGEPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// Computes or recomputes the results
  EXCHANGEPLUGIN_EXPORT virtual void execute();

  /// Reimplemented from ModelAPI_Feature::isMacro(). Returns false.
  // It is macro for not-XAO export. For XAO the feature is kept invisible in the tree
  // for the export to GEOM functionality correct working.
  EXCHANGEPLUGIN_EXPORT virtual bool isMacro() const;

  /// Reimplemented from ModelAPI_Feature::isPreviewNeeded(). Returns false.
  EXCHANGEPLUGIN_EXPORT virtual bool isPreviewNeeded() const { return false; }

  /// Do not put in history.
  /// Since it is not a macro, it is not deleted, but we don't want to see it.
  bool isInHistory()  { return false; }

protected:
  /// Performs export of the file
  EXCHANGEPLUGIN_EXPORT void exportFile(const std::string& theFileName,
                                        const std::string& theFormat);

  /// Performs export to XAO file
  EXCHANGEPLUGIN_EXPORT void exportXAO(const std::string& theFileName);

  /// Performs export to XAO file
  EXCHANGEPLUGIN_EXPORT void exportROOT(const std::string& theFileName);
};

#endif /* EXPORT_EXPORTFEATURE_H_ */
