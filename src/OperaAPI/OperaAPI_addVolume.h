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

#ifndef OPERAAPI_ADDVOLUME_H_
#define OPERAAPI_ADDVOLUME_H_

#include "OperaAPI.h"

#include <OperaPlugin_Volume.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Double;
class ModelHighAPI_Selection;

/// \class OperaAPI_addVolume
/// \ingroup CPPHighAPI
/// \brief Interface for primitive Box feature.
class OperaAPI_addVolume: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  OPERAAPI_EXPORT
  explicit OperaAPI_addVolume(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  OPERAAPI_EXPORT
  OperaAPI_addVolume(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                     const ModelHighAPI_Double& theMedium,
                     const std::list<ModelHighAPI_Selection>& theVolumeList);

  /// Destructor.
  OPERAAPI_EXPORT
  virtual ~OperaAPI_addVolume();

  INTERFACE_2(OperaPlugin_Volume::ID(),
             medium, OperaPlugin_Volume::MEDIUM(),
             ModelAPI_AttributeString, /** Volume medium */,
             volumeList, OperaPlugin_Volume::LIST_ID(),
             ModelAPI_AttributeSelectionList, /** Group list*/)

  /// Set medium
  OPERAAPI_EXPORT
  void setMedium(const ModelHighAPI_Double& theMedium);

  /// Set main objects list.
  OPERAAPI_EXPORT
  void setVolumeList(const std::list<ModelHighAPI_Selection>& theVolumeList);

  /// Dump wrapped feature
  OPERAAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer addVolume feature
typedef std::shared_ptr<OperaAPI_addVolume> VolumePtr;

/// \ingroup CPPHighAPI
/// \brief Create addVolume feature.
OPERAAPI_EXPORT
VolumePtr addVolume(const std::shared_ptr<ModelAPI_Document>& thePart,
                    const ModelHighAPI_Double& theMedium,
                    const std::list<ModelHighAPI_Selection>& theVolumeList);

#endif // OperaAPI_addVolume_H_
