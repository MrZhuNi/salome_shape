// File:        PartSetPlugin_Part.cxx
// Created:     27 Mar 2014
// Author:      Mikhail PONIKAROV

#include "PartSetPlugin_Part.h"
#include "ModelAPI_Session.h"
#include "ModelAPI_Document.h"
#include "ModelAPI_Data.h"
#include "ModelAPI_AttributeDocRef.h"
#include <ModelAPI_ResultPart.h>

using namespace std;

PartSetPlugin_Part::PartSetPlugin_Part()
{
}

void PartSetPlugin_Part::initAttributes()
{  // all is in part result
}

void PartSetPlugin_Part::execute()
{
  ResultPartPtr aResult = boost::dynamic_pointer_cast<ModelAPI_ResultPart>(firstResult());
  if (!aResult) {
    aResult = document()->createPart(data());
    setResult(aResult);
  }
  /*
  boost::shared_ptr<ModelAPI_AttributeDocRef> aDocRef = aResult->data()->docRef(
      ModelAPI_ResultPart::DOC_REF());
  
  if (!aDocRef->value()) {  // create a document if not yet created
    boost::shared_ptr<ModelAPI_Document> aPartSetDoc =
        ModelAPI_Session::get()->moduleDocument();
    aDocRef->setValue(aPartSetDoc->subDocument(data()->name()));
  }
  */
}

boost::shared_ptr<ModelAPI_Document> PartSetPlugin_Part::documentToAdd()
{
  return ModelAPI_Session::get()->moduleDocument();
}
