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

#include "FiltersPlugin_Plugin.h"
#include "FiltersPlugin_Selection.h"
#include "FiltersPlugin_HorizontalFace.h"
#include "FiltersPlugin_VerticalFace.h"

#include <ModelAPI_Session.h>
#include <ModelAPI_Filter.h>

// the only created instance of this plugin
static FiltersPlugin_Plugin* MY_VIEWFILTERS_INSTANCE = new FiltersPlugin_Plugin();

FiltersPlugin_Plugin::FiltersPlugin_Plugin()
{
  // register validators
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_FiltersFactory* aFactory = aMgr->filters();
  aFactory->registerFilter("HorizontalFaces", new FiltersPlugin_HorizontalFace);
  aFactory->registerFilter("VerticalFaces", new FiltersPlugin_VerticalFace);

  ModelAPI_Session::get()->registerPlugin(this);
}

FeaturePtr FiltersPlugin_Plugin::createFeature(std::string theFeatureID)
{
  if (theFeatureID == FiltersPlugin_Selection::ID())
    return FeaturePtr(new FiltersPlugin_Selection);
  // feature of such kind is not found
  return FeaturePtr();
}