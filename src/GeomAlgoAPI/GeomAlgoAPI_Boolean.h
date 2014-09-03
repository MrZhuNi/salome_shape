// File:        GeomAlgoAPI_Boolean.h
// Created:     02 Sept 2014
// Author:      Vitaly Smetannikov

#ifndef GeomAlgoAPI_Boolean_H_
#define GeomAlgoAPI_Boolean_H_

#include <GeomAlgoAPI.h>
#include <GeomAPI_Shape.h>
#include <boost/shared_ptr.hpp>

/**\class GeomAlgoAPI_Boolean
 * \ingroup DataAlgo
 * \brief Allows to perform of boolean operations
 */
class GEOMALGOAPI_EXPORT GeomAlgoAPI_Boolean
{
 public:
  /* \brief Creates cut boolean operation
   * \param[in] theShape face or wire to be extruded
   * \param[in] theTool  toole shape for boolean
   * \return a solid as result of operation
   */
  static boost::shared_ptr<GeomAPI_Shape> makeCut(boost::shared_ptr<GeomAPI_Shape> theShape,
                                                        boost::shared_ptr<GeomAPI_Shape> theTool);

};

#endif
