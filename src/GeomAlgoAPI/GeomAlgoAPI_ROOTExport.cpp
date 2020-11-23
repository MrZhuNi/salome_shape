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

#include <gp_Ax1.hxx>

#include <OSD_OpenFile.hxx>

#include <fstream>
#include <sstream>
#include <algorithm>


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
  myContent += "\n";
}

//=================================================================================================
void GeomAlgoAPI_ROOTExport::buildBox(const std::string& theObjectName,
                                      const double theOX, const double theOY, const double theOZ,
                                      const double theDX, const double theDY, const double theDZ)
{
  myContent += "Double_t point_"+theObjectName+"[3] = {"+doubleToString(theOX)+",";
  myContent += doubleToString(theOY)+","+doubleToString(theOZ)+"};\n";
  myContent += "TGeoBBox *" + theObjectName + "_tmp = new TGeoBBox(\"" +theObjectName + "_tmp\",";
  myContent += doubleToString(theDX)+","+doubleToString(theDY)+","+doubleToString(theDZ)+",point_";
  myContent += theObjectName + ");\n";
  myContent += "\n";
}

//=================================================================================================
void GeomAlgoAPI_ROOTExport::buildTranslation(const std::string& theObjectName,
                                              const double theDX, const double theDY,
                                              const double theDZ)
{
  myContent += "TGeoTranslation *" + theObjectName;
  myContent += " = new TGeoTranslation(\"" + theObjectName + "\",";
  myContent += doubleToString(theDX) + "," + doubleToString(theDY) + ",";
  myContent += doubleToString(theDZ) + ");\n";
  myContent += "\n";
}

//=================================================================================================
void GeomAlgoAPI_ROOTExport::buildMultiTranslation(const std::vector<std::string>& theObjNames,
                                                   const std::vector<std::vector<std::string>>& theResulNames,
                                                   const double theStep,
                                                   const int theNb, 
                                                   std::shared_ptr<GeomAPI_Ax1> theAxis)
{
  const gp_Ax1& anAxis = theAxis->impl<gp_Ax1>();
  
  for (int i=0; i< theResulNames.size(); i++) {
    std::vector<std::string> aVector = theResulNames[i];
    for (int j=0; j<aVector.size();j++) {
      double aDistance = j*theStep;
      gp_Vec aVec = gp_Vec(anAxis.Direction()) * aDistance;
      myContent += "TGeoTranslation *" + aVector[j];
      myContent += " = new TGeoTranslation(\"" + aVector[j] + "\",";
      myContent += doubleToString(aVec.X()) + "," + doubleToString(aVec.Y()) + ",";
      myContent += doubleToString(aVec.Z()) + ");\n";
      myContent += "\n";
    }
  }
}

//=================================================================================================
void GeomAlgoAPI_ROOTExport::buildPartition(const std::string& theMainName,
                                            const std::string theObjectName,
                                            const std::string theOperationName,
                                            const int theIndex)
{
  myContent += theMainName + "->AddNode(";
  myContent += theObjectName + ", " + intToString(theIndex) + ", ";
  myContent += theOperationName + ");\n";
  myContent += "\n";
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
  myContent += "\n";
}

//=================================================================================================
void GeomAlgoAPI_ROOTExport::buildVolume(const std::string theName,
                                         const std::string theGeometryName,
                                         const std::string theMediumName,
                                         std::vector<std::string> theListMedium)
{
  bool aFound = (std::find(theListMedium.begin(), theListMedium.end(), theMediumName) 
                != theListMedium.end());
  if (aFound) {
    myContent += "TGeoVolume *" + theGeometryName + " = new TGeoVolume(\"" + theName;
    myContent += "\"," + theGeometryName + "_tmp," + theMediumName + ");\n";
    myContent += "\n";
  }
}

//=================================================================================================
void GeomAlgoAPI_ROOTExport::buildEnd(const std::string theSolidName,
                                      const std::string theExportName)
{
  myContent += "// ####################################\n";
  myContent += "geom->SetTopVolume(" + theSolidName + ");\n";
  myContent += "geom->CloseGeometry();\n";
  myContent += theSolidName + "->SetVisContainers(kTRUE);\n";
  myContent += "geom->SetTopVisible(kTRUE);\n";
  myContent += "geom->Export(\"" + theExportName + "\");\n";
  myContent += "geom->CheckOverlaps(0.0001);\n";
  myContent += "geom->PrintOverlaps();\n";
  myContent += theSolidName + "->Draw();\n";
  myContent += "}";
}

//=================================================================================================
bool GeomAlgoAPI_ROOTExport::write()
{
  std::ofstream aFile;
  OSD_OpenStream(aFile, myFileName.c_str(), std::ofstream::out);
  if (!aFile.is_open())
    return false;
    
  aFile << myContent << std::endl;
  aFile.close();
  return true;
}

//=================================================================================================
const std::string GeomAlgoAPI_ROOTExport::intToString(const int& value)
{
    std::ostringstream str;
    str << value;
    return str.str();
}

//=================================================================================================
const std::string GeomAlgoAPI_ROOTExport::doubleToString(const double& value)
{
    std::ostringstream str;
    str << value;
    return str.str();
}
