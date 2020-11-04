// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#ifndef PrimitivesPlugin_Tube_H_
#define PrimitivesPlugin_Tube_H_

#include <PrimitivesPlugin.h>
#include <GeomAlgoAPI_Tube.h>
#include <ModelAPI_Feature.h>

/// \class PrimitivesPlugin_Tube
/// \ingroup Plugins
/// \brief Feature for creation of a tube primitive using various methods.
///
/// ToDo
/// Supported following methods:
/// * two points that define a diagonal,
/// * three lengths that define the rectangular dimensions,
/// * one point and three lengths that define the rectangular dimensions.
class PrimitivesPlugin_Tube : public ModelAPI_Feature
{
 public:
  /// Tube kind
  inline static const std::string& ID()
  {
    static const std::string MY_TUBE_ID("Tube");
    return MY_TUBE_ID;
  }
  
  /// Attribute name for creation method
  inline static const std::string& CREATION_METHOD()
  {
    static const std::string MY_CREATION_METHOD_ID("CreationMethod");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name for creation method
  inline static const std::string& METHODE_SIMPLE_TUBE_ID()
  {
    static const std::string MY_CREATION_METHOD_ID("SimpleTube");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name for creation method
  inline static const std::string& METHODE_SEGMENT_TUBE_ID()
  {
    static const std::string MY_CREATION_METHOD_ID("SegmentTube");
    return MY_CREATION_METHOD_ID;
  }

  /// Attribute name of the inner radius of tube
  inline static const std::string& RMIN_ID()
  {
    static const std::string MY_RMIN("rmin");
    return MY_RMIN;
  }

  /// Attribute name of the outer radius of tube
  inline static const std::string& RMAX_ID()
  {
    static const std::string MY_RMAX("rmax");
    return MY_RMAX;
  }

  /// Attribute name of the z length of tube
  inline static const std::string& Z_ID()
  {
    static const std::string MY_Z("z");
    return MY_Z;
  }
  
  /// attribute name of the starting phi position angle of segment
  inline static const std::string& START_PHI_ID()
  {
    static const std::string MY_START_PHI("startphi");
    return MY_START_PHI;
  }
  
  /// attribute name of the delta angle of segment
  inline static const std::string& DELTA_PHI_ID()
  {
    static const std::string MY_DELTA_PHI("deltaphi");
    return MY_DELTA_PHI;
  }
  
  /// Returns the kind of a feature
  PRIMITIVESPLUGIN_EXPORT virtual const std::string& getKind()
  {
    static std::string MY_KIND = PrimitivesPlugin_Tube::ID();
    return MY_KIND;
  }

  /// Creates a new part document if needed
  PRIMITIVESPLUGIN_EXPORT virtual void execute();

  /// Request for initialization of data model of the feature: adding all attributes
  PRIMITIVESPLUGIN_EXPORT virtual void initAttributes();

  /// Use plugin manager for features creation
  PrimitivesPlugin_Tube();

 private:
  /// Load Naming data structure of the feature to the document
  void loadNamingDS(std::shared_ptr<GeomAlgoAPI_Tube> theTubeAlgo,
                    std::shared_ptr<ModelAPI_ResultBody> theResultBox);
  
  ///Perform the creation of the simple tube
  void createSimpleTube();
  
  ///Perform the creation of the segment tube
  void createSegmentTube();
};

#endif
