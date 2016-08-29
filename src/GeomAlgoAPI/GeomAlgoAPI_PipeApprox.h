// Copyright

// File:        GeomAlgoAPI_PipeApprox.h
// Created:     
// Author:      

#ifndef GeomAlgoAPI_PipeApprox_H_
#define GeomAlgoAPI_PipeApprox_H_

#include <GeomAlgoAPI.h>
#include <GeomAlgoAPI_MakeSweep.h>
#include <ModelAPI_Feature.h>
#include <GeomAPI_Face.h>
#include <memory>
#include <list>
#include <vector>
/// \class GeomAlgoAPI_PipeApprox
/// \ingroup DataAlgo
/// \brief Allows to create the Pipe based on a given face and path.

class GeomAlgoAPI_PipeApprox : public GeomAlgoAPI_MakeSweep
{
public:
  /// \brief Creates Pipe for the given shape.
  /// \param[in] theBaseShape face for Pipe.
  /// \param[in] thePath path for Pipe.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_PipeApprox();

  //basic pipe with thrusection algo
  GEOMALGOAPI_EXPORT GeomAlgoAPI_PipeApprox(std::shared_ptr<GeomAPI_Shape> theProfileShape,
                                            std::shared_ptr<GeomAPI_Shape>   thePathShape);

  GEOMALGOAPI_EXPORT GeomAlgoAPI_PipeApprox(std::shared_ptr<GeomAPI_Shape> theProfileShape,
                                            std::shared_ptr<GeomAPI_Shape>   thePathShape,
											std::shared_ptr<GeomAPI_Shape>   theBiNormal);

  //single law, does a "scaling"
  GEOMALGOAPI_EXPORT GeomAlgoAPI_PipeApprox(std::shared_ptr<GeomAPI_Shape> theProfileShape,
                                            std::shared_ptr<GeomAPI_Shape>   thePathShape,
											std::shared_ptr<GeomAPI_Shape>   theDef,
											std::shared_ptr<GeomAPI_Shape>   theRef);



  //WHEN USING LAWS
  GEOMALGOAPI_EXPORT GeomAlgoAPI_PipeApprox(ListOfShape theProfileShapeList,
			std::shared_ptr<GeomAPI_Shape>   thePath,
			bool isReverse,
			int nbSect,
			bool withFrenet);

  GEOMALGOAPI_EXPORT GeomAlgoAPI_PipeApprox(ListOfShape theProfileShapeList,
			std::shared_ptr<GeomAPI_Shape>   thePath,
			std::shared_ptr<GeomAPI_Shape>   theBinormal,
			bool isReverse,
			int nbSect);

private:
  /// Builds resulting shape.
  void build(const std::shared_ptr<GeomAPI_Shape>& theProfileShape,
             const std::shared_ptr<GeomAPI_Shape>&   thePathShape);

  void build(const std::shared_ptr<GeomAPI_Shape>& theProfileShape,
									const std::shared_ptr<GeomAPI_Shape>&   thePathShape,
									const std::shared_ptr<GeomAPI_Shape>&   theBiNormal);
  //
  void build(const std::shared_ptr<GeomAPI_Shape>& theProfileShape,
									const std::shared_ptr<GeomAPI_Shape>&   thePathShape,
									const std::shared_ptr<GeomAPI_Shape>&   theDef,
									const std::shared_ptr<GeomAPI_Shape>&   theRef);

  //
  void build(const ListOfShape& theProfileShapeList,
			const std::shared_ptr<GeomAPI_Shape>&   thePath,
			const bool isReverse,
			const int nbSect,
			const bool withFrenet);

  void build(const ListOfShape& theProfileShapeList,
			const std::shared_ptr<GeomAPI_Shape>&   thePath,
			const std::shared_ptr<GeomAPI_Shape>&   theBinormal,
			const bool isReverse,
			const int nbSect);
};
#endif