// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

#include "SketchPlugin_SketchEntity.h"

#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

SketchPlugin_SketchEntity::SketchPlugin_SketchEntity()
: SketchPlugin_Feature()
{
}

void SketchPlugin_SketchEntity::initAttributes()
{
  data()->addAttribute(AUXILIARY_ID(), ModelAPI_AttributeBoolean::typeId());

  initDerivedClassAttributes();

  AttributePtr anAttr = data()->addAttribute(SketchPlugin_SketchEntity::COPY_ID(), ModelAPI_AttributeBoolean::typeId());
  anAttr->setIsArgument(false);
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), SketchPlugin_SketchEntity::COPY_ID());
}
