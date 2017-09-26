// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include "ModelAPI_ResultBody.h"
#include <ModelAPI_BodyBuilder.h>
#include <Events_Loop.h>
#include <ModelAPI_Events.h>

ModelAPI_ResultBody::ModelAPI_ResultBody()
: myBuilder(0)
{
  myConnect = ConnectionNotComputed;
}

ModelAPI_ResultBody::~ModelAPI_ResultBody()
{
  if (myBuilder)
  delete myBuilder;
}

std::string ModelAPI_ResultBody::groupName()
{
  return group();
}

void ModelAPI_ResultBody::store(const std::shared_ptr<GeomAPI_Shape>& theShape,
                                const bool theIsStoreSameShapes)
{
  myBuilder->store(theShape, theIsStoreSameShapes);
  myConnect = ConnectionNotComputed;

  static Events_Loop* aLoop = Events_Loop::loop();
  static Events_ID aRedispEvent = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);
  static const ModelAPI_EventCreator* aECreator = ModelAPI_EventCreator::get();
  aECreator->sendUpdated(data()->owner(), aRedispEvent);
}

void ModelAPI_ResultBody::storeGenerated(const std::shared_ptr<GeomAPI_Shape>& theFromShape,
	                          const std::shared_ptr<GeomAPI_Shape>& theToShape)
{
  myBuilder->storeGenerated(theFromShape, theToShape);
  myConnect = ConnectionNotComputed;

  static Events_Loop* aLoop = Events_Loop::loop();
  static Events_ID aRedispEvent = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);
  static const ModelAPI_EventCreator* aECreator = ModelAPI_EventCreator::get();
  aECreator->sendUpdated(data()->owner(), aRedispEvent);
}

void ModelAPI_ResultBody::storeModified(const std::shared_ptr<GeomAPI_Shape>& theOldShape,
	                          const std::shared_ptr<GeomAPI_Shape>& theNewShape,
                            const int theDecomposeSolidsTag)
{
  myBuilder->storeModified(theOldShape, theNewShape, theDecomposeSolidsTag);
  myConnect = ConnectionNotComputed;

  static Events_Loop* aLoop = Events_Loop::loop();
  static Events_ID aRedispEvent = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);
  static const ModelAPI_EventCreator* aECreator = ModelAPI_EventCreator::get();
  aECreator->sendUpdated(data()->owner(), aRedispEvent);
}

void ModelAPI_ResultBody::storeWithoutNaming(const std::shared_ptr<GeomAPI_Shape>& theShape)
{
  myBuilder->storeWithoutNaming(theShape);
  myConnect = ConnectionNotComputed;

  static Events_Loop* aLoop = Events_Loop::loop();
  static Events_ID aRedispEvent = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);
  static const ModelAPI_EventCreator* aECreator = ModelAPI_EventCreator::get();
  aECreator->sendUpdated(data()->owner(), aRedispEvent);
}

std::shared_ptr<GeomAPI_Shape> ModelAPI_ResultBody::shape()
{
  return myBuilder->shape();
}

void ModelAPI_ResultBody::generated(const std::shared_ptr<GeomAPI_Shape>& theNewShape,
    const std::string& theName, const int theTag)
{
  myBuilder->generated(theNewShape, theName, theTag);
}

void ModelAPI_ResultBody::generated(const std::shared_ptr<GeomAPI_Shape>& theOldShape,
    const std::shared_ptr<GeomAPI_Shape>& theNewShape, const std::string& theName,
    const int theTag)
{
  myBuilder->generated(theOldShape, theNewShape, theName, theTag);
}

void ModelAPI_ResultBody::modified(const std::shared_ptr<GeomAPI_Shape>& theOldShape,
    const std::shared_ptr<GeomAPI_Shape>& theNewShape, const std::string& theName,
    const int theTag)
{
  myBuilder->modified(theOldShape, theNewShape, theName, theTag);
}


void ModelAPI_ResultBody::deleted(
    const std::shared_ptr<GeomAPI_Shape>& theOldShape, const int theTag)
{
  myBuilder->deleted(theOldShape, theTag);
}

void ModelAPI_ResultBody::loadDeletedShapes (GeomAlgoAPI_MakeShape* theMS,
                                  std::shared_ptr<GeomAPI_Shape>  theShapeIn,
                                  const int  theKindOfShape,
                                  const int  theTag)
{
  myBuilder->loadDeletedShapes(theMS, theShapeIn, theKindOfShape, theTag);
}

void ModelAPI_ResultBody::loadAndOrientModifiedShapes (GeomAlgoAPI_MakeShape* theMS,
    std::shared_ptr<GeomAPI_Shape>  theShapeIn, const int  theKindOfShape, const int  theTag,
    const std::string& theName, GeomAPI_DataMapOfShapeShape& theSubShapes,
    const bool theIsStoreSeparate,
    const bool theIsStoreAsGenerated,
    const bool /*theSplitInSubs*/)
{
  myBuilder->loadAndOrientModifiedShapes(
    theMS, theShapeIn, theKindOfShape, theTag, theName, theSubShapes, theIsStoreSeparate,
    theIsStoreAsGenerated);
}

void ModelAPI_ResultBody::loadAndOrientGeneratedShapes (GeomAlgoAPI_MakeShape* theMS,
    std::shared_ptr<GeomAPI_Shape>  theShapeIn, const int  theKindOfShape,
    const int  theTag, const std::string& theName, GeomAPI_DataMapOfShapeShape& theSubShapes)
{
  myBuilder->loadAndOrientGeneratedShapes(
    theMS, theShapeIn, theKindOfShape, theTag, theName, theSubShapes);
}

void ModelAPI_ResultBody::loadFirstLevel(std::shared_ptr<GeomAPI_Shape> theShape,
    const std::string& theName, int&  theTag)
{
  myBuilder->loadFirstLevel(theShape, theName, theTag);
}

void ModelAPI_ResultBody::loadDisconnectedEdges(std::shared_ptr<GeomAPI_Shape> theShape,
    const std::string& theName, int&  theTag)
{
  myBuilder->loadDisconnectedEdges(theShape, theName, theTag);
}

void ModelAPI_ResultBody::loadDisconnectedVertexes(std::shared_ptr<GeomAPI_Shape> theShape,
    const std::string& theName,int&  theTag)
{
  myBuilder->loadDisconnectedVertexes(theShape, theName, theTag);
}

bool ModelAPI_ResultBody::isConnectedTopology()
{
  if (myConnect == ConnectionNotComputed) {
    myConnect = shape()->isConnectedTopology() ? IsConnected : IsNotConnected;
  }
  return myConnect == IsConnected;
}
