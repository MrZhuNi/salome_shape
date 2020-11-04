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

#include <GeomAlgoAPI_GluerAlgo.h>
#include <IntTools_Context.hxx>

//=======================================================================
//function : GeomAlgoAPI_GluerAlgo
//purpose  :
//=======================================================================
GeomAlgoAPI_GluerAlgo::GeomAlgoAPI_GluerAlgo()
{
  myTolerance=0.0001;
  myCheckGeometry=Standard_True;
}
//=======================================================================
//function : ~GeomAlgoAPI_GluerAlgo
//purpose  :
//=======================================================================
GeomAlgoAPI_GluerAlgo::~GeomAlgoAPI_GluerAlgo()
{
}
//=======================================================================
//function : SetArgument
//purpose  :
//=======================================================================
void GeomAlgoAPI_GluerAlgo::SetArgument(const TopoDS_Shape& theShape)
{
  myArgument=theShape;
}
//=======================================================================
//function : Argument
//purpose  :
//=======================================================================
const TopoDS_Shape& GeomAlgoAPI_GluerAlgo::Argument()const
{
  return myArgument;
}
//=======================================================================
//function : SetTolerance
//purpose  :
//=======================================================================
void GeomAlgoAPI_GluerAlgo::SetTolerance(const Standard_Real aT)
{
  myTolerance=aT;
}
//=======================================================================
//function : Tolerance
//purpose  :
//=======================================================================
Standard_Real GeomAlgoAPI_GluerAlgo::Tolerance()const
{
  return myTolerance;
}

//=======================================================================
//function : SetCheckGeometry
//purpose  :
//=======================================================================
void GeomAlgoAPI_GluerAlgo::SetCheckGeometry(const Standard_Boolean aFlag)
{
  myCheckGeometry=aFlag;
}
//=======================================================================
//function : CheckGeometry
//purpose  :
//=======================================================================
Standard_Boolean GeomAlgoAPI_GluerAlgo::CheckGeometry() const
{
  return myCheckGeometry;
}
//=======================================================================
//function : SetContext
//purpose  :
//=======================================================================
void GeomAlgoAPI_GluerAlgo::SetContext(const Handle(IntTools_Context)& theContext)
{
  myContext=theContext;
}
//=======================================================================
//function : Context
//purpose  :
//=======================================================================
const Handle(IntTools_Context)& GeomAlgoAPI_GluerAlgo::Context()
{
  return myContext;
}
//=======================================================================
//function : Images
//purpose  :
//=======================================================================
const TopTools_DataMapOfShapeListOfShape& GeomAlgoAPI_GluerAlgo::Images()const
{
  return myImages;
}
//=======================================================================
//function : Origins
//purpose  :
//=======================================================================
const TopTools_DataMapOfShapeShape& GeomAlgoAPI_GluerAlgo::Origins()const
{
  return myOrigins;
}
//=======================================================================
//function : Clear
//purpose  :
//=======================================================================
void GeomAlgoAPI_GluerAlgo::Clear()
{
  myImages.Clear();
  myOrigins.Clear();
}
//=======================================================================
//function : Perform
//purpose  :
//=======================================================================
void GeomAlgoAPI_GluerAlgo::Perform()
{
  if (myContext.IsNull()) {
    myContext=new IntTools_Context;
  }
}
