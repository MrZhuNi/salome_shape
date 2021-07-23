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

#ifndef ModelAPI_ResultBody_H_
#define ModelAPI_ResultBody_H_

#include "ModelAPI_ResultMeta.h"
#include <GeomAPI_Shape.h>
#include <GeomAPI_DataMapOfShapeShape.h>
#include <string>
#include <map>
#include <vector>

class ModelAPI_BodyBuilder;
class GeomAlgoAPI_MakeShape;

/**\class ModelAPI_ResultBody
* \ingroup DataModel
* \brief The body (shape) result of a feature.
*
* Provides a shape that may be displayed in the viewer.
* May provide really huge results, so, working with this kind
* of result must be optimized.
* Also provides a container of sub-body result in case it is compound or compsolid.
*/
class ModelAPI_ResultBody : public ModelAPI_ResultMeta
{
  public:
    MODELAPI_EXPORT virtual ~ModelAPI_ResultBody();

    /// \brief Stores the shape (called by the execution method).
    /// param[in] theShape shape to store.
    /// param[in] theIsStoreSameShapes if false stores reference to the same shape
    ///                                if it is already in document.
    MODELAPI_EXPORT virtual void store(const GeomShapePtr& theShape,
                                      const bool theIsStoreSameShapes = true);

    // /// Returns the group identifier of this result
    MODELAPI_EXPORT virtual std::string groupName();

    /// Returns the sub-result by zero-base index
    MODELAPI_EXPORT virtual std::shared_ptr<ModelAPI_ResultBody> subResult(
    const int theIndex, bool forTree = false) const = 0;

    /// Set displayed flag to the result and all sub results
    /// \param theDisplay a boolean value
    MODELAPI_EXPORT virtual void setDisplayed(const bool theDisplay);

    inline static std::string group()
    {
      static std::string MY_GROUP = "Bodies";
      return MY_GROUP;
    }

    /// default color for a result volume
    inline static const std::string DEFAULT_COLOR()
    {
      static const std::string RESULT_BODY_COLOR("200,200,230");
      return RESULT_BODY_COLOR;
    }


  protected:
    /// Default constructor accessible only from Model_Objects
    MODELAPI_EXPORT ModelAPI_ResultBody();
};

//! Pointer on feature object
typedef std::shared_ptr<ModelAPI_ResultBody> ResultBodyPtr;

#endif
