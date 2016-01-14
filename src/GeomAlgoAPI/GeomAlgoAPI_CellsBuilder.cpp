// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_CellsBuilder.h
// Created:     11 December 2015
// Author:      Dmitry Bobylev

#include "GeomAlgoAPI_CellsBuilder.h"

#include <GeomAlgoAPI_ShapeTools.h>

#include <BOPAlgo_CellsBuilder.hxx>
#include <TopoDS_Builder.hxx>

#define MY_CELLSBUILDER implPtr<BOPAlgo_CellsBuilder>()

/// \brief Converts std::list<GeomAPI_Shape> to NCollection_List<TopoDS_Shape>.
/// \param[in] theStdListOfShape std::list.
/// \param[out] theNCollectionListOfShape NCollection_List. Will be cleared before converting.
static void stdListToNCollectionList(const ListOfShape& theStdListOfShape,
                                     NCollection_List<TopoDS_Shape>& theNCollectionListOfShape);

//=================================================================================================
GeomAlgoAPI_CellsBuilder::GeomAlgoAPI_CellsBuilder()
: GeomAlgoAPI_MakeShape(new BOPAlgo_CellsBuilder(), OCCT_BOPAlgo_Builder)
{
}

//=================================================================================================
void GeomAlgoAPI_CellsBuilder::setArguments(const ListOfShape &theLS)
{
  NCollection_List<TopoDS_Shape> aLS;
  stdListToNCollectionList(theLS, aLS);

  MY_CELLSBUILDER->SetArguments(aLS);
}

//=================================================================================================
void GeomAlgoAPI_CellsBuilder::setFuzzyValue(const double theFuzz)
{
  MY_CELLSBUILDER->SetFuzzyValue(theFuzz);
}

//=================================================================================================
void GeomAlgoAPI_CellsBuilder::setRunParallel(const bool theFlag)
{
  MY_CELLSBUILDER->SetRunParallel(theFlag);
}

//=================================================================================================
void GeomAlgoAPI_CellsBuilder::perform()
{
  MY_CELLSBUILDER->Perform();
}

//=================================================================================================
void GeomAlgoAPI_CellsBuilder::clear()
{
  MY_CELLSBUILDER->Clear();
}

//=================================================================================================
void GeomAlgoAPI_CellsBuilder::addToResult(const ListOfShape& theLSToTake,
                                           const ListOfShape& theLSToAvoid,
                                           const int theMaterial,
                                           const bool theUpdate)
{
  NCollection_List<TopoDS_Shape> aLSToTake, aLSToAvoid;
  stdListToNCollectionList(theLSToTake, aLSToTake);
  stdListToNCollectionList(theLSToAvoid, aLSToAvoid);

  MY_CELLSBUILDER->AddToResult(aLSToTake, aLSToAvoid, theMaterial, theUpdate);
}

//=================================================================================================
void GeomAlgoAPI_CellsBuilder::addAllToResult(const int theMaterial,
                                              const bool theUpdate)
{
  MY_CELLSBUILDER->AddAllToResult(theMaterial, theUpdate);
}

//=================================================================================================
void GeomAlgoAPI_CellsBuilder::removeFromResult(const ListOfShape& theLSToTake,
                                                const ListOfShape& theLSToAvoid)
{
  NCollection_List<TopoDS_Shape> aLSToTake, aLSToAvoid;
  stdListToNCollectionList(theLSToTake, aLSToTake);
  stdListToNCollectionList(theLSToAvoid, aLSToAvoid);

  MY_CELLSBUILDER->RemoveFromResult(aLSToTake, aLSToAvoid);
}

//=================================================================================================
void GeomAlgoAPI_CellsBuilder::removeAllFromResult()
{
  MY_CELLSBUILDER->RemoveAllFromResult();
}

//=================================================================================================
void GeomAlgoAPI_CellsBuilder::removeInternalBoundaries()
{
  MY_CELLSBUILDER->RemoveInternalBoundaries();
}

//=================================================================================================
const std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_CellsBuilder::getAllParts() const
{
  const TopoDS_Shape aShape = MY_CELLSBUILDER->GetAllParts();
  std::shared_ptr<GeomAPI_Shape> aGeomShape(new GeomAPI_Shape());
  aGeomShape->setImpl(new TopoDS_Shape(aShape));
  return aGeomShape;
}

//=================================================================================================
void GeomAlgoAPI_CellsBuilder::makeContainers()
{
  MY_CELLSBUILDER->MakeContainers();
}

//=================================================================================================
bool GeomAlgoAPI_CellsBuilder::isDone() const
{
  return MY_CELLSBUILDER->ErrorStatus() == 0;
}

//=================================================================================================
GEOMALGOAPI_EXPORT const std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_CellsBuilder::shape() const
{
  std::shared_ptr<GeomAPI_Shape> aResShape(new GeomAPI_Shape());
  TopoDS_Shape aShape = MY_CELLSBUILDER->Shape();

  if(aShape.ShapeType() == TopAbs_COMPOUND) {
    std::shared_ptr<GeomAPI_Shape> aCompound(new GeomAPI_Shape);
    aCompound->setImpl(new TopoDS_Shape(aShape));
    ListOfShape aCompSolids, aFreeSolids;
    GeomAlgoAPI_ShapeTools::combineShapes(aCompound, GeomAPI_Shape::COMPSOLID, aCompSolids, aFreeSolids);
    if(aCompSolids.size() == 1 && aFreeSolids.size() == 0) {
      aShape = aCompSolids.front()->impl<TopoDS_Shape>();
    } else if (aCompSolids.size() > 1 || (aCompSolids.size() >= 1 && aFreeSolids.size() >= 1)) {
      TopoDS_Compound aResultComp;
      TopoDS_Builder aBuilder;
      aBuilder.MakeCompound(aResultComp);
      for(ListOfShape::const_iterator anIter = aCompSolids.cbegin(); anIter != aCompSolids.cend(); anIter++) {
        aBuilder.Add(aResultComp, (*anIter)->impl<TopoDS_Shape>());
      }
      for(ListOfShape::const_iterator anIter = aFreeSolids.cbegin(); anIter != aFreeSolids.cend(); anIter++) {
        aBuilder.Add(aResultComp, (*anIter)->impl<TopoDS_Shape>());
      }
      aShape = aResultComp;
    }
  }

  aResShape->setImpl(new TopoDS_Shape(aShape));
  const_cast<GeomAlgoAPI_CellsBuilder*>(this)->setShape(aResShape);

  return GeomAlgoAPI_MakeShape::shape();
}

//=================================================================================================
void stdListToNCollectionList(const ListOfShape& theStdListOfShape,
                              NCollection_List<TopoDS_Shape>& theNCollectionListOfShape)
{
  theNCollectionListOfShape.Clear();
  for(ListOfShape::const_iterator anIt = theStdListOfShape.cbegin(); anIt != theStdListOfShape.cend(); anIt++) {
    const TopoDS_Shape aShape = (*anIt)->impl<TopoDS_Shape>();
    theNCollectionListOfShape.Append(aShape);
  }
}
