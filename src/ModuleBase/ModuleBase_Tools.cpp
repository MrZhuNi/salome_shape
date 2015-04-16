// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_Tools.cpp
// Created:     11 July 2014
// Author:      Vitaly Smetannikov

#include "ModuleBase_Tools.h"

#include <ModelAPI_Result.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Attribute.h>
#include <ModelAPI_AttributeRefAttr.h>

#include <GeomDataAPI_Point2D.h>
#include <Events_Error.h>

#include <QWidget>
#include <QLayout>
#include <QPainter>
#include <QBitmap>
#include <QDoubleSpinBox>

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

void setSpinValue(QDoubleSpinBox* theSpin, double theValue)
{
  bool isBlocked = theSpin->blockSignals(true);
  theSpin->setValue(theValue);
  theSpin->blockSignals(isBlocked);
}

QString objectInfo(const ObjectPtr& theObj, const bool isUseAttributesInfo)
{
  ResultPtr aRes = std::dynamic_pointer_cast<ModelAPI_Result>(theObj);
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theObj);
  QString aFeatureStr = "feature";
  if(aRes.get()) {
    aFeatureStr.append("(Result)");
    aFeature = ModelAPI_Feature::feature(aRes);
  }
  if (aFeature.get()) {
    aFeatureStr.append(QString(": %1").arg(aFeature->getKind().c_str()).toStdString().c_str());
    if (aFeature->data().get() && aFeature->data()->isValid())
      aFeatureStr.append(QString(", name=%1").arg(aFeature->data()->name().c_str()).toStdString()
                                                                                       .c_str());
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
  }
  QString aType = theType.toLower();
  if (MyShapeTypes.contains(aType))
    return MyShapeTypes[aType];
  Events_Error::send("Shape type defined in XML is not implemented!");
  return TopAbs_SHAPE;
}


}


