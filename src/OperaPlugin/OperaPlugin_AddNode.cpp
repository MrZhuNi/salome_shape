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

#include "OperaPlugin_AddNode.h"

#include <ModelAPI_Feature.h>

#include <iostream>

//=================================================================================================
OperaPlugin_AddNode::OperaPlugin_AddNode() // Nothing to do during instantiation
{
}

//=================================================================================================
void OperaPlugin_AddNode::initAttributes()
{
  data()->addAttribute(MAIN_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(TOOL_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
}


//=================================================================================================
void OperaPlugin_AddNode::handleNaming(const GeomMakeShapePtr& theBoolAlgo,
                                       const GeomShapePtr theTool,
                                       const ListOfShape& theToolsList)
{
  //Get result shape
  int anIndexToRemove = 0;
  GeomShapePtr theResShape = theBoolAlgo->shape();

  std::shared_ptr<GeomAlgoAPI_MakeShapeList> aMakeShapeList(new GeomAlgoAPI_MakeShapeList());
  aMakeShapeList->appendAlgo(theBoolAlgo);

  // Result list
  ListOfShape aResList;
  if (!theToolsList.empty())
    for (auto it = theToolsList.begin(); it != theToolsList.end(); it++)
      aResList.push_back(*it);
  aResList.push_front(theResShape);
  aResList.push_back(theTool);

  //Build result compund
  GeomShapePtr aCompound = GeomAlgoAPI_CompoundBuilder::compound(aResList);
  if (aCompound) {
    ResultBodyPtr aResultBody = document()->createBody(data(), anIndexToRemove++);
    aResultBody->storeModified(aResList, aCompound, theBoolAlgo);
    aResultBody->loadModifiedShapes(aMakeShapeList, aCompound, GeomAPI_Shape::VERTEX);
    aResultBody->loadModifiedShapes(aMakeShapeList, aCompound, GeomAPI_Shape::EDGE);
    aResultBody->loadModifiedShapes(aMakeShapeList, aCompound, GeomAPI_Shape::FACE);
    setResult(aResultBody);
  }
}


//=================================================================================================
void OperaPlugin_AddNode::performAlgo(const GeomShapePtr& theObject,
                                      GeomShapePtr theTool,
                                      const ListOfShape& theToolsList)
{
  //Intersecting tool case
  if (theObject->intersect(theTool)){
    std::shared_ptr<GeomAlgoAPI_MakeShape> aCommonAlgo;
    aCommonAlgo.reset(new GeomAlgoAPI_Boolean(theObject,
                                              theTool,
                                              GeomAlgoAPI_Tools::BOOL_COMMON));
    theTool = aCommonAlgo->shape();
  }

  //Perform algorithm
  std::shared_ptr<GeomAlgoAPI_MakeShape> aBoolAlgo;
  aBoolAlgo.reset(new GeomAlgoAPI_Boolean(theObject,
                                          theTool,
                                          GeomAlgoAPI_Tools::BOOL_CUT));
  //Check for error
  std::string anError;
  if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aBoolAlgo, getKind(), anError))
    return setError(anError);

  //Naming
  handleNaming(aBoolAlgo, theTool, theToolsList);
}

//=================================================================================================
void OperaPlugin_AddNode::execute()
{
  ListOfShape aToolList;

  //Get Selection and Shapes
  AttributeSelectionPtr aMainObjectAttr = selection(MAIN_OBJECT_ID());
  AttributeSelectionPtr aToolsAttrList = selection(TOOL_OBJECT_ID());

  // Get Shapes from selection
  GeomShapePtr aMainObjectShape = shapeOfSelection(aMainObjectAttr);
  GeomShapePtr aToolShape = shapeOfSelection(aToolsAttrList);

  // Get older tools and main shape
  if (aMainObjectShape->shapeType() == GeomAPI_Shape::COMPOUND){
    ListOfShape aMainObjectSubShapes = aMainObjectShape->subShapes(GeomAPI_Shape::SOLID);
    for (auto it = aMainObjectSubShapes.begin(); it != aMainObjectSubShapes.end(); it++) {
      if (it == aMainObjectSubShapes.begin())
        aMainObjectShape = *it;
      else
        aToolList.push_back(*it);
    }
  }

  // Get Case
  if (!aMainObjectShape->isIntersect(aToolShape))
    return setError("Error: All tools must intersect the main object");
  if(aMainObjectShape == nullptr )
    return setError("Error: cannot perform an AddNode with no main object.");
  if(aToolShape == nullptr)
    return setError("Error: cannot perform an AddNode with no tool object.");

  //Perform Algorithm
  performAlgo(aMainObjectShape, aToolShape, aToolList);
}
