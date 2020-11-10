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

#include "FeaturesPlugin_WidgetFilletMultiRadiuses.h"
#include "FeaturesPlugin_Fillet.h"

#include <ModuleBase_Tools.h>
#include <ModuleBase_IWorkshop.h>
#include <ModuleBase_IModule.h>
#include <ModuleBase_ISelection.h>
#include <ModuleBase_IPropertyPanel.h>

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeDoubleArray.h>
#include <ModelAPI_AttributeDouble.h>

#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeIntArray.h>
#include <ModelAPI_AttributeString.h>

#include <QLayout>
#include <QWidget>
#include <QFormLayout>
#include <QComboBox>
#include <QSpinBox>
#include <QLabel>
#include <QSlider>
#include <QTableWidget>
#include <QPushButton>
#include <QHeaderView>
#include <QStackedWidget>
#include <QValidator>
#include <QLineEdit>
#include <QEvent>
#include <QMouseEvent>
#include <QScrollBar>
#include <QApplication>
#include <qabstractitemmodel.h>
#include <GeomAlgoAPI_PointBuilder.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Curve.h>
#include <Locale_Convert.h>
#include <QStandardItem>
#include <map>


const char* MYFirstCol = "Shape";
const char* MYTrue = "True";
const char* MYFalse = "False";

class TextFieldDoubleValidator : public QDoubleValidator {
public:
  TextFieldDoubleValidator (QObject * theParent = 0) : QDoubleValidator(theParent) {}
  TextFieldDoubleValidator (double theBottom, double theTop, int theDecimals, QObject * theParent) :
  QDoubleValidator(theBottom, theTop, theDecimals, theParent) {}

  QValidator::State validate(QString & theS, int & thePos) const {
      if (theS.isEmpty() || theS.startsWith("0.") || theS == "0" ){//|| s.startsWith("-")) {
          // allow empty field or minus sign
          return QValidator::Intermediate;
      }
      // check length of decimal places
      QChar point = locale().decimalPoint();
      if(theS.indexOf(point) != -1) {
          int alengthDecimals = theS.length() - theS.indexOf(point) - 1;
          if (alengthDecimals > decimals()) {
              return QValidator::Invalid;
          }
      }
      // check range of value
      bool isNumber;
      double aValue = locale().toDouble(theS, &isNumber);
      if (isNumber && bottom() <= aValue && aValue <= top()) {
          return QValidator::Acceptable;
      }
      return QValidator::Invalid;
  }
};

DataArrayItemDelegate::DataArrayItemDelegate(bool theTypeMethod)
  : QStyledItemDelegate(), myTypeMethodPoint(theTypeMethod)
{
}

QWidget* DataArrayItemDelegate::createEditor(QWidget* theParent,
                                             const QStyleOptionViewItem & theOption,
                                             const QModelIndex& theIndex ) const
{
  QWidget* aEditor = 0;
  if ((theIndex.column() == 0) && (theIndex.row() > 0 )) {
    QWidget* aWgt = QStyledItemDelegate::createEditor(theParent, theOption, theIndex);
    QLineEdit* aEdt = static_cast<QLineEdit*>(aWgt);
    aEdt->setReadOnly(true);
    aEditor = aEdt;
  } else {
    QLineEdit* aLineEdt = 0;

    aLineEdt = dynamic_cast<QLineEdit*>(QStyledItemDelegate::createEditor(theParent,
                                                                          theOption,
                                                                          theIndex));
    if (aLineEdt) {
      if( theIndex.column() == 1 ){
        TextFieldDoubleValidator* doubleVal =
                            new TextFieldDoubleValidator(0.00001 , 0.99, 6, aLineEdt);
        doubleVal->setNotation(TextFieldDoubleValidator::StandardNotation);
        aLineEdt->setValidator(doubleVal);
      }
      else{
        QDoubleValidator* doubleVal = new QDoubleValidator(0.0 , 10000.0, 6, aLineEdt);
        doubleVal->setNotation(QDoubleValidator::StandardNotation);
        aLineEdt->setValidator(doubleVal);
      }
      aEditor = aLineEdt;
    }
  }

  connect(aEditor, SIGNAL(textEdited(const QString&)),
      SLOT(onEditItem(const QString&)));
  return aEditor;
}

void DataArrayItemDelegate::onEditItem(const QString& theText)
{
  QWidget* aWgt = dynamic_cast<QWidget*>(sender());
  commitData(aWgt);
}

//**********************************************************************************
FeaturesPlugin_WidgetFilletMultiRadiuses::
  FeaturesPlugin_WidgetFilletMultiRadiuses(QWidget* theParent,
                                           ModuleBase_IWorkshop* theWorkshop,
                                           const Config_WidgetAPI* theData,
                                           bool theTypeMethodBypoint):
ModuleBase_WidgetSelector(theParent, theWorkshop, theData), myHeaderEditor(0),
 myTypeMethodBypoint(theTypeMethodBypoint),mySetSelection(true)
{
  QVBoxLayout* aMainLayout = new QVBoxLayout(this);

  aMainLayout->addWidget(new QLabel("Radii", this));
  // Radiuses controls
  QFrame* aRadiusesFrame = new QFrame(this);
  aRadiusesFrame->setFrameShape( QFrame::HLine );
  aRadiusesFrame->setFrameShape(QFrame::Box);
  aRadiusesFrame->setFrameStyle(QFrame::StyledPanel);
  QVBoxLayout* aRadiusesLayout = new QVBoxLayout();


  myDataTbl = new QTableWidget(2, 3, aRadiusesFrame);

  myDelegate = new DataArrayItemDelegate(myTypeMethodBypoint);
  myDataTbl->setItemDelegate(myDelegate);
  myDataTbl->verticalHeader()->hide();
  myDataTbl->setRowHeight(0, 25);

  if(myTypeMethodBypoint){
    myfirstRowValue.push_back("Start extremity");
    myfirstRowValue.push_back("0");
    myfirstRowValue.push_back("1");
    myLastRowValue.push_back("End extremity");
    myLastRowValue.push_back("1");
    myLastRowValue.push_back("2");
  }else{
    myfirstRowValue.push_back("0");
    myfirstRowValue.push_back("0");
    myfirstRowValue.push_back("1");
    myLastRowValue.push_back("1");
    myLastRowValue.push_back("1");
    myLastRowValue.push_back("2");
  }

  if(myTypeMethodBypoint)
    myDataTbl->hideColumn(1);
  else
    myDataTbl->hideColumn(0);

  QStringList aHeaders;
  aHeaders << "Point";
  aHeaders << "Curvilinear \n Abscissa";
  aHeaders << "Radius";

  myDataTbl->setHorizontalHeaderLabels(aHeaders);
  myDataTbl->installEventFilter(this);

  QTableWidgetItem* anItem;
  for(int j =0; j<3;j++)
  {
    anItem = new QTableWidgetItem(myfirstRowValue[j]);
    if(j==0 || j== 1)
      anItem->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled);
    myDataTbl->setItem(0, j, anItem);
    anItem = new QTableWidgetItem(myLastRowValue[j]);
    if(j==0 || j== 1)
      anItem->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled);
    myDataTbl->setItem(1, j, anItem);
  }

  connect(myDataTbl, SIGNAL(cellChanged(int, int)), SLOT(onTableEdited(int, int)));

  aRadiusesLayout->addWidget(myDataTbl);
  ///========================

  // Buttons below
  QWidget* aBtnWgt = new QWidget(this);
  aRadiusesLayout->addWidget(aBtnWgt);
  QHBoxLayout* aBtnLayout = new QHBoxLayout(aBtnWgt);
  aBtnLayout->setAlignment(Qt::AlignLeft);
  aBtnLayout->setContentsMargins(0, 0, 0, 0);

  QPushButton* aAddBtn = new QPushButton(tr("+"), aBtnWgt);
  QFont aFont = aAddBtn->font();
  aFont.setPointSize(12);
  aAddBtn->setFont(aFont);
  aBtnLayout->addWidget(aAddBtn);
  aBtnLayout->addStretch(1);

  myRemoveBtn = new QPushButton(tr("-"), aBtnWgt);
  aFont = myRemoveBtn->font();
  aFont.setPointSize(12);
  myRemoveBtn->setFont(aFont);
  aBtnLayout->addWidget(myRemoveBtn);

  aRadiusesFrame->setLayout(aRadiusesLayout ) ;
  aMainLayout->addWidget(aRadiusesFrame);

  connect(aAddBtn, SIGNAL(clicked(bool)), SLOT(onAdd()));
  connect(myRemoveBtn, SIGNAL(clicked(bool)), SLOT(onRemove()));
  connect(qApp,SIGNAL(focusChanged(QWidget*, QWidget*)),SLOT(onFocusChanged(QWidget*,QWidget*)));
  connect(myDataTbl, SIGNAL(itemSelectionChanged()), SLOT(onListSelection()));
}

QList<QWidget*> FeaturesPlugin_WidgetFilletMultiRadiuses::getControls() const
{
  QList<QWidget*> aControls;
  // this control will accept focus and will be highlighted in the Property Panel
  aControls.append(myDataTbl);
  return aControls;
}

//**********************************************************************************
QIntList FeaturesPlugin_WidgetFilletMultiRadiuses::shapeTypes() const
{
  QIntList aRes;

  aRes.append(ModuleBase_Tools::shapeType("vertex"));
  return aRes;
}

//**********************************************************************************
void FeaturesPlugin_WidgetFilletMultiRadiuses::deactivate()
{

  myWorkshop->module()->enableCustomModes();

  ModuleBase_WidgetSelector::deactivate();

  myWorkshop->module()->deactivateCustomPrs(ModuleBase_IModule::CustomizeHighlightedObjects, true);

  storeValueCustom();
}

//**********************************************************************************
bool FeaturesPlugin_WidgetFilletMultiRadiuses::eventFilter(QObject* theObject, QEvent* theEvent)
{
  if (theEvent->type() == QEvent::KeyPress) {
    QKeyEvent* akey = static_cast<QKeyEvent*>(theEvent);
    if ( (akey->key()==Qt::Key_Enter) || (akey->key()==Qt::Key_Return) ) {
        updateObject(myFeature);
    }
  }
  return ModuleBase_WidgetSelector::eventFilter(theObject, theEvent);
}

//**********************************************************************************
bool FeaturesPlugin_WidgetFilletMultiRadiuses::storeValueCustom()
{

  DataPtr aData = myFeature->data();

  AttributeTablesPtr aTablesAttr;

  if(myTypeMethodBypoint)
    aTablesAttr = aData->tables(FeaturesPlugin_Fillet::VALUES_ID());
  else
    aTablesAttr = aData->tables(FeaturesPlugin_Fillet::VALUES_CURV_ID());

  // Store data-
  int aRows = myDataTbl->rowCount();

  aTablesAttr->setSize(aRows, 2);
  aTablesAttr->setType(ModelAPI_AttributeTables::ValueType::DOUBLE);

  for (int i = 0; i < aRows; i++) {
    for (int j = 0; j < 2; j++) {
      QString aTblVal = myDataTbl->item(i, j+1)->text();
      aTablesAttr->setValue( getValue( aTblVal ), i, j);
    }
  }

  if(myTypeMethodBypoint && mySortList ){
      AttributeSelectionListPtr aSelectionListAttr =
          aData->selectionList(FeaturesPlugin_Fillet::ARRAY_POINT_RADIUS_BY_POINTS());
    if(  aSelectionListAttr->isInitialized() )
    {
      std::map<double,std::pair<QString,QString>>::iterator anItValuesSort;
      QList<std::shared_ptr<ModuleBase_ViewerPrs>> alist = getAttributeSelection();
      aSelectionListAttr->clear();
      anItValuesSort = myValuesSort.begin();
      for(;anItValuesSort != myValuesSort.end();++anItValuesSort)
      {
        std::pair<QString,QString> elem = anItValuesSort->second;
        ResultPtr aResult;
        GeomShapePtr aShape;
        foreach(ModuleBase_ViewerPrsPtr aPrs, alist) {
          aResult = std::dynamic_pointer_cast<ModelAPI_Result>(aPrs->object());
          aShape = aPrs->shape();
          if (!aResult.get() && !aShape.get())
            continue;
          if (aResult->data()->name()== Locale::Convert::toWString(elem.first.toStdString())) {
            aSelectionListAttr->append(aResult, aShape);
            break;
          }
        }
      }
    }
  }
  return true;
}

//**********************************************************************************
bool FeaturesPlugin_WidgetFilletMultiRadiuses::restoreValueCustom()
{

  if ( !mySetSelection) {
    mySetSelection = true;
    return false;
  }
  DataPtr aData = myFeature->data();

  AttributeTablesPtr aTablesAttr;

  if(myTypeMethodBypoint){
    aTablesAttr = aData->tables(FeaturesPlugin_Fillet::VALUES_ID());
  }
  else{
    aTablesAttr = aData->tables(FeaturesPlugin_Fillet::VALUES_CURV_ID());
  }

  if( aTablesAttr->rows() == 0 )
    {
      aTablesAttr->setSize(2,2);
      ModelAPI_AttributeTables::Value aVar;
      aVar.myDouble = 0.0;
      aTablesAttr->setValue(aVar,0,0);
      aVar.myDouble = 1;
      aTablesAttr->setValue(aVar,0,1);
      aTablesAttr->setValue(aVar,1,0);
      aVar.myDouble = 2;
      aTablesAttr->setValue(aVar,1,1);
    }

  AttributeSelectionPtr anEdges =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelection>
                (aData->attribute(FeaturesPlugin_Fillet::EDGE_SELECTED_ID()));

  myValuesSort.clear();
  double aRes;
  int aRows = 0;
  std::map<double,std::pair<QString,QString>>::iterator anItValuesSort;
  if(myTypeMethodBypoint)
  {
    if( !anEdges->isInitialized() )
      return false;

    GeomEdgePtr anEdge = GeomEdgePtr(new GeomAPI_Edge( anEdges->value()));
    GeomPointPtr aFirst =  anEdge->firstPoint();
    GeomPointPtr aLast  =  anEdge->lastPoint();
    double taille = aFirst->distance(aLast);

    std::shared_ptr<GeomAPI_Curve> aCurve(new GeomAPI_Curve(anEdges->value()));

    // Load points
    AttributeSelectionListPtr aSelectionListAttr =
            aData->selectionList(FeaturesPlugin_Fillet::ARRAY_POINT_RADIUS_BY_POINTS());
    AttributeDoubleArrayPtr aArrayAttr;
    ListOfShape aPoints;

    std::set<GeomShapePtr> aContexts;
    for (int anIndex = 0; anIndex < aSelectionListAttr->size(); ++anIndex) {
        AttributeSelectionPtr aSelection = aSelectionListAttr->value(anIndex);
        GeomShapePtr aShape = aSelection->value();

        ResultPtr aContext = aSelection->context();
        aContexts.insert(aContext->shape());

        if (!aShape.get()) {
          aShape = aContext->shape();
        }

        aPoints.push_back(aShape);
    }

    int anI =0;

    ListOfShape::const_iterator aPointsIt = aPoints.begin();
    for (; aPointsIt != aPoints.end(); ++aPointsIt) {
      AttributeSelectionPtr attsel = aSelectionListAttr->value(anI);
      std::shared_ptr<GeomAPI_Pnt> aPnt = GeomAlgoAPI_PointBuilder::point(*aPointsIt);
      std::shared_ptr<GeomAPI_Pnt> aPntCurv = aCurve->project(aPnt);
      aRes = (aPntCurv->distance(aFirst) / taille);
      QString aName = QString::fromStdWString(attsel->namingName());
      QString aRad = findRadius( QString::number(aRes) );
      if ( myValuesSort.find( aRes ) == myValuesSort.end() )
        myValuesSort[ aRes ] = std::make_pair(aName, aRad );
      anI++;
      mySortList = true;
    }

    aRes = 0.0;
    myValuesSort[ aRes ] = std::make_pair (myfirstRowValue[0], findRadius( QString::number(aRes)));
    aRes = 1.0;
    myValuesSort[ aRes ] = std::make_pair (myLastRowValue[0], findRadius( QString::number(aRes)));
    aRows =  myValuesSort.size();
  }else{

    ModelAPI_AttributeTables::Value aVal;
    if (aTablesAttr->isInitialized()){

        for (int anIndex = 0; anIndex < aTablesAttr->rows(); ++anIndex) {
          aVal = aTablesAttr->value(anIndex,0);
          double aCurv = getValueText(aVal).toDouble();
          if ( myValuesSort.find( aCurv ) == myValuesSort.end() )
            myValuesSort[ aCurv ] =
                          std::make_pair(getValueText(aVal),findRadius(getValueText(aVal)));
        }
        aRows = myValuesSort.size();
    }else{
      aRes = 0.0;
      myValuesSort[ aRes ] = std::make_pair (myfirstRowValue[0], myfirstRowValue[2]) ;
      aRes = 1.0;
      myValuesSort[ aRes ] = std::make_pair (myLastRowValue[0], myLastRowValue[2]) ;
      aRows = 2;
    }

  }

  QTableWidgetItem* anItem = 0;
  myDataTbl->blockSignals(true);
  anItem = myDataTbl->item( myDataTbl->rowCount() -1, 0 );
  anItem->setFlags(Qt::NoItemFlags | Qt::ItemIsSelectable
                 | Qt::ItemIsEnabled | Qt::ItemIsEditable);
  anItem = myDataTbl->item( myDataTbl->rowCount() -1, 1 );
  anItem->setFlags(Qt::NoItemFlags | Qt::ItemIsSelectable
                 | Qt::ItemIsEnabled | Qt::ItemIsEditable);

  myDataTbl->setRowCount(aRows);

  anItValuesSort = myValuesSort.begin();

  for (int k = 0; k < aRows; k++, ++anItValuesSort ) {

    std::pair<QString,QString> elem = anItValuesSort->second;
    QString aCurv = QString::number(anItValuesSort->first);

    anItem = myDataTbl->item(k, 0);
    if (anItem) {
          anItem->setText( elem.first);
    } else {
          anItem = new QTableWidgetItem(elem.first);
          myDataTbl->setItem(k, 0, anItem);
    }
    anItem = myDataTbl->item(k, 1);
    if (anItem) {
          anItem->setText( aCurv);
    } else {
          anItem = new QTableWidgetItem(aCurv);
          myDataTbl->setItem(k, 1, anItem);
    }
    anItem = myDataTbl->item(k, 2);
    if (anItem) {
          anItem->setText( elem.second);
    } else {
          anItem = new QTableWidgetItem(elem.second);
          myDataTbl->setItem(k, 2, anItem);
    }
  }
  anItem = myDataTbl->item(myDataTbl->rowCount()-1, 0 );
  anItem->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled );
  anItem = myDataTbl->item(myDataTbl->rowCount()-1, 1 );
  anItem->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled );
  myDataTbl->blockSignals(false);

  return true;
}


//**********************************************************************************
void FeaturesPlugin_WidgetFilletMultiRadiuses::onAdd()
{
  QModelIndex anIndex = myDataTbl->currentIndex();
   int i  = anIndex.row();
  if( i != -1
      && !(!myDataTbl->currentItem()->isSelected() && myDataTbl->rowCount() >2))
  {

    myDataTbl->blockSignals(true);

    if ( i == myDataTbl->rowCount() -1)
      i = myDataTbl->rowCount() - 2;

    if ( i == 0)
      i = 1;
    else
      i= i+1;

    myDataTbl->model()->insertRow(i);

    QTableWidgetItem* anItem =0;

    anItem = myDataTbl->item( i, 0 );
    anItem = new QTableWidgetItem( "" );
    myDataTbl->setItem(i, 0, anItem);
    anItem = new QTableWidgetItem("0.1");
    myDataTbl->setItem(i, 1, anItem);
    anItem = new QTableWidgetItem("0.5");
    myDataTbl->setItem(i, 2, anItem);
    myDataTbl->blockSignals(false);

    emit valuesChanged();
    myDataTbl->setCurrentCell( i, 0);
  }
}

//**********************************************************************************
void FeaturesPlugin_WidgetFilletMultiRadiuses::onRemove()
{
  QModelIndex anIndex = myDataTbl->currentIndex();

  if( myDataTbl->currentItem()->isSelected() && myDataTbl->rowCount() >2
    && anIndex.row() != -1
    && anIndex.row() != 0
    && anIndex.row() != myDataTbl->rowCount() -1){

    myDataTbl->blockSignals(true);
    if (myTypeMethodBypoint)
    {
      QString aName = myDataTbl->item( anIndex.row() , 0 )->text();
      AttributeSelectionListPtr aSelList =
          myFeature->data()->selectionList(FeaturesPlugin_Fillet::ARRAY_POINT_RADIUS_BY_POINTS());

      AttributeSelectionPtr anAttr;
      for (int i = 0; i < aSelList->size(); i++) {
          anAttr = aSelList->value(i);
          if( aName == QString::fromStdWString( anAttr->namingName()) )
          {
            aSelList->remove({i});
          }
      }
    }
    myDataTbl->model()->removeRow(anIndex.row());
    myDataTbl->blockSignals(false);
    emit valuesChanged();
    updateObject(myFeature);
  }
}

//**********************************************************************************
void FeaturesPlugin_WidgetFilletMultiRadiuses::onTableEdited(int theRow, int theCol)
{
  // Do not store here column of names
  if (theCol != 0 && myFeature.get())
  {

    if( theCol == 1 && myDataTbl->item(theRow, theCol)->text() == "0" ) {
      myDataTbl->blockSignals(true);
      myDataTbl->item(theRow, theCol)->setText("0.1");
      myDataTbl->blockSignals(false);
    }

    ModelAPI_AttributeTables::Value aVal = getValue(myDataTbl->item(theRow, theCol)->text());

    AttributeTablesPtr aTablesAttr;

    if(myTypeMethodBypoint)
      aTablesAttr = myFeature->data()->tables(FeaturesPlugin_Fillet::VALUES_ID());
    else
      aTablesAttr = myFeature->data()->tables(FeaturesPlugin_Fillet::VALUES_CURV_ID());

    if (aTablesAttr->isInitialized())
    {
      aTablesAttr->setValue(aVal,theRow, theCol - 1);
      emit valuesChanged();
    }
  }
}

//**********************************************************************************
bool FeaturesPlugin_WidgetFilletMultiRadiuses::
  isValidSelectionCustom(const std::shared_ptr<ModuleBase_ViewerPrs>& thePrs)
{
  return true;
}

//**********************************************************************************
bool FeaturesPlugin_WidgetFilletMultiRadiuses::processEnter()
{
  return true;
}

//**********************************************************************************
bool FeaturesPlugin_WidgetFilletMultiRadiuses::
  setSelection(QList<std::shared_ptr<ModuleBase_ViewerPrs>>& theValues, const bool theToValidate)
{
  mySortList = false;
  if ( theValues.size() > 1 || !myTypeMethodBypoint || theValues.size() == 0 )
  {
    mySetSelection = false;
    return false;
  }
  QModelIndex index = myDataTbl->currentIndex();
  if(  index.row() == -1 )
  {
    mySetSelection = false;
    return false;
  }

  if( !myDataTbl->currentItem()->isSelected()){
    mySetSelection = false;
    return false;
  }

  AttributeSelectionListPtr aSelList =
    myFeature->data()->selectionList(FeaturesPlugin_Fillet::ARRAY_POINT_RADIUS_BY_POINTS());

  ResultPtr aResult;
  GeomShapePtr aShape;
  ModuleBase_ViewerPrsPtr aValue = theValues.first();
  aResult = std::dynamic_pointer_cast<ModelAPI_Result>(aValue->object());
  aShape = aValue->shape();
  if ( (aResult.get() || aShape.get() ) && !aSelList->isInList(aResult, aShape)) {
    aSelList->append(aResult, aShape);
    onRemove();
   }else{
      mySetSelection = false;
      return false;
  }

  return true;
}

//**********************************************************************************
QList<std::shared_ptr<ModuleBase_ViewerPrs>>
  FeaturesPlugin_WidgetFilletMultiRadiuses::getAttributeSelection() const
{
  QList<std::shared_ptr<ModuleBase_ViewerPrs>> aList;
  if(myFeature) {
    DataPtr aData = myFeature->data();
    AttributeSelectionListPtr aSelList =
      aData->selectionList(FeaturesPlugin_Fillet::ARRAY_POINT_RADIUS_BY_POINTS());
    AttributeSelectionPtr aAttr;
    ObjectPtr anObject;
    for (int i = 0; i < aSelList->size(); i++) {
      aAttr = aSelList->value(i);
      ModuleBase_ViewerPrsPtr
        aPrs(new ModuleBase_ViewerPrs(aAttr->context(), aAttr->value(), NULL));
      aList.append(aPrs);
    }
  }
  return aList;
}

//**********************************************************************************
ModelAPI_AttributeTables::Value
          FeaturesPlugin_WidgetFilletMultiRadiuses::getValue(QString theStrVal) const
{
  ModelAPI_AttributeTables::Value aVal;
  aVal.myDouble = theStrVal.toDouble();
  return aVal;
}

//**********************************************************************************
QString FeaturesPlugin_WidgetFilletMultiRadiuses::findRadius(QString thename) const
{
  AttributeTablesPtr aTablesAttr;

  if(myTypeMethodBypoint)
    aTablesAttr = myFeature->data()->tables(FeaturesPlugin_Fillet::VALUES_ID());
  else
    aTablesAttr = myFeature->data()->tables(FeaturesPlugin_Fillet::VALUES_CURV_ID());

  for(int i = 0; i < aTablesAttr->rows(); ++i)
  {
    ModelAPI_AttributeTables::Value aVal = aTablesAttr->value( i, 0) ;
    if( getValueText(aVal) == thename )
    {
        aVal = aTablesAttr->value( i, 1);
        return getValueText(aVal);
    }
  }
  return "0.5";
}

//**********************************************************************************
QString FeaturesPlugin_WidgetFilletMultiRadiuses::
                          getValueText(ModelAPI_AttributeTables::Value& theVal) const
{
    return QString::number(theVal.myDouble);
}

//********************************************************************
void FeaturesPlugin_WidgetFilletMultiRadiuses::
                              getHighlighted(QList<ModuleBase_ViewerPrsPtr>& theValues)
{
  std::set<int> anAttributeIds;
  getSelectedAttributeIndices(anAttributeIds);
  if (!anAttributeIds.empty())
    convertIndicesToViewerSelection(anAttributeIds, theValues);
}

//********************************************************************
void FeaturesPlugin_WidgetFilletMultiRadiuses::
                            getSelectedAttributeIndices(std::set<int>& theAttributeIds)
{
  QModelIndexList indexList = myDataTbl->selectionModel()->selectedIndexes();
  int aRow;
  foreach (QModelIndex index, indexList) {
    aRow = index.row();
    theAttributeIds.insert(aRow-1);
  }
}

void FeaturesPlugin_WidgetFilletMultiRadiuses::
            convertIndicesToViewerSelection(std::set<int> theAttributeIds,
                                            QList<ModuleBase_ViewerPrsPtr>& theValues) const
{
  if(myFeature.get() == NULL)
    return;

  DataPtr aData = myFeature->data();
  AttributePtr anAttribute =
                    aData->attribute(FeaturesPlugin_Fillet::ARRAY_POINT_RADIUS_BY_POINTS());

  AttributeSelectionListPtr aSelectionListAttr =
                    aData->selectionList(FeaturesPlugin_Fillet::ARRAY_POINT_RADIUS_BY_POINTS());
  for (int i = 0; i < aSelectionListAttr->size(); i++) {
    // filter by attribute indices only if the container is not empty otherwise return all items
    if (!theAttributeIds.empty() && theAttributeIds.find(i) == theAttributeIds.end())
      continue;
    AttributeSelectionPtr anAttr = aSelectionListAttr->value(i);
    ObjectPtr anObject = anAttr->contextObject();
    if (anObject.get())
      theValues.append(std::shared_ptr<ModuleBase_ViewerPrs>(
              new ModuleBase_ViewerPrs(anObject, anAttr->value(), NULL)));
  }
}

//********************************************************************
void FeaturesPlugin_WidgetFilletMultiRadiuses::onListSelection()
{
  myWorkshop->module()->customizeFeature(myFeature,ModuleBase_IModule::CustomizeHighlightedObjects,
                                         true);
}

//********************************************************************
void FeaturesPlugin_WidgetFilletMultiRadiuses::activateCustom()
{
  ModuleBase_WidgetSelector::activateCustom();

  ModuleBase_IModule* aModule = myWorkshop->module();
  aModule->activateCustomPrs(myFeature,
                            ModuleBase_IModule::CustomizeHighlightedObjects, true);
}
