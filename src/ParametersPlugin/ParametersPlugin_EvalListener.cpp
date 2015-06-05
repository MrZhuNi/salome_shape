/*
 * ParametersPlugin_EvalListener.cpp
 *
 *  Created on: Apr 28, 2015
 *      Author: sbh
 */

#include <pyconfig.h>

#include <ParametersPlugin_EvalListener.h>
#include <ParametersPlugin_Parameter.h>
#include <ParametersPlugin_PyInterp.h>

#include <Events_Error.h>

#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Tools.h>

#include <ModelAPI_AttributeDouble.h>
#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Point2D.h>

#include <string>
#include <sstream>
#include <regex>

//debug
#include <iostream>

ParametersPlugin_EvalListener::ParametersPlugin_EvalListener()
{
  Events_Loop* aLoop = Events_Loop::loop();
  const Events_ID kEvaluationEvent = ModelAPI_AttributeEvalMessage::eventId();
  aLoop->registerListener(this, kEvaluationEvent, NULL, true);
  const Events_ID kObjectRenamedEvent = ModelAPI_ObjectRenamedMessage::eventId();
  aLoop->registerListener(this, kObjectRenamedEvent, NULL, true);

  myInterp = std::shared_ptr<ParametersPlugin_PyInterp>(new ParametersPlugin_PyInterp());
  myInterp->initialize();
}

ParametersPlugin_EvalListener::~ParametersPlugin_EvalListener()
{
}

void ParametersPlugin_EvalListener::processEvent(
    const std::shared_ptr<Events_Message>& theMessage)
{
  if (!theMessage.get())
    return;

  const Events_ID kEvaluationEvent = ModelAPI_AttributeEvalMessage::eventId();
  const Events_ID kObjectRenamedEvent = ModelAPI_ObjectRenamedMessage::eventId();
  if (theMessage->eventID() == kEvaluationEvent) {
    processEvaluationEvent(theMessage);
  } else if (theMessage->eventID() == kObjectRenamedEvent) {
    processObjectRenamedEvent(theMessage);
  } else {
    Events_Error::send(std::string("ParametersPlugin python interpreter, unhandled message caught: ")
                       + theMessage->eventID().eventText());
  }
}

double ParametersPlugin_EvalListener::evaluate(const std::string& theExpression,
                                               std::string& theError)
{
  std::list<std::string> anExprParams = myInterp->compile(theExpression);
  // find expression's params in the model
  std::list<std::string> aContext;
  std::list<std::string>::iterator it = anExprParams.begin();
  for ( ; it != anExprParams.end(); it++) {
    double aValue;
    ResultParameterPtr aParamRes;
    if (!ModelAPI_Tools::findVariable(*it, aValue, aParamRes)) continue;

    std::ostringstream sstream;
    sstream << aValue;
    std::string aParamValue = sstream.str();
    aContext.push_back(*it + "=" + aParamValue);
  }
  myInterp->extendLocalContext(aContext);
  double result = myInterp->evaluate(theExpression, theError);
  myInterp->clearLocalContext();
  return result;
}

void ParametersPlugin_EvalListener::processEvaluationEvent(
    const std::shared_ptr<Events_Message>& theMessage)
{
  std::shared_ptr<ModelAPI_AttributeEvalMessage> aMessage =
      std::dynamic_pointer_cast<ModelAPI_AttributeEvalMessage>(theMessage);

  // Double
  AttributeDoublePtr aDoubleAttribute =
      std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(aMessage->attribute());
  if (aDoubleAttribute.get()) {
    std::string anError;
    double aValue = evaluate(aDoubleAttribute->text(), anError);
    if (anError.empty()) {
      aDoubleAttribute->setValue(aValue);
      aDoubleAttribute->setExpressionInvalid(false);
    } else { // set feature as invalid-parameter arguments
      aDoubleAttribute->setExpressionInvalid(true);
    }
  }

  // Point
  AttributePointPtr aPointAttribute =
      std::dynamic_pointer_cast<GeomDataAPI_Point>(aMessage->attribute());
  if (aPointAttribute.get()) {
    std::string anError[3];
    double aValue[3] = {
        evaluate(aPointAttribute->textX(), anError[0]),
        evaluate(aPointAttribute->textY(), anError[1]),
        evaluate(aPointAttribute->textZ(), anError[2])
    };
    bool isValid[3] = {
        anError[0].empty(),
        anError[1].empty(),
        anError[2].empty()
    };
    aPointAttribute->setExpressionInvalid(0, !isValid[0]);
    aPointAttribute->setExpressionInvalid(1, !isValid[1]);
    aPointAttribute->setExpressionInvalid(2, !isValid[2]);

    aPointAttribute->setValue(
        isValid[0] ? aValue[0] : aPointAttribute->x(),
        isValid[1] ? aValue[1] : aPointAttribute->y(),
        isValid[2] ? aValue[2] : aPointAttribute->z()
    );
  }

  // Point2D
  AttributePoint2DPtr aPoint2DAttribute =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aMessage->attribute());
  if (aPoint2DAttribute.get()) {
    std::string anError[2];
    double aValue[2] = {
        evaluate(aPoint2DAttribute->textX(), anError[0]),
        evaluate(aPoint2DAttribute->textY(), anError[1])
    };
    bool isValid[2] = {
        anError[0].empty(),
        anError[1].empty()
    };
    aPoint2DAttribute->setExpressionInvalid(0, !isValid[0]);
    aPoint2DAttribute->setExpressionInvalid(1, !isValid[1]);

    aPoint2DAttribute->setValue(
        isValid[0] ? aValue[0] : aPoint2DAttribute->x(),
        isValid[1] ? aValue[1] : aPoint2DAttribute->y()
    );
  }
}

std::string ParametersPlugin_EvalListener::renameInPythonExpression(
    const std::string& theExpression,
    const std::string& theOldName,
    const std::string& theNewName)
{
  std::string anExpressionString = theExpression;

  std::list<std::pair<int, int> > aPositions =
      myInterp->positions(anExpressionString, theOldName);

  if (aPositions.empty())
    return anExpressionString;

  std::map<int, std::list<int> > aLines;
  std::list<std::pair<int, int> >::const_iterator it = aPositions.begin();
  for (; it != aPositions.end(); ++it)
    aLines[it->first].push_back(it->second);

  // Start renaming from the end to keep indexes if theNewName is longer then theOldName
  std::map<int, std::list<int> >::const_reverse_iterator ritLine = aLines.rbegin();
  for (; ritLine != aLines.rend(); ++ritLine) {
    // Calculate the start of the line (find the aLineNo occurrence of "\n" )
    int aLineNo = ritLine->first - 1;
    size_t aLineStart = 0;
    for (int i = 0; i < aLineNo; ++i)
      aLineStart = anExpressionString.find("\n", aLineStart) + 1;

    const std::list<int>& aColOffsets = ritLine->second;
    std::list<int>::const_reverse_iterator ritOffset = aColOffsets.rbegin();
    for (; ritOffset != aColOffsets.rend(); ++ritOffset) {
      int anOffset = *ritOffset;
      anExpressionString.replace(aLineStart + anOffset, theOldName.size(), theNewName);
    }
  }

  return anExpressionString;
}

void ParametersPlugin_EvalListener::renameInParameter(
    std::shared_ptr<ParametersPlugin_Parameter> theParameter,
    const std::string& theOldName,
    const std::string& theNewName)
{
  std::shared_ptr<ModelAPI_AttributeString> anExpressionAttribute =
      theParameter->string(ParametersPlugin_Parameter::EXPRESSION_ID());

  std::string anExpressionString = anExpressionAttribute->value();
  anExpressionString = renameInPythonExpression(anExpressionString,
                                                theOldName,
                                                theNewName);
  anExpressionAttribute->setValue(anExpressionString);
}

void renameInAttribute(std::shared_ptr<ModelAPI_Attribute> theAttribute,
                       const std::string& theOldName,
                       const std::string& theNewName)
{
  // TODO: implement

  // dynamic_cast<Double, Point, Point2D>(attribute)
  // rename(attribute)
}

void ParametersPlugin_EvalListener::processObjectRenamedEvent(
    const std::shared_ptr<Events_Message>& theMessage)
{
  std::shared_ptr<ModelAPI_ObjectRenamedMessage> aMessage =
      std::dynamic_pointer_cast<ModelAPI_ObjectRenamedMessage>(theMessage);

  // List of documents to process
  std::list<DocumentPtr> aDocList;
  SessionPtr aSession = ModelAPI_Session::get();
  DocumentPtr aDocument = aSession->activeDocument();
  DocumentPtr aRootDocument = aSession->moduleDocument();
  aDocList.push_back(aDocument);
  if (aDocument != aRootDocument) {
    aDocList.push_back(aRootDocument);
  }
  // Find parameters
  for (std::list<DocumentPtr>::const_iterator it = aDocList.begin(); it != aDocList.end(); ++it) {
    DocumentPtr aDocument = *it;

    for (int anIndex = 0, aSize = aDocument->size(ModelAPI_ResultParameter::group());
         anIndex < aSize; ++anIndex) {
      std::shared_ptr<ModelAPI_ResultParameter> aResultParameter =
          std::dynamic_pointer_cast<ModelAPI_ResultParameter>(
              aDocument->object(ModelAPI_ResultParameter::group(), anIndex));
      if (!aResultParameter.get())
        continue;

      std::shared_ptr<ParametersPlugin_Parameter> aParameter =
          std::dynamic_pointer_cast<ParametersPlugin_Parameter>(
              aDocument->feature(aResultParameter));
      if (!aParameter.get())
        continue;

//      std::cout << "Rename expression of parameter " << aParameter->string(ParametersPlugin_Parameter::VARIABLE_ID())->value()
//          << " (" << aMessage->oldName() << ", " << aMessage->newName() << ")" << std::endl;

      // Rename
      renameInParameter(aParameter, aMessage->oldName(), aMessage->newName());
    }
  }
// TODO: implement renaming in attributes for the next version
/*
  // Find features
  for (std::list<DocumentPtr>::const_iterator it = aDocList.begin(); it != aDocList.end(); ++it) {
    DocumentPtr aDocument = *it;
    for (int anIndex = 0, aSize = aDocument->size(ModelAPI_Feature::group());
         anIndex < aSize; ++anIndex) {
      std::shared_ptr<ModelAPI_Feature> aFeature =
          std::dynamic_pointer_cast<ModelAPI_Feature>(
              aDocument->object(ModelAPI_Feature::group(), anIndex));
      if (!aFeature.get())
        continue;

      std::list<std::shared_ptr<ModelAPI_Attribute> > anAttributes =
          aFeature->data()->attributes(std::string());

      std::list<std::shared_ptr<ModelAPI_Attribute> >::const_iterator it =
          anAttributes.begin();
      for (; it != anAttributes.end(); ++it) {

        std::cout << "Rename expression of attribute " << aFeature->data()->id(*it)
            << " (" << aMessage->oldName() << ", " << aMessage->newName() << ")" << std::endl;

        // Rename
        renameInAttribute(*it, aMessage->oldName(), aMessage->newName());
      }
    }
  }
*/
}

