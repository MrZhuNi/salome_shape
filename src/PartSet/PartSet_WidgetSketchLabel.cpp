// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_WidgetSketchLabel.cpp
// Created:     07 July 2014
// Author:      Vitaly SMETANNIKOV

#include "PartSet_WidgetSketchLabel.h"
#include "PartSet_Tools.h"

#include "SketchPlugin_SketchEntity.h"

#include <XGUI_Workshop.h>
#include <XGUI_Displayer.h>
#include <XGUI_SelectionMgr.h>
#include <XGUI_Selection.h>
#include <XGUI_ViewerProxy.h>
#include <XGUI_ActionsMgr.h>

#include <ModuleBase_Operation.h>
#include <ModuleBase_ViewerPrs.h>
#include <ModuleBase_Tools.h>
#include <ModuleBase_IModule.h>

#include <GeomAlgoAPI_FaceBuilder.h>
#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Dir.h>
#include <GeomAPI_XYZ.h>

#include <SketchPlugin_Sketch.h>

#include <Precision.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <AIS_Shape.hxx>
#include <AIS_DimensionSelectionMode.hxx>

#include <Config_WidgetAPI.h>
#include <Config_PropManager.h>

#include <QLabel>
#include <QTimer>
#include <QApplication>
#include <QVBoxLayout>


PartSet_WidgetSketchLabel::PartSet_WidgetSketchLabel(QWidget* theParent,
                                                     const Config_WidgetAPI* theData,
                                                     const std::string& theParentId)
    : ModuleBase_WidgetValidated(theParent, theData, theParentId),
      myPreviewDisplayed(false),
      myWorkshop(NULL)
{
  myText = QString::fromStdString(theData->getProperty("title"));
  myLabel = new QLabel("", theParent);
  myLabel->setWordWrap(true);
  myTooltip = QString::fromStdString(theData->getProperty("tooltip"));
  myLabel->setToolTip("");
  myLabel->setIndent(5);

  mySelectionTimer = new QTimer(this);
  connect(mySelectionTimer, SIGNAL(timeout()), SLOT(setSketchingMode()));
  mySelectionTimer->setSingleShot(true);

  QVBoxLayout* aLayout = new QVBoxLayout(this);
  ModuleBase_Tools::zeroMargins(aLayout);
  aLayout->addWidget(myLabel);
  setLayout(aLayout);
}

PartSet_WidgetSketchLabel::~PartSet_WidgetSketchLabel()
{
  erasePreviewPlanes();
}

QList<QWidget*> PartSet_WidgetSketchLabel::getControls() const
{
  QList<QWidget*> aResult;
  aResult << myLabel;
  return aResult;
}

void PartSet_WidgetSketchLabel::onPlaneSelected()
{

  XGUI_Selection* aSelection = myWorkshop->selector()->selection();
  QList<ModuleBase_ViewerPrs> aSelected = aSelection->getSelected();
  if (!aSelected.empty()) {
    ModuleBase_ViewerPrs aPrs = aSelected.first();
    Handle(SelectMgr_EntityOwner) anOwner = aSelected.first().owner();
    if (isValid(anOwner)) {
      setSelection(anOwner);

      TopoDS_Shape aShape = aPrs.shape();
      if (!aShape.IsNull()) {
        erasePreviewPlanes();
        DataPtr aData = feature()->data();
        AttributeSelectionPtr aSelAttr = std::dynamic_pointer_cast<ModelAPI_AttributeSelection>
                                  (aData->attribute(SketchPlugin_SketchEntity::EXTERNAL_ID()));
        if (aSelAttr) {
          GeomShapePtr aShapePtr = aSelAttr->value();
          if (aShapePtr.get() == NULL || aShapePtr->isNull()) {
            std::shared_ptr<GeomAPI_Shape> aGShape(new GeomAPI_Shape);
            aGShape->setImpl(new TopoDS_Shape(aShape));
            // get plane parameters
            std::shared_ptr<GeomAPI_Pln> aPlane = GeomAlgoAPI_FaceBuilder::plane(aGShape);
            std::shared_ptr<GeomAPI_Dir> aDir = aPlane->direction();

            myWorkshop->viewer()->setViewProjection(aDir->x(), aDir->y(), aDir->z());
          }
        }

        // Clear text in the label
        myLabel->setText("");
        myLabel->setToolTip("");
        disconnect(myWorkshop->selector(), SIGNAL(selectionChanged()), 
                   this, SLOT(onPlaneSelected()));
        activateFilters(myWorkshop->module()->workshop(), false);

        // Clear selection mode and define sketching mode
        //XGUI_Displayer* aDisp = myWorkshop->displayer();
        //aDisp->closeLocalContexts();
        emit planeSelected(plane());
        setSketchingMode();

        // Update sketcher actions
        XGUI_ActionsMgr* anActMgr = myWorkshop->actionsMgr();
        anActMgr->update();
        myWorkshop->viewer()->update();
      }
    }
  }
}

std::shared_ptr<GeomAPI_Pln> PartSet_WidgetSketchLabel::plane() const
{
  CompositeFeaturePtr aSketch = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(myFeature);
  return PartSet_Tools::sketchPlane(aSketch);

}

bool PartSet_WidgetSketchLabel::focusTo()
{
  myLabel->setFocus();
  return true;
}

void PartSet_WidgetSketchLabel::enableFocusProcessing()
{
  myLabel->installEventFilter(this);
}

void PartSet_WidgetSketchLabel::backupAttributeValue(const bool isBackup)
{
  // it is not necessary to save the previous plane value because the plane is chosen once
  if (!isBackup) {
    DataPtr aData = feature()->data();
    AttributeSelectionPtr aSelAttr = std::dynamic_pointer_cast<ModelAPI_AttributeSelection>
      (aData->attribute(SketchPlugin_SketchEntity::EXTERNAL_ID()));
    if (aSelAttr) {
      ResultPtr anEmptyResult;
      GeomShapePtr anEmptyShape;
      aSelAttr->setValue(anEmptyResult, anEmptyShape);
    }
  }
}

bool PartSet_WidgetSketchLabel::setSelection(const Handle_SelectMgr_EntityOwner& theOwner)
{
  bool isOwnerSet = false;

  ModuleBase_ViewerPrs aPrs;
  myWorkshop->selector()->selection()->fillPresentation(aPrs, theOwner);

  const TopoDS_Shape& aShape = aPrs.shape();
  std::shared_ptr<GeomAPI_Dir> aDir;

  if (aPrs.object() && (feature() != aPrs.object())) {
    DataPtr aData = feature()->data();
    AttributeSelectionPtr aSelAttr = 
      std::dynamic_pointer_cast<ModelAPI_AttributeSelection>
      (aData->attribute(SketchPlugin_SketchEntity::EXTERNAL_ID()));
    if (aSelAttr) {
      ResultPtr aRes = std::dynamic_pointer_cast<ModelAPI_Result>(aPrs.object());
      if (aRes) {
        GeomShapePtr aShapePtr(new GeomAPI_Shape());
        aShapePtr->setImpl(new TopoDS_Shape(aShape));
        aSelAttr->setValue(aRes, aShapePtr);
        isOwnerSet = true;
      }
    }
  }
  else if (!aShape.IsNull()) {
    aDir = setSketchPlane(aShape);
    isOwnerSet = aDir;
  }
  return isOwnerSet;
}

void PartSet_WidgetSketchLabel::activateCustom()
{
  std::shared_ptr<GeomAPI_Pln> aPlane = plane();
  if (aPlane) {
    //setSketchingMode();
    // In order to avoid Opening/Closing of context too often
    // it can be useful for a delay on the property panel filling
    // it is possible that it is not necessary anymore, but it requires a check
    mySelectionTimer->start(20);
  } else {
    // We have to select a plane before any operation
    showPreviewPlanes();

    XGUI_Displayer* aDisp = myWorkshop->displayer();
    //aDisp->openLocalContext();
    //aDisp->activateObjects(QIntList());
    QIntList aModes;
    aModes << TopAbs_FACE;
    aDisp->activateObjects(aModes);

    myLabel->setText(myText);
    myLabel->setToolTip(myTooltip);

    connect(myWorkshop->selector(), SIGNAL(selectionChanged()), this, SLOT(onPlaneSelected()));
    activateFilters(myWorkshop->module()->workshop(), true);

    aDisp->updateViewer();
  }
}

void PartSet_WidgetSketchLabel::deactivate()
{
  // Do not set selection mode if the widget was activated for a small moment 
  mySelectionTimer->stop();
  //XGUI_Displayer* aDisp = myWorkshop->displayer();
  //aDisp->closeLocalContexts();
  erasePreviewPlanes();
}

void PartSet_WidgetSketchLabel::erasePreviewPlanes()
{
  if (myPreviewDisplayed) {
    XGUI_Displayer* aDisp = myWorkshop->displayer();
    aDisp->eraseAIS(myYZPlane, false);
    aDisp->eraseAIS(myXZPlane, false);
    aDisp->eraseAIS(myXYPlane, false);
    myPreviewDisplayed = false;
  }
}

void PartSet_WidgetSketchLabel::showPreviewPlanes()
{
  if (myPreviewDisplayed)
    return;

  if (!myYZPlane) { // If planes are not created
    // Create Preview
    std::shared_ptr<GeomAPI_Pnt> anOrigin(new GeomAPI_Pnt(0, 0, 0));
    std::shared_ptr<GeomAPI_Dir> aYZDir(new GeomAPI_Dir(1, 0, 0));
    std::shared_ptr<GeomAPI_Dir> aXZDir(new GeomAPI_Dir(0, 1, 0));
    std::shared_ptr<GeomAPI_Dir> aXYDir(new GeomAPI_Dir(0, 0, 1));

    std::vector<int> aYZRGB, aXZRGB, aXYRGB;
    aYZRGB = Config_PropManager::color("Visualization", "yz_plane_color",
                                                        YZ_PLANE_COLOR);
    aXZRGB = Config_PropManager::color("Visualization", "xz_plane_color",
                                                        XZ_PLANE_COLOR);
    aXYRGB = Config_PropManager::color("Visualization", "xy_plane_color",
                                                        XY_PLANE_COLOR);
    int aR[] = {aYZRGB[0], aYZRGB[1], aYZRGB[2]};
    int aG[] = {aXZRGB[0], aXZRGB[1], aXZRGB[2]};
    int aB[] = {aXYRGB[0], aXYRGB[1], aXYRGB[2]};

    myYZPlane = createPreviewPlane(anOrigin, aYZDir, aR);
    myXZPlane = createPreviewPlane(anOrigin, aXZDir, aG);
    myXYPlane = createPreviewPlane(anOrigin, aXYDir, aB);
  }
  XGUI_Displayer* aDisp = myWorkshop->displayer();
  aDisp->displayAIS(myYZPlane, false);
  aDisp->displayAIS(myXZPlane, false);
  aDisp->displayAIS(myXYPlane, false);
  myPreviewDisplayed = true;
}


AISObjectPtr PartSet_WidgetSketchLabel::createPreviewPlane(std::shared_ptr<GeomAPI_Pnt> theOrigin, 
                                                           std::shared_ptr<GeomAPI_Dir> theNorm, 
                                                           const int theRGB[3])
{
  double aSize = Config_PropManager::integer("Sketch planes", "planes_size", PLANE_SIZE);
  std::shared_ptr<GeomAPI_Shape> aFace = GeomAlgoAPI_FaceBuilder::square(theOrigin, theNorm, aSize);
  AISObjectPtr aAIS = AISObjectPtr(new GeomAPI_AISObject());
  aAIS->createShape(aFace);
  aAIS->setWidth(Config_PropManager::integer("Sketch planes", "planes_thickness", SKETCH_WIDTH));
  aAIS->setColor(theRGB[0], theRGB[1], theRGB[2]);
  return aAIS;
}


std::shared_ptr<GeomAPI_Dir> PartSet_WidgetSketchLabel::setSketchPlane(const TopoDS_Shape& theShape)
{
  if (theShape.IsNull())
    return std::shared_ptr<GeomAPI_Dir>();

  // get selected shape
  std::shared_ptr<GeomAPI_Shape> aGShape(new GeomAPI_Shape);
  aGShape->setImpl(new TopoDS_Shape(theShape));

  // get plane parameters
  std::shared_ptr<GeomAPI_Pln> aPlane = GeomAlgoAPI_FaceBuilder::plane(aGShape);

  // set plane parameters to feature
  std::shared_ptr<ModelAPI_Data> aData = feature()->data();
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


void PartSet_WidgetSketchLabel::setSketchingMode()
{
  XGUI_Displayer* aDisp = myWorkshop->displayer();
  // Clear standard selection modes if they are defined
  //aDisp->activateObjects(aModes);
  //aDisp->openLocalContext();

  // Get default selection modes
  QIntList aModes;
  aModes.append(AIS_DSM_Text);
  aModes.append(AIS_DSM_Line);
  aModes.append(AIS_Shape::SelectionMode((TopAbs_ShapeEnum) TopAbs_VERTEX));
  aModes.append(AIS_Shape::SelectionMode((TopAbs_ShapeEnum) TopAbs_EDGE));

  aDisp->activateObjects(aModes);
}
