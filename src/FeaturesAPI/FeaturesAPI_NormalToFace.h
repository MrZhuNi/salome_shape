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

#ifndef FeaturesAPI_NormalToFace_H_
#define FeaturesAPI_NormalToFace_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_CreateNormalToFace.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>


class ModelHighAPI_Selection;

/// \class FeaturesAPI_NormalToFace
/// \ingroup CPPHighAPI
/// \brief Interface for NormalToface feature.
class FeaturesAPI_NormalToFace: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_NormalToFace(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  FEATURESAPI_EXPORT
  explicit FeaturesAPI_NormalToFace(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                    const ModelHighAPI_Selection& theBaseFace,
                                    const ModelHighAPI_Selection& theOptionnelPoint);

  FEATURESAPI_EXPORT
  explicit FeaturesAPI_NormalToFace(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                    const ModelHighAPI_Selection& theBaseFace);
  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_NormalToFace();

  INTERFACE_3(FeaturesPlugin_CreateNormalToFace::ID(),
              faceSelected, FeaturesPlugin_CreateNormalToFace::FACE_SELECTED_ID(),
                              ModelAPI_AttributeSelection,
                              /** base face */,
              vertexSelected, FeaturesPlugin_CreateNormalToFace::VERTEX_SELECTED_ID(),
                              ModelAPI_AttributeSelection,
                              /** vetex option */,
              vertexOption, FeaturesPlugin_CreateNormalToFace::VERTEX_OPTION_ID(),
                           ModelAPI_AttributeString,
                           /** option */)

  /// Dump wrapped feature
  FEATURESAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;

};

/// Pointer on the NormalToface object.
typedef std::shared_ptr<FeaturesAPI_NormalToFace> NormalPtr;

/// \ingroup CPPHighAPI
/// \brief Create NormalToFace feature.
FEATURESAPI_EXPORT
NormalPtr getNormal(const std::shared_ptr<ModelAPI_Document>& thePart,
                    const ModelHighAPI_Selection& theBaseFace,
                    const ModelHighAPI_Selection& theOptionnelPoint);

/// \ingroup CPPHighAPI
/// \brief Create NormalToFace feature.
FEATURESAPI_EXPORT
NormalPtr getNormal(const std::shared_ptr<ModelAPI_Document>& thePart,
                    const ModelHighAPI_Selection& theBaseFace);

#endif // FeaturesAPI_Fillet_H_
