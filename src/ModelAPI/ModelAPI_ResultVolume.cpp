// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#include "ModelAPI_ResultVolume.h"

#include <ModelAPI_BodyBuilder.h>
#include <Events_Loop.h>
#include <ModelAPI_Events.h>

ModelAPI_ResultVolume::ModelAPI_ResultVolume() {}

ModelAPI_ResultVolume::~ModelAPI_ResultVolume() {}

std::string ModelAPI_ResultVolume::groupName()
{
  return ModelAPI_ResultVolume::group();
}

void ModelAPI_ResultVolume::setDisplayed(const bool theDisplay)
{
  ModelAPI_Result::setDisplayed(theDisplay);
  for (int i = 0; i < numberOfSubs(); i++)
    subResult(i)->setDisplayed(theDisplay);
}

void ModelAPI_ResultVolume::store(const GeomShapePtr& theShape,
                                const bool theIsStoreSameShapes)
{
  myBuilder->store(theShape, theIsStoreSameShapes);
  myConnect = ConnectionNotComputed;

  static Events_Loop* aLoop = Events_Loop::loop();
  static Events_ID aRedispEvent = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);
  static const ModelAPI_EventCreator* aECreator = ModelAPI_EventCreator::get();
  aECreator->sendUpdated(data()->owner(), aRedispEvent);

  updateSubs(theShape);
}
