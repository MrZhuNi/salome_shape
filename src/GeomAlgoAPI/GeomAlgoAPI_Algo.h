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

#ifndef GEOMALGOAPI_ALGO_H_
#define GEOMALGOAPI_ALGO_H_

#include <GeomAlgoAPI.h>
#include <Standard.hxx>
#include <Standard_Macro.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>

//=======================================================================
//class    : GeomAlgoAPI_Algo
//purpose  :
//=======================================================================
class GeomAlgoAPI_Algo
{
 public:
  Standard_EXPORT
    virtual  void Perform()  = 0;

  Standard_EXPORT
    Standard_Integer ErrorStatus() const;

  Standard_EXPORT
    Standard_Integer WarningStatus() const;

  //! Allows to omit of creation of internal shapes (manifold topology). <br>
  //!          Needed for the SALOME/TRIPOLI module. <br>
  Standard_EXPORT
    void ComputeInternalShapes(const Standard_Boolean theFlag) ;

protected:
  Standard_EXPORT
    GeomAlgoAPI_Algo();

  Standard_EXPORT
    virtual ~GeomAlgoAPI_Algo();

  Standard_EXPORT
    virtual  void CheckData() ;

  Standard_EXPORT
    virtual  void CheckResult() ;


  Standard_Integer myErrorStatus;
  Standard_Integer myWarningStatus;
  Standard_Boolean myComputeInternalShapes;
};
#endif
