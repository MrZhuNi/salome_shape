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

#ifndef ModelAPI_ViewFilter_H_
#define ModelAPI_ViewFilter_H_

#include "ModelAPI.h"

#include <GeomAPI_Shape.h>

#include <list>


/**\class ModelAPI_ViewFilter
* \ingroup DataModel
* \brief An interface class for parameters of filters definition
*/
class ModelAPI_FilterParameter
{
public:
  virtual ~ModelAPI_FilterParameter() {}
};

typedef std::shared_ptr<ModelAPI_FilterParameter> FilterParamPtr;

/**\class ModelAPI_ViewFilter
* \ingroup DataModel
* \brief An interface class for filter objects
*/
class ModelAPI_ViewFilter
{
public:

  /// Returns true if the given shape is accepted by filter
  /// \param theShape the given shape
  virtual bool isOk(const GeomShapePtr& theShape) const = 0;

  /// Returns list of supported types of shapes (see GeomAPI_Shape::ShapeType)
  virtual std::list<int> shapeTypes() const = 0;

  /// Set parameter for the filter
  virtual void setParameter(const FilterParamPtr& theParam) {}

  /// Returns name of the filter to represent it in GUI
  virtual std::string name() const = 0;
};

typedef std::shared_ptr<ModelAPI_ViewFilter> FilterPtr;


/**\class ModelAPI_ValidatorsFactory
* \ingroup DataModel
* \brief Manages registering of filters
*/
class ModelAPI_FiltersFactory
{
public:
  /// Register an instance of a filter
  /// \param theID
  virtual void registerFilter(const std::string& theID, ModelAPI_ViewFilter* theFilter) = 0;

  /// Returns list of filters for the given shape type
  /// \param theType a shape type
  virtual std::list<FilterPtr> filtersForShapeType(GeomAPI_Shape::ShapeType theType) = 0;

protected:
  /// Get instance from Session
  ModelAPI_FiltersFactory() {}
};

typedef std::shared_ptr<ModelAPI_FiltersFactory> FilterFactoryPtr;

#endif