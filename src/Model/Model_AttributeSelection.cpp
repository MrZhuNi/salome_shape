// File:        Model_AttributeSelection.h
// Created:     2 Oct 2014
// Author:      Mikhail PONIKAROV

#include "Model_AttributeSelection.h"
#include "Model_Application.h"
#include "Model_Events.h"
#include "Model_Data.h"
#include <ModelAPI_Feature.h>

#include <TNaming_Selector.hxx>
#include <TNaming_NamedShape.hxx>
#include <TopoDS_Shape.hxx>

using namespace std;

void Model_AttributeSelection::setValue(const ResultBodyPtr& theContext,
  const boost::shared_ptr<GeomAPI_Shape>& theSubShape)
{
  const boost::shared_ptr<GeomAPI_Shape>& anOldShape = value();
  bool isOldShape = 
    (theSubShape == anOldShape || (theSubShape && anOldShape && theSubShape->isEqual(anOldShape)));
  if (isOldShape) return; // shape is the same, so context is also unchanged
  // update the referenced object if needed
  bool isOldContext = theContext == myRef.value();
  if (!isOldContext)
    myRef.setValue(theContext);
  
  // perform the selection
  TNaming_Selector aSel(myRef.myRef->Label());
  TopoDS_Shape aNewShape = theSubShape ? theSubShape->impl<TopoDS_Shape>() : TopoDS_Shape();
  TopoDS_Shape aContext = theContext->shape()->impl<TopoDS_Shape>();
  aSel.Select(aNewShape, aContext);
  myIsInitialized = true;

  owner()->data()->sendAttributeUpdated(this);
}

boost::shared_ptr<GeomAPI_Shape> Model_AttributeSelection::value()
{
  boost::shared_ptr<GeomAPI_Shape> aResult;
  if (myIsInitialized) {
    Handle(TNaming_NamedShape) aSelection;
    if (myRef.myRef->Label().FindAttribute(TNaming_NamedShape::GetID(), aSelection)) {
      TopoDS_Shape aSelShape = aSelection->Get();
      aResult->setImpl(&aSelShape);
    }
  }
  return aResult;
}

Model_AttributeSelection::Model_AttributeSelection(TDF_Label& theLabel)
  : myRef(theLabel)
{
  myIsInitialized = myRef.isInitialized();
}

ResultBodyPtr Model_AttributeSelection::context() {
  return boost::dynamic_pointer_cast<ModelAPI_ResultBody>(myRef.value());
}
