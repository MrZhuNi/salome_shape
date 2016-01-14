// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_CellsBuilder.h
// Created:     11 December 2015
// Author:      Dmitry Bobylev

#ifndef GeomAlgoAPI_CellsBuilder_H_
#define GeomAlgoAPI_CellsBuilder_H_

#include <GeomAlgoAPI.h>

#include <GeomAlgoAPI_MakeShape.h>

#include <memory>

/// \class GeomAlgoAPI_CellsBuilder
/// \ingroup DataAlgo
/// \brief The purpose of this algorithm is to provide the result with the content of:
///        1. Cells (parts) defined by the user;
///        2. Internal boundaries defined by the user.
class GeomAlgoAPI_CellsBuilder : public GeomAlgoAPI_MakeShape
{
public:
  /// Default empty constructor.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_CellsBuilder();

  /// \brief Sets arguments for the algorithm.
  /// \param[in] theLS list of shape.
  GEOMALGOAPI_EXPORT void setArguments(const ListOfShape &theLS);

  /// Sets the additional tolerance.
  /// \param[in] theFuzz fuzz value.
  GEOMALGOAPI_EXPORT void setFuzzyValue(const double theFuzz);

  /// \brief Sets the flag of parallel processing. If \p theFlag is true the parallel processing is switched on,
  /// if \p theFlag is false the parallel processing is switched off.
  /// \param[in] theFlag flag value.
  GEOMALGOAPI_EXPORT void setRunParallel(const bool theFlag);

  /// \brief Make cells from the arguments.
  GEOMALGOAPI_EXPORT void perform();

  /// Clears the contents.
  GEOMALGOAPI_EXPORT void clear();

  /// \brief Adding the parts to result.
  /// The parts are defined by two lists of shapes. To be taken into result the part must be IN
  /// for all shapes from the list \p theLSToTake and must be OUT of all shapes from the list \p theLSToAvoid.
  /// To remove internal boundaries between any cells in the result \p theMaterial variable should be used.
  /// The boundaries between cells with the same material will be removed. Default value is 0.
  /// Thus, to remove any boundary the value of this variable should not be equal to 0.
  /// \param[in] theLSToTake defines the arguments which parts should be taken into result.
  /// \param[in] theLSToAvoid defines the arguments which parts should not be taken into result.
  /// \param[in] theMaterial material id.
  /// \param[in] theUpdate defines whether to remove boundaries now or not.
  GEOMALGOAPI_EXPORT void addToResult(const ListOfShape& theLSToTake,
                                      const ListOfShape& theLSToAvoid,
                                      const int theMaterial = 0,
                                      const bool theUpdate = false);

  /// \brief Add all split parts to result
  /// \param[in] theMaterial defines the removal of internal boundaries.
  /// \param[in] theUpdate defines whether to remove boundaries now or not.
  GEOMALGOAPI_EXPORT void addAllToResult(const int theMaterial = 0,
                                         const bool theUpdate = false);

  /// \brief Removing the parts from result.
  /// The parts are defined by two lists of shapes. To be removed from the result the part must be IN
  /// for all shapes from the list \p theLSToTake and must be OUT of all shapes from the list \p theLSToAvoid.
  /// \param[in] theLSToTake defines the arguments which parts should be removed from result.
  /// \param[in] theLSToAvoid defines the arguments which parts should not be removed from result.
  GEOMALGOAPI_EXPORT void removeFromResult(const ListOfShape& theLSToTake,
                                           const ListOfShape& theLSToAvoid);

  /// Remove all parts from result.
  GEOMALGOAPI_EXPORT void removeAllFromResult();

  /// Removes internal boundaries between cells with the same material.
  GEOMALGOAPI_EXPORT void removeInternalBoundaries();

  /// Get all split parts.
  GEOMALGOAPI_EXPORT const std::shared_ptr<GeomAPI_Shape> getAllParts() const;

  /// Makes the Containers of proper type from the parts added to result.
  GEOMALGOAPI_EXPORT void makeContainers();

  /// \return status of builder.
  GEOMALGOAPI_EXPORT bool isDone() const;

  /// \return a shape built by the shape construction algorithm.
  GEOMALGOAPI_EXPORT const std::shared_ptr<GeomAPI_Shape> shape() const;
};

#endif