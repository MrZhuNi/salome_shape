// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_Tools.cpp
// Created:     11 July 2014
// Author:      Vitaly Smetannikov

#include "ModuleBase_Tools.h"

#include <ModuleBase_ParamIntSpinBox.h>
#include <ModuleBase_ParamSpinBox.h>
#include <ModuleBase_WidgetFactory.h>
#include <ModuleBase_IWorkshop.h>
#include <ModuleBase_IModule.h>

#include <ModelAPI_Attribute.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeRefAttrList.h>

#include <ModelAPI_Data.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_ResultCompSolid.h>
#include <ModelAPI_ResultParameter.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_Session.h>

#include <TopoDS_Iterator.hxx>

#include <GeomDataAPI_Point2D.h>
#include <Events_Error.h>

#include <Config_PropManager.h>

#include <QWidget>
#include <QLayout>
#include <QPainter>
#include <QBitmap>
#include <QDoubleSpinBox>
#include <QGraphicsDropShadowEffect>
#include <QColor>

#include <sstream>
#include <string>

const double tolerance = 1e-7;

//#define DEBUG_ACTIVATE_WINDOW
//#define DEBUG_SET_FOCUS

namespace ModuleBase_Tools {

//******************************************************************

//******************************************************************

void adjustMargins(QWidget* theWidget)
{
  if(!theWidget)
    return;
  adjustMargins(theWidget->layout());
}

void adjustMargins(QLayout* theLayout)
{
  if(!theLayout)
    return;
  theLayout->setContentsMargins(2, 5, 2, 5);
  theLayout->setSpacing(4);
}

void zeroMargins(QWidget* theWidget)
{
  if(!theWidget)
    return;
  zeroMargins(theWidget->layout());
}

void zeroMargins(QLayout* theLayout)
{
  if(!theLayout)
    return;
  theLayout->setContentsMargins(0, 0, 0, 0);
  theLayout->setSpacing(5);
}

void activateWindow(QWidget* theWidget, const QString& theInfo)
{
  theWidget->activateWindow();

#ifdef DEBUG_ACTIVATE_WINDOW
  qDebug(QString("activateWindow: %1").arg(theInfo).toStdString().c_str());
#endif
}

void setFocus(QWidget* theWidget, const QString& theInfo)
{
  theWidget->setFocus();

#ifdef DEBUG_SET_FOCUS
  qDebug(QString("setFocus: %1").arg(theInfo).toStdString().c_str());
#endif
}

void setShadowEffect(QWidget* theWidget, const bool isSetEffect)
{
  if (isSetEffect) {
    QGraphicsDropShadowEffect* aGlowEffect = new QGraphicsDropShadowEffect();
    aGlowEffect->setOffset(.0);
    aGlowEffect->setBlurRadius(10.0);
    aGlowEffect->setColor(QColor(0, 170, 255)); // Light-blue color, #00AAFF
    theWidget->setGraphicsEffect(aGlowEffect);
  }
  else {
    QGraphicsEffect* anEffect = theWidget->graphicsEffect();
    if(anEffect)
    anEffect->deleteLater();
    theWidget->setGraphicsEffect(NULL);
  }
}

QPixmap composite(const QString& theAdditionalIcon, const QString& theIcon)
{
  QImage anIcon(theIcon);
  QImage anAditional(theAdditionalIcon);

  if (anIcon.isNull())
    return QPixmap();

  int anAddWidth = anAditional.width();
  int anAddHeight = anAditional.height();

  int aWidth = anIcon.width();
  int aHeight = anIcon.height();

  int aStartWidthPos = aWidth - anAddWidth - 1;
  int aStartHeightPos = aHeight - anAddHeight - 1;

  for (int i = 0; i < anAddWidth && i + aStartWidthPos < aWidth; i++)
  {
    for (int j = 0; j < anAddHeight && j + aStartHeightPos < aHeight; j++)
    {
      if (qAlpha(anAditional.pixel(i, j)) > 0)
        anIcon.setPixel(i + aStartWidthPos, j + aStartHeightPos, anAditional.pixel(i, j));
    }
  }
  return QPixmap::fromImage(anIcon);
}

QPixmap lighter(const QString& theIcon, const int theLighterValue)
{
  QImage anIcon(theIcon);
  if (anIcon.isNull())
    return QPixmap();

  QImage aResult(theIcon);
  for ( int i = 0; i < anIcon.width(); i++ )
  {
    for ( int j = 0; j < anIcon.height(); j++ )
    {
      QRgb anRgb = anIcon.pixel( i, j );
      QColor aPixelColor(qRed(anRgb), qGreen(anRgb), qBlue(anRgb),
                         qAlpha( aResult.pixel( i, j ) ));

      QColor aLighterColor = aPixelColor.lighter(theLighterValue);
      aResult.setPixel(i, j, qRgba( aLighterColor.red(), aLighterColor.green(),
                                    aLighterColor.blue(), aLighterColor.alpha() ) );
    }
  }
  return QPixmap::fromImage(aResult);
}

void setSpinText(ModuleBase_ParamSpinBox* theSpin, const QString& theText)
{
  if (theSpin->text() == theText) 
    return;
  // In order to avoid extra text setting because it will
  // reset cursor position in control
  bool isBlocked = theSpin->blockSignals(true);
  theSpin->setText(theText);
  theSpin->blockSignals(isBlocked);
}

void setSpinValue(QDoubleSpinBox* theSpin, double theValue)
{
  if (fabs(theSpin->value() - theValue) < tolerance)
    return;
  bool isBlocked = theSpin->blockSignals(true);
  theSpin->setValue(theValue);
  theSpin->blockSignals(isBlocked);
}

void setSpinValue(ModuleBase_ParamSpinBox* theSpin, double theValue)
{
  if (fabs(theSpin->value() - theValue) < tolerance)
    return;
  bool isBlocked = theSpin->blockSignals(true);
  theSpin->setValue(theValue);
  theSpin->blockSignals(isBlocked);
}

void setSpinText(ModuleBase_ParamIntSpinBox* theSpin, const QString& theText)
{
  // In order to avoid extra text setting because it will
  // reset cursor position in control
  if (theSpin->text() == theText)
    return;
  bool isBlocked = theSpin->blockSignals(true);
  theSpin->setText(theText);
  theSpin->blockSignals(isBlocked);
}

void setSpinValue(ModuleBase_ParamIntSpinBox* theSpin, int theValue)
{
  if (theSpin->value() == theValue)
    return;
  bool isBlocked = theSpin->blockSignals(true);
  theSpin->setValue(theValue);
  theSpin->blockSignals(isBlocked);
}

QString objectInfo(const ObjectPtr& theObj, const bool isUseAttributesInfo)
{
  QString aFeatureStr = "feature";
  if (!theObj.get())
    return aFeatureStr;

  std::ostringstream aPtrStr;
  aPtrStr << "[" << theObj.get() << "]";

  ResultPtr aRes = std::dynamic_pointer_cast<ModelAPI_Result>(theObj);
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theObj);
  if(aRes.get()) {
    aFeatureStr.append(QString("(result%1)").arg(aPtrStr.str().c_str()).toStdString() .c_str());
    if (aRes->isDisabled())
      aFeatureStr.append("[disabled]");
    if (aRes->isConcealed())
      aFeatureStr.append("[concealed]");
    if (ModelAPI_Tools::hasSubResults(aRes))
      aFeatureStr.append("[hasSubResults]");

    aFeature = ModelAPI_Feature::feature(aRes);
  }
  else
    aFeatureStr.append(aPtrStr.str().c_str());

  if (aFeature.get()) {
    aFeatureStr.append(QString(": %1").arg(aFeature->getKind().c_str()).toStdString().c_str());
    if (aFeature->data()->isValid()) {
      aFeatureStr.append(QString(", name=%1").arg(aFeature->data()->name().c_str()).toStdString()
                                                                                       .c_str());
    }
    if (isUseAttributesInfo) {
      std::list<AttributePtr> anAttrs = aFeature->data()->attributes("");
      std::list<AttributePtr>::const_iterator anIt = anAttrs.begin(), aLast = anAttrs.end();
      QStringList aValues;
      for(; anIt != aLast; anIt++) {
        AttributePtr anAttr = *anIt;
        QString aValue = "not defined";
        std::string aType = anAttr->attributeType();
        if (aType == GeomDataAPI_Point2D::typeId()) {
          std::shared_ptr<GeomDataAPI_Point2D> aPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                                                                                         anAttr);
          if (aPoint.get())
            aValue = QString("(%1, %2)").arg(aPoint->x()).arg(aPoint->y());
        }
        else if (aType == ModelAPI_AttributeRefAttr::typeId()) {
        }

        aValues.push_back(QString("%1: %2").arg(anAttr->id().c_str()).arg(aValue).toStdString().c_str());
      }
      if (!aValues.empty())
        aFeatureStr.append(QString(", attributes: %1").arg(aValues.join(", ").toStdString().c_str()));
    }
  }

  return aFeatureStr;
}

typedef QMap<QString, TopAbs_ShapeEnum> ShapeTypes;
static ShapeTypes MyShapeTypes;

TopAbs_ShapeEnum shapeType(const QString& theType)
{
  if (MyShapeTypes.count() == 0) {
    MyShapeTypes["face"] = TopAbs_FACE;
    MyShapeTypes["faces"] = TopAbs_FACE;
    MyShapeTypes["vertex"] = TopAbs_VERTEX;
    MyShapeTypes["vertices"] = TopAbs_VERTEX;
    MyShapeTypes["wire"] = TopAbs_WIRE;
    MyShapeTypes["edge"] = TopAbs_EDGE;
    MyShapeTypes["edges"] = TopAbs_EDGE;
    MyShapeTypes["shell"] = TopAbs_SHELL;
    MyShapeTypes["solid"] = TopAbs_SOLID;
    MyShapeTypes["solids"] = TopAbs_SOLID;
    MyShapeTypes["objects"] = TopAbs_SHAPE;
  }
  QString aType = theType.toLower();
  if (MyShapeTypes.contains(aType))
    return MyShapeTypes[aType];
  Events_Error::send("Shape type defined in XML is not implemented!");
  return TopAbs_SHAPE;
}

void checkObjects(const QObjectPtrList& theObjects, bool& hasResult, bool& hasFeature, bool& hasParameter, bool& hasSubFeature)
{
  hasResult = false;
  hasFeature = false;
  hasParameter = false;
  hasSubFeature = false;
  foreach(ObjectPtr aObj, theObjects) {
    FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(aObj);
    ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(aObj);
    ResultParameterPtr aConstruction = std::dynamic_pointer_cast<ModelAPI_ResultParameter>(aResult);

    hasResult |= (aResult.get() != NULL);
    hasFeature |= (aFeature.get() != NULL);
    hasParameter |= (aConstruction.get() != NULL);
    if (hasFeature) 
      hasSubFeature |= (ModelAPI_Tools::compositeOwner(aFeature) != NULL);
    if (hasFeature && hasResult  && hasParameter && hasSubFeature)
      break;
  }
}

void setDefaultDeviationCoefficient(const TopoDS_Shape& theShape,
                                    const Handle(Prs3d_Drawer)& theDrawer)
{
  if (theShape.IsNull())
    return;
  TopAbs_ShapeEnum aType = theShape.ShapeType();
  if ((aType == TopAbs_EDGE) || (aType == TopAbs_WIRE)) 
    theDrawer->SetDeviationCoefficient(1.e-4);
}

Quantity_Color color(const std::string& theSection,
                     const std::string& theName,
                     const std::string& theDefault)
{
  std::vector<int> aColor = Config_PropManager::color(theSection, theName, theDefault);
  return Quantity_Color(aColor[0] / 255., aColor[1] / 255., aColor[2] / 255., Quantity_TOC_RGB);
}

ObjectPtr getObject(const AttributePtr& theAttribute)
{
  ObjectPtr anObject;
  std::string anAttrType = theAttribute->attributeType();
  if (anAttrType == ModelAPI_AttributeRefAttr::typeId()) {
    AttributeRefAttrPtr anAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
    if (anAttr != NULL && anAttr->isObject())
      anObject = anAttr->object();
  }
  if (anAttrType == ModelAPI_AttributeSelection::typeId()) {
    AttributeSelectionPtr anAttr = std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
    if (anAttr != NULL)
      anObject = anAttr->context();
  }
  if (anAttrType == ModelAPI_AttributeReference::typeId()) {
    AttributeReferencePtr anAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(theAttribute);
    if (anAttr.get() != NULL)
      anObject = anAttr->value();
  }
  return anObject;
}

TopAbs_ShapeEnum getCompoundSubType(const TopoDS_Shape& theShape)
{
  TopAbs_ShapeEnum aShapeType = theShape.ShapeType();

  // for compounds check sub-shapes: it may be compound of needed type:
  // Booleans may produce compounds of Solids
  if (aShapeType == TopAbs_COMPOUND) {
    for(TopoDS_Iterator aSubs(theShape); aSubs.More(); aSubs.Next()) {
      if (!aSubs.Value().IsNull()) {
        TopAbs_ShapeEnum aSubType = aSubs.Value().ShapeType();
        if (aSubType == TopAbs_COMPOUND) { // compound of compound(s)
          aShapeType = TopAbs_COMPOUND;
          break;
        }
        if (aShapeType == TopAbs_COMPOUND) {
          aShapeType = aSubType;
        } else if (aShapeType != aSubType) { // compound of shapes of different types
          aShapeType = TopAbs_COMPOUND;
          break;
        }
      }
    }
  }
  return aShapeType;
}

void getParameters(QStringList& theParameters)
{
  theParameters.clear();

  SessionPtr aSession = ModelAPI_Session::get();
  std::list<DocumentPtr> aDocList;
  DocumentPtr anActiveDocument = aSession->activeDocument();
  DocumentPtr aRootDocument = aSession->moduleDocument();
  aDocList.push_back(anActiveDocument);
  if (anActiveDocument != aRootDocument) {
    aDocList.push_back(aRootDocument);
  }
  std::string aGroupId = ModelAPI_ResultParameter::group();
  for(std::list<DocumentPtr>::const_iterator it = aDocList.begin(); it != aDocList.end(); ++it) {
    DocumentPtr aDocument = *it;
    int aSize = aDocument->size(aGroupId);
    for (int i = 0; i < aSize; i++) {
      ObjectPtr anObject = aDocument->object(aGroupId, i);
      std::string aParameterName = anObject->data()->name();
      theParameters.append(aParameterName.c_str());
    }
  }
}

std::string findGreedAttribute(ModuleBase_IWorkshop* theWorkshop, const FeaturePtr& theFeature)
{
  std::string anAttributeId;

  std::string aXmlCfg, aDescription;
  theWorkshop->module()->getXMLRepresentation(theFeature->getKind(), aXmlCfg, aDescription);

  ModuleBase_WidgetFactory aFactory(aXmlCfg, theWorkshop);
  std::string anAttributeTitle;
  aFactory.getGreedAttribute(anAttributeId);

  return anAttributeId;
}

void setObject(const AttributePtr& theAttribute, const ObjectPtr& theObject,
               const GeomShapePtr& theShape, ModuleBase_IWorkshop* theWorkshop,
               const bool theTemporarily)
{
  if (!theAttribute.get())
    return;

  std::string aType = theAttribute->attributeType();
  if (aType == ModelAPI_AttributeReference::typeId()) {
    AttributeReferencePtr aRef = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(theAttribute);
    ObjectPtr aObject = aRef->value();
    if (!(aObject && aObject->isSame(theObject))) {
      aRef->setValue(theObject);
    }
  } else if (aType == ModelAPI_AttributeRefAttr::typeId()) {
    AttributeRefAttrPtr aRefAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);

    AttributePtr anAttribute = theWorkshop->module()->findAttribute(theObject, theShape);
    if (anAttribute.get())
      aRefAttr->setAttr(anAttribute);
    else {
      ObjectPtr aObject = aRefAttr->object();
      if (!(aObject && aObject->isSame(theObject))) {
        aRefAttr->setObject(theObject);
      }
    }
  } else if (aType == ModelAPI_AttributeSelection::typeId()) {
    AttributeSelectionPtr aSelectAttr =
                             std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
    ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theObject);
    if (aSelectAttr.get() != NULL) {
      aSelectAttr->setValue(aResult, theShape, theTemporarily);
    }
  }
  if (aType == ModelAPI_AttributeSelectionList::typeId()) {
    AttributeSelectionListPtr aSelectionListAttr =
                         std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
    ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theObject);
    if (!aSelectionListAttr->isInList(aResult, theShape, theTemporarily))
      aSelectionListAttr->append(aResult, theShape, theTemporarily);
  }
  else if (aType == ModelAPI_AttributeRefList::typeId()) {
    AttributeRefListPtr aRefListAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefList>(theAttribute);
    if (!aRefListAttr->isInList(theObject))
      aRefListAttr->append(theObject);
  }
  else if (aType == ModelAPI_AttributeRefAttrList::typeId()) {
    AttributeRefAttrListPtr aRefAttrListAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttrList>(theAttribute);
    AttributePtr anAttribute = theWorkshop->module()->findAttribute(theObject, theShape);

    if (anAttribute.get()) {
      if (!aRefAttrListAttr->isInList(anAttribute))
        aRefAttrListAttr->append(anAttribute);
    }
    else {
      if (!aRefAttrListAttr->isInList(theObject))
        aRefAttrListAttr->append(theObject);
    }
  }
}

} // namespace ModuleBase_Tools


