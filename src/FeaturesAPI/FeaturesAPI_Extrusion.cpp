// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesAPI_Extrusion.cpp
// Created:     09 June 2016
// Author:      Dmitry Bobylev

#include "FeaturesAPI_Extrusion.h"

#include <ModelHighAPI_Double.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
FeaturesAPI_Extrusion::FeaturesAPI_Extrusion(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

//==================================================================================================
FeaturesAPI_Extrusion::FeaturesAPI_Extrusion(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                             const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                             const ModelHighAPI_Double& theSize)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    setSizes(theSize, ModelHighAPI_Double());
  }
}

//==================================================================================================
FeaturesAPI_Extrusion::FeaturesAPI_Extrusion(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                             const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                             const ModelHighAPI_Selection& theDirection,
                                             const ModelHighAPI_Double& theSize)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    fillAttribute(theDirection, mydirection);
    setSizes(theSize, ModelHighAPI_Double());
  }
}

//==================================================================================================
FeaturesAPI_Extrusion::FeaturesAPI_Extrusion(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                             const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                             const ModelHighAPI_Double& theToSize,
                                             const ModelHighAPI_Double& theFromSize)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    setSizes(theToSize, theFromSize);
  }
}

//==================================================================================================
FeaturesAPI_Extrusion::FeaturesAPI_Extrusion(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                             const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                             const ModelHighAPI_Selection& theDirection,
                                             const ModelHighAPI_Double& theToSize,
                                             const ModelHighAPI_Double& theFromSize)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    fillAttribute(theDirection, mydirection);
    setSizes(theToSize, theFromSize);
  }
}

//==================================================================================================
FeaturesAPI_Extrusion::FeaturesAPI_Extrusion(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                             const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                             const ModelHighAPI_Selection& theToObject,
                                             const ModelHighAPI_Double& theToOffset,
                                             const ModelHighAPI_Selection& theFromObject,
                                             const ModelHighAPI_Double& theFromOffset)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    setPlanesAndOffsets(theToObject, theToOffset, theFromObject, theFromOffset);
  }
}

//==================================================================================================
FeaturesAPI_Extrusion::FeaturesAPI_Extrusion(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                             const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                             const ModelHighAPI_Selection& theDirection,
                                             const ModelHighAPI_Selection& theToObject,
                                             const ModelHighAPI_Double& theToOffset,
                                             const ModelHighAPI_Selection& theFromObject,
                                             const ModelHighAPI_Double& theFromOffset)
: ModelHighAPI_Interface(theFeature)
{
  if(initialize()) {
    fillAttribute(theBaseObjects, mybaseObjects);
    fillAttribute(theDirection, mydirection);
    setPlanesAndOffsets(theToObject, theToOffset, theFromObject, theFromOffset);
  }
}

//==================================================================================================
FeaturesAPI_Extrusion::~FeaturesAPI_Extrusion()
{

}

//==================================================================================================
void FeaturesAPI_Extrusion::setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects)
{
  fillAttribute(theBaseObjects, mybaseObjects);

  execute();
}

//==================================================================================================
void FeaturesAPI_Extrusion::setDirection(const ModelHighAPI_Selection& theDirection)
{
  fillAttribute(theDirection, mydirection);

  execute();
}

//==================================================================================================
void FeaturesAPI_Extrusion::setSizes(const ModelHighAPI_Double& theToSize,
                                     const ModelHighAPI_Double& theFromSize)
{
  fillAttribute("BySizes", mycreationMethod);
  fillAttribute(theToSize, mytoSize);
  fillAttribute(theFromSize, myfromSize);

  execute();
}

//==================================================================================================
void FeaturesAPI_Extrusion::setSize(const ModelHighAPI_Double& theSize)
{
  fillAttribute("BySizes", mycreationMethod);
  fillAttribute(theSize, mytoSize);
  fillAttribute(ModelHighAPI_Double(), myfromSize);

  execute();
}

//==================================================================================================
void FeaturesAPI_Extrusion::setPlanesAndOffsets(const ModelHighAPI_Selection& theToObject,
                                                const ModelHighAPI_Double& theToOffset,
                                                const ModelHighAPI_Selection& theFromObject,
                                                const ModelHighAPI_Double& theFromOffset)
{
  fillAttribute("ByPlanesAndOffsets", mycreationMethod);
  fillAttribute(theToObject, mytoObject);
  fillAttribute(theToOffset, mytoOffset);
  fillAttribute(theFromObject, myfromObject);
  fillAttribute(theFromOffset, myfromOffset);

  execute();
}

// TODO(spo): make add* as static functions of the class
//==================================================================================================
ExtrusionPtr addExtrusion(const std::shared_ptr<ModelAPI_Document>& thePart,
                          const std::list<ModelHighAPI_Selection>& theBaseObjects,
                          const ModelHighAPI_Double& theSize)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Extrusion::ID());
  return ExtrusionPtr(new FeaturesAPI_Extrusion(aFeature, theBaseObjects, theSize));
}

//==================================================================================================
ExtrusionPtr addExtrusion(const std::shared_ptr<ModelAPI_Document>& thePart,
                          const std::list<ModelHighAPI_Selection>& theBaseObjects,
                          const ModelHighAPI_Selection& theDirection,
                          const ModelHighAPI_Double& theSize)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Extrusion::ID());
  return ExtrusionPtr(new FeaturesAPI_Extrusion(aFeature, theBaseObjects, theDirection, theSize));
}

//==================================================================================================
ExtrusionPtr addExtrusion(const std::shared_ptr<ModelAPI_Document>& thePart,
                          const std::list<ModelHighAPI_Selection>& theBaseObjects,
                          const ModelHighAPI_Double& theToSize,
                          const ModelHighAPI_Double& theFromSize)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Extrusion::ID());
  return ExtrusionPtr(new FeaturesAPI_Extrusion(aFeature, theBaseObjects, theToSize, theFromSize));
}

//==================================================================================================
ExtrusionPtr addExtrusion(const std::shared_ptr<ModelAPI_Document>& thePart,
                          const std::list<ModelHighAPI_Selection>& theBaseObjects,
                          const ModelHighAPI_Selection& theDirection,
                          const ModelHighAPI_Double& theToSize,
                          const ModelHighAPI_Double& theFromSize)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Extrusion::ID());
  return ExtrusionPtr(new FeaturesAPI_Extrusion(aFeature,
                                                theBaseObjects,
                                                theDirection,
                                                theToSize,
                                                theFromSize));
}

//==================================================================================================
ExtrusionPtr addExtrusion(const std::shared_ptr<ModelAPI_Document>& thePart,
                          const std::list<ModelHighAPI_Selection>& theBaseObjects,
                          const ModelHighAPI_Selection& theToObject,
                          const ModelHighAPI_Double& theToOffset,
                          const ModelHighAPI_Selection& theFromObject,
                          const ModelHighAPI_Double& theFromOffset)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Extrusion::ID());
  return ExtrusionPtr(new FeaturesAPI_Extrusion(aFeature,
                                                theBaseObjects,
                                                theToObject,
                                                theToOffset,
                                                theFromObject,
                                                theFromOffset));
}

//==================================================================================================
ExtrusionPtr addExtrusion(const std::shared_ptr<ModelAPI_Document>& thePart,
                          const std::list<ModelHighAPI_Selection>& theBaseObjects,
                          const ModelHighAPI_Selection& theDirection,
                          const ModelHighAPI_Selection& theToObject,
                          const ModelHighAPI_Double& theToOffset,
                          const ModelHighAPI_Selection& theFromObject,
                          const ModelHighAPI_Double& theFromOffset)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(FeaturesAPI_Extrusion::ID());
  return ExtrusionPtr(new FeaturesAPI_Extrusion(aFeature,
                                                theBaseObjects,
                                                theDirection,
                                                theToObject,
                                                theToOffset,
                                                theFromObject,
                                                theFromOffset));
}
