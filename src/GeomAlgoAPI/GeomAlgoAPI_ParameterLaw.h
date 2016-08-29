// Copyright

// File:        GeomAlgoAPI_ParameterLaw.h
// Created:     
// Author:      

#ifndef GeomAlgoAPI_ParameterLaw_H_
#define GeomAlgoAPI_ParameterLaw_H_

#include <GeomAlgoAPI.h>
#include <GeomAlgoAPI_MakeSweep.h>
#include <ModelAPI_Feature.h>
#include <GeomAPI_Face.h>
#include <memory>
#include <list>
#include <vector>


class GeomAlgoAPI_ParameterLaw : public GeomAlgoAPI_MakeSweep
{
public:

  GEOMALGOAPI_EXPORT GeomAlgoAPI_ParameterLaw();

  GEOMALGOAPI_EXPORT GeomAlgoAPI_ParameterLaw(std::shared_ptr<GeomAPI_Shape> theDefShape,
                                            std::shared_ptr<GeomAPI_Shape>   theRefShape,
											bool isReverse);

  GEOMALGOAPI_EXPORT std::vector<double> getVectorOfScalar(const std::shared_ptr<GeomAPI_Shape>&   theDef,
	const std::shared_ptr<GeomAPI_Shape>&   theRef,
	const bool isReverse,
	const int nbSect);

private:
  /// Builds resulting shape.
  void build(const std::shared_ptr<GeomAPI_Shape>& theDef,
             const std::shared_ptr<GeomAPI_Shape>&   theRef,
			 const bool isReverse);
};
#endif