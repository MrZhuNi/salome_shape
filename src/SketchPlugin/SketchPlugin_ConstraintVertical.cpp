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

#include "SketchPlugin_ConstraintVertical.h"

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_ResultConstruction.h>

#include <SketchPlugin_Line.h>
#include <SketchPlugin_Sketch.h>

#include <SketcherPrs_Factory.h>

#include <Config_PropManager.h>
#include <Events_Loop.h>

SketchPlugin_ConstraintVertical::SketchPlugin_ConstraintVertical()
{
}

void SketchPlugin_ConstraintVertical::initAttributes()
{
  data()->addAttribute(SketchPlugin_Constraint::ENTITY_A(), ModelAPI_AttributeRefAttr::typeId());
}

void SketchPlugin_ConstraintVertical::execute()
{
}

AISObjectPtr SketchPlugin_ConstraintVertical::getAISObject(AISObjectPtr thePrevious)
{
  if (!sketch())
    return thePrevious;

  AISObjectPtr anAIS = SketcherPrs_Factory::verticalConstraint(this, sketch(),
                                                               thePrevious);
  return anAIS;
}

void SketchPlugin_ConstraintVertical::createVerticalFeature(SketchPlugin_Sketch* theSketch,
                                          const std::shared_ptr<ModelAPI_Result>& theLine)
{
  FeaturePtr aFeature;
  if (theSketch) {
    aFeature = theSketch->addFeature(SketchPlugin_ConstraintVertical::ID());
  } else {
    std::shared_ptr<ModelAPI_Document> aDoc = theSketch->document();
    aFeature = aDoc->addFeature(SketchPlugin_ConstraintVertical::ID());
  }

  std::shared_ptr<ModelAPI_Data> aData = aFeature->data();

  std::shared_ptr<ModelAPI_AttributeRefAttr> aRef = std::dynamic_pointer_cast<
      ModelAPI_AttributeRefAttr>(aData->attribute(SketchPlugin_Constraint::ENTITY_A()));
  aRef->setObject(theLine);

  // we need to flush created signal in order to coincidence is processed by solver
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_CREATED));
}

