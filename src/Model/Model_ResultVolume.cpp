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

#include <Model_ResultVolume.h>

#include <Model_Document.h>
#include <Model_Objects.h>
#include <Model_BodyBuilder.h>
#include <Model_Document.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Tools.h>
#include <Model_Data.h>
#include <Events_Loop.h>
#include <GeomAPI_ShapeIterator.h>
#include <GeomAPI_ShapeExplorer.h>
#include <GeomAPI_Face.h>
#include <GeomAPI_Pnt.h>

#include <TopoDS_Shape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TDataStd_UAttribute.hxx>

// if this attribute exists, the shape is connected topology
Standard_GUID kIsConnectedTopology("e51392e0-3a4d-405d-8e36-bbfe19858ef5");
// if this attribute exists, the connected topology flag must be recomputed
Standard_GUID kUpdateConnectedTopology("01ef7a45-0bec-4266-b0b4-4aa570921818");

Model_ResultVolume::Model_ResultVolume() : Model_ResultVolume()
{
  myBuilder = new Model_BodyBuilder(this);
  // myLastConcealed = false;
  // updateSubs(shape()); // in case of open, etc.
}

Model_ResultVolume::~Model_ResultVolume()
{
  // updateSubs(std::shared_ptr<GeomAPI_Shape>()); // erase sub-results
  delete myBuilder;
}
