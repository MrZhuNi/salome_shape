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

#include <pyconfig.h>

#include <BuildPlugin_EvalListener.h>
#include <BuildPlugin_Interpolation.h>
#include <BuildPlugin_PyInterp.h>

#include <Events_InfoMessage.h>

#include <Locale_Convert.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeTables.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeValidator.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_ResultParameter.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Tools.h>

#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Point2D.h>

#include <string>
#include <set>
#include <sstream>

//------------------------------------------------------------------------------
// Tools

std::wstring toString(double theValue)
{
  std::wostringstream sstream;
  // write value in scientific format with 16 digits,
  // thus, not check the dot position
  sstream.precision(16);
  sstream << std::scientific << theValue;
  return sstream.str();
}

std::set<std::wstring> toSet(const std::list<std::wstring>& theContainer)
{
  return std::set<std::wstring>(theContainer.begin(), theContainer.end());
}

//------------------------------------------------------------------------------

BuildPlugin_EvalListener::BuildPlugin_EvalListener()
{
  Events_Loop* aLoop = Events_Loop::loop();

  aLoop->registerListener(this, ModelAPI_AttributeEvalMessage::eventId(), NULL, true);
  aLoop->registerListener(this, ModelAPI_BuildEvalMessage::eventId(), NULL, true);
  aLoop->registerListener(this, ModelAPI_ComputePositionsMessage::eventId(), NULL, true);

  myInterp = std::shared_ptr<BuildPlugin_PyInterp>(new BuildPlugin_PyInterp());
  myInterp->initialize();
}

BuildPlugin_EvalListener::~BuildPlugin_EvalListener()
{
}

double BuildPlugin_EvalListener::evaluate(FeaturePtr theParameter,
  const std::wstring& theExpression, std::string& theError,
  std::list<std::shared_ptr<ModelAPI_ResultParameter> >& theParamsList,
  const bool theIsParameter)
{
  std::list<std::wstring> anExprParams = myInterp->compile(theExpression);
  // find expression's params in the model
  std::list<std::wstring> aContext;
  std::list<std::wstring>::iterator it = anExprParams.begin();
  for ( ; it != anExprParams.end(); it++) {
    double aValue;
    ResultParameterPtr aParamRes;
    // If variable does not exist python interpreter will generate an error. It is OK.
    // But due to the issue 1479 it should not check the history position of parameters relatively
    // to feature that contains expression
    if (!ModelAPI_Tools::findVariable(theIsParameter ? theParameter : FeaturePtr(),
      *it, aValue, aParamRes, theParameter->document()))
      continue;

    if (theIsParameter)
      theParamsList.push_back(aParamRes);
    aContext.push_back(*it + L"=" + toString(aValue));
  }
  myInterp->extendLocalContext(aContext);
  double result = myInterp->evaluate(theExpression, theError);
  myInterp->clearLocalContext();
  return result;
}


void BuildPlugin_EvalListener::processEvent(
    const std::shared_ptr<Events_Message>& theMessage)
{
  if (!theMessage.get())
    return;

  if (theMessage->eventID() == ModelAPI_AttributeEvalMessage::eventId()) {
    processEvaluationEvent(theMessage);
  } else if (theMessage->eventID() == ModelAPI_BuildEvalMessage::eventId()) {
    std::shared_ptr<ModelAPI_BuildEvalMessage> aMsg =
      std::dynamic_pointer_cast<ModelAPI_BuildEvalMessage>(theMessage);
    FeaturePtr aParam = aMsg->parameter();

    AttributeStringPtr anVariableAttr = aParam->string(BuildPlugin_Interpolation::VARIABLE_ID());
    std::wstring anVar = anVariableAttr->isUValue() ?
        Locale::Convert::toWString(anVariableAttr->valueU()) :
        Locale::Convert::toWString(anVariableAttr->value());
    AttributeTablesPtr anValueAttr = aParam->tables(BuildPlugin_Interpolation::VALUE_ID());
    std::string anError;
    std::list<std::shared_ptr<ModelAPI_ResultParameter> > aParamsList;

    AttributeStringPtr anExprAttr;
    ModelAPI_AttributeTables::Value aVal;
    for(int step =0; step < anValueAttr->rows(); step++ ){
        anExprAttr = aParam->string(BuildPlugin_Interpolation::XT_ID());
        std::wstring anExp = anExprAttr->isUValue() ?
        Locale::Convert::toWString(anExprAttr->valueU()) :
        Locale::Convert::toWString(anExprAttr->value());
        aVal.myDouble = evaluate(anVar,anValueAttr->value(step,0).myDouble,
                                 aParam,
                                 anExp,
                                 anError,aParamsList,
                                 true);
        anValueAttr->setValue(aVal,step,1);

        anExprAttr = aParam->string(BuildPlugin_Interpolation::YT_ID());
        anExp = anExprAttr->isUValue() ?
        Locale::Convert::toWString(anExprAttr->valueU()) :
        Locale::Convert::toWString(anExprAttr->value());
        aVal.myDouble = evaluate(anVar,
                                anValueAttr->value(step,0).myDouble,
                                aParam,
                                anExp,
                                anError,
                                aParamsList,
                                true);
        anValueAttr->setValue(aVal,step,2);

        anExprAttr = aParam->string(BuildPlugin_Interpolation::ZT_ID());
        anExp = anExprAttr->isUValue() ?
        Locale::Convert::toWString(anExprAttr->valueU()) :
        Locale::Convert::toWString(anExprAttr->value());
        aVal.myDouble = evaluate(anVar,
                                  anValueAttr->value(step,0).myDouble,
                                  aParam,
                                  anExp,
                                  anError,
                                  aParamsList,
                                  true);
        anValueAttr->setValue(aVal,step,3);
    }

    aMsg->setResults(aParamsList, 1, anError);
  } else if (theMessage->eventID() == ModelAPI_ComputePositionsMessage::eventId()) {
    std::shared_ptr<ModelAPI_ComputePositionsMessage> aMsg =
      std::dynamic_pointer_cast<ModelAPI_ComputePositionsMessage>(theMessage);
    aMsg->setPositions(myInterp->positions(aMsg->expression(), aMsg->parameter()));
  }
}

double BuildPlugin_EvalListener::evaluate(
  std::wstring& theVariable,
  double theValueVariable,
  FeaturePtr theParameter,
  const std::wstring& theExpression,
  std::string& theError,
  std::list<std::shared_ptr<ModelAPI_ResultParameter> >& theParamsList,
  const bool theIsParameter)
{
  std::list<std::wstring> anExprParams = myInterp->compile(theExpression);
  // find expression's params in the model
  std::list<std::wstring> aContext;
  std::list<std::wstring>::iterator it = anExprParams.begin();
  for ( ; it != anExprParams.end(); it++) {
    double aValue;
    ResultParameterPtr aParamRes;
    // If variable does not exist python interpreter will generate an error. It is OK.
    // But due to the issue 1479 it should not check the history position of parameters relatively
    // to feature that contains expression
    if (!ModelAPI_Tools::findVariable(theIsParameter ? theParameter : FeaturePtr(),
      *it, aValue, aParamRes, theParameter->document()))
      continue;

    if (theIsParameter)
      theParamsList.push_back(aParamRes);
    aContext.push_back(*it + L"=" + toString(aValue));
  }
  aContext.push_back(theVariable + L"=" + toString(theValueVariable));
  myInterp->extendLocalContext(aContext);
  double result = myInterp->evaluate(theExpression, theError);
  myInterp->clearLocalContext();
  return result;
}

void BuildPlugin_EvalListener::processEvaluationEvent(
    const std::shared_ptr<Events_Message>& theMessage)
{
  std::shared_ptr<ModelAPI_AttributeEvalMessage> aMessage =
      std::dynamic_pointer_cast<ModelAPI_AttributeEvalMessage>(theMessage);

  std::list<std::shared_ptr<ModelAPI_ResultParameter> > aParamsList;
  FeaturePtr aParamFeature =
    std::dynamic_pointer_cast<ModelAPI_Feature>(aMessage->attribute()->owner());
  if (aMessage->attribute()->attributeType() == ModelAPI_AttributeInteger::typeId()) {
    AttributeIntegerPtr anAttribute =
        std::dynamic_pointer_cast<ModelAPI_AttributeInteger>(aMessage->attribute());
    std::string anError;
    int aValue = (int)evaluate(aParamFeature, anAttribute->text(), anError, aParamsList);
    bool isValid = anError.empty();
    if (isValid)
      anAttribute->setCalculatedValue(aValue);
    anAttribute->setUsedParameters(isValid ?
      toSet(myInterp->compile(anAttribute->text())) : std::set<std::wstring>());
    anAttribute->setExpressionInvalid(!isValid);
    anAttribute->setExpressionError(anAttribute->text().empty() ? "" : anError);
  } else
  if (aMessage->attribute()->attributeType() == ModelAPI_AttributeDouble::typeId()) {
    AttributeDoublePtr anAttribute =
        std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(aMessage->attribute());
    std::string anError;
    double aValue = evaluate(aParamFeature, anAttribute->text(), anError, aParamsList);
    bool isValid = anError.empty();
    if (isValid)
      anAttribute->setCalculatedValue(aValue);
    anAttribute->setUsedParameters(isValid ?
      toSet(myInterp->compile(anAttribute->text())) : std::set<std::wstring>());
    anAttribute->setExpressionInvalid(!isValid);
    anAttribute->setExpressionError(anAttribute->text().empty() ? "" : anError);
  } else
  if (aMessage->attribute()->attributeType() == GeomDataAPI_Point::typeId()) {
    AttributePointPtr anAttribute =
        std::dynamic_pointer_cast<GeomDataAPI_Point>(aMessage->attribute());
    std::wstring aText[] = {
      anAttribute->textX(),
      anAttribute->textY(),
      anAttribute->textZ()
    };
    double aCalculatedValue[] = {
      anAttribute->x(),
      anAttribute->y(),
      anAttribute->z()
    };
    for (int i = 0; i < 3; ++i) {
      std::string anError;
      double aValue = evaluate(aParamFeature, aText[i], anError, aParamsList);
      bool isValid = anError.empty();
      if (isValid) aCalculatedValue[i] = aValue;
      anAttribute->setUsedParameters(i,
        isValid ? toSet(myInterp->compile(aText[i])) : std::set<std::wstring>());
      anAttribute->setExpressionInvalid(i, !isValid);
      anAttribute->setExpressionError(i, aText[i].empty() ? "" : anError);
    }
    anAttribute->setCalculatedValue(aCalculatedValue[0],
                                    aCalculatedValue[1],
                                    aCalculatedValue[2]);
  } else
  if (aMessage->attribute()->attributeType() == GeomDataAPI_Point2D::typeId()) {
    AttributePoint2DPtr anAttribute =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aMessage->attribute());
    std::wstring aText[] = {
      anAttribute->textX(),
      anAttribute->textY()
    };
    double aCalculatedValue[] = {
      anAttribute->x(),
      anAttribute->y()
    };
    for (int i = 0; i < 2; ++i) {
      std::string anError;
      double aValue = evaluate(aParamFeature, aText[i], anError, aParamsList);
      bool isValid = anError.empty();
      if (isValid) aCalculatedValue[i] = aValue;
      anAttribute->setUsedParameters(i,
        isValid ? toSet(myInterp->compile(aText[i])) : std::set<std::wstring>());
      anAttribute->setExpressionInvalid(i, !isValid);
      anAttribute->setExpressionError(i, aText[i].empty() ? "" : anError);
    }
    anAttribute->setCalculatedValue(aCalculatedValue[0],
                                    aCalculatedValue[1]);
  }
}

void BuildPlugin_EvalListener::initDataModel()
{
  myInterp->runString("import salome_iapp;salome_iapp.register_module_in_study(\"Shaper\")");
}
