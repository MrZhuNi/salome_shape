// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#ifndef SketchPlugin_Polyline_H_
#define SketchPlugin_Polyline_H_

#include <ModelAPI_IReentrant.h>

#include "SketchPlugin.h"
#include <SketchPlugin_SketchEntity.h>
#include <SketchPlugin_Sketch.h>
#include <list>

#include <GeomAPI_IPresentable.h>

class GeomAPI_Pnt2d;

/**\class SketchPlugin_Polyline
 * \ingroup Plugins
 * \brief Feature for creation of polyline in PartSet.
 */
class SketchPlugin_Polyline : public SketchPlugin_SketchEntity,
                               public GeomAPI_IPresentable
{
 public:
  /// Arc feature kind
  inline static const std::string& ID()
  {
    static const std::string SKETCH_LINE_ID("SketchPolyline");
    return SKETCH_LINE_ID;
  }

  /// list of polyline points
  inline static const std::string& POINTS_ID()
  {
    static const std::string ID("points");
    return ID;
  }

  /// Returns the kind of a feature
  SKETCHPLUGIN_EXPORT virtual const std::string& getKind();

  /// Request for initialization of data model of the feature: adding all attributes
  virtual void initAttributes();

  /// Creates a new part document if needed
  SKETCHPLUGIN_EXPORT virtual void execute();

  /// Reimplemented from ModelAPI_Feature::isMacro().
  /// \returns true
  SKETCHPLUGIN_EXPORT virtual bool isMacro() const {return true;};

  SKETCHPLUGIN_EXPORT virtual bool isPreviewNeeded() const {return false;};

  /// Called on change of any argument-attribute of this object
  SKETCHPLUGIN_EXPORT virtual void attributeChanged(const std::string& theID);

  /// Returns the AIS preview
  virtual AISObjectPtr getAISObject(AISObjectPtr thePrevious);

  /// Use plugin manager for features creation
  SketchPlugin_Polyline();

private:

  void createLineFeature();

protected:
  /// \brief Initializes attributes of derived class.
  virtual void initDerivedClassAttributes();
};

#endif
