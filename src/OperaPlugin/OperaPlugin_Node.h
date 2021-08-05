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

#ifndef OperaPlugin_Node_H_
#define OperaPlugin_Node_H_

#include <OperaPlugin.h>
#include <ModelAPI_Feature.h>

/**\class OperaPlugin_Node
 * \ingroup Plugins
 * \brief Feature for creation of a Node using solids.
 *
 * Node creates Node object - This Node takes selected solids shape
 * and transform the result to a Node with a medium for OPERA.
 */
class OperaPlugin_Node : public ModelAPI_Feature
{
  public:
    /// Node kind
    inline static const std::string& ID()
    {
      static const std::string MY_NODE_ID("Node");
      return MY_NODE_ID;
    }
    /// Attribute name of main object.
    inline static const std::string& MAIN_OBJECT_ID()
    {
      static const std::string MY_MAIN_OBJECT_ID("main_object");
      return MY_MAIN_OBJECT_ID;
    }
    /// Attribute name of selected tools list
    inline static const std::string& TOOLS_LIST_ID()
    {
      static const std::string MY_TOOLS_LIST_ID("tools_list");
      return MY_TOOLS_LIST_ID;
    }

    // Creates a new part document if needed
    OPERAPLUGIN_EXPORT virtual void execute();

    /// Request for initialization of data model of the feature: adding all attributes
    OPERAPLUGIN_EXPORT virtual void initAttributes();

    /// Returns the kind of a feature
    OPERAPLUGIN_EXPORT virtual const std::string& getKind()
    {
      static std::string MY_KIND = OperaPlugin_Node::ID();
      return MY_KIND;
    }

    /// Use plugin manager for features creation
    OperaPlugin_Node();
};

#endif
