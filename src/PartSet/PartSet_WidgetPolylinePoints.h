// Copyright (C) 2019-2020  CEA/DEN, EDF R&D
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

#ifndef PartSet_WidgetPolylinePoints_H
#define PartSet_WidgetPolylinePoints_H

#include "PartSet.h"
#include "PartSet_MouseProcessor.h"

#include <ModuleBase_ModelWidget.h>

#include <QObject>

class GeomAPI_Pnt2d;
class ModelAPI_CompositeFeature;
class ModuleBase_LabelValue;
class PartSet_ExternalObjectsMgr;
class QGroupBox;
class QScrollArea;

/**\class PartSet_WidgetPolylinePoints
 * \ingroup Modules
 * \brief Implementation of model widget to provide widget to input a list of 2D points
 *        of polyline
 * In XML can be defined as following:
 * \code
 * <sketch-polyline_selector id="points"/>
 * \endcode
 */
class PARTSET_EXPORT PartSet_WidgetPolylinePoints : public ModuleBase_ModelWidget,
                                                   public PartSet_MouseProcessor
{
Q_OBJECT
public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theWorkshop a current workshop
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  PartSet_WidgetPolylinePoints(QWidget* theParent, ModuleBase_IWorkshop* theWorkshop,
                              const Config_WidgetAPI* theData);
  /// Destructor
  virtual ~PartSet_WidgetPolylinePoints();

  /// Fills given container with selection modes if the widget has it
  /// \param [out] theModuleSelectionModes module additional modes, -1 means all default modes
  /// \param theModes [out] a container of modes
  virtual void selectionModes(int& theModuleSelectionModes, QIntList& theModes);

  /// Fills the attribute with the value of the selected owner
  /// \param thePrs a selected owner
  bool setSelectionCustom(const std::shared_ptr<ModuleBase_ViewerPrs>& theValue);

  /// Returns list of widget controls
  /// \return a control list
  virtual QList<QWidget*> getControls() const;

  /// The methiod called when widget is deactivated
  virtual void deactivate();

  /// \returns the sketch instance
  std::shared_ptr<ModelAPI_CompositeFeature> sketch() const { return mySketch; }

  /// Set sketch instance
  void setSketch(std::shared_ptr<ModelAPI_CompositeFeature> theSketch) { mySketch = theSketch; }

  /// Fill the widget values by given point
  /// \param theX the X coordinate
  /// \param theY the Y coordinate
  /// \returns True in case of success
  bool setPoint(double theX, double theY);

  /// Returns true if the event is processed.
  virtual bool processEscape();

  /// Returns true if the attribute can be changed using the selected shapes in the viewer
  /// and creating a coincidence constraint to them. This control use them.
  virtual bool useSelectedShapes() const;

  /// Processing the mouse move event in the viewer
  /// \param theWindow a view window
  /// \param theEvent a mouse event
  virtual void mouseMoved(ModuleBase_IViewWindow* theWindow, QMouseEvent* theEvent);

  /// Processing the mouse release event in the viewer
  /// \param theWindow a view window
  /// \param theEvent a mouse event
  virtual void mouseReleased(ModuleBase_IViewWindow* theWindow, QMouseEvent* theEvent);

protected:
  /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValueCustom();

  /// Restore value from attribute data to the widget's control
  virtual bool restoreValueCustom();

  /// Store current value in cashed value
  void storeCurentValue();

  /// Restore cashed value in the model attribute
  /// \return boolean state if the restored feature shoud be hidden
  bool restoreCurentValue();

private:
  /// Create labels for the next B-spline point
  void createNextPoint();
  /// Remove labels for the last B-spline point
  void removeLastPoint();

  /// Save polyline points to corresponding attributes
  void storePoints() const;

protected:
  ModuleBase_IWorkshop* myWorkshop; ///< workshop

private:
  QGroupBox* myBox;
  QWidget* myGroupBox;  ///< the parent group box for all intenal widgets
  QScrollArea* myScrollArea;
  std::vector<ModuleBase_LabelValue*> myXSpin; ///< the label for the X coordinate
  std::vector<ModuleBase_LabelValue*> myYSpin; ///< the label for the Y coordinate

  /// value used as selection in mouse release method
  std::shared_ptr<ModuleBase_ViewerPrs> myPreSelected;

  /// it is important during restart operation
  CompositeFeaturePtr mySketch;

  bool myValueIsCashed; /// boolean state if the value is cashed during value state change
  bool myIsFeatureVisibleInCash; /// boolean value if the feature was visible when cash if filled
  std::vector<double> myXValueInCash; /// the cashed X value during value state change
  std::vector<double> myYValueInCash; /// the cashed Y value during value state change

  bool myFinished; /// \c true if building the polyline is finished (escape pressed)
};
#endif
