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

#ifndef EXCHANGEPLUGIN_EXPORTROOT_H_
#define EXCHANGEPLUGIN_EXPORTROOT_H_

#include <ExchangePlugin.h>
#include <ModelAPI_Feature.h>

#include <map>

/**
 * \class ExchangePlugin_Tools
 * \brief Internal tools for the ExchangePlugin.
 */
class EXCHANGEPLUGIN_EXPORT ExchangePlugin_ExportRoot {
public:
  /// Compute the list of materials and medias
  static void readFileMat(const std::string theFileMat,
                          std::map<std::string, std::vector<std::string> >& aMat,
                          std::map<std::string, std::vector<std::string> >& aMedias);
  
  /// Compute ....
  static void computeBox(FeaturePtr theCurFeature,
                         double& OX, double& OY, double& OZ, double& DX, double& DY, double& DZ);
  
  /// Compute ....
  static void computeGroup(FeaturePtr theCurFeature, std::vector<std::string>& theListNames);
};

#endif /* EXCHANGEPLUGIN_EXPORTROOT_H_ */
