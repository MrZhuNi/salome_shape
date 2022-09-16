// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#include <GeomAlgoAPI_ROOTExport.h>

#include <OSD_OpenFile.hxx>

#include <fstream>
#include <iostream>

#include <map>
#include <vector>

//=================================================================================================
static std::string doubleToString(const double& theValue)
{
  std::ostringstream str;
  str << theValue;
  return str.str();
}

//=================================================================================================
GeomAlgoAPI_ROOTExport::GeomAlgoAPI_ROOTExport(const std::string& theFileName)
{
  myFileName = theFileName;
}

//=================================================================================================
void GeomAlgoAPI_ROOTExport::buildHead(const std::string& theMethodName, const std::string& theName,
                                       const std::string& theTitle)
{
  myContent << "void " << theMethodName << "() {" << std::endl;
  myContent << "gSystem->Load(\"libGeom\");" << std::endl;
  myContent << "TGeoManager *geom = new TGeoManager(\"" << theName;
  myContent << "\",\"" << theTitle  << "\");" << std::endl;
  myContent << std::endl;
}

//=================================================================================================
void GeomAlgoAPI_ROOTExport::buildEnd(const std::string theSolidName,
                                      const std::string theExportName)
{
  myContent << "// ####################################" << std::endl;
  myContent << "geom->SetTopVolume("  << theSolidName << ");" << std::endl;
  myContent << "geom->CloseGeometry();" << std::endl;
  myContent << theSolidName + "->SetVisContainers(kTRUE);" << std::endl;
  myContent << "geom->SetTopVisible(kTRUE);" << std::endl;
  myContent << "geom->Export(\"" + theExportName + "\");" << std::endl;
  myContent << "geom->CheckOverlaps(0.0001);" << std::endl;
  myContent << "geom->PrintOverlaps();" << std::endl;
  myContent << theSolidName + "->Draw();" << std::endl;
  myContent << "}" << std::endl;
}

//=================================================================================================
void GeomAlgoAPI_ROOTExport::buildMaterialsMedias(
    const std::map<std::string, std::vector<std::string> > theMaterials,
    const std::map<std::string, std::vector<std::string> > theMedias)
{
  const std::map<std::string, std::vector<std::string> >::const_iterator
     anItMat = theMaterials.begin(), aLastMat = theMaterials.end();
  for(; anItMat != aLastMat; anItMat++) {
    std::vector<std::string> aValues = anItMat->second;
    myContent << "TGeoMaterial *" << anItMat->first << " = new TGeoMaterial(\"" << aValues[1] <<
              "\"," << aValues[2] << "," << aValues[3] << "," << aValues[4] << ");" << std::endl;
  }

  const std::map<std::string, std::vector<std::string> >::const_iterator
     anItMed = theMedias.begin(), aLastMed = theMedias.end();
  for(; anItMed != aLastMed; anItMed++) {
    std::vector<std::string> aValues = anItMed->second;
    myContent << "TGeoMedium *" << anItMed->first << " = new TGeoMedium(\"" << aValues[1] <<
              "\"," << aValues[2] << "," << aValues[3] << ");" << std::endl;
  }
  myContent << std::endl;
}

//=================================================================================================
void GeomAlgoAPI_ROOTExport::buildBox(const std::wstring& theObjectName,
                                      const std::vector<double> theCenterDims)
{
  std::string anObjectName(theObjectName.begin(), theObjectName.end());

  myContent << "Double_t point_" << anObjectName << "[3] = {";
  myContent << doubleToString(theCenterDims[0]) << "," << doubleToString(theCenterDims[1]) << ",";
  myContent << (theCenterDims[2]) << "};" << std::endl;

  myContent << "TGeoBBox *" << anObjectName << " = new TGeoBBox(\"";
  myContent << anObjectName << "\",";
  myContent << doubleToString(theCenterDims[3]) << "," << doubleToString(theCenterDims[4]) << ",";
  myContent << doubleToString(theCenterDims[5]) << ",point_";
  myContent << anObjectName << ");" << std::endl << std::endl;
}

//=================================================================================================
bool GeomAlgoAPI_ROOTExport::write()
{
  std::ofstream aFile;
  OSD_OpenStream(aFile, myFileName.c_str(), std::ofstream::out);
  if (!aFile.is_open())
    return false;

  aFile << myContent.str() << std::endl;
  aFile.close();
  return true;
}
