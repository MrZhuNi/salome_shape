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

#include "Filters_Plugin.h"
#include "Filters_HorizontalPlane.h"
#include "Filters_VerticalPlane.h"

#include <ModelAPI_Session.h>
#include <ModelAPI_Filter.h>

// the only created instance of this plugin
static Filters_Plugin* MY_VIEWFILTERS_INSTANCE = new Filters_Plugin();

Filters_Plugin::Filters_Plugin()
{
  // register validators
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_FiltersFactory* aFactory = aMgr->filters();
  aFactory->registerFilter("HorizontalFaces", new Filters_HorizontalPlane);
  aFactory->registerFilter("VerticalFaces", new Filters_VerticalPlane);

  // Do not register this plugin because it doesn't creates features
  //ModelAPI_Session::get()->registerPlugin(this);
}

FeaturePtr Filters_Plugin::createFeature(std::string theFeatureID)
{
  // feature of such kind is not found
  return FeaturePtr();
}
