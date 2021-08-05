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

//=================================================================================================
OperaPlugin_AddNode::OperaPlugin_AddNode() // Nothing to do during instantiation
{
}

//=================================================================================================
void OperaPlugin_AddNode::initAttributes()
{
  data()->addAttribute(MAIN_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(TOOLS_LIST_ID(), ModelAPI_AttributeSelectionList::typeId());
}

//=================================================================================================
bool toolsIntersect(ListOfShape& theToolsList){

  for(auto it = theToolsList.begin(); it != theToolsList.end(); it++)
    for(auto jt = theToolsList.begin(); jt != theToolsList.end(); jt++)
    {
      GeomShapePtr first = *it;
      GeomShapePtr second = *jt;
      if (!(first == second))
        if (first->isIntersect(second))
          return true;
    }
  return false;
}

//=================================================================================================
void OperaPlugin_AddNode::performAlgo(const GeomAlgoAPI_Tools::BOPType theBooleanType,
                                      const GeomShapePtr& theObject,
                                      const ListOfShape& theTools,
                                      const ListOfShape& thePlanes,
                                      int& theResultIndex)
{
  //Perform algorithm
  ListOfShape aListWithObject;
  aListWithObject.push_back(theObject);
  std::shared_ptr<GeomAlgoAPI_MakeShapeList> aMakeShapeList(new GeomAlgoAPI_MakeShapeList());
  std::shared_ptr<GeomAlgoAPI_MakeShape> aBoolAlgo;
  GeomShapePtr aResShape;
  aBoolAlgo.reset(new GeomAlgoAPI_Boolean(aListWithObject,
                                          theTools,
                                          theBooleanType));
  //Check for error
  std::string anError;
  if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aBoolAlgo, getKind(), anError))
    return setError(anError);

  //Naming
  handleNaming(aResShape, aBoolAlgo, aMakeShapeList, theTools);
}

//=================================================================================================
void OperaPlugin_AddNode::handleNaming(GeomShapePtr theResShape,
                                       GeomMakeShapePtr theBoolAlgo,
                                       std::shared_ptr<GeomAlgoAPI_MakeShapeList> theMakeShapeList,
                                       const ListOfShape& theTools)
{
  //Get result shape
  int anIndexToRemove = 0;
  theResShape = theBoolAlgo->shape();
  theMakeShapeList->appendAlgo(theBoolAlgo);
  ListOfShape aTestList = theTools;
  aTestList.push_front(theResShape);

  //Build result compund
  GeomShapePtr aCompound = GeomAlgoAPI_CompoundBuilder::compound(aTestList);
  if (aCompound) {
    ResultBodyPtr aResultBody = document()->createBody(data(), anIndexToRemove++);
    aResultBody->storeModified(aTestList, aCompound, theBoolAlgo);
    aResultBody->loadModifiedShapes(theMakeShapeList, aCompound, GeomAPI_Shape::VERTEX);
    aResultBody->loadModifiedShapes(theMakeShapeList, aCompound, GeomAPI_Shape::EDGE);
    aResultBody->loadModifiedShapes(theMakeShapeList, aCompound, GeomAPI_Shape::FACE);
    setResult(aResultBody);
  }
}

//=================================================================================================
void OperaPlugin_AddNode::execute()
{
  int aResultIndex = 0;
  ListOfShape aPlanes, aToolList;

  //Get Selection and Shapes
  AttributeSelectionPtr aMainObjectAttr = selection(MAIN_OBJECT_ID());
  AttributeSelectionListPtr aToolsAttrList = selectionList(TOOLS_LIST_ID());

  // Get Shapes from selection and test intersection with Main Object
  GeomShapePtr aMainObjectShape = shapeOfSelection(aMainObjectAttr);
  for (int anObjectsIndex = 0; anObjectsIndex < aToolsAttrList->size(); anObjectsIndex++){
    GeomShapePtr currentToolShape = shapeOfSelection(aToolsAttrList->value(anObjectsIndex));
    if (!aMainObjectShape->isIntersect(currentToolShape) || aMainObjectShape->intersect(currentToolShape))
      return setError("Error: All tools must be fully inside the main object");
    aToolList.push_back(currentToolShape);
  }

  // Check tools intersections
  if (toolsIntersect(aToolList))
    return setError("Error: Tools must not intersect each others");

  //Check for error then clean part results
  if(aMainObjectShape == nullptr )
    return setError("Error: cannot perform an AddNode with no main object.");
  if(aToolList.empty())
    return setError("Error: Tools list cannot be empty.");
  aMainObjectAttr->context()->setDisabled(aMainObjectAttr->context(), true); // To avoid activation of the Part result

  //Perform Algorithm
  performAlgo(GeomAlgoAPI_Tools::BOOL_CUT,
              shapeOfSelection(aMainObjectAttr), aToolList, aPlanes,
              aResultIndex);
}
