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

#include <OperaPlugin_Volume.h>

#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeIntArray.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_ResultVolume.h>
#include <ModelAPI_Tools.h>

#include <sstream>
#include <memory>
#include <iostream>

//=================================================================================================
OperaPlugin_Volume::OperaPlugin_Volume() // Nothing to do during instantiation
{
}

//=================================================================================================
void OperaPlugin_Volume::initAttributes()
{
  //Get Medium
  data()->addAttribute(OperaPlugin_Volume::MEDIUM(), ModelAPI_AttributeString::typeId());
  //Get Objects
  AttributeSelectionListPtr aList = std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(
    data()->addAttribute(LIST_ID(), ModelAPI_AttributeSelectionList::typeId()));
  aList->setWholeResultAllowed(true); // allow to select the whole result
}

//=================================================================================================
void OperaPlugin_Volume::execute()
{
  // Getting objects.
  std::cout << "Before" << std::endl;

  if (results().empty() || firstResult()->isDisabled()) { // just create result if not exists
    ResultPtr aGroup = document()->createVolume(data());
    setResult(aGroup);
  }

  std::cout << "After" << std::endl;
}

//=================================================================================================
void OperaPlugin_Volume::loadNamingDS(std::shared_ptr<GeomAlgoAPI_Box> theBoxAlgo,
                                        std::shared_ptr<ModelAPI_ResultBody> theResultBox)
{
  // Load the result
  theResultBox->store(theBoxAlgo->shape());

  // Prepare the naming
  theBoxAlgo->prepareNamingFaces();

  // Insert to faces
  std::map< std::string, std::shared_ptr<GeomAPI_Shape> > listOfFaces =
    theBoxAlgo->getCreatedFaces();
  for (std::map< std::string, std::shared_ptr<GeomAPI_Shape> >::iterator it = listOfFaces.begin();
       it != listOfFaces.end();
       ++it)
  {
    theResultBox->generated((*it).second, (*it).first);
  }
}
