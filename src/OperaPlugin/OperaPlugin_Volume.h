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

#ifndef OperaPlugin_Volume_H_
#define OperaPlugin_Volume_H_

#include <OperaPlugin.h>
#include <ModelAPI_Feature.h>
#include <GeomAlgoAPI_Box.h>

class GeomAPI_Shape;
class ModelAPI_ResultBody;

/**\class OperaPlugin_Volume
 * \ingroup Plugins
 * \brief Feature for creation of a box primitive using various methods.
 *
 * Box creates a cuboid - Parallelepiped with 6 rectangular faces. It can be built via three
 * methods : using two points that define a diagonal, a point that define a center and 3 lengths
 * that define the half-lengths on X, Y and Z-axes, or using 3 lengths that define the
 * rectangular dimensions.
 */
class OperaPlugin_Volume : public ModelAPI_Feature
{
  public:
    /// Box kind
    inline static const std::string& ID()
    {
      static const std::string MY_VOLUME_ID("Volume");
      return MY_VOLUME_ID;
    }
    inline static const std::string& MEDIUM()
    {
      static const std::string MY_MEDIUM_ID("medium");
      return MY_MEDIUM_ID;
    }
    /// attribute name of selected entities list
    inline static const std::string& VOLUME_LIST_ID()
    {
      static const std::string MY_VOLUME_LIST_ID("volume_list");
      return MY_VOLUME_LIST_ID;
    }

    // Creates a new part document if needed
    OPERAPLUGIN_EXPORT virtual void execute();

    /// Request for initialization of data model of the feature: adding all attributes
    OPERAPLUGIN_EXPORT virtual void initAttributes();

    /// Returns the kind of a feature
    OPERAPLUGIN_EXPORT virtual const std::string& getKind()
    {
      static std::string MY_KIND = OperaPlugin_Volume::ID();
      return MY_KIND;
    }

    /// Use plugin manager for features creation
    OperaPlugin_Volume();

    private:
    //
    void loadNamingDS(std::shared_ptr<GeomAlgoAPI_Box> theBoxAlgo,
                      std::shared_ptr<ModelAPI_ResultBody> theResultBox);
};

#endif
