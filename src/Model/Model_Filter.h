#pragma once
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

#ifndef Model_Filter_H_
#define Model_Filter_H_

#include "Model.h"

#include <ModelAPI_Filter.h>

#include <map>


/**\class Model_ValidatorsFactory
* \ingroup DataModel
* \brief Manages registering of filters
*/
class Model_FiltersFactory : public ModelAPI_FiltersFactory
{
public:
  /// Register an instance of a filter
  /// \param theID
  virtual void registerFilter(const std::string& theID, ModelAPI_Filter* theFilter);

  /// Returns list of filters for the given shape type
  /// \param theType a shape type
  virtual std::list<FilterPtr> filtersForShapeType(GeomAPI_Shape::ShapeType theType);

protected:
  /// Get instance from Session
  Model_FiltersFactory() {}

private:
  std::map<std::string, FilterPtr> myFilters;  ///< map from ID to registered filters

  friend class Model_Session;
};


#endif