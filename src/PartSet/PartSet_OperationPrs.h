// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_OperationPrs.h
// Created:     01 Jul 2015
// Author:      Natalia ERMOLAEVA

#ifndef PartSet_OperationPrs_H
#define PartSet_OperationPrs_H

#include "PartSet.h"

#include <ModelAPI_Object.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_Attribute.h>

#include <ModuleBase_IWorkshop.h>

#include <GeomAPI_Shape.h>

#include <ViewerData_AISShape.hxx>
#include <Standard_DefineHandle.hxx>

#include <Quantity_Color.hxx>

#include <QMap>
#include <QList>

#include <list>

DEFINE_STANDARD_HANDLE(PartSet_OperationPrs, ViewerData_AISShape)

class XGUI_Workshop;
class XGUI_Displayer;

/**
* \ingroup GUI
* A redefinition of standard AIS Interactive Object in order to provide colored presentation of
* a list of shapes. It contains a shapes where the parameter feature refers. It processes the following
* types of attributes: ModelAPI_AttributeSelectionList, ModelAPI_AttributeRefAttr,
* ModelAPI_AttributeSelection and ModelAPI_AttributeReference.
* The selection for this presentation is absent.
*/
class PartSet_OperationPrs : public ViewerData_AISShape
{
public:
  /// Constructor
  Standard_EXPORT PartSet_OperationPrs(ModuleBase_IWorkshop* theWorkshop);

  /// Sets the operation feature. It is used in Compute method to group the feature parameter shapes
  /// theFeature a feature
  void setFeature(const FeaturePtr& theFeature);

  // Recompute internal list of shaped dependent on the current feature
  void updateShapes();

  /// Returns true if the feature contains shapes or results
  bool hasShapes();

  /// Sets the colors for the presentation compute
  /// \param theShapeColor an argument shapes color
  /// \param theResultColor a color for operation result
  void setColors(const Quantity_Color& theShapeColor, const Quantity_Color& theResultColor);

  /// Switch on using of the AIS presentation with of the shape object increased on the delta
  void useAISWidth();

  DEFINE_STANDARD_RTTI(PartSet_OperationPrs)

protected:
  /// Redefinition of virtual function
  Standard_EXPORT virtual void Compute(
            const Handle(PrsMgr_PresentationManager3d)& thePresentationManager,
            const Handle(Prs3d_Presentation)& thePresentation, const Standard_Integer theMode = 0);

  /// Redefinition of virtual function
  Standard_EXPORT virtual void ComputeSelection(const Handle(SelectMgr_Selection)& aSelection,
                                                const Standard_Integer aMode) ;

protected:
  /// Reference to a feature object
  FeaturePtr getFeature() { return myFeature; }

  /// Returns map of feature shapes to be able to fill it outside this class, e.g. in friend
  /// \return a map of object to shape
  QMap<ObjectPtr, QList<GeomShapePtr> >& featureShapes() { return myFeatureShapes; }

private:
  /// Return true if the object is visible. If the object is feature, it returns true
  /// if all results of the feature are shown
  /// \param theDisplayer a displayer
  /// \param theObject an object
  /// \return a boolean value
  bool isVisible(XGUI_Displayer* theDislayer, const ObjectPtr& theObject);

  /// Fills the map by the feature object and shapes, which should be visuaziled
  /// Gets feature attributes, collect objects to whom the attributes refer
  /// \param theFeature a current feature
  /// \param theWorkshop a current workshop
  /// \param theObjectShapes an output map
  /// \param theObjectShape an output map of objects
  static void getFeatureShapes(const FeaturePtr& theFeature,
                               ModuleBase_IWorkshop* theWorkshop,
                               QMap<ObjectPtr, QList<GeomShapePtr> >& theObjectShapes);

  /// Fills the map by the feature object and shapes, which should be visuaziled
  /// Gets the active widget, obtain the highlighted presentations if it has such and
  /// fill map by object and shapes
  /// \param theWorkshop a current workshop
  /// \param theObjectShapes an output map of objects
  static void getHighlightedShapes(ModuleBase_IWorkshop* theWorkshop,
                                   QMap<ObjectPtr, QList<GeomShapePtr> >& theObjectShapes);

  /// Returns true if the attribute type belong to reference attribute
  /// \param theAttribute an attribute
  /// \return a boolean value
  static bool isSelectionAttribute(const AttributePtr& theAttribute);

  /// Converts the current workshop to XGUI workshop
  /// \return a workshop instance
  XGUI_Workshop* workshop() const;

private:
  ModuleBase_IWorkshop* myWorkshop;
  FeaturePtr myFeature; /// Reference to a feature object
  QMap<ObjectPtr, QList<GeomShapePtr> > myFeatureShapes; /// visualized shapes
  std::list<ResultPtr> myFeatureResults; /// visualized feature results

  Quantity_Color myShapeColor; /// color of feature depended shapes
  Quantity_Color myResultColor; /// color of feature result

  bool myUseAISWidth; /// flag if the width of a shape object should be used for the shape visualization

  friend class PartSet_CustomPrs;
};


#endif