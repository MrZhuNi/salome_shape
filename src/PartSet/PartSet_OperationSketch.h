// File:        PartSet_OperationSketch.h
// Created:     20 Apr 2014
// Author:      Natalia ERMOLAEVA

#ifndef PartSet_OperationSketch_H
#define PartSet_OperationSketch_H

#include "PartSet.h"

#include <PartSet_OperationSketchBase.h>

#include <SketchPlugin_Sketch.h>

#include <QObject>

class Handle_AIS_InteractiveObject;

/*!
 \class PartSet_OperationSketch
 * \brief The operation for the sketch feature creation
*/
class PARTSET_EXPORT PartSet_OperationSketch : public PartSet_OperationSketchBase
{
  Q_OBJECT
public:
  /// Returns the operation type key
  static std::string Type() { return SKETCH_KIND; }

public:
  /// Constructor
  /// \param theId the feature identifier
  /// \param theParent the operation parent
  PartSet_OperationSketch(const QString& theId, QObject* theParent);
  /// Destructor
  virtual ~PartSet_OperationSketch();

  /// Returns the operation local selection mode
  /// \param theFeature the feature object to get the selection mode
  /// \return the selection mode
  virtual std::list<int> getSelectionModes(FeaturePtr theFeature) const;

  /// Initializes the operation with previously created feature. It is used in sequental operations
  virtual void initFeature(FeaturePtr theFeature);

  /// Returns the operation sketch feature
  /// \returns the sketch instance
  virtual FeaturePtr sketch() const;

  /// Processes the mouse pressed in the point
  /// \param theEvent the mouse event
  /// \param theView a viewer to have the viewer the eye position
  /// \param theSelected the list of selected presentations
  /// \param theHighlighted the list of highlighted presentations
  virtual void mousePressed(QMouseEvent* theEvent, Handle_V3d_View theView,
                            const std::list<XGUI_ViewerPrs>& theSelected,
                            const std::list<XGUI_ViewerPrs>& theHighlighted);
  /// Processes the mouse release in the point
  /// \param theEvent the mouse event
  /// \param theView a viewer to have the viewer the eye position
  /// \param theSelected the list of selected presentations
  /// \param theHighlighted the list of highlighted presentations
  virtual void mouseReleased(QMouseEvent* theEvent, Handle_V3d_View theView,
                             const std::list<XGUI_ViewerPrs>& theSelected,
                             const std::list<XGUI_ViewerPrs>& theHighlighted);

  /// Gives the current mouse point in the viewer
  /// \param thePoint a point clicked in the viewer
  /// \param theEvent the mouse event
  virtual void mouseMoved(QMouseEvent* theEvent, Handle_V3d_View theView);

  /// Returns the list of the nested features
  /// \return the list of subfeatures
  virtual std::list<FeaturePtr> subFeatures() const;

  /// Virtual method called when operation stopped - committed or aborted.
  /// Emits a signal to hide the preview of the operation
  virtual void stopOperation();

  /// Returns whether the nested operations are enabled.
  /// The state can depend on the operation current state.
  /// It returns true after the sketch plane is choosen.
  /// \return enabled state
  virtual bool isNestedOperationsEnabled() const;

  /// Returns whether the sketch plane is set
  /// \return the boolean value whether the sketch is set
  bool hasSketchPlane() const;

signals:
  /// signal about the sketch plane is selected
  /// \param theX the value in the X direction of the plane
  /// \param theX the value in the Y direction value of the plane
  /// \param theX the value in the Z direction of the plane
  void planeSelected(double theX, double theY, double theZ);
  // signal about the viewer fit all perform
  void fitAllView();

protected:
  /// Virtual method called when operation started (see start() method for more description)
  /// Default impl calls corresponding slot and commits immediately.
  virtual void startOperation();

  /// Set the plane to the current sketch
  /// \param theShape the shape
  void setSketchPlane(const TopoDS_Shape& theShape);

private:
  std::list<XGUI_ViewerPrs> myFeatures; ///< the features to apply the edit operation
};

#endif
