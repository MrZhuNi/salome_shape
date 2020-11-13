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

#include <GeomAlgoAPI_PointBuilder.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>

#include <CollectionPlugin_Group.h>
#include <ExchangePlugin_Tools.h>
#include <FeaturesPlugin_Translation.h>
#include <PrimitivesPlugin_Box.h>

#include <fstream>
#include <iostream>

#include "math.h"

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
      //std::cout<<"aType :: "<<aName<<std::endl;
      //std::cout<<"aName :: "<<aData[0]<<std::endl;
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
  //std::cout<<"Size of aMat :: "<<aMat.size()<<std::endl;
  //std::cout<<"Size of aMedias :: "<<aMedias.size()<<std::endl;
}

void ExchangePlugin_ExportRoot::computeBox(FeaturePtr theCurFeature,
                                           double& OX, double& OY, double& OZ,
                                           double& DX, double& DY, double& DZ)
{
  std::string aMethodName =
    theCurFeature->data()->string(PrimitivesPlugin_Box::CREATION_METHOD())->value();
  if (aMethodName == "BoxByDimensions") {
    DX = (theCurFeature->data()->real(PrimitivesPlugin_Box::DX_ID())->value())/2;
    DY = (theCurFeature->data()->real(PrimitivesPlugin_Box::DY_ID())->value())/2;
    DZ = (theCurFeature->data()->real(PrimitivesPlugin_Box::DZ_ID())->value())/2;
    OX = DX;
    OY = DY;
    OZ = DZ;
  } else if (aMethodName == "BoxByTwoPoints") {
    AttributeSelectionPtr aRef1 =
      theCurFeature->data()->selection(PrimitivesPlugin_Box::POINT_FIRST_ID());
    AttributeSelectionPtr aRef2 =
      theCurFeature->data()->selection(PrimitivesPlugin_Box::POINT_SECOND_ID());
    GeomShapePtr aShape1 = aRef1->value();
    if (!aShape1.get())
      aShape1 = aRef1->context()->shape();
    GeomShapePtr aShape2 = aRef2->value();
    if (!aShape2.get())
      aShape2 = aRef2->context()->shape();
    std::shared_ptr<GeomAPI_Pnt> aFirstPoint = GeomAlgoAPI_PointBuilder::point(aShape1);
    std::shared_ptr<GeomAPI_Pnt> aSecondPoint = GeomAlgoAPI_PointBuilder::point(aShape2);
    double x1 = aFirstPoint->x();
    double y1 = aFirstPoint->y();
    double z1 = aFirstPoint->z();
    double x2 = aSecondPoint->x();
    double y2 = aSecondPoint->y();
    double z2 = aSecondPoint->z();
    DX = fabs(x2 -x1)/2;
    DY = fabs(y2 -y1)/2;
    DZ = fabs(z2 -z1)/2;
    OX = fmin(x1,x2)+DX;
    OY = fmin(y1,y2)+DY;
    OZ = fmin(z1,z2)+DZ;
  } else if (aMethodName == "BoxByOnePointAndDims") {
    DX = theCurFeature->data()->real(PrimitivesPlugin_Box::HALF_DX_ID())->value();
    DY = theCurFeature->data()->real(PrimitivesPlugin_Box::HALF_DY_ID())->value();
    DZ = theCurFeature->data()->real(PrimitivesPlugin_Box::HALF_DZ_ID())->value();
    OX = theCurFeature->data()->real(PrimitivesPlugin_Box::OX_ID())->value();
    OY = theCurFeature->data()->real(PrimitivesPlugin_Box::OY_ID())->value();
    OZ = theCurFeature->data()->real(PrimitivesPlugin_Box::OZ_ID())->value();
  }
}

void ExchangePlugin_ExportRoot::computeTranslation(FeaturePtr theCurFeature,
                                                   double& DX, double& DY, double& DZ)
{
  std::string aMethodName =
    theCurFeature->data()->string(FeaturesPlugin_Translation::CREATION_METHOD())->value();
  if (aMethodName == "ByDimensions") {
    DX = theCurFeature->data()->real(FeaturesPlugin_Translation::DX_ID())->value();
    DY = theCurFeature->data()->real(FeaturesPlugin_Translation::DY_ID())->value();
    DZ = theCurFeature->data()->real(FeaturesPlugin_Translation::DZ_ID())->value();
  }
}

void ExchangePlugin_ExportRoot::computeGroup(FeaturePtr theCurFeature,
                                             std::vector<std::string>& theListNames)
{
  //std::cout<<"COMPUTE GROUP"<<std::endl;
  AttributeSelectionListPtr anObjectsSelList =
      theCurFeature->data()->selectionList(CollectionPlugin_Group::LIST_ID());
      
  for(int anObjectsIndex = 0; anObjectsIndex < anObjectsSelList->size(); anObjectsIndex++) {
    std::shared_ptr<ModelAPI_AttributeSelection> anObjectAttr =
      anObjectsSelList->value(anObjectsIndex);
    ObjectPtr anObject = anObjectAttr->contextObject();
    std::string aName = anObject->data()->name();
    //std::cout<<"NAME :: "<<aName<<std::endl;
    theListNames.push_back(aName);
  }
}
