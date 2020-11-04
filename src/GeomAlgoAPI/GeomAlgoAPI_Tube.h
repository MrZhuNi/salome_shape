// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#ifndef GeomAlgoAPI_Tube_H_
#define GeomAlgoAPI_Tube_H_

#include <GeomAlgoAPI_MakeShape.h>

/**\class GeomAlgoAPI_TubeSegment
 * \ingroup DataAlgo
 * \brief Allows to create Tube Segment GDML Primitives
 */
class GeomAlgoAPI_Tube : public GeomAlgoAPI_MakeShape
{
 public: 
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Tube();
  
  /// Creates a tube segment using standard GDML parameters
  /// \param theRMin inside radius of segment
  /// \param theRMax outside radius of segment
  /// \param theZ z length of tube segment
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Tube(const double theRMin,
                                      const double theRMax,
                                      const double theZ);
  
  /// Creates a tube segment using standard GDML parameters
  /// \param theRMin inside radius of segment
  /// \param theRMax outside radius of segment
  /// \param theZ z length of tube segment
  /// \param theStartPhi starting phi position angle of segment
  /// \param theDeltaPhi delta angle of segment
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Tube(const double theRMin,
                                      const double theRMax,
                                      const double theZ,
                                      const double theStartPhi,
                                      const double theDeltaPhi);
  
  /// Checks if the set of parameters used to define the tube are OK.
  GEOMALGOAPI_EXPORT bool check();
  
  /// Builds the tube based on the parameters given in the constructor.
  GEOMALGOAPI_EXPORT void build();
  
 private:
  bool isSimple; /// attribute to indicate whether we are building a simple tube
  double myRMin; /// attribute inside radius of segment
  double myRMax; /// attribute outside radius of segment
  double myZ; /// attribute z length of tube segment
  double myStartPhi; /// attribute starting phi position angle of segment
  double myDeltaPhi; /// attribute delta angle of segment
  
  std::shared_ptr<GeomAPI_Shape> buildSimpleTube();
  std::shared_ptr<GeomAPI_Shape> buildSegmentTube();
};

#endif

