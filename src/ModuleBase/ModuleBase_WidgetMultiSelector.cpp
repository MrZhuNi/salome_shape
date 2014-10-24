/*
 * ModuleBase_WidgetMultiSelector.cpp
 *
 *  Created on: Aug 28, 2014
 *      Author: sbh
 */

#include <ModuleBase_WidgetMultiSelector.h>
#include <ModuleBase_WidgetShapeSelector.h>
#include <ModuleBase_ISelection.h>
#include <ModuleBase_IWorkshop.h>
#include <ModuleBase_Tools.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_AttributeSelectionList.h>

#include <Config_WidgetAPI.h>

#include <QGridLayout>
#include <QLabel>
#include <QListWidget>
#include <QObject>
#include <QString>
#include <QComboBox>
#include <QEvent>

#include <boost/smart_ptr/shared_ptr.hpp>
#include <string>

ModuleBase_WidgetMultiSelector::ModuleBase_WidgetMultiSelector(QWidget* theParent,
                                                               ModuleBase_IWorkshop* theWorkshop,
                                                               const Config_WidgetAPI* theData,
                                                               const std::string& theParentId)
    : ModuleBase_ModelWidget(theParent, theData, theParentId),
      myWorkshop(theWorkshop), myIsActive(false), myUseSubShapes(false)
{
  myMainWidget = new QWidget(theParent);
  QGridLayout* aMainLay = new QGridLayout(myMainWidget);
  ModuleBase_Tools::adjustMargins(aMainLay);

  QLabel* aTypeLabel = new QLabel(tr("Type"), myMainWidget);
  aMainLay->addWidget(aTypeLabel, 0, 0);

  myTypeCombo = new QComboBox(myMainWidget);
  // There is no sence to paramerize list of types while we can not parametrize selection mode
  QString aTypesStr("Vertices Edges Faces Solids");
  myShapeTypes = aTypesStr.split(' ');
  myTypeCombo->addItems(myShapeTypes);
  aMainLay->addWidget(myTypeCombo, 0, 1);

  QLabel* aListLabel = new QLabel(tr("Selected objects:"), myMainWidget);
  aMainLay->addWidget(aListLabel, 1, 0, 1, -1);

  myListControl = new QListWidget(myMainWidget);
  aMainLay->addWidget(myListControl, 2, 0, 2, -1);
  aMainLay->setColumnStretch(1, 1);
  myMainWidget->setLayout(aMainLay);

  //TODO: Move into the base class
  myUseSubShapes = theData->getBooleanAttribute("use_subshapes", false);
  //TODO_END
  connect(myTypeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(onSelectionTypeChanged()));
  activateSelection(true);
}

ModuleBase_WidgetMultiSelector::~ModuleBase_WidgetMultiSelector()
{
  activateSelection(false);
}

//********************************************************************
bool ModuleBase_WidgetMultiSelector::storeValue() const
{
  // A rare case when plugin was not loaded. 
  if(!myFeature)
    return false;
  DataPtr aData = myFeature->data();
  AttributeSelectionListPtr aSelectionListAttr = 
    boost::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(aData->attribute(attributeID()));

  if (aSelectionListAttr && (mySelection.size() > 0)) {
    aSelectionListAttr->clear();
    foreach (GeomSelection aSelec, mySelection) {
      aSelectionListAttr->append(aSelec.first, aSelec.second);
    }
    updateObject(myFeature);
    return true;
  }
  return false;
}

//********************************************************************
bool ModuleBase_WidgetMultiSelector::restoreValue()
{
  // A rare case when plugin was not loaded. 
  if(!myFeature)
    return false;
  DataPtr aData = myFeature->data();
  AttributeSelectionListPtr aSelectionListAttr = 
    boost::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(aData->attribute(attributeID()));

  if (aSelectionListAttr) {
    mySelection.clear();
    for (int i = 0; i < aSelectionListAttr->size(); i++) {
      AttributeSelectionPtr aSelectAttr = aSelectionListAttr->value(i);
      mySelection.append(GeomSelection(aSelectAttr->context(), aSelectAttr->value()));
    }
    updateSelectionList();
    return true;
  }
  return false;
}

//********************************************************************
QWidget* ModuleBase_WidgetMultiSelector::getControl() const
{
  return myMainWidget;
}

//********************************************************************
QList<QWidget*> ModuleBase_WidgetMultiSelector::getControls() const
{
  QList<QWidget*> result;
  result << myTypeCombo;
  result << myListControl;
  return result;
}

//********************************************************************
bool ModuleBase_WidgetMultiSelector::eventFilter(QObject* theObj, QEvent* theEvent)
{
  if (theObj == myListControl) {
    if (theEvent->type() == QEvent::FocusIn)
      activateSelection(true);
  }
  return ModuleBase_ModelWidget::eventFilter(theObj, theEvent);
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::onSelectionChanged()
{
  ModuleBase_ISelection* aSelection = myWorkshop->selection();
  NCollection_List<TopoDS_Shape> aSelectedShapes; //, aFilteredShapes;
  std::list<ObjectPtr> aOwnersList;
  aSelection->selectedShapes(aSelectedShapes, aOwnersList);

  mySelection.clear();
  std::list<ObjectPtr>::const_iterator aIt;
  NCollection_List<TopoDS_Shape>::Iterator aShpIt(aSelectedShapes);
  GeomShapePtr aShape;
  for (aIt = aOwnersList.cbegin(); aIt != aOwnersList.cend(); aShpIt.Next(), aIt++) {
    ResultPtr aResult = boost::dynamic_pointer_cast<ModelAPI_Result>(*aIt);
    aShape = boost::shared_ptr<GeomAPI_Shape>(new GeomAPI_Shape());
    aShape->setImpl(new TopoDS_Shape(aShpIt.Value()));
    mySelection.append(GeomSelection(aResult, aShape));
  }
  updateSelectionList();
  storeValue();
  emit valuesChanged();
}


//********************************************************************
void ModuleBase_WidgetMultiSelector::updateSelectionList()
{
  QString aType;
  if (myTypeCombo->currentText().toLower() == "vertices")
    aType = "vertex";
  else if (myTypeCombo->currentText().toLower() == "edges")
    aType = "edge";
  else if (myTypeCombo->currentText().toLower() == "faces")
    aType = "face";
  else if (myTypeCombo->currentText().toLower() == "solids")
    aType = "solid";
 
  myListControl->clear();
  int i = 1;
  foreach (GeomSelection aSel, mySelection) {
    QString aName(aSel.first->data()->name().c_str());
    aName += ":" + aType + QString("_%1").arg(i);
    myListControl->addItem(aName);
    i++;
  }
}


//********************************************************************
void ModuleBase_WidgetMultiSelector::filterShapes(const NCollection_List<TopoDS_Shape>& theShapesToFilter,
                                                  NCollection_List<TopoDS_Shape>& theResult)
{
  if(myTypeCombo->count() == 0 || theShapesToFilter.IsEmpty())
    return;
  TopAbs_ShapeEnum aReferenceType =
      ModuleBase_WidgetShapeSelector::shapeType(myTypeCombo->currentText());
  NCollection_List<TopoDS_Shape>::Iterator anIter(theShapesToFilter);
  for (; anIter.More(); anIter.Next()) {
    TopoDS_Shape aShape = anIter.Value();
    if (aShape.IsNull() || aShape.ShapeType() != aReferenceType)
      continue;
    theResult.Append(aShape);
  }
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::activateSelection(bool toActivate)
{
  myIsActive = toActivate;
  if (myIsActive) {
    connect(myWorkshop, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
    onSelectionTypeChanged();
  } else {
    disconnect(myWorkshop, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
    myWorkshop->deactivateSubShapesSelection();
  }
}

//********************************************************************
void ModuleBase_WidgetMultiSelector::onSelectionTypeChanged()
{
  QString aNewType = myTypeCombo->currentText();
  QIntList aList;
  aList.append(ModuleBase_WidgetShapeSelector::shapeType(aNewType));
  myWorkshop->activateSubShapesSelection(aList);
}
