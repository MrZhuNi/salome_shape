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

#ifndef Model_ResultVolume_H_
#define Model_ResultVolume_H_

#include "Model.h"
#include <ModelAPI_ResultVolume.h>
#include <vector>
#include <map>

#include <TopTools_DataMapOfShapeListOfShape.hxx>

/**\class Model_ResultVolume
* \ingroup DataModel
* \brief The body (shape) result of a feature.
*
* Provides a shape that may be displayed in the viewer.
* May provide really huge results, so, working with this kind
* of result must be optimized.
* Also provides a container of sub-body result in case it is compound or compsolid.
*/
class Model_ResultVolume : public ModelAPI_ResultVolume
{

  public:
    /// Removes the stored builders
    MODEL_EXPORT virtual ~Model_ResultVolume();

  protected:
    /// Makes a body on the given feature
    Model_ResultVolume();
};

#endif
