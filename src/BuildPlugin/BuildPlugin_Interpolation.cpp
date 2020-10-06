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
#include <ModelAPI_AttributeRefList.h>

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

std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

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
  data()->addAttribute(CREATION_METHODE_ANALYTICAL_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(EXPRESSION_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(EXPRESSION_ERROR_ID(), ModelAPI_AttributeString::typeId());
  data()->addAttribute(VARIABLE_ID(), ModelAPI_AttributeString::typeId());
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
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), EXPRESSION_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), VARIABLE_ID());

  data()->addAttribute(ARGUMENTS_ID(), ModelAPI_AttributeRefList::typeId());
  data()->reflist(ARGUMENTS_ID())->setIsArgument(false);
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), ARGUMENTS_ID());

}

void BuildPlugin_Interpolation::attributeChanged(const std::string& theID)
{

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
    double aMint = real(MINT_ID())->value(); 
    double aMaxt = real(MAXT_ID())->value();
    int aNbrStep = integer(NUMSTEP_ID())->value(); 
    double scale = (aMaxt - aMint )/aNbrStep; 
    std::cout << "aMint = " << aMint << std::endl;
    std::cout << "aMaxt = " << aMaxt << std::endl;
    std::cout << "aNbrStep = " << aNbrStep << std::endl;
    std::cout << "scale = " << scale << std::endl;

    std::list<std::vector<double> > aCoodPoints;
    for( int step = 0; step < aNbrStep; step++ ){
      std::vector<double> coodPoint;
      double result; 
      double value = step * scale + aMint; 
      //x
      std::string exp = string( XT_ID())->value();
      exp = ReplaceAll(exp,"t", std::to_string(value) ); 
      string(EXPRESSION_ID())->setValue(exp);
      if (!updateExpression(result))
        setError("Expression error.", false);
      coodPoint.push_back(result); 
      //y
      exp = string( YT_ID())->value();
      exp = ReplaceAll(exp,"t", std::to_string(value) ); 
      string(EXPRESSION_ID())->setValue(exp);
      if (!updateExpression(result))
        setError("Expression error.", false);
      coodPoint.push_back(result); 
      //Z
      exp = string( ZT_ID())->value();
      exp = ReplaceAll(exp,"t", std::to_string(value) ); 
      string(EXPRESSION_ID())->setValue(exp);
      if (!updateExpression(result))
        setError("Expression error.", false);
      coodPoint.push_back(result);

      aCoodPoints.push_back(coodPoint);
    }

    std::list<GeomPointPtr> aPoints;
    std::list<std::vector<double> >::const_iterator aItCoodPoints = aCoodPoints.begin();
 
    for( ; aItCoodPoints != aCoodPoints.end(); ++aItCoodPoints ){
      std::cout << "cood = " << "(" << (*aItCoodPoints)[0] << ", "<< 
                                       (*aItCoodPoints)[1] << ", "<< 
                                       (*aItCoodPoints)[2] << " ) "<<  std::endl;
      std::shared_ptr<GeomAPI_Vertex> vertex = 
          GeomAlgoAPI_PointBuilder::vertex( (*aItCoodPoints)[0],
                                            (*aItCoodPoints)[1],
                                            (*aItCoodPoints)[2]);
      aPoints.push_back (vertex->point()); 
    } 
    GeomDirPtr aDirStart(new GeomAPI_Dir( aCoodPoints.front()[0],
                                        aCoodPoints.front()[1], 
                                        aCoodPoints.front()[2]));
    GeomDirPtr aDirEnd(new GeomAPI_Dir( aCoodPoints.back()[0],
                                        aCoodPoints.back()[1], 
                                        aCoodPoints.back()[2]));
    // Create curve from points
    GeomEdgePtr anEdge =
      GeomAlgoAPI_CurveBuilder::edge(aPoints, false, true, aDirStart, aDirEnd);
    if (!anEdge.get()) {
      setError("Error: Result curve is empty.");
      return;
    }

    // Store result.
    ResultBodyPtr aResultBody = document()->createBody(data());
    /*std::set<GeomShapePtr>::const_iterator aContextIt = aContexts.begin();
    for (; aContextIt != aContexts.end(); aContextIt++) {
      aResultBody->storeModified(*aContextIt, anEdge, aContextIt == aContexts.begin());
    }
    std::list<GeomPointPtr>::const_iterator aPointsIt = aPoints.begin();
    GeomAPI_ShapeExplorer anExp(anEdge, GeomAPI_Shape::EDGE);
    for (; anExp.more() && aPointsIt != aPoints.cend(); anExp.next(), ++aPointsIt) {
      GeomShapePtr aPoint = std::dynamic_pointer_cast<GeomAPI_Shape>( *aPointsIt );
      GeomShapePtr anEdge = anExp.current();
      aResultBody->generated(aPoint, anEdge);
    }*/
    int aVertexIndex = 1;
    for (GeomAPI_ShapeExplorer anExp(anEdge, GeomAPI_Shape::VERTEX); anExp.more(); anExp.next()) {
      std::string aVertexName = "Vertex_" + std::to_string((long long)aVertexIndex);
      aResultBody->generated(anExp.current(), aVertexName);
    }

    setResult(aResultBody);
        
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
    // compare the list of parameters to store if changed
  /*  AttributeRefListPtr aParams = reflist(ARGUMENTS_ID());
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


bool BuildPlugin_Interpolation::updateExpression(double& result)
{
  std::wstring anExpression = string(EXPRESSION_ID())->isUValue() ?
      Locale::Convert::toWString(string(EXPRESSION_ID())->valueU()) :
      Locale::Convert::toWString(string(EXPRESSION_ID())->value());

  std::string outErrorMessage;
  result= evaluate(anExpression, outErrorMessage);

  data()->string(EXPRESSION_ERROR_ID())->setValue(outErrorMessage);
  if (!outErrorMessage.empty())
    return false;
  
  std::cout << "result = " << result << std::endl;
  return true;
}
