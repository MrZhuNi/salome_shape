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

#include <OperaPlugin_Plugin.h>
#include <OperaPlugin_Volume.h>

#include <ModelAPI_Session.h>

#include <string>
#include <memory>
#include <iostream>

// the only created instance of this plugin
static OperaPlugin_Plugin* MY_PRIMITIVES_INSTANCE = new OperaPlugin_Plugin();

OperaPlugin_Plugin::OperaPlugin_Plugin()
{
  // register this plugin
  ModelAPI_Session::get()->registerPlugin(this);
}

FeaturePtr OperaPlugin_Plugin::createFeature(std::string theFeatureID)
{
  if (theFeatureID == OperaPlugin_Volume::ID()) {
    return FeaturePtr(new OperaPlugin_Volume);
  } else {
    return FeaturePtr();
  }
}
