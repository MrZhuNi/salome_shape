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
#include "OperaAPI_AddNode.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
OperaAPI_AddNode::OperaAPI_AddNode(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}


//==================================================================================================
OperaAPI_AddNode::OperaAPI_AddNode(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                   const ModelHighAPI_Selection& theMainObject,
                                   const std::list<ModelHighAPI_Selection>& theToolsList)
								   : ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(theMainObject, mainObject());
    setToolsList(theToolsList);
  }
}

//==================================================================================================
OperaAPI_AddNode::~OperaAPI_AddNode()
{
}

//==================================================================================================
void OperaAPI_AddNode::setMainObject(const ModelHighAPI_Selection& theMainObject)
{
  fillAttribute(theMainObject, mainObject());
  execute();
}

//==================================================================================================
void OperaAPI_AddNode::setToolsList(const std::list<ModelHighAPI_Selection>& theToolsList)
{
  fillAttribute(theToolsList, toolsList());
  execute();
}

//==================================================================================================
void OperaAPI_AddNode::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeSelectionPtr anAttrObject = aBase->selection(OperaPlugin_AddNode::MAIN_OBJECT_ID());
  theDumper << aBase << " = model.addAddNode(" << aDocName << ", " << anAttrObject << ", ";

  AttributeSelectionListPtr anAttrList =
    aBase->selectionList(OperaPlugin_AddNode::TOOLS_LIST_ID());
  theDumper << anAttrList << ")" << std::endl;
}

//==================================================================================================
AddNodePtr addAddNode(const std::shared_ptr<ModelAPI_Document>& thePart,
                      const ModelHighAPI_Selection& theMainObject,
                      const std::list<ModelHighAPI_Selection>& theToolsList)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(OperaAPI_AddNode::ID());
  return AddNodePtr(new OperaAPI_AddNode(aFeature, theMainObject, theToolsList));
}
