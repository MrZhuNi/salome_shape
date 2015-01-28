// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_SketcherMgr.h
// Created:     19 Dec 2014
// Author:      Vitaly SMETANNIKOV

#ifndef PartSet_SketcherMgr_H
#define PartSet_SketcherMgr_H

#include "PartSet.h"

#include <ModelAPI_Feature.h>
#include <ModelAPI_Attribute.h>
#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_Result.h>

#include <ModuleBase_ViewerFilters.h>
#include <ModuleBase_Definitions.h>

#include <GeomAPI_Pln.h>
#include <SelectMgr_IndexedMapOfOwner.hxx>

#include <QObject>
#include <QList>
#include <QMap>

class PartSet_Module;
class ModuleBase_IViewWindow;
class ModuleBase_ModelWidget;
class ModuleBase_Operation;
class QMouseEvent;

/**
* \ingroup Module
* A class for management of sketch operations
*/
class PARTSET_EXPORT PartSet_SketcherMgr : public QObject
{
  Q_OBJECT
  /// Struct to define gp point, with the state is the point is initialized
  struct Point
  {
    /// Constructor
    Point()
    {
      myIsInitialized = false;
    }
    /// Destructor
    ~Point()
    {
    }

    /// clear the initialized flag.
    void clear()
    {
      myIsInitialized = false;
    }
    /// set the point and switch on the initialized flag
    /// \param thePoint the point
    void setValue(const double theX, const double theY)
    {
      myIsInitialized = true;
      myCurX = theX;
      myCurY = theY;
    }

    bool myIsInitialized;  /// the state whether the point is set
    double myCurX, myCurY; /// the point coordinates
  };
public:
  /// Constructor
  /// \param theModule a pointer to PartSet module
  PartSet_SketcherMgr(PartSet_Module* theModule);

  virtual ~PartSet_SketcherMgr();

  /// Returns list of strings which contains id's of sketch operations
  static QStringList sketchOperationIdList();

  /// Launches the operation from current highlighting
  void launchEditing();

  /// Returns current Sketch feature/ Returns NULL if there is no launched sketch operation
  CompositeFeaturePtr activeSketch() const { return myCurrentSketch; }

  /// Starts sketch operation
  void startSketch(ModuleBase_Operation* );

  /// Stops sketch operation
  void stopSketch(ModuleBase_Operation* );

public slots:
  /// Process sketch plane selected event
  void onPlaneSelected(const std::shared_ptr<GeomAPI_Pln>& thePln);


private slots:
  void onMousePressed(ModuleBase_IViewWindow*, QMouseEvent*);
  void onMouseReleased(ModuleBase_IViewWindow*, QMouseEvent*);
  void onMouseMoved(ModuleBase_IViewWindow*, QMouseEvent*);
  void onMouseDoubleClick(ModuleBase_IViewWindow*, QMouseEvent*);
  void onApplicationStarted();
  void onBeforeWidgetActivated(ModuleBase_ModelWidget* theWidget);

private:
  /// Returns whethe the current operation is a sketch distance - lenght, distance or radius
  /// \param the operation
  /// \return a boolean value
  bool isDistanceOperation(ModuleBase_Operation* theOperation) const;

  /// Converts mouse position to 2d coordinates. 
  /// Member myCurrentSketch has to be correctly defined
  void get2dPoint(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent, 
                  Point& thePoint);

  typedef QList<AttributePtr> AttributeList;
  typedef QMap<FeaturePtr, AttributeList> FeatureToAttributesMap;
  typedef std::map<FeaturePtr, std::pair<std::set<AttributePtr>, std::set<ResultPtr> > >
                                                                       FeatureToSelectionMap;
  /// Obtains the current selection of the object in the workshop viewer by a map of feature to attributes
  /// It calls the next method for each feature
  /// \param theFeatureToAttributes a map of feature to attributes
  /// \param theSketch a current sketch feature
  /// \param theWorkshop a workshop to have an access to AIS context and displayer
  /// \param theSelection a container for the selection, to save results and attributres for a feature
  static void getCurrentSelection(const FeatureToAttributesMap& theFeatureToAttributes,
                                  const FeaturePtr& theSketch,
                                  ModuleBase_IWorkshop* theWorkshop,
                                  FeatureToSelectionMap& theSelection);

  /// Obtains the current selection of the object in the workshop viewer 
  /// It includes the selection in all modes of activation, even local context - vertices, edges
  /// It gets all results of the feature, find an AIS object in the viewer and takes all BRep
  /// selection owners. If the owner is vertex, the corresponded attribute is seached in
  /// the feature, if the owner is edge, the current result is added to the container of results.
  /// \param theFeature a feature or result object
  /// \param theSketch a current sketch feature
  /// \param theWorkshop a workshop to have an access to AIS context and displayer
  /// \param theSelection a container for the selection, to save results and attributres for a feature
  static void getCurrentSelection(const FeaturePtr& theFeature,
                                  const FeaturePtr& theSketch,
                                  ModuleBase_IWorkshop* theWorkshop,
                                  FeatureToSelectionMap& theSelection);

  /// Applyes the current selection to the object in the workshop viewer 
  /// It includes the selection in all modes of activation, even local context - vertexes, edges
  /// It gets all results of the feature, find an AIS object in the viewer and takes all BRep
  /// selection owners. If the owner is vertex, the corresponded attribute is seached in
  /// the feature and if it is in the container of selected attributes, the owner is put in the
  /// out container. If the owner is edge and the current result is in the container of selected
  /// results, the owner is put in the out container.
  /// \param theFeature a feature or result object
  /// \param theSketch a current sketch feature
  /// \param theWorkshop a workshop to have an access to AIS context and displayer
  /// \param theSelection a container of the selection, it has results and attributres for a feature
  /// \param theOwnersToSelect an out container of found owners
  static void getSelectionOwners(const FeaturePtr& theFeature,
                                  const FeaturePtr& theSketch,
                                  ModuleBase_IWorkshop* theWorkshop,
                                  const FeatureToSelectionMap& theSelection,
                                  SelectMgr_IndexedMapOfOwner& anOwnersToSelect);

private:
  PartSet_Module* myModule;

  bool myPreviousSelectionEnabled; // the previous selection enabled state in the viewer
  bool myIsDragging;
  bool myDragDone;
  Point myCurrentPoint;
  Point myClickedPoint;

  CompositeFeaturePtr myCurrentSketch;

  FeatureToAttributesMap myFeature2AttributeMap; /// a map of a feature to attributes

  Handle(ModuleBase_ShapeInPlaneFilter) myPlaneFilter;
};


#endif