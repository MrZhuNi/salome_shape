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

#ifndef OPERAAPI_ADDNODE_H_
#define OPERAAPI_ADDNODE_H_

#include "OperaAPI.h"
#include <OperaPlugin_AddNode.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Selection;

/// \class OperaAPI_AddNode
/// \ingroup CPPHighAPI
/// \brief Interface for AddNode feature.
class OperaAPI_AddNode: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  OPERAAPI_EXPORT
  explicit OperaAPI_AddNode(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  OPERAAPI_EXPORT
  explicit OperaAPI_AddNode(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                            const ModelHighAPI_Selection& theMainObject,
                            const std::list<ModelHighAPI_Selection>& theToolsList);

  /// Destructor.
  OPERAAPI_EXPORT
  virtual ~OperaAPI_AddNode();

  INTERFACE_2(OperaPlugin_AddNode::ID(),
              mainObject, OperaPlugin_AddNode::MAIN_OBJECT_ID(),
              ModelAPI_AttributeSelection, /** Main Object */,
              toolsList, OperaPlugin_AddNode::TOOLS_LIST_ID(),
              ModelAPI_AttributeSelectionList, /** Tools list*/)

  /// Set main object
  OPERAAPI_EXPORT
  void setMainObject(const ModelHighAPI_Selection& theMainObject);

  /// Set tools list
  OPERAAPI_EXPORT
  void setToolsList(const std::list<ModelHighAPI_Selection>& theToolsList);

  /// Dump wrapped feature
  OPERAAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer AddNode feature
typedef std::shared_ptr<OperaAPI_AddNode> AddNodePtr;

/// \ingroup CPPHighAPI
/// \brief Create Volume feature.
OPERAAPI_EXPORT
AddNodePtr addAddNode(const std::shared_ptr<ModelAPI_Document>& thePart,
                      const ModelHighAPI_Selection& theMainObject,
                      const std::list<ModelHighAPI_Selection>& theToolsList);

#endif // OPERAAPI_ADDNODE_H_
