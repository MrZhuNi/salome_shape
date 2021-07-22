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

#ifndef ModelAPI_ResultVolume_H_
#define ModelAPI_ResultVolume_H_

#include "ModelAPI_ResultMeta.h"

/**\class ModelAPI_ResultVolume
* \ingroup DataModel
* \brief The body (shape) result of a feature.
*
* Provides a shape that may be displayed in the viewer.
* May provide really huge results, so, working with this kind
* of result must be optimized.
* Also provides a container of sub-body result in case it is compound or compsolid.
*/
class ModelAPI_ResultVolume : public ModelAPI_ResultMeta
{
  public:
    MODELAPI_EXPORT virtual ~ModelAPI_ResultVolume();

    /// Returns the group identifier of this result
    MODELAPI_EXPORT std::string groupName();

    /// Returns the sub-result by zero-base index
    MODELAPI_EXPORT virtual std::shared_ptr<ModelAPI_ResultVolume> subResult(
    const int theIndex, bool forTree = false) const = 0;

    /// Set displayed flag to the result and all sub results
    /// \param theDisplay a boolean value
    MODELAPI_EXPORT virtual void setDisplayed(const bool theDisplay);

    /// Returns the group identifier of this result
    inline static std::string group()
    {
      static std::string MY_GROUP = "Volumes";
      return MY_GROUP;
    }

    /// default color for a result volume
    inline static const std::string DEFAULT_COLOR()
    {
      static const std::string RESULT_VOLUME_COLOR("102,0,204");
      return RESULT_VOLUME_COLOR;
    }

  protected:
    /// Default constructor accessible only from Model_Objects
    MODELAPI_EXPORT ModelAPI_ResultVolume();

};

//! Pointer on feature object
typedef std::shared_ptr<ModelAPI_ResultVolume> ResultVolumePtr;

#endif
