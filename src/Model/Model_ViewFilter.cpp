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

#include "Model_ViewFilter.h"

#include <Events_InfoMessage.h>


void Model_FiltersFactory::registerFilter(const std::string& theID, ModelAPI_ViewFilter* theFilter)
{
  if (myFilters.find(theID) != myFilters.end()) {
    Events_InfoMessage("Model_ViewFilter", "Filter %1 is already registered").arg(theID).send();
  }
  else {
    myFilters[theID] = FilterPtr(theFilter);
  }
}

/// Returns list of filters for the given shape type
/// \param theType a shape type
std::list<FilterPtr> Model_FiltersFactory::filtersForShapeType(GeomAPI_Shape::ShapeType theType)
{
  std::list<FilterPtr> aResult;
  std::map<std::string, FilterPtr>::const_iterator aIt;
  std::list<int> aTypes;
  std::list<int>::const_iterator aTIt;
  for (aIt = myFilters.cbegin(); aIt != myFilters.cend(); aIt++) {
    aTypes = aIt->second->shapeTypes();
    for (aTIt = aTypes.cbegin(); aTIt != aTypes.cend(); aTIt++) {
      if ((*aTIt) == theType) {
        aResult.push_back(aIt->second);
        break;
      }
    }
  }
  return aResult;
}
