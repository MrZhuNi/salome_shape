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

#include <GeomAlgoAPI_ROOTExport.h>

#include <OSD_OpenFile.hxx>

#include <fstream>
#include <sstream>


//=================================================================================================
GeomAlgoAPI_ROOTExport::GeomAlgoAPI_ROOTExport(const std::string& theFileName)
{
  myFileName = theFileName;
  myContent = "";
}

//=================================================================================================
void GeomAlgoAPI_ROOTExport::buildHead(const std::string& theMethodName,
                                       const std::string& theName,
                                       const std::string& theTitle)
{
  std::string tmp = "void " + theMethodName + "() {\n";
  myContent += tmp;
  tmp = "gSystem->Load(\"libGeom\");\n";
  myContent += tmp;
  tmp = "TGeoManager *geom = new TGeoManager(\"" + theName + "\",\"" + theTitle +"\");\n";
  myContent += tmp;
}

//=================================================================================================
void GeomAlgoAPI_ROOTExport::buildBox(const std::string& theObjectName,
                                      const double theOX, const double theOY, const double theOZ,
                                      const double theDX, const double theDY, const double theDZ)
{
  std::cout<<"buildBox"<<std::endl;
  myContent += "Double_t point_"+theObjectName+"[3] = {"+doubleToString(theOX)+",";
  myContent += doubleToString(theOY)+","+doubleToString(theOZ)+"};\n";
  myContent += "TGeoBBox* " + theObjectName + "= new TGeoBBox(\"" +theObjectName + "\",";
  myContent += doubleToString(theDX)+","+doubleToString(theDY)+","+doubleToString(theDZ)+",point_";
  myContent += theObjectName + ");\n";
}


//=================================================================================================
void GeomAlgoAPI_ROOTExport::buildMatAndMedium(
    const std::map<std::string,std::vector<std::string> > theMat,
    const std::map<std::string, std::vector<std::string> > theMedium)
{
  std::map<std::string, std::vector<std::string> >::const_iterator anIt = theMat.begin(),
                                                                   aLast = theMat.end();
  for (; anIt != aLast; anIt++) {
    std::vector<std::string> aValues = anIt->second;
    myContent += "TGeoMaterial *" + anIt->first + " = new TGeoMaterial(\"";
    myContent += aValues[1] + "\"," + aValues[2] + "," + aValues[3] + "," + aValues[4] + ") ;\n";
  }
  
  std::map<std::string, std::vector<std::string> >::const_iterator anIt2 = theMedium.begin(),
                                                                   aLast2 = theMedium.end();
  for (; anIt2 != aLast2; anIt2++) {
    std::vector<std::string> aValues = anIt2->second;
    myContent += "TGeoMedium *" + anIt2->first + " = new TGeoMedium(\"";
    myContent += aValues[1] + "\"," + aValues[2] + "," + aValues[3] + ") ;\n";
  }
}

//=================================================================================================
void GeomAlgoAPI_ROOTExport::BuildVolume(const std::string theName,
                                         const std::string theGeometryName,
                                         const std::string theMediumName)
{
  myContent += "TGeoVolume *" + theName + " = new TGeoVolume(\"" + theName;
  myContent += "\"," + theGeometryName + "," + theMediumName + ");\n";
}

//=================================================================================================
void GeomAlgoAPI_ROOTExport::buildEnd()
{
  myContent += "}";
}

//=================================================================================================
bool GeomAlgoAPI_ROOTExport::write()
{
  std::ofstream aFile;
  std::cout<<myFileName.c_str()<<std::endl;
  OSD_OpenStream(aFile, myFileName.c_str(), std::ofstream::out);
  if (!aFile.is_open())
    return false;
    
  aFile << myContent << std::endl;
  aFile.close();
  return true;
}

//=================================================================================================
const std::string GeomAlgoAPI_ROOTExport::doubleToString(const double& value)
{
    std::ostringstream str;
    str << value;
    return str.str();
}
