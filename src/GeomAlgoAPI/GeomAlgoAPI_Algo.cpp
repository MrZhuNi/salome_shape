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

#include <GeomAlgoAPI_Algo.h>

//=======================================================================
// function:
// purpose:
//=======================================================================
GeomAlgoAPI_Algo::GeomAlgoAPI_Algo()
:
  myErrorStatus(1),
  myWarningStatus(0),
  myComputeInternalShapes(Standard_True)
{}
//=======================================================================
// function: ~
// purpose:
//=======================================================================
GeomAlgoAPI_Algo::~GeomAlgoAPI_Algo()
{
}
//=======================================================================
// function: CheckData
// purpose:
//=======================================================================
void GeomAlgoAPI_Algo::CheckData()
{
  myErrorStatus=0;
}
//=======================================================================
// function: CheckResult
// purpose:
//=======================================================================
void GeomAlgoAPI_Algo::CheckResult()
{
  myErrorStatus=0;
}
//=======================================================================
// function: ErrorStatus
// purpose:
//=======================================================================
Standard_Integer GeomAlgoAPI_Algo::ErrorStatus()const
{
  return myErrorStatus;
}
//=======================================================================
// function: WarningStatus
// purpose:
//=======================================================================
Standard_Integer GeomAlgoAPI_Algo::WarningStatus()const
{
  return myWarningStatus;
}
//  myErrorStatus
//
// 1 - object is just initialized

//=======================================================================
//function : ComputeInternalShapes
//purpose  :
//=======================================================================
void GeomAlgoAPI_Algo::ComputeInternalShapes(const Standard_Boolean theFlag)
{
  myComputeInternalShapes = theFlag;
}
