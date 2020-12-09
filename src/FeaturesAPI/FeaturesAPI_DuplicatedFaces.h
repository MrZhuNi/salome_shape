// Copyright (C) 2018-2020  CEA/DEN, EDF R&D
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

#ifndef FeaturesAPI_DuplicatedFaces_H_
#define FeaturesAPI_DuplicatedFaces_H_

#include "FeaturesAPI.h"

#include "FeaturesPlugin_GroupDuplicatedFaces.h"

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Integer.h>
#include <ModelHighAPI_Double.h>
#include <ModelHighAPI_Macro.h>

#include <memory>

class ModelAPI_Document;
class ModelHighAPI_Selection;

/// \class FeaturesAPI_DuplicatedFaces
/// \ingroup CPPHighAPI
/// \brief Interface to find duplicated faces
class FeaturesAPI_DuplicatedFaces: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_DuplicatedFaces(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  FEATURESAPI_EXPORT
  explicit FeaturesAPI_DuplicatedFaces(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                       const ModelHighAPI_Selection& theobject,
                                       const ModelHighAPI_Integer& theTransparency,
                                       const ModelHighAPI_Double& theTolerance,
                                       const std::string & theNameGroup);

  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_DuplicatedFaces();

    INTERFACE_4(FeaturesPlugin_GroupDuplicatedFaces::ID(),
                objectselected, FeaturesPlugin_GroupDuplicatedFaces::OBJECT_ID(),
                              ModelAPI_AttributeSelection,
                              /** object selected*/,
                transparency, FeaturesPlugin_GroupDuplicatedFaces::TRANSPARENCY_ID(),
                              ModelAPI_AttributeInteger,
                              /** transparency*/,
                tolerance, FeaturesPlugin_GroupDuplicatedFaces::TOLERANCE_ID(),
                              ModelAPI_AttributeDouble,
                              /** tolerance*/,
                groupname, FeaturesPlugin_GroupDuplicatedFaces::GROUP_NAME_ID(),
                              ModelAPI_AttributeString,
                              /** group name*/)

  /// Dump wrapped feature
  FEATURESAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;

};

/// Pointer on the SharedFaces object.
typedef std::shared_ptr<FeaturesAPI_DuplicatedFaces> DuplicatedFacesPtr;

/// \ingroup CPPHighAPI
/// \brief get the duplicated Faces
/// \param thePart the part
/// \param theobject the object selected
FEATURESAPI_EXPORT
DuplicatedFacesPtr getDuplicatedFaces(const std::shared_ptr<ModelAPI_Document>& thePart,
                                      const ModelHighAPI_Selection& theObject,
                                      const ModelHighAPI_Integer& theTransparency,
                                      const ModelHighAPI_Double& theTolerance,
                                      const std::string & theNameGroup);

#endif // FeaturesAPI_DuplicatedFaces_H_
