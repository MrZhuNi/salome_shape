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

#ifndef FeaturesPlugin_DuplicatedFaces_H_
#define FeaturesPlugin_DuplicatedFaces_H_

#include "FeaturesPlugin.h"
#include <ModelAPI_Feature.h>

#include <GeomAPI_IPresentable.h>
#include <GeomAPI_IScreenParams.h>
#include <GeomAlgoAPI_Box.h>

/// \class FeaturesPlugin_DuplicatedFaces
/// \ingroup Plugins
/// \brief Feature to check the duplicated faces of solid

class FeaturesPlugin_DuplicatedFaces : public ModelAPI_Feature
{
public:
   inline static const std::string& ID()
  {
    static const std::string MY_ID("Duplicated_faces");
    return MY_ID;
  }

  /// \return the kind of a feature.
  virtual const std::string& getKind()
  {
    return ID();
  }

  /// Attribute name for object selected.
  inline static const std::string& OBJECT_ID()
  {
    static const std::string MY_OBJECT_ID("main_object");
    return MY_OBJECT_ID;
  }

  /// Attribute name for number of faces.
  inline static const std::string& NUMBER_FACES_ID()
  {
    static const std::string MY_NUMBER_FACES_ID("number_duplicated_faces");
    return MY_NUMBER_FACES_ID;
  }

  /// Attribute name for z coodinate.
  inline static const std::string& LIST_FACES_ID()
  {
    static const std::string MY_LIST_FACES_ID("faces");
    return MY_LIST_FACES_ID;
  }

    /// Attribute name for transparency.
  inline static const std::string& TRANSPARENCY_ID()
  {
    static const std::string MY_TRANSPARENCY_ID("transparency");
    return MY_TRANSPARENCY_ID;
  }

  /// Attribute name for checkbox create group.
  inline static const std::string& CREATE_GROUP_ID()
  {
    static const std::string MY_CREATE_GROUP_ID("create_group");
    return MY_CREATE_GROUP_ID;
  }

  /// Attribute name for group name.
  inline static const std::string& GROUP_NAME_ID()
  {
    static const std::string MY_GROUP_NAME_ID("group_name");
    return MY_GROUP_NAME_ID;
  }

  /// Performs the algorithm and stores results it in the data structure.
  FEATURESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  FEATURESPLUGIN_EXPORT virtual void initAttributes();

  /// Called on change of any argument-attribute of this object
  /// \param theID identifier of changed attribute
  FEATURESPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// Use plugin manager for features creation
  FeaturesPlugin_DuplicatedFaces();

  private:

  //Set group of faces
  void setFacesGroup(const std::wstring& theName );

};

#endif
