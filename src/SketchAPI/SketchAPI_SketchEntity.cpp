// Name   : SketchAPI_SketchEntity.cpp
// Purpose: 
//
// History:
// 07/06/16 - Sergey POKHODENKO - Creation of the file

//--------------------------------------------------------------------------------------
#include "SketchAPI_SketchEntity.h"
//--------------------------------------------------------------------------------------
#include <ModelHighAPI_Tools.h>
//--------------------------------------------------------------------------------------
SketchAPI_SketchEntity::SketchAPI_SketchEntity(
    const std::shared_ptr<ModelAPI_Feature> & theFeature)
: ModelHighAPI_Interface(theFeature)
{
  initialize();
}

SketchAPI_SketchEntity::~SketchAPI_SketchEntity()
{

}

//--------------------------------------------------------------------------------------
bool SketchAPI_SketchEntity::initialize()
{
  SET_ATTRIBUTE(Auxiliary, ModelAPI_AttributeBoolean, SketchPlugin_SketchEntity::AUXILIARY_ID())

  return true;
}

//--------------------------------------------------------------------------------------
std::shared_ptr<ModelAPI_AttributeBoolean> SketchAPI_SketchEntity::auxiliary() const
{
  return myAuxiliary;
}

void SketchAPI_SketchEntity::setAuxiliary(bool theAuxiliary)
{
  fillAttribute(theAuxiliary, myAuxiliary);

  execute();
}

//--------------------------------------------------------------------------------------
