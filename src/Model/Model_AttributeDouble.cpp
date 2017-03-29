// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_AttributeDouble.cxx
// Created:     2 Apr 2014
// Author:      Mikhail PONIKAROV

#include "Model_AttributeDouble.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_Events.h>
#include <Model_Expression.h>
#include <ModelAPI_Object.h>

Model_AttributeDouble::Model_AttributeDouble(TDF_Label& theLabel)
{
  TDF_Label anExpressionLab = theLabel.FindChild(1);
  myExpression.reset(new Model_ExpressionDouble(anExpressionLab));
  myIsInitialized = myExpression->isInitialized();
}

void Model_AttributeDouble::reinit()
{
  myExpression->reinit();
  myIsInitialized = myExpression->isInitialized();
}

void Model_AttributeDouble::reset()
{
  myExpression->reset();
  myIsInitialized = false;
}

void Model_AttributeDouble::setCalculatedValue(const double theValue)
{
  if (!myIsInitialized || value() != theValue) {
    myExpression->setValue(theValue);
    owner()->data()->sendAttributeUpdated(this);
  }
}

void Model_AttributeDouble::setValue(const double theValue)
{
  setCalculatedValue(text().empty() ? theValue : value());
}

double Model_AttributeDouble::value()
{
  return myExpression->value();
}

void Model_AttributeDouble::setText(const std::string& theValue)
{
  if (text() != theValue) {
    myExpression->setText(theValue);
    // Send it to evaluator to convert text to double and store in the attribute
    ModelAPI_AttributeEvalMessage::send(owner()->data()->attribute(id()), this);
    owner()->data()->sendAttributeUpdated(this);
  }
}

std::string Model_AttributeDouble::text()
{
  return myExpression->text();
}

void Model_AttributeDouble::setExpressionInvalid(const bool theFlag)
{
  myExpression->setInvalid(theFlag);
}

bool Model_AttributeDouble::expressionInvalid()
{
  return myExpression->isInvalid();
}

void Model_AttributeDouble::setExpressionError(const std::string& theError)
{
  if (expressionError() != theError)
    myExpression->setError(theError);
}

std::string Model_AttributeDouble::expressionError()
{
  return myExpression->error();
}

void Model_AttributeDouble::setUsedParameters(const std::set<std::string>& theUsedParameters)
{
  myExpression->setUsedParameters(theUsedParameters);
}

std::set<std::string> Model_AttributeDouble::usedParameters() const
{
  return myExpression->usedParameters();
}
