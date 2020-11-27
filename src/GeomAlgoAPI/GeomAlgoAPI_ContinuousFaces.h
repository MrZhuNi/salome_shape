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

#ifndef GEOMALGOAPI_CONTINUOUSFACES_H_
#define GEOMALGOAPI_CONTINUOUSFACES_H_

#include <GeomAlgoAPI.h>
#include <GeomAPI_Shape.h>
#include <Standard_TypeDef.hxx>
#include <GeomAPI_Pnt.h>

/// indicate if two faces are continuous 
/// with an angular tolerance used for G1 continuity to compare the angle between the normals
/// \param theFace1  the first face
/// \param theFace2  the second face
/// \param thePoint  the point for the normal
/// \param theAngle  the angular tolerance
/// \param theError  error
GEOMALGOAPI_EXPORT
bool isContinuousFaces(const GeomShapePtr& theFace1,
                       const GeomShapePtr& theFace2,
                       const GeomPointPtr& thePoint,
                       const double & theAngle, 
                       std::string& theError);

#endif //GEOMALGOAPI_SHAREDFACES_H_
