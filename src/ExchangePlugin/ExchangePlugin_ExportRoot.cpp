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

#include <ExchangePlugin_ExportRoot.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>

#include <CollectionPlugin_Group.h>
#include <ExchangePlugin_Tools.h>
#include <PrimitivesPlugin_Box.h>

#include <fstream>
#include <iostream>

void ExchangePlugin_ExportRoot::readFileMat(const std::string theFileMat,
                                std::map<std::string, std::vector<std::string> >& aMat,
                                std::map<std::string, std::vector<std::string> >& aMedias)
{
  std::string aLine;
  std::ifstream aFile (theFileMat);
  if (aFile.is_open())
  {
    while ( getline (aFile,aLine) )
    {
      std::list<std::string> aList = ExchangePlugin_Tools::split(aLine, ' '); 
      std::list<std::string>::iterator itList = aList.begin();
      
      bool isFirst = true;
      std::string aName;
      std::vector<std::string> aData;
      for (; itList != aList.end(); ++itList)
      {
        std::string anElem = *itList;
        if (isFirst) {
          aName = anElem;
          isFirst = false;
        } else {
          aData.push_back(anElem);
        }
      }
      std::cout<<"aType :: "<<aName<<std::endl;
      std::cout<<"aName :: "<<aData[0]<<std::endl;
      if (aName == "mat") {
        aMat[aData[0]] = aData;
      } else if (aName == "medium") {
        aMedias[aData[0]] = aData;
      } else {
        std::cout<<"ERROR : ToDo"<<std::endl;
      }
    }
    
    aFile.close();
  }
  std::cout<<"Size of aMat :: "<<aMat.size()<<std::endl;
  std::cout<<"Size of aMedias :: "<<aMedias.size()<<std::endl;
}

void ExchangePlugin_ExportRoot::computeBox(FeaturePtr theCurFeature,
                                           double& OX, double& OY, double& OZ,
                                           double& DX, double& DY, double& DZ)
{
  std::string aMethodName = theCurFeature->data()->string(PrimitivesPlugin_Box::CREATION_METHOD())->value();
  if (aMethodName == "BoxByDimensions") {
    DX = (theCurFeature->data()->real(PrimitivesPlugin_Box::DX_ID())->value())/2;
    DY = (theCurFeature->data()->real(PrimitivesPlugin_Box::DY_ID())->value())/2;
    DZ = (theCurFeature->data()->real(PrimitivesPlugin_Box::DZ_ID())->value())/2;
    OX = DX;
    OY = DY;
    OZ = DZ;
  } else if (aMethodName == "BoxByTwoPoints") {
    // A completer
  } else if (aMethodName == "BoxByOnePointAndDims") {
    DX = theCurFeature->data()->real(PrimitivesPlugin_Box::HALF_DX_ID())->value();
    DY = theCurFeature->data()->real(PrimitivesPlugin_Box::HALF_DY_ID())->value();
    DY = theCurFeature->data()->real(PrimitivesPlugin_Box::HALF_DZ_ID())->value();
    OX = theCurFeature->data()->real(PrimitivesPlugin_Box::OX_ID())->value();
    OY = theCurFeature->data()->real(PrimitivesPlugin_Box::OY_ID())->value();
    OZ = theCurFeature->data()->real(PrimitivesPlugin_Box::OZ_ID())->value();
  }
}

void ExchangePlugin_ExportRoot::computeGroup(FeaturePtr theCurFeature,
                                             std::vector<std::string>& theListNames)
{
  std::cout<<"COMPUTE GROUP"<<std::endl;
  AttributeSelectionListPtr anObjectsSelList =
      theCurFeature->data()->selectionList(CollectionPlugin_Group::LIST_ID());
      
  for(int anObjectsIndex = 0; anObjectsIndex < anObjectsSelList->size(); anObjectsIndex++) {
    std::shared_ptr<ModelAPI_AttributeSelection> anObjectAttr =
      anObjectsSelList->value(anObjectsIndex);
    ObjectPtr anObject = anObjectAttr->contextObject();
    std::string aName = anObject->data()->name();
    std::cout<<"NAME :: "<<aName<<std::endl;
    theListNames.push_back(aName);
  }
}
