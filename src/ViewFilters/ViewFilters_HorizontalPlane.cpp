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

#include "ViewFilters_HorizontalPlane.h"

#include <GeomAPI_Face.h>
#include <GeomAPI_Pln.h>

bool ViewFilters_HorizontalPlane::isOk(const GeomShapePtr& theShape) const
{
  if (!theShape->isFace())
    return false;

  if (!theShape->isPlanar())
    return false;
  GeomFacePtr aFace = std::dynamic_pointer_cast<GeomAPI_Face>(theShape);

  GeomPlanePtr aPlane = aFace->getPlane();
  GeomDirPtr aDir = aPlane->direction();
  if (aDir->isParallel(GeomDirPtr(new GeomAPI_Dir(0,0,1))))
    return true;
  return false;
}

std::list<int> ViewFilters_HorizontalPlane::shapeTypes() const
{
  std::list<int> aList;
  aList.push_back(GeomAPI_Shape::FACE);
  return aList;
}
