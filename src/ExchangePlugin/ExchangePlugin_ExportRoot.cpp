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

#include <PrimitivesPlugin_Box.h>

#include <GeomAlgoAPI_PointBuilder.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>

#include <string>
#include <iostream>

#include "math.h"

namespace ExchangePlugin_ExportRoot
{
  //===============================================================================================
  std::vector<double> computeBox(FeaturePtr theFeature)
  {
    std::string aMethodName =
      theFeature->data()->string(PrimitivesPlugin_Box::CREATION_METHOD())->value();

    double OX, OY, OZ;
    double DX, DY, DZ;

    if (aMethodName == "BoxByDimensions") {
      DX = (theFeature->data()->real(PrimitivesPlugin_Box::DX_ID())->value())/2;
      DY = (theFeature->data()->real(PrimitivesPlugin_Box::DY_ID())->value())/2;
      DZ = (theFeature->data()->real(PrimitivesPlugin_Box::DZ_ID())->value())/2;
      OX = DX;
      OY = DY;
      OZ = DZ;
    } else if (aMethodName == "BoxByTwoPoints") {
      // Get the first point
      AttributeSelectionPtr aRef1 =
        theFeature->data()->selection(PrimitivesPlugin_Box::POINT_FIRST_ID());
      GeomShapePtr aShape1 = aRef1->value();
      if (!aShape1.get())
        aShape1 = aRef1->context()->shape();
      std::shared_ptr<GeomAPI_Pnt> aFirstPoint = GeomAlgoAPI_PointBuilder::point(aShape1);
      // Get the second point
      AttributeSelectionPtr aRef2 =
        theFeature->data()->selection(PrimitivesPlugin_Box::POINT_SECOND_ID());
      GeomShapePtr aShape2 = aRef2->value();
      if (!aShape2.get())
        aShape2 = aRef2->context()->shape();
      std::shared_ptr<GeomAPI_Pnt> aSecondPoint = GeomAlgoAPI_PointBuilder::point(aShape2);
      DX = fabs(aSecondPoint->x() - aFirstPoint->x())/2;
      DY = fabs(aSecondPoint->y() - aFirstPoint->y())/2;
      DZ = fabs(aSecondPoint->z() - aFirstPoint->z())/2;
      OX = fmin(aFirstPoint->x(), aSecondPoint->x()) + DX;
      OY = fmin(aFirstPoint->y(), aSecondPoint->y()) + DY;
      OZ = fmin(aFirstPoint->z(), aSecondPoint->z()) + DZ;
    } else if (aMethodName == "BoxByOnePointAndDims") {
      OX = theFeature->data()->real(PrimitivesPlugin_Box::OX_ID())->value();
      OY = theFeature->data()->real(PrimitivesPlugin_Box::OY_ID())->value();
      OZ = theFeature->data()->real(PrimitivesPlugin_Box::OZ_ID())->value();
      DX = theFeature->data()->real(PrimitivesPlugin_Box::HALF_DX_ID())->value();
      DY = theFeature->data()->real(PrimitivesPlugin_Box::HALF_DY_ID())->value();
      DZ = theFeature->data()->real(PrimitivesPlugin_Box::HALF_DZ_ID())->value();
    } else {
      // ToDo error
     return;
    }

    std::vector<double> aCenterDims;
    // Put the center
    aCenterDims.push_back(OX);
    aCenterDims.push_back(OY);
    aCenterDims.push_back(OZ);

    // Put the dimensions
    aCenterDims.push_back(DX);
    aCenterDims.push_back(DY);
    aCenterDims.push_back(DZ);

    return aCenterDims;

  }
} // namespace ExchangePlugin_ExportRoot
