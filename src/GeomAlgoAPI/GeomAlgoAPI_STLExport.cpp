// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#include "GeomAlgoAPI_STLExport.h"

#include "GeomAlgoAPI_Tools.h"

#include <TopoDS_Shape.hxx>

// OOCT includes
#include <BRepBuilderAPI_Copy.hxx>
#include <StlAPI_Writer.hxx>
#include <TopoDS_Shape.hxx>
#include <Bnd_Box.hxx>
#include <BRepBndLib.hxx>
#include <BRepTools.hxx>
#include <BRepMesh_IncrementalMesh.hxx>



#define MAX2(X, Y)      ( Abs(X) > Abs(Y) ? Abs(X) : Abs(Y) )
#define MAX3(X, Y, Z)   ( MAX2 ( MAX2(X, Y) , Z ) )

bool STLExport(const std::string& theFileName,
                const std::string& /*theFormatName*/,
                const std::shared_ptr<GeomAPI_Shape>& theShape,
                const double aDeflection,
                const bool anIsRelative,
                const bool anIsASCII,
                std::string& theError)
{
  #ifdef _DEBUG
  std::cout << "Export STl into file " << theFileName << std::endl;
  #endif

  if (!theShape.get()) {
    theError = "STl Export failed: An invalid argument";
    return false;
  }

  try
  {
    // Set "C" numeric locale to save numbers correctly
    GeomAlgoAPI_Tools::Localizer loc;

    double lDeflection = aDeflection;
    StlAPI_Writer aWriter;
    // copy source shape
    BRepBuilderAPI_Copy aCopy( theShape->impl<TopoDS_Shape>(), Standard_False );
    TopoDS_Shape aCopyShape = aCopy.Shape();
    // ASCII mode
    aWriter.ASCIIMode() = anIsASCII;
    if ( anIsRelative ) {
      Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
      Bnd_Box bndBox;
      BRepBndLib::Add( theShape->impl<TopoDS_Shape>(), bndBox );
      bndBox.Get( aXmin, aYmin, aZmin, aXmax, aYmax, aZmax );
      lDeflection = MAX3( aXmax-aXmin, aYmax-aYmin, aZmax-aZmin ) * aDeflection;
    }
    //Compute triangulation
    BRepTools::Clean( aCopyShape );
    BRepMesh_IncrementalMesh aMesh( aCopyShape, lDeflection );
    aWriter.Write( aCopyShape, theFileName.c_str() );

    return true;
  }
  catch( Standard_Failure )
  {
    theError = "Exception catched in STlExport";
  }
  return false;
}
