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

#include "ModelAPI_Folder.h"

#include <ModelAPI_AttributeReference.h>

ModelAPI_Folder::ModelAPI_Folder()
{
}

ModelAPI_Folder::~ModelAPI_Folder()
{
}

void ModelAPI_Folder::init()
{
}

void ModelAPI_Folder::initAttributes()
{
  data()->addAttribute(FIRST_FEATURE_ID(), ModelAPI_AttributeReference::typeId());
  data()->addAttribute(LAST_FEATURE_ID(),  ModelAPI_AttributeReference::typeId());
  // nullify references for sure
  reference(FIRST_FEATURE_ID())->setValue(ObjectPtr());
  reference(LAST_FEATURE_ID())->setValue(ObjectPtr());
}

void ModelAPI_Folder::execute()
{
}
