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

#ifndef OPERAAPI_VOLUME_H_
#define OPERAAPI_VOLUME_H_

#include "OperaAPI.h"
#include <OperaPlugin_Volume.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Selection;

/// \class OperaAPI_Volume
/// \ingroup CPPHighAPI
/// \brief Interface for primitive Box feature.
class OperaAPI_Volume: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  OPERAAPI_EXPORT
  explicit OperaAPI_Volume(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  OPERAAPI_EXPORT
  explicit OperaAPI_Volume(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                  const std::string& theMedium,
                  const std::list<ModelHighAPI_Selection>& theObjectList);

  /// Destructor.
  OPERAAPI_EXPORT
  virtual ~OperaAPI_Volume();

  INTERFACE_2(OperaPlugin_Volume::ID(),
              medium, OperaPlugin_Volume::MEDIUM_ID(),
              ModelAPI_AttributeString, /** Volume medium */,
              volumeList, OperaPlugin_Volume::VOLUME_LIST_ID(),
              ModelAPI_AttributeSelectionList, /** Group list*/)

  /// Set medium
  OPERAAPI_EXPORT
  void setMedium(const std::string& theMedium);

  /// Set main objects list.
  OPERAAPI_EXPORT
  void setObjectList(const std::list<ModelHighAPI_Selection>& theObjectList);

  /// Dump wrapped feature
  OPERAAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer Volume feature
typedef std::shared_ptr<OperaAPI_Volume> VolumePtr;

/// \ingroup CPPHighAPI
/// \brief Create Volume feature.
OPERAAPI_EXPORT
VolumePtr addVolume(const std::shared_ptr<ModelAPI_Document>& thePart,
                 const std::string& theMedium,
                 const std::list<ModelHighAPI_Selection>& theObjectList);

#endif // OperaAPI_Volume_H_
