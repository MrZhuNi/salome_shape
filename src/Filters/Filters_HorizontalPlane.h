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

#ifndef FILTERS_HORIZONTALPLANE_H_
#define FILTERS_HORIZONTALPLANE_H_

#include "Filters.h"

#include <ModelAPI_Filter.h>

class Filters_HorizontalPlane : public ModelAPI_Filter
{
public:
  virtual bool isOk(const GeomShapePtr& theShape) const;

  /// Returns list of supported types of shapes (see GeomAPI_Shape::ShapeType)
  virtual std::list<int> shapeTypes() const;

  /// Returns name of the filter to represent it in GUI
  virtual std::string name() const { return "Horizontal faces"; }
};


#endif