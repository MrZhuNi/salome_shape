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

#include "OperaAPI_Volume.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
OperaAPI_Volume::OperaAPI_Volume(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
OperaAPI_Volume::OperaAPI_Volume(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                       const ModelHighAPI_Double& theMedium,
                                       const std::list<ModelHighAPI_Selection>& theObjectList)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(OperaPlugin_Volume::MEDIUM(), medium());
    setObjectList(theObjectList);
  }
}

//==================================================================================================
OperaAPI_Volume::~OperaAPI_Volume() {}

//==================================================================================================
void OperaAPI_Volume::setMedium(const ModelHighAPI_Double& theMedium)
{
  fillAttribute(OperaPlugin_Volume::MEDIUM(), medium());

  execute();
}

//==================================================================================================
void OperaAPI_Volume::setObjectList(const std::list<ModelHighAPI_Selection>& theObjectList)
{
  fillAttribute(theObjectList, myvolumeList);

  execute();
}

//==================================================================================================
void OperaAPI_Volume::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeStringPtr anAttrMedium = aBase->string(OperaPlugin_Volume::MEDIUM());
  theDumper << aBase << " = model.addVolume(" << aDocName << ", " << anAttrMedium << ", ";

  AttributeSelectionListPtr anAttrList = aBase->selectionList(OperaPlugin_Volume::VOLUME_LIST_ID());
  if (anAttrList->isWholeResultAllowed() && !anAttrList->selectionType().empty())
    theDumper<<"\""<<anAttrList->selectionType()<<"\", ";
  theDumper << anAttrList;
  if (anAttrList->isGeometricalSelection())
    theDumper <<", True";
  theDumper << ")" << std::endl;
}

//==================================================================================================
VolumePtr addVolume(const std::shared_ptr<ModelAPI_Document>& thePart,
                    const ModelHighAPI_Double& theMedium,
                    const std::list<ModelHighAPI_Selection>& theObjectList)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(OperaAPI_Volume::ID());
  //aFeature->selectionList(OperaPlugin_Volume::VOLUME_LIST_ID())->setGeometricalSelection(true);
  return VolumePtr(new OperaAPI_Volume(aFeature, theMedium, theObjectList));
}
