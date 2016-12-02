// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_WidgetSketchLabel.cpp
// Created:     07 July 2014
// Author:      Vitaly SMETANNIKOV

#include "PartSet_WidgetSketchLabel.h"
#include "PartSet_Tools.h"
#include "PartSet_Module.h"
#include "PartSet_PreviewPlanes.h"

#include "SketchPlugin_SketchEntity.h"

#include <XGUI_Workshop.h>
#include <XGUI_Displayer.h>
#include <XGUI_SelectionMgr.h>
#include <XGUI_Selection.h>
#include <XGUI_ViewerProxy.h>
#include <XGUI_ActionsMgr.h>
#include <XGUI_Tools.h>
#include <XGUI_ModuleConnector.h>

#include <ModuleBase_Operation.h>
#include <ModuleBase_ViewerPrs.h>
#include <ModuleBase_Tools.h>
#include <ModuleBase_IModule.h>

#include <ModelAPI_ResultBody.h>
#include <ModelAPI_Tools.h>

#include <GeomAlgoAPI_FaceBuilder.h>
#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Dir.h>
#include <GeomAPI_XYZ.h>
#include <GeomAPI_Face.h>

#include <SketchPlugin_Sketch.h>
#include <SketcherPrs_Tools.h>

#include <Precision.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <AIS_Shape.hxx>
#include <AIS_DimensionSelectionMode.hxx>
#include <Bnd_Box.hxx>

#include <Config_WidgetAPI.h>
#include <Config_PropManager.h>

#include <QLabel>
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QGroupBox>
#include <QPushButton>
#include <QStackedWidget>
#include <QLineEdit>
#include <QDoubleValidator>

#ifndef DBL_MAX
#define DBL_MAX 1.7976931348623158e+308
#endif

PartSet_WidgetSketchLabel::PartSet_WidgetSketchLabel(QWidget* theParent,
                        ModuleBase_IWorkshop* theWorkshop,
                        const Config_WidgetAPI* theData,
                        const QMap<PartSet_Tools::ConstraintVisibleState, bool>& toShowConstraints)
: ModuleBase_WidgetValidated(theParent, theWorkshop, theData)
{
  QVBoxLayout* aLayout = new QVBoxLayout(this);
  ModuleBase_Tools::zeroMargins(aLayout);

  myStackWidget = new QStackedWidget(this);
  myStackWidget->setContentsMargins(0,0,0,0);
  aLayout->addWidget(myStackWidget);

  // Define label for plane selection
  QWidget* aFirstWgt = new QWidget(this);

  // Size of the View control
  mySizeOfViewWidget = new QWidget(aFirstWgt);
  QHBoxLayout* aSizeLayout = new QHBoxLayout(mySizeOfViewWidget);
  aSizeLayout->addWidget(new QLabel("Size of the view", mySizeOfViewWidget));
  mySizeOfView = new QLineEdit(mySizeOfViewWidget);

  QDoubleValidator* aValidator = new QDoubleValidator(0, DBL_MAX, 12, mySizeOfView);
  aValidator->setLocale(ModuleBase_Tools::doubleLocale());
  aValidator->setNotation(QDoubleValidator::StandardNotation);
  mySizeOfView->setValidator(aValidator);
  aSizeLayout->addWidget(mySizeOfView);

  QString aText = QString::fromStdString(theData->getProperty("title"));
  QLabel* aLabel = new QLabel(aText, aFirstWgt);
  aLabel->setWordWrap(true);
  QString aTooltip = QString::fromStdString(theData->getProperty("tooltip"));
  aLabel->setToolTip(aTooltip);
  aLabel->setIndent(5);

  aLayout = new QVBoxLayout(aFirstWgt);
  ModuleBase_Tools::zeroMargins(aLayout);
  aLayout->addWidget(mySizeOfViewWidget);
  aLayout->addWidget(aLabel);
  aLayout->addStretch(1);

  myStackWidget->addWidget(aFirstWgt);

  // Define widget for sketch manmagement
  QWidget* aSecondWgt = new QWidget(this);
  aLayout = new QVBoxLayout(aSecondWgt);
  ModuleBase_Tools::zeroMargins(aLayout);

  QGroupBox* aViewBox = new QGroupBox(tr("Sketcher plane"), this);
  QVBoxLayout* aViewLayout = new QVBoxLayout(aViewBox);

  myViewInverted = new QCheckBox(tr("Reversed"), aViewBox);
  aViewLayout->addWidget(myViewInverted);

  QPushButton* aSetViewBtn =
    new QPushButton(QIcon(":icons/plane_view.png"), tr("Set plane view"), aViewBox);
  connect(aSetViewBtn, SIGNAL(clicked(bool)), this, SLOT(onSetPlaneView()));
  aViewLayout->addWidget(aSetViewBtn);

  aLayout->addWidget(aViewBox);

  QMap<PartSet_Tools::ConstraintVisibleState, QString> aStates;
  aStates[PartSet_Tools::Geometrical] = tr("Show geometrical constraints");
  aStates[PartSet_Tools::Dimensional] = tr("Show dimensional constraints");
  aStates[PartSet_Tools::Expressions] = tr("Show existing expressions");

  QMap<PartSet_Tools::ConstraintVisibleState, QString>::const_iterator anIt = aStates.begin(),
                                                        aLast = aStates.end();
  for (; anIt != aLast; anIt++) {
    QCheckBox* aShowConstraints = new QCheckBox(anIt.value(), this);
    connect(aShowConstraints, SIGNAL(toggled(bool)), this, SLOT(onShowConstraint(bool)));
    aLayout->addWidget(aShowConstraints);

    PartSet_Tools::ConstraintVisibleState aState = anIt.key();
    myShowConstraints[aState] = aShowConstraints;

    if (toShowConstraints.contains(aState))
      aShowConstraints->setChecked(toShowConstraints[aState]);
  }

  myStackWidget->addWidget(aSecondWgt);
  //setLayout(aLayout);

  myPreviewPlanes = new PartSet_PreviewPlanes();
}

PartSet_WidgetSketchLabel::~PartSet_WidgetSketchLabel()
{
}

bool PartSet_WidgetSketchLabel::setSelection(QList<ModuleBase_ViewerPrsPtr>& theValues,
                                             const bool theToValidate)
{
  // do not use the given selection if the plane of the sketch has been already set.
  // If this check is absent, a selected plane in the viewer can be set in the sketch
  // even if the sketch is built on another plane.
  if (plane().get())
    return true;

  ModuleBase_ViewerPrsPtr aPrs = theValues.first();
  bool aDone = setSelectionInternal(theValues, theToValidate);
  if (aDone)
    updateByPlaneSelected(aPrs);
  return aDone;
}

QList<QWidget*> PartSet_WidgetSketchLabel::getControls() const
{
  QList<QWidget*> aResult;
  aResult << myStackWidget;
  return aResult;
}

void PartSet_WidgetSketchLabel::onSelectionChanged()
{
  QList<ModuleBase_ViewerPrsPtr> aSelected = getFilteredSelected();

  if (aSelected.empty())
    return;
  ModuleBase_ViewerPrsPtr aPrs = aSelected.first();
  bool aDone = setSelectionInternal(aSelected, false);
  if (aDone) {
    updateByPlaneSelected(aPrs);
    updateObject(myFeature);
  }
}

void PartSet_WidgetSketchLabel::onShowConstraint(bool theOn)
{
  QCheckBox* aSenderCheckBox = qobject_cast<QCheckBox*>(sender());

  QMap<PartSet_Tools::ConstraintVisibleState, QCheckBox*>::const_iterator
                          anIt = myShowConstraints.begin(), aLast = myShowConstraints.end();

  PartSet_Tools::ConstraintVisibleState aState = PartSet_Tools::Geometrical;
  bool aFound = false;
  for (; anIt != aLast && !aFound; anIt++) {
    aFound = anIt.value() == aSenderCheckBox;
    if (aFound)
      aState = anIt.key();
  }
  if (aFound)
    emit showConstraintToggled(aState, theOn);
}

void PartSet_WidgetSketchLabel::blockAttribute(const AttributePtr& theAttribute,
                                               const bool& theToBlock, bool& isFlushesActived,
                                               bool& isAttributeSetInitializedBlocked)
{
  ModuleBase_WidgetValidated::blockAttribute(theAttribute, theToBlock, isFlushesActived,
                                             isAttributeSetInitializedBlocked);
  // We do not restore the previous state of isAttributeSetInitializedBlocked for each of
  // attributes. It it is necessary, these states should be append to the method attributes
  // or stored in the widget

  std::list<AttributePtr> anAttributes = myFeature->data()->attributes("");
  std::list<AttributePtr>::const_iterator anIt = anAttributes.begin(), aLast = anAttributes.end();
  QStringList aValues;
  for(; anIt != aLast; anIt++) {
    AttributePtr anAttribute = *anIt;
    if (theToBlock)
      anAttribute->blockSetInitialized(true);
    else
      anAttribute->blockSetInitialized(isAttributeSetInitializedBlocked);
  }
}

bool PartSet_WidgetSketchLabel::setSelectionInternal(
                                          const QList<ModuleBase_ViewerPrsPtr>& theValues,
                                          const bool theToValidate)
{
  bool aDone = false;
  if (theValues.empty()) {
    // In order to make reselection possible, set empty object and shape should be done
    setSelectionCustom(std::shared_ptr<ModuleBase_ViewerPrs>(
                              new ModuleBase_ViewerPrs(ObjectPtr(), GeomShapePtr(), NULL)));
    aDone = false;
  }
  else {
    // it removes the processed value from the parameters list
    ModuleBase_ViewerPrsPtr aValue = theValues.first();//.takeFirst();
    if (!theToValidate || isValidInFilters(aValue))
      aDone = setSelectionCustom(aValue);
  }

  return aDone;
}

void PartSet_WidgetSketchLabel::updateByPlaneSelected(const ModuleBase_ViewerPrsPtr& thePrs)
{
  // 1. hide main planes if they have been displayed
  myPreviewPlanes->erasePreviewPlanes(myWorkshop);
  // 2. if the planes were displayed, change the view projection
  const GeomShapePtr& aShape = thePrs->shape();
  std::shared_ptr<GeomAPI_Shape> aGShape;
  std::shared_ptr<GeomAPI_Shape> aBaseShape;

  DataPtr aData = feature()->data();
  AttributeSelectionPtr aSelAttr = std::dynamic_pointer_cast<ModelAPI_AttributeSelection>
                            (aData->attribute(SketchPlugin_SketchEntity::EXTERNAL_ID()));

  // selection happens in OCC viewer
  if (aShape.get() && !aShape->isNull()) {
    aGShape = aShape;

    if (aSelAttr && aSelAttr->context()) {
      aBaseShape = aSelAttr->context()->shape();
    }
  }
  else { // selection happens in OCC viewer(on body) of in the OB browser
    if (aSelAttr) {
      aGShape = aSelAttr->value();
    }
  }
  if (aGShape.get() != NULL) {
    // get plane parameters
    std::shared_ptr<GeomAPI_Face> aFace(new GeomAPI_Face(aGShape));
    std::shared_ptr<GeomAPI_Pln> aPlane = aFace->getPlane();
    std::shared_ptr<GeomAPI_Dir> aDir = aPlane->direction();
    gp_XYZ aXYZ = aDir->impl<gp_Dir>().XYZ();
    double aTwist = 0.0;

    // orienting projection is not needed: it is done in GeomAlgoAPI_FaceBuilder::plane
    /*if (aGShape->impl<TopoDS_Shape>().Orientation() == TopAbs_REVERSED) {
      aXYZ.Reverse();
    }*/

    // Rotate view if the sketcher plane is selected only from preview planes
    // Preview planes are created only if there is no any shape
    bool aRotate = Config_PropManager::boolean(SKETCH_TAB_NAME, "rotate_to_plane", "false");
    if (aRotate) {
      myWorkshop->viewer()->setViewProjection(aXYZ.X(), aXYZ.Y(), aXYZ.Z(), aTwist);
      PartSet_Module* aModule = dynamic_cast<PartSet_Module*>(myWorkshop->module());
      if (aModule)
        aModule->onViewTransformed();
    }
    QString aSizeOfViewStr = mySizeOfView->text();
    if (!aSizeOfViewStr.isEmpty()) {
      bool isOk;
      double aSizeOfView = aSizeOfViewStr.toDouble(&isOk);
      if (isOk && aSizeOfView > 0) {
        Handle(V3d_View) aView3d = myWorkshop->viewer()->activeView();
        if (!aView3d.IsNull()) {
          Bnd_Box aBndBox;
          double aHalfSize = aSizeOfView/2.0;
          aBndBox.Update(-aHalfSize, -aHalfSize, -aHalfSize, aHalfSize, aHalfSize, aHalfSize);
          aView3d->FitAll(aBndBox, 0.01, false);
        }
      }
    }
  }
  // 3. Clear text in the label
  myStackWidget->setCurrentIndex(1);
  //myLabel->setText("");
  //myLabel->setToolTip("");
  XGUI_Workshop* aWorkshop = XGUI_Tools::workshop(myWorkshop);
  disconnect(aWorkshop->selector(), SIGNAL(selectionChanged()),
              this, SLOT(onSelectionChanged()));
  // 4. deactivate face selection filter
  activateFilters(false);

  // 5. Clear selection mode and define sketching mode
  //XGUI_Displayer* aDisp = aWorkshop->displayer();
  //aDisp->closeLocalContexts();
  emit planeSelected(plane());
  // after the plane is selected in the sketch, the sketch selection should be activated
  // it can not be performed in the sketch label widget because, we don't need to switch off
  // the selection by any label deactivation, but need to switch it off by stop the sketch
  activateSelection(true);

  // 6. Update sketcher actions
  XGUI_ActionsMgr* anActMgr = aWorkshop->actionsMgr();
  myWorkshop->updateCommandStatus();
  myWorkshop->viewer()->update();
}

std::shared_ptr<GeomAPI_Pln> PartSet_WidgetSketchLabel::plane() const
{
  CompositeFeaturePtr aSketch = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(myFeature);
  return PartSet_Tools::sketchPlane(aSketch);
}

bool PartSet_WidgetSketchLabel::focusTo()
{
  ModuleBase_Tools::setFocus(myStackWidget, "PartSet_WidgetSketchLabel::focusTo()");
  return true;
}

void PartSet_WidgetSketchLabel::enableFocusProcessing()
{
  myStackWidget->installEventFilter(this);
}

void PartSet_WidgetSketchLabel::storeAttributeValue(const AttributePtr& theAttribute)
{
  ModuleBase_WidgetValidated::storeAttributeValue(theAttribute);
}

void PartSet_WidgetSketchLabel::restoreAttributeValue(const AttributePtr& theAttribute,
                                                      const bool theValid)
{
  ModuleBase_WidgetValidated::restoreAttributeValue(theAttribute, theValid);

  // it is not necessary to save the previous plane value because the plane is chosen once
  DataPtr aData = feature()->data();
  AttributeSelectionPtr aSelAttr = std::dynamic_pointer_cast<ModelAPI_AttributeSelection>
    (aData->attribute(SketchPlugin_SketchEntity::EXTERNAL_ID()));
  if (aSelAttr) {
    ResultPtr anEmptyResult;
    GeomShapePtr anEmptyShape;
    aSelAttr->setValue(anEmptyResult, anEmptyShape);
  }
}

bool PartSet_WidgetSketchLabel::setSelectionCustom(const ModuleBase_ViewerPrsPtr& thePrs)
{
  return fillSketchPlaneBySelection(feature(), thePrs);
}

bool PartSet_WidgetSketchLabel::canFillSketch(const ModuleBase_ViewerPrsPtr& thePrs)
{
  bool aCanFillSketch = true;
  // avoid any selection on sketch object
  ObjectPtr anObject = thePrs->object();
  ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(anObject);
  if (aResult.get()) {
    FeaturePtr aFeature = ModelAPI_Feature::feature(aResult);
    if (aFeature->getKind() == SketchPlugin_Sketch::ID())
      aCanFillSketch = false;
  }
  // check plane or planar face of any non-sketch object
  if (aCanFillSketch) {
    std::shared_ptr<GeomAPI_Face> aGeomFace;

    GeomShapePtr aGeomShape = thePrs->shape();
    if ((!aGeomShape.get() || aGeomShape->isNull()) && aResult.get()) {
      aGeomShape = aResult->shape();
    }

    if (aGeomShape.get() && aGeomShape->shapeType() == GeomAPI_Shape::FACE) {
      std::shared_ptr<GeomAPI_Face> aGeomFace(new GeomAPI_Face(aGeomShape));
      aCanFillSketch = aGeomFace.get() && aGeomFace->isPlanar();
    }
    else
      aCanFillSketch = false;
  }
  return aCanFillSketch;
}

bool PartSet_WidgetSketchLabel::fillSketchPlaneBySelection(const FeaturePtr& theFeature,
                                                           const ModuleBase_ViewerPrsPtr& thePrs)
{
  bool isOwnerSet = false;

  const GeomShapePtr& aShape = thePrs->shape();
  std::shared_ptr<GeomAPI_Dir> aDir;

  if (thePrs->object() && (theFeature != thePrs->object())) {
    DataPtr aData = theFeature->data();
    AttributeSelectionPtr aSelAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeSelection>
      (aData->attribute(SketchPlugin_SketchEntity::EXTERNAL_ID()));
    if (aSelAttr) {
      ResultPtr aRes = std::dynamic_pointer_cast<ModelAPI_Result>(thePrs->object());
      if (aRes) {
        GeomShapePtr aShapePtr(new GeomAPI_Shape());
        if (!aShape.get() || aShape->isNull()) {  // selection happens in the OCC viewer
          aShapePtr = ModelAPI_Tools::shape(aRes);
        }
        else { // selection happens in OB browser
          aShapePtr = aShape;
        }
        if (aShapePtr.get() != NULL) {
          aSelAttr->setValue(aRes, aShapePtr);
          isOwnerSet = true;
        }
      }
    }
  }
  else if (aShape.get() && !aShape->isNull()) {
    const TopoDS_Shape& aTDShape = aShape->impl<TopoDS_Shape>();
    aDir = setSketchPlane(theFeature, aTDShape);
    isOwnerSet = aDir.get();
  }
  return isOwnerSet;
}

void PartSet_WidgetSketchLabel::activateCustom()
{
  std::shared_ptr<GeomAPI_Pln> aPlane = plane();
  if (aPlane.get()) {
    myStackWidget->setCurrentIndex(1);
    activateSelection(true);
    return;
  }

  myStackWidget->setCurrentIndex(0);
  bool aBodyIsVisualized = myPreviewPlanes->hasVisualizedBodies(myWorkshop);

  // Clear previous selection mode It is necessary for correct activation of preview planes
  XGUI_Workshop* aWorkshop = XGUI_Tools::workshop(myWorkshop);
  XGUI_Displayer* aDisp = aWorkshop->displayer();
  aDisp->activateObjects(QIntList(), aDisp->displayedObjects(), false);

  if (!aBodyIsVisualized) {
    // We have to select a plane before any operation
    myPreviewPlanes->showPreviewPlanes(myWorkshop);
    mySizeOfViewWidget->setVisible(true);
  }
  else
    mySizeOfViewWidget->setVisible(false);

  activateSelection(true);

  //myLabel->setText(myText);
  //myLabel->setToolTip(myTooltip);

  connect(XGUI_Tools::workshop(myWorkshop)->selector(), SIGNAL(selectionChanged()),
          this, SLOT(onSelectionChanged()));
  activateFilters(true);
}

void PartSet_WidgetSketchLabel::deactivate()
{
  ModuleBase_ModelWidget::deactivate();
  bool aHidePreview = myPreviewPlanes->isPreviewDisplayed();
  myPreviewPlanes->erasePreviewPlanes(myWorkshop);
  activateSelection(false);

  activateFilters(false);
  if (aHidePreview)
    myWorkshop->viewer()->update();
}

void PartSet_WidgetSketchLabel::activateSelection(bool toActivate)
{
  if (toActivate) {
    QIntList aModes;
    std::shared_ptr<GeomAPI_Pln> aPlane = plane();
    if (aPlane.get()) {
      myWorkshop->module()->activeSelectionModes(aModes);
    }
    else {
      aModes << TopAbs_FACE;
    }
    myWorkshop->activateSubShapesSelection(aModes);
  } else {
    myWorkshop->deactivateSubShapesSelection();
  }
}


std::shared_ptr<GeomAPI_Dir> PartSet_WidgetSketchLabel::setSketchPlane(const FeaturePtr& theFeature,
                                                                       const TopoDS_Shape& theShape)
{
  if (theShape.IsNull())
    return std::shared_ptr<GeomAPI_Dir>();

  // get selected shape
  std::shared_ptr<GeomAPI_Shape> aGShape(new GeomAPI_Shape);
  aGShape->setImpl(new TopoDS_Shape(theShape));



  // get plane parameters
  std::shared_ptr<GeomAPI_Face> aFace(new GeomAPI_Face(aGShape));
  std::shared_ptr<GeomAPI_Pln> aPlane = aFace->getPlane();
  if (!aPlane.get())
    return std::shared_ptr<GeomAPI_Dir>();

  // set plane parameters to feature
  std::shared_ptr<ModelAPI_Data> aData = theFeature->data();
  double anA, aB, aC, aD;
  aPlane->coefficients(anA, aB, aC, aD);

  // calculate attributes of the sketch
  std::shared_ptr<GeomAPI_Dir> aNormDir(new GeomAPI_Dir(anA, aB, aC));
  std::shared_ptr<GeomAPI_XYZ> aCoords = aNormDir->xyz();
  std::shared_ptr<GeomAPI_XYZ> aZero(new GeomAPI_XYZ(0, 0, 0));
  aCoords = aCoords->multiplied(-aD * aCoords->distance(aZero));
  std::shared_ptr<GeomAPI_Pnt> anOrigPnt(new GeomAPI_Pnt(aCoords));
  // X axis is preferable to be dirX on the sketch
  const double tol = Precision::Confusion();
  bool isX = fabs(anA - 1.0) < tol && fabs(aB) < tol && fabs(aC) < tol;
  std::shared_ptr<GeomAPI_Dir> aTempDir(
      isX ? new GeomAPI_Dir(0, 1, 0) : new GeomAPI_Dir(1, 0, 0));
  std::shared_ptr<GeomAPI_Dir> aYDir(new GeomAPI_Dir(aNormDir->cross(aTempDir)));
  std::shared_ptr<GeomAPI_Dir> aXDir(new GeomAPI_Dir(aYDir->cross(aNormDir)));

  std::shared_ptr<GeomDataAPI_Point> anOrigin = std::dynamic_pointer_cast<GeomDataAPI_Point>(
      aData->attribute(SketchPlugin_Sketch::ORIGIN_ID()));
  anOrigin->setValue(anOrigPnt);
  std::shared_ptr<GeomDataAPI_Dir> aNormal = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
      aData->attribute(SketchPlugin_Sketch::NORM_ID()));
  aNormal->setValue(aNormDir);
  std::shared_ptr<GeomDataAPI_Dir> aDirX = std::dynamic_pointer_cast<GeomDataAPI_Dir>(
      aData->attribute(SketchPlugin_Sketch::DIRX_ID()));
  aDirX->setValue(aXDir);
  std::shared_ptr<GeomAPI_Dir> aDir = aPlane->direction();
  return aDir;
}

void PartSet_WidgetSketchLabel::onSetPlaneView()
{
  std::shared_ptr<GeomAPI_Pln> aPlane = plane();
  if (aPlane.get()) {
    std::shared_ptr<GeomAPI_Dir> aDirection = aPlane->direction();
    gp_Dir aDir = aDirection->impl<gp_Dir>();
    if (myViewInverted->isChecked())
      aDir.Reverse();
    myWorkshop->viewer()->setViewProjection(aDir.X(), aDir.Y(), aDir.Z(), 0.);
    PartSet_Module* aModule = dynamic_cast<PartSet_Module*>(myWorkshop->module());
    if (aModule)
      aModule->onViewTransformed();
  }
}
