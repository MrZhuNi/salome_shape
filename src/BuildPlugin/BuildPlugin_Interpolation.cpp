// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#include "BuildPlugin_Interpolation.h"

#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_ResultParameter.h>

#include <Events_InfoMessage.h>

#include <Locale_Convert.h>

#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAlgoAPI_CurveBuilder.h>
#include <GeomAlgoAPI_PointBuilder.h>

#include <GeomAPI_Edge.h>
#include <GeomAPI_Lin.h>
#include <GeomAPI_ShapeExplorer.h>
#include <ModelAPI_Tools.h>

#include <algorithm>

#include <iostream>

//=================================================================================================
BuildPlugin_Interpolation::BuildPlugin_Interpolation()
{
}

//=================================================================================================
void BuildPlugin_Interpolation::initAttributes()
{
  data()->addAttribute(BASE_OBJECTS_ID(), ModelAPI_AttributeSelectionList::typeId());
  data()->addAttribute(CLOSED_ID(), ModelAPI_AttributeBoolean::typeId());
  data()->addAttribute(REORDER_ID(), ModelAPI_AttributeBoolean::typeId());
  data()->addAttribute(USE_TANGENTS_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(TANGENT_START_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(TANGENT_END_ID(), ModelAPI_AttributeSelection::typeId());

  data()->addAttribute(CREATION_METHODE_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(CREATION_METHODE_BY_SELECTION_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(CREATION_METHODE_ANALYTICAL_ID(), ModelAPI_AttributeString::typeId());

  data()->addAttribute(EXPRESSION_ERROR_ID(), ModelAPI_AttributeString::typeId());
  data()->string(EXPRESSION_ERROR_ID())->setIsArgument(false);
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), EXPRESSION_ERROR_ID());

    data()->addAttribute(XT_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(YT_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(ZT_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(MINT_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(MAXT_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(NUMSTEP_ID(), ModelAPI_AttributeInteger::typeId());
  
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), CREATION_METHODE_ANALYTICAL_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), CREATION_METHODE_BY_SELECTION_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), YT_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), ZT_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), MINT_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), MAXT_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), NUMSTEP_ID()); 

}

//=================================================================================================
static GeomDirPtr selectionToDir(const AttributeSelectionPtr& theSelection)
{
  GeomDirPtr aDir;
  GeomEdgePtr anEdge;

  GeomShapePtr aShape = theSelection->value();
  if (!aShape && theSelection->context()) {
    aShape = theSelection->context()->shape();
  }

  if (aShape && aShape->isEdge()) {
    anEdge = GeomEdgePtr(new GeomAPI_Edge(aShape));
  }

  if (anEdge && anEdge->isLine()) {
    aDir = anEdge->line()->direction();
  }

  return aDir;
}

//=================================================================================================
void BuildPlugin_Interpolation::execute()
{
  if( string(CREATION_METHODE_ID())->value() == CREATION_METHODE_BY_SELECTION_ID() )
  {
    // Get closed flag value
    bool isClosed = boolean(CLOSED_ID())->value();

    // Get reorder flag value
    bool isToReorder = boolean(REORDER_ID())->value();

    // Get use tangents flag value
    bool isToUseTangents = isClosed? false : (!string(USE_TANGENTS_ID())->value().empty());

    // Get tangent for start and end points
    GeomDirPtr aDirStart, aDirEnd;
    if (isToUseTangents) {
      aDirStart = selectionToDir(selection(TANGENT_START_ID()));
      aDirEnd = selectionToDir(selection(TANGENT_END_ID()));
    }

    // Get base objects list.
    AttributeSelectionListPtr aSelectionList = selectionList(BASE_OBJECTS_ID());

    // Collect points.
    std::list<GeomPointPtr> aPoints;
    std::set<GeomShapePtr> aContexts;
    for (int anIndex = 0; anIndex < aSelectionList->size(); ++anIndex) {
      AttributeSelectionPtr aSelection = aSelectionList->value(anIndex);

      GeomShapePtr aContextShape = aSelection->context()->shape();
      aContexts.insert(aContextShape);

      GeomShapePtr aShape = aSelection->value();
      if (!aShape.get()) {
        aShape = aContextShape;
      }

      GeomPointPtr aPoint = GeomAlgoAPI_PointBuilder::point(aShape);
      aPoints.push_back(aPoint);
    }

    // Create curve from points
    GeomEdgePtr anEdge =
      GeomAlgoAPI_CurveBuilder::edge(aPoints, isClosed, isToReorder, aDirStart, aDirEnd);
    if (!anEdge.get()) {
      setError("Error: Result curve is empty.");
      return;
    }

    // Store result.
    ResultBodyPtr aResultBody = document()->createBody(data());
    std::set<GeomShapePtr>::const_iterator aContextIt = aContexts.begin();
    for (; aContextIt != aContexts.end(); aContextIt++) {
      aResultBody->storeModified(*aContextIt, anEdge, aContextIt == aContexts.begin());
    }
    int aVertexIndex = 1;
    for (GeomAPI_ShapeExplorer anExp(anEdge, GeomAPI_Shape::VERTEX); anExp.more(); anExp.next()) {
      std::string aVertexName = "Vertex_" + std::to_string((long long)aVertexIndex);
      aResultBody->generated(anExp.current(), aVertexName);
    }

    setResult(aResultBody);
  }else
  {
      string(EXPRESSION_ERROR_ID())->setValue("rien");
      string(EXPRESSION_ID())->setValue(string( XT_ID())->value());
      string(VARIABLE_ID())->setValue("t");
      updateName();
      double res;
      if (!updateExpression(res))
        setError("Expression error.", false);
      std::cout << "res = " << res << std::endl;
  }

}

double BuildPlugin_Interpolation::evaluate(const std::wstring& /*theExpression*/,
                                            std::string& theError)
{
  FeaturePtr aMyPtr = std::dynamic_pointer_cast<ModelAPI_Feature>(data()->owner());
  std::shared_ptr<ModelAPI_ParameterEvalMessage> aProcessMessage =
    ModelAPI_ParameterEvalMessage::send(aMyPtr, this);

  double aResult = 0;
  if (aProcessMessage->isProcessed()) {
    const std::list<ResultParameterPtr>& aParamsList = aProcessMessage->params();
    aResult = aProcessMessage->result();
    theError = aProcessMessage->error();
    /* compare the list of parameters to store if changed
    AttributeRefListPtr aParams = reflist(ARGUMENTS_ID());
    bool aDifferent = aParams->size() != (int)aParamsList.size();
    if (!aDifferent) {
      std::list<ResultParameterPtr>::const_iterator aNewIter = aParamsList.begin();
      std::list<ObjectPtr> anOldList = aParams->list();
      std::list<ObjectPtr>::const_iterator anOldIter = anOldList.begin();
      for(; !aDifferent && aNewIter != aParamsList.end(); aNewIter++, anOldIter++) {
        if (*aNewIter != *anOldIter)
          aDifferent = true;
      }
    }
    if (aDifferent) {
      aParams->clear();
      std::list<ResultParameterPtr>::const_iterator aNewIter = aParamsList.begin();
      for(; aNewIter != aParamsList.end(); aNewIter++) {
        aParams->append(*aNewIter);
      }
    }*/
  } else { // error: python interpreter is not active
    theError = "Python interpreter is not available";
  }
  return aResult;
}


bool BuildPlugin_Interpolation::updateExpression(double& aValue)
{
  std::wstring anExpression = string(EXPRESSION_ID())->isUValue() ?
      Locale::Convert::toWString(string(EXPRESSION_ID())->valueU()) :
      Locale::Convert::toWString(string(EXPRESSION_ID())->value());

  std::string outErrorMessage;
  aValue = evaluate(anExpression, outErrorMessage);

  data()->string(EXPRESSION_ERROR_ID())->setValue(outErrorMessage);
  if (!outErrorMessage.empty())
    return false;

  /*ResultParameterPtr aParam = document()->createParameter(data());
  AttributeDoublePtr aValueAttribute = aParam->data()->real(ModelAPI_ResultParameter::VALUE());
  aValueAttribute->setValue(aValue);
  setResult(aParam);*/
  
  return true;
}

void BuildPlugin_Interpolation::updateName()
{
  std::wstring aName = string(VARIABLE_ID())->isUValue() ?
      Locale::Convert::toWString(string(VARIABLE_ID())->valueU()) :
      Locale::Convert::toWString(string(VARIABLE_ID())->value());
  data()->setName(aName);

  ResultParameterPtr aParam = document()->createParameter(data());
  std::wstring anOldName = aParam->data()->name();
  aParam->data()->setName(aName);
  setResult(aParam);


  // #2474 : if parameter name now hides/shows the higher level parameter name,
  // update the depended expressions
  DocumentPtr aRootDoc = ModelAPI_Session::get()->moduleDocument();
  if (aParam->document() != aRootDoc) {
    std::list<std::wstring> aNames; // collect names in the root document that must be checked
    aNames.push_back(aName);
    if (anOldName != aName) {
      aNames.push_back(anOldName);
    }
    std::list<std::wstring>::iterator aNIter = aNames.begin();
    for (; aNIter != aNames.end(); aNIter++) {
      double aValue;
      ResultParameterPtr aRootParam;
      FeaturePtr aThis =
        std::dynamic_pointer_cast<ModelAPI_Feature>(string(VARIABLE_ID())->owner());
      if (ModelAPI_Tools::findVariable(aThis, *aNIter, aValue, aRootParam, aRootDoc)) {
        std::set<std::shared_ptr<ModelAPI_Attribute> > anAttributes =
          aRootParam->data()->refsToMe();
        std::set<std::shared_ptr<ModelAPI_Attribute> >::const_iterator anAttributeIt =
          anAttributes.cbegin();
        for (; anAttributeIt != anAttributes.cend(); ++anAttributeIt) {
          const AttributePtr& anAttribute = *anAttributeIt;
          ModelAPI_AttributeEvalMessage::send(anAttribute, NULL);
        }
      }
    }
  }
}