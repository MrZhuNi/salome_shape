// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#ifndef GEOMALGOAPI_ROOTEXPORT_H_
#define GEOMALGOAPI_ROOTEXPORT_H_

#include <GeomAlgoAPI.h>

#include <map>
#include <string>
#include <vector>

/**\class GeomAlgoAPI_ROOTExport
 * \ingroup DataAlgo
 * \brief Allows to export results to a root file
 */
class GeomAlgoAPI_ROOTExport
{
public:
  GEOMALGOAPI_EXPORT GeomAlgoAPI_ROOTExport(const std::string& theFileName);

  /// Build the head of file
  GEOMALGOAPI_EXPORT void buildHead(const std::string& theMethodName, const std::string& theName,
                                    const std::string& theTitle);

  /// Build box
  GEOMALGOAPI_EXPORT void buildBox(const std::string& theObjectName,
                                   const double theOX, const double theOY, const double theOZ,
                                   const double theDX, const double theDY, const double theDZ);
  
  /// Build translation
  GEOMALGOAPI_EXPORT void buildTranslation(const std::string& theObjectName,
                                           const double theDX, const double theDY,
                                           const double theDZ);

  /// Build mat and medium
  GEOMALGOAPI_EXPORT void buildMatAndMedium(
      const std::map<std::string,std::vector<std::string> > theMat,
      const std::map<std::string, std::vector<std::string> > theMedium);
  
  ///
  GEOMALGOAPI_EXPORT void BuildVolume(const std::string theName, 
                                      const std::string theGeometryName,
                                      const std::string theMediumName);
  
  /// Build the end of file
  GEOMALGOAPI_EXPORT void buildEnd(const std::string theName, const std::string theExportName);

  /// Write the file
  GEOMALGOAPI_EXPORT bool write();
  
  GEOMALGOAPI_EXPORT const std::string doubleToString(const double& value);

private:
  std::string myFileName;
  std::string myContent;
};

#endif /* GEOMALGOAPI_ROOTEXPORT_H_ */
