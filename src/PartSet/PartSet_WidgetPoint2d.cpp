// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_WidgetPoint2D.cpp
// Created:     25 Apr 2014
// Author:      Natalia ERMOLAEVA

#include "PartSet_WidgetPoint2d.h"
#include <PartSet_Tools.h>
#include <PartSet_Module.h>
#include <PartSet_SketcherReetntrantMgr.h>

#include <XGUI_Tools.h>
#include <XGUI_Workshop.h>
#include <XGUI_Displayer.h>

#include <ModuleBase_ParamSpinBox.h>
#include <ModuleBase_Tools.h>
#include <ModuleBase_IViewer.h>
#include <ModuleBase_IViewWindow.h>
#include <ModuleBase_ISelection.h>
#include <ModuleBase_ViewerPrs.h>
#include <ModuleBase_WidgetValidator.h>

#include <Config_Keywords.h>
#include <Config_WidgetAPI.h>

#include <Events_Loop.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_AttributeBoolean.h>

#include <ModelAPI_Feature.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Object.h>
#include <GeomDataAPI_Point2D.h>
#include <GeomAPI_Pnt2d.h>

#include <GeomAPI_ShapeExplorer.h>
#include <GeomAPI_Vertex.h>

#include <SketchPlugin_Feature.h>
#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_Point.h>

#include <QGroupBox>
#include <QGridLayout>
#include <QLabel>
#include <QEvent>
#include <QMouseEvent>
#include <QApplication>

#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <BRep_Tool.hxx>

#include <cfloat>
#include <climits>

const double MaxCoordinate = 1e12;

static QStringList MyFeaturesForCoincedence;

PartSet_WidgetPoint2D::PartSet_WidgetPoint2D(QWidget* theParent, 
                                             ModuleBase_IWorkshop* theWorkshop,
                                             const Config_WidgetAPI* theData)
: ModuleBase_ModelWidget(theParent, theData), myWorkshop(theWorkshop),
  myValueIsCashed(false), myIsFeatureVisibleInCash(true),
  myXValueInCash(0), myYValueInCash(0)
{
  if (MyFeaturesForCoincedence.isEmpty()) {
    MyFeaturesForCoincedence << SketchPlugin_Line::ID().c_str()
      << SketchPlugin_Arc::ID().c_str()
      << SketchPlugin_Point::ID().c_str()
      << SketchPlugin_Circle::ID().c_str();
  }

  // the control should accept the focus, so the boolean flag is corrected to be true
  myIsObligatory = true;
  QString aPageName = QString::fromStdString(theData->getProperty(CONTAINER_PAGE_NAME));
  myGroupBox = new QGroupBox(aPageName, theParent);
  myGroupBox->setFlat(false);

  bool aAcceptVariables = theData->getBooleanAttribute(DOUBLE_WDG_ACCEPT_EXPRESSIONS, true);

  QGridLayout* aGroupLay = new QGridLayout(myGroupBox);
  ModuleBase_Tools::adjustMargins(aGroupLay);
  aGroupLay->setSpacing(2);
  aGroupLay->setColumnStretch(1, 1);
  {
    QLabel* aLabel = new QLabel(myGroupBox);
    aLabel->setText(tr("X "));
    aGroupLay->addWidget(aLabel, 0, 0);

    myXSpin = new ModuleBase_ParamSpinBox(myGroupBox);
    myXSpin->setAcceptVariables(aAcceptVariables);
    myXSpin->setMinimum(-DBL_MAX);
    myXSpin->setMaximum(DBL_MAX);
    myXSpin->setToolTip(tr("X"));
    aGroupLay->addWidget(myXSpin, 0, 1);

    connect(myXSpin, SIGNAL(valueChanged(const QString&)), this, SIGNAL(valuesModified()));
    myXSpin->setValueEnabled(isValueEnabled());
  }
  {
    QLabel* aLabel = new QLabel(myGroupBox);
    aLabel->setText(tr("Y "));
    aGroupLay->addWidget(aLabel, 1, 0);

    myYSpin = new ModuleBase_ParamSpinBox(myGroupBox);
    myYSpin->setAcceptVariables(aAcceptVariables);
    myYSpin->setMinimum(-DBL_MAX);
    myYSpin->setMaximum(DBL_MAX);
    myYSpin->setToolTip(tr("Y"));
    aGroupLay->addWidget(myYSpin, 1, 1);

    connect(myYSpin, SIGNAL(valueChanged(const QString&)), this, SIGNAL(valuesModified()));
    myYSpin->setValueEnabled(isValueEnabled());
  }
  QVBoxLayout* aLayout = new QVBoxLayout(this);
  ModuleBase_Tools::zeroMargins(aLayout);
  aLayout->addWidget(myGroupBox);
  setLayout(aLayout);

  myWidgetValidator = new ModuleBase_WidgetValidator(this, myWorkshop);
}

bool PartSet_WidgetPoint2D::isValidSelectionCustom(const ModuleBase_ViewerPrsPtr& theValue)
{
  bool aValid = true;

  PartSet_Module* aModule = dynamic_cast<PartSet_Module*>(myWorkshop->module());
  if (aModule->sketchReentranceMgr()->isInternalEditActive()) 
    return true; /// when internal edit is started a new feature is created. I has not results, AIS

  /// the selection is not possible if the current feature has no presentation for the current
  /// attribute not in AIS not in results. If so, no object in current feature where make
  /// coincidence, so selection is not necessary
  bool aFoundPoint = false;
  GeomShapePtr anAISShape;
  GeomPresentablePtr aPrs = std::dynamic_pointer_cast<GeomAPI_IPresentable>(myFeature);
  if (aPrs.get()) {
    AISObjectPtr anAIS;
    anAIS = aPrs->getAISObject(anAIS);
    if (anAIS.get()) {
      anAISShape = anAIS->getShape();
    }
  }
  const std::list<std::shared_ptr<ModelAPI_Result> >& aResults = myFeature->results();
  if (!anAISShape.get() && aResults.empty())
    return true;

  /// analysis of AIS
  std::shared_ptr<ModelAPI_Data> aData = myFeature->data();
  std::shared_ptr<GeomDataAPI_Point2D> aPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(attributeID()));
  std::shared_ptr<GeomAPI_Pnt2d> aPoint = aPointAttr->pnt();
  if (anAISShape.get())
    aFoundPoint = shapeContainsPoint(anAISShape, aPoint, mySketch);

  /// analysis of results
  std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aRIter = aResults.cbegin();
  for (; aRIter != aResults.cend() && !aFoundPoint; aRIter++) {
    ResultPtr aResult = *aRIter;
    if (aResult.get() && aResult->shape().get()) {
      GeomShapePtr aShape = aResult->shape();
      aFoundPoint = shapeContainsPoint(aShape, aPoint, mySketch);
    }
  }
  return aFoundPoint;
}

bool PartSet_WidgetPoint2D::resetCustom()
{
  bool aDone = false;
  if (!isUseReset() || isComputedDefault() || myXSpin->hasVariable() || myYSpin->hasVariable()) {
    aDone = false;
  }
  else {
    if (myValueIsCashed) {
      // if the restored value should be hidden, aDone = true to set
      // reset state for the widget in the parent
      aDone = restoreCurentValue();
      emit objectUpdated();
    }
    else {
      bool isOk;
      double aDefValue = QString::fromStdString(getDefaultValue()).toDouble(&isOk);
      // it is important to block the spin box control in order to do not through out the
      // locking of the validating state.
      ModuleBase_Tools::setSpinValue(myXSpin, isOk ? aDefValue : 0.0);
      ModuleBase_Tools::setSpinValue(myYSpin, isOk ? aDefValue : 0.0);
      storeValueCustom();
      aDone = true;
    }
  }
  return aDone;
}

PartSet_WidgetPoint2D::~PartSet_WidgetPoint2D()
{
}

bool PartSet_WidgetPoint2D::setSelection(QList<ModuleBase_ViewerPrsPtr>& theValues,
                                         const bool theToValidate)
{
  bool isDone = false;
  if (theValues.empty())
    return isDone;

  ModuleBase_ViewerPrsPtr aValue = theValues.takeFirst();
  GeomShapePtr aShape = aValue->shape();
  if (aShape.get() && !aShape->isNull()) {
    Handle(V3d_View) aView = myWorkshop->viewer()->activeView();
    double aX, aY;
    const TopoDS_Shape& aTDShape = aShape->impl<TopoDS_Shape>();
    if (getPoint2d(aView, aTDShape, aX, aY)) {
      isDone = setPoint(aX, aY);
      PartSet_Tools::setConstraints(mySketch, feature(), attributeID(), aX, aY);
    }
  }
  return isDone;
}

void PartSet_WidgetPoint2D::selectContent()
{
  myXSpin->selectAll();
}

bool PartSet_WidgetPoint2D::setPoint(double theX, double theY)
{
  if (fabs(theX) >= MaxCoordinate)
    return false;
  if (fabs(theY) >= MaxCoordinate)
    return false;

  ModuleBase_Tools::setSpinValue(myXSpin, theX);
  ModuleBase_Tools::setSpinValue(myYSpin, theY);

  storeValue();
  return true;
}

bool PartSet_WidgetPoint2D::storeValueCustom()
{
  std::shared_ptr<ModelAPI_Data> aData = myFeature->data();
  if (!aData) // can be on abort of sketcher element
    return false;
  std::shared_ptr<GeomDataAPI_Point2D> aPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(attributeID()));

  PartSet_WidgetPoint2D* that = (PartSet_WidgetPoint2D*) this;
  bool isBlocked = that->blockSignals(true);
  bool isImmutable = aPoint->setImmutable(true);

  // if text is not empty then setValue will be ignored
  // so we should set the text at first
  aPoint->setText(myXSpin->hasVariable() ? myXSpin->text().toStdString() : "",
                  myYSpin->hasVariable() ? myYSpin->text().toStdString() : "");
  aPoint->setValue(!myXSpin->hasVariable() ? myXSpin->value() : aPoint->x(),
                   !myYSpin->hasVariable() ? myYSpin->value() : aPoint->y());

  // after movement the solver will call the update event: optimization
  moveObject(myFeature);
  aPoint->setImmutable(isImmutable);
  that->blockSignals(isBlocked);

  return true;
}

bool PartSet_WidgetPoint2D::restoreValueCustom()
{
  std::shared_ptr<ModelAPI_Data> aData = myFeature->data();
  std::shared_ptr<GeomDataAPI_Point2D> aPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(attributeID()));
  QString aTextX = QString::fromStdString(aPoint->isInitialized() ? aPoint->textX() : "0");
  QString aTextY = QString::fromStdString(aPoint->isInitialized() ? aPoint->textY() : "0");

  bool isDouble = false;
  double aVal = 0;
  if (aTextX.isEmpty()) {
    ModuleBase_Tools::setSpinValue(myXSpin, aPoint->x());
  } else {
    aVal = aTextX.toDouble(&isDouble);
    if (isDouble)
      ModuleBase_Tools::setSpinValue(myXSpin, aVal);
    else
      ModuleBase_Tools::setSpinText(myXSpin, aTextX);
  }
  if (aTextY.isEmpty()) {
    ModuleBase_Tools::setSpinValue(myYSpin, aPoint->y());
  } else {
    aVal = aTextY.toDouble(&isDouble);
    if (isDouble)
      ModuleBase_Tools::setSpinValue(myYSpin, aVal);
    else
      ModuleBase_Tools::setSpinText(myYSpin, aTextY);
  }
  //if (aTextX.empty() || aTextY.empty()) {
  //  ModuleBase_Tools::setSpinValue(myXSpin, aPoint->x());
  //  ModuleBase_Tools::setSpinValue(myYSpin, aPoint->y());
  //} else {
  //  ModuleBase_Tools::setSpinText(myXSpin, QString::fromStdString(aTextX));
  //  ModuleBase_Tools::setSpinText(myYSpin, QString::fromStdString(aTextY));
  //}
  return true;
}

void PartSet_WidgetPoint2D::storeCurentValue()
{
  // do not use cash if a variable is used
  if (myXSpin->hasVariable() || myYSpin->hasVariable())
    return;

  myValueIsCashed = true;
  myIsFeatureVisibleInCash = XGUI_Displayer::isVisible(
                       XGUI_Tools::workshop(myWorkshop)->displayer(), myFeature);
  myXValueInCash = myXSpin->value();
  myYValueInCash = myYSpin->value();
}

bool PartSet_WidgetPoint2D::restoreCurentValue()
{
  bool aRestoredAndHidden = true;

  bool isVisible = myIsFeatureVisibleInCash;
  // fill the control widgets by the cashed value

  myValueIsCashed = false;
  myIsFeatureVisibleInCash = true;
  ModuleBase_Tools::setSpinValue(myXSpin, myXValueInCash);
  ModuleBase_Tools::setSpinValue(myYSpin, myYValueInCash);

  // store value to the model
  storeValueCustom();
  if (isVisible) {
    setValueState(Stored);
    aRestoredAndHidden = false;
  }
  else
    aRestoredAndHidden = true;

  return aRestoredAndHidden;
}

QList<QWidget*> PartSet_WidgetPoint2D::getControls() const
{
  QList<QWidget*> aControls;
  aControls.append(myXSpin);
  aControls.append(myYSpin);
  return aControls;
}


void PartSet_WidgetPoint2D::activateCustom()
{
  QIntList aModes;
  aModes << TopAbs_VERTEX;
  aModes << TopAbs_EDGE;
  myWorkshop->activateSubShapesSelection(aModes);

  if (!isEditingMode()) {
    FeaturePtr aFeature = feature();
    if (aFeature.get() && aFeature->getKind() == SketchPlugin_Point::ID())
      storeValue();
  }
}

void PartSet_WidgetPoint2D::deactivate()
{
  // the value of the control should be stored to model if it was not
  // initialized yet. It is important when we leave this control by Tab key.
  // It should not be performed by the widget activation as the preview
  // is visualized with default value. Line point is moved to origin.
  AttributePtr anAttribute = myFeature->data()->attribute(attributeID());
  if (anAttribute && !anAttribute->isInitialized())
    storeValue();

  ModuleBase_ModelWidget::deactivate();
  myWorkshop->deactivateSubShapesSelection();
}

bool PartSet_WidgetPoint2D::getPoint2d(const Handle(V3d_View)& theView, 
                                       const TopoDS_Shape& theShape, 
                                       double& theX, double& theY) const
{
  if (!theShape.IsNull()) {
    if (theShape.ShapeType() == TopAbs_VERTEX) {
      const TopoDS_Vertex& aVertex = TopoDS::Vertex(theShape);
      if (!aVertex.IsNull()) {
        // A case when point is taken from existing vertex
        gp_Pnt aPoint = BRep_Tool::Pnt(aVertex);
        PartSet_Tools::convertTo2D(aPoint, mySketch, theView, theX, theY);
        return true;
      }
    }
  }
  return false;
}

bool PartSet_WidgetPoint2D::setConstraintWith(const ObjectPtr& theObject)
{
  std::shared_ptr<GeomDataAPI_Point2D> aFeaturePoint;
  if (feature()->isMacro()) {
    AttributePtr aThisAttr = feature()->data()->attribute(attributeID());
    std::shared_ptr<GeomDataAPI_Point2D> anAttrPoint =
                               std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aThisAttr);
    if (anAttrPoint.get()) {
      // the macro feature will be removed after the operation is stopped, so we need to build
      // coicidence to possible sub-features
      aFeaturePoint = PartSet_Tools::findFirstEqualPointInArgumentFeatures(feature(),
                                                                 anAttrPoint->pnt());
    }
  }
  else {
    AttributePtr aThisAttr = feature()->data()->attribute(attributeID());
    aFeaturePoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aThisAttr);
  }
  if (!aFeaturePoint.get())
    return false;

  // Create point-edge coincedence
  FeaturePtr aFeature = mySketch->addFeature(SketchPlugin_ConstraintCoincidence::ID());
  std::shared_ptr<ModelAPI_Data> aData = aFeature->data();

  std::shared_ptr<ModelAPI_AttributeRefAttr> aRef1 = std::dynamic_pointer_cast<
      ModelAPI_AttributeRefAttr>(aData->attribute(SketchPlugin_Constraint::ENTITY_A()));

  aRef1->setAttr(aFeaturePoint);

  std::shared_ptr<ModelAPI_AttributeRefAttr> aRef2 = std::dynamic_pointer_cast<
      ModelAPI_AttributeRefAttr>(aData->attribute(SketchPlugin_Constraint::ENTITY_B()));
  aRef2->setObject(theObject);

  // we need to flush created signal in order to coincidence is processed by solver
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_CREATED));

  return true;
}

void PartSet_WidgetPoint2D::mouseReleased(ModuleBase_IViewWindow* theWindow, QMouseEvent* theEvent)
{
  // the contex menu release by the right button should not be processed by this widget
  if (theEvent->button() != Qt::LeftButton)
    return;

  ModuleBase_ISelection* aSelection = myWorkshop->selection();
  Handle(V3d_View) aView = theWindow->v3dView();

  QList<ModuleBase_ViewerPrsPtr> aList = aSelection->getSelected(ModuleBase_ISelection::Viewer);
  ModuleBase_ViewerPrsPtr aFirstValue = aList.size() > 0 ? aList.first() : ModuleBase_ViewerPrsPtr();
  if (!aFirstValue.get() && myPreSelected.get()) {
    aFirstValue = myPreSelected;
  }
  // if we have selection and use it
  if (aFirstValue.get() && isValidSelectionCustom(aFirstValue)) {
    GeomShapePtr aGeomShape = aFirstValue->shape();
    TopoDS_Shape aShape = aGeomShape->impl<TopoDS_Shape>();
    ObjectPtr aObject = aFirstValue->object();

    FeaturePtr aSelectedFeature = ModelAPI_Feature::feature(aObject);
    bool anExternal = false;
    std::shared_ptr<SketchPlugin_Feature> aSPFeature;
    if (aSelectedFeature.get() != NULL)
      aSPFeature = std::dynamic_pointer_cast<SketchPlugin_Feature>(aSelectedFeature);
      if ((!aSPFeature && !aShape.IsNull()) ||
          (aSPFeature.get() && aSPFeature->isExternal())) {
        ResultPtr aFixedObject;
        anExternal = true;
        aFixedObject = PartSet_Tools::findFixedObjectByExternal(aShape, aObject, mySketch);
        if (!aFixedObject.get())
          aFixedObject = PartSet_Tools::createFixedObjectByExternal(aShape, aObject, mySketch);
        double aX, aY;
        if (getPoint2d(aView, aShape, aX, aY) && isFeatureContainsPoint(myFeature, aX, aY)) {
          // do not create a constraint to the point, which already used by the feature
          // if the feature contains the point, focus is not switched
          setPoint(aX, aY);
        }
        else {
          if (getPoint2d(aView, aShape, aX, aY))
            setPoint(aX, aY);
          else
            setValueState(Stored); // in case of edge selection, Apply state should also be updated
          bool anOrphanPoint = aShape.ShapeType() == TopAbs_VERTEX ||
                               isOrphanPoint(aSelectedFeature, mySketch, aX, aY);
          if (anExternal) {
            anOrphanPoint = true; // we should not stop reentrant operation on external objects because
            // they are not participate in the contour creation excepting external vertices
            if (aShape.ShapeType() == TopAbs_VERTEX) {
              FeaturePtr aFixedFeature = ModelAPI_Feature::feature(aFixedObject);
              if (aFixedFeature.get() && aFixedFeature->getKind() == SketchPlugin_Point::ID()) {
                anOrphanPoint = isOrphanPoint(aFixedFeature, mySketch, aX, aY);
              }
            }
            else {
              // point is taken from mouse event and set in attribute. It should be done before setting
              // coinident constraint to the external line. If a point is created, it should be in the mouse
              // clicked point
              gp_Pnt aPoint = PartSet_Tools::convertClickToPoint(theEvent->pos(), theWindow->v3dView());
              double aX, anY;
              PartSet_Tools::convertTo2D(aPoint, mySketch, aView, aX, anY);
              setPoint(aX, anY);
            }
          }
          if (aFixedObject.get())
            setConstraintWith(aFixedObject);
          // fignal updated should be flushed in order to visualize possible created external objects
          // e.g. selection of trihedron axis when input end arc point
          updateObject(feature());

          if (!anOrphanPoint)
            emit vertexSelected(); // it stops the reentrant operation

          emit focusOutWidget(this);
        }
      }
    if (!anExternal) {
      double aX, aY;
      bool isProcessed = false;
      if (getPoint2d(aView, aShape, aX, aY) && isFeatureContainsPoint(myFeature, aX, aY)) {
        // when the point is selected, the coordinates of the point should be set into the attribute
        // if the feature contains the point, focus is not switched
        setPoint(aX, aY);
      }
      else {
        bool anOrphanPoint = isOrphanPoint(aSelectedFeature, mySketch, aX, aY);
        // do not set a coincidence constraint in the attribute if the feature contains a point
        // with the same coordinates. It is important for line creation in order to do not set
        // the same constraints for the same points, oterwise the result line has zero length.
        bool isAuxiliaryFeature = false;
        if (getPoint2d(aView, aShape, aX, aY)) {
          setPoint(aX, aY);
          feature()->execute();
          PartSet_Tools::setConstraints(mySketch, feature(), attributeID(), aX, aY);
        }
        else if (aShape.ShapeType() == TopAbs_EDGE) {
          // point is taken from mouse event and set in attribute. It should be done before setting
          // coinident constraint to the external line. If a point is created, it should be in the mouse
          // clicked point
          gp_Pnt aPoint = PartSet_Tools::convertClickToPoint(theEvent->pos(), theWindow->v3dView());
          PartSet_Tools::convertTo2D(aPoint, mySketch, aView, aX, aY);
          setPoint(aX, aY);
          setConstraintWith(aObject);
          setValueState(Stored); // in case of edge selection, Apply state should also be updated
          isAuxiliaryFeature = PartSet_Tools::isAuxiliarySketchEntity(aObject);
        }
        // it is important to perform updateObject() in order to the current value is 
        // processed by Sketch Solver. Test case: line is created from a previous point
        // to some distance, but in the area of the highlighting of the point. Constraint
        // coincidence is created, after the solver is performed, the distance between the
        // points of the line becomes less than the tolerance. Validator of the line returns
        // false, the line will be aborted, but sketch stays valid.
        updateObject(feature());
        if (!anOrphanPoint && !anExternal && !isAuxiliaryFeature)
          emit vertexSelected();
        emit focusOutWidget(this);
      }
    }
  }
  // End of Bug dependent fragment
  else {
    // A case when point is taken from mouse event
    gp_Pnt aPoint = PartSet_Tools::convertClickToPoint(theEvent->pos(), theWindow->v3dView());
    double aX, anY;
    PartSet_Tools::convertTo2D(aPoint, mySketch, aView, aX, anY);

    // if the feature contains the point, focus is not switched
    if (!setPoint(aX, anY) || isFeatureContainsPoint(myFeature, aX, anY))
      return;

    /// Start alternative code
    //std::shared_ptr<GeomDataAPI_Point2D> aFeaturePoint = std::dynamic_pointer_cast<
    //    GeomDataAPI_Point2D>(feature()->data()->attribute(attributeID()));
    //QList<FeaturePtr> aIgnore;
    //aIgnore.append(feature());

    //double aTolerance = aView->Convert(7);
    //std::shared_ptr<GeomDataAPI_Point2D> aAttrPnt = 
    //  PartSet_Tools::findAttributePoint(mySketch, aX, anY, aTolerance, aIgnore);
    //if (aAttrPnt.get() != NULL) {
    //  aFeaturePoint->setValue(aAttrPnt->pnt());
    //  PartSet_Tools::createConstraint(mySketch, aAttrPnt, aFeaturePoint);
    //  emit vertexSelected();
    //}
    /// End alternative code
    emit focusOutWidget(this);
  }
}

void PartSet_WidgetPoint2D::setPreSelection(const std::shared_ptr<ModuleBase_ViewerPrs>& thePreSelected)
{
  myPreSelected = thePreSelected;
}

void PartSet_WidgetPoint2D::mouseMoved(ModuleBase_IViewWindow* theWindow, QMouseEvent* theEvent)
{
  PartSet_Module* aModule = dynamic_cast<PartSet_Module*>(myWorkshop->module());

  if (isEditingMode() || aModule->sketchReentranceMgr()->isInternalEditStarted())
    return;

  gp_Pnt aPoint = PartSet_Tools::convertClickToPoint(theEvent->pos(), theWindow->v3dView());

  double aX, anY;
  PartSet_Tools::convertTo2D(aPoint, mySketch, theWindow->v3dView(), aX, anY);
  if (myState != ModifiedInViewer)
    storeCurentValue();
  // we need to block the value state change 
  bool isBlocked = blockValueState(true);
  setPoint(aX, anY);
  blockValueState(isBlocked);
  setValueState(ModifiedInViewer);
}

double PartSet_WidgetPoint2D::x() const
{
  return myXSpin->value();
}

double PartSet_WidgetPoint2D::y() const
{
  return myYSpin->value();
}


bool PartSet_WidgetPoint2D::isFeatureContainsPoint(const FeaturePtr& theFeature,
                                                   double theX, double theY)
{
  bool aPointIsFound = false;

  if (feature()->getKind() != SketchPlugin_Line::ID())
    return aPointIsFound;

  AttributePtr aWidgetAttribute = myFeature->attribute(attributeID());

  std::shared_ptr<GeomAPI_Pnt2d> aPnt2d = 
                                    std::shared_ptr<GeomAPI_Pnt2d>(new GeomAPI_Pnt2d(theX, theY));
  std::list<AttributePtr> anAttributes =
                                myFeature->data()->attributes(GeomDataAPI_Point2D::typeId());
  std::list<AttributePtr>::iterator anIter = anAttributes.begin();
  for(; anIter != anAttributes.end() && !aPointIsFound; anIter++) {
    AttributePoint2DPtr aPoint2DAttribute =
      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(*anIter);
    if (aPoint2DAttribute == aWidgetAttribute)
      continue;
    if (aPoint2DAttribute.get() && aPoint2DAttribute->isInitialized()) {
      aPointIsFound = aPoint2DAttribute->pnt()->isEqual(aPnt2d);
    }
  }
  return aPointIsFound;
}

void PartSet_WidgetPoint2D::initializeValueByActivate()
{
}

/*void PartSet_WidgetPoint2D::onValuesChanged()
{
  emit valuesChanged();
}*/

bool PartSet_WidgetPoint2D::processEnter()
{
  bool isModified = getValueState() == ModifiedInPP;
  if (isModified) {
    bool isXModified = myXSpin->hasFocus();
    emit valuesChanged();
    if (isXModified)
      myXSpin->selectAll();
    else
      myYSpin->selectAll();
  }
  return isModified;
}

bool PartSet_WidgetPoint2D::useSelectedShapes() const
{
  return true;
}

bool PartSet_WidgetPoint2D::isOrphanPoint(const FeaturePtr& theFeature,
                                          const CompositeFeaturePtr& theSketch,
                                          double theX, double theY)
{
  bool anOrphanPoint = false;
  if (theFeature.get()) {
    std::shared_ptr<GeomDataAPI_Point2D> aPointAttr;
    std::string aFeatureKind = theFeature->getKind();
    if (aFeatureKind == SketchPlugin_Point::ID())
      aPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                                       theFeature->attribute(SketchPlugin_Point::COORD_ID()));
    else if (aFeatureKind == SketchPlugin_Circle::ID())
      aPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                                       theFeature->attribute(SketchPlugin_Circle::CENTER_ID()));

    else if (aFeatureKind == SketchPlugin_Arc::ID())
      aPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                                       theFeature->attribute(SketchPlugin_Arc::CENTER_ID()));

    /// check that the geometry point with the given coordinates is the checked point
    /// e.g. in arc the (x,y) point can not coicide to the center point and it automatically
    /// means that this point is not an orphant one.
    if (aPointAttr.get()) {
      std::shared_ptr<GeomAPI_Pnt2d> aCheckedPoint = std::shared_ptr<GeomAPI_Pnt2d>(
                                                    new GeomAPI_Pnt2d(theX, theY));
      if (!aCheckedPoint->isEqual(aPointAttr->pnt()))
        return anOrphanPoint;
    }

    if (aPointAttr.get()) {
      std::shared_ptr<GeomAPI_Pnt2d> aPoint = aPointAttr->pnt();
      // we need to find coincidence features in results also, because external object(point)
      // uses refs to me in another feature.
      FeaturePtr aCoincidence = PartSet_Tools::findFirstCoincidence(theFeature, aPoint);
      anOrphanPoint = true;
      // if there is at least one concident line to the point, the point is not an orphant
      if (aCoincidence.get()) {
        QList<FeaturePtr> aCoinsideLines;
        QList<FeaturePtr> aCoins;
        PartSet_Tools::findCoincidences(aCoincidence, aCoinsideLines, aCoins,
                                        SketchPlugin_ConstraintCoincidence::ENTITY_A());
        PartSet_Tools::findCoincidences(aCoincidence, aCoinsideLines, aCoins,
                                        SketchPlugin_ConstraintCoincidence::ENTITY_B());
        QList<FeaturePtr>::const_iterator anIt = aCoinsideLines.begin(),
                                          aLast = aCoinsideLines.end();
        for (; anIt != aLast && anOrphanPoint; anIt++) {
          anOrphanPoint = (*anIt)->getKind() != SketchPlugin_Line::ID();
        }
      }
    }
  }
  return anOrphanPoint;
}

bool PartSet_WidgetPoint2D::shapeContainsPoint(const GeomShapePtr& theShape,
                                               const std::shared_ptr<GeomAPI_Pnt2d>& thePoint,
                                               const CompositeFeaturePtr& theSketch)
{
  std::shared_ptr<GeomAPI_Pnt> aPoint = PartSet_Tools::point3D(thePoint, theSketch);

  bool aContainPoint = false;
  GeomAPI_ShapeExplorer anExp(theShape, GeomAPI_Shape::VERTEX);
  for(; anExp.more() && !aContainPoint; anExp.next()) {
    std::shared_ptr<GeomAPI_Shape> aVertexInCompSolid = anExp.current();
    std::shared_ptr<GeomAPI_Vertex> aVertex(new GeomAPI_Vertex(aVertexInCompSolid));
    if (aVertex.get())
      aContainPoint = aPoint->isEqual(aVertex->point());
  }
  return aContainPoint;
}
