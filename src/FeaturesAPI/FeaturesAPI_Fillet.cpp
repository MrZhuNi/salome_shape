// Copyright (C) 2017-2020  CEA/DEN, EDF R&D
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

#include "FeaturesAPI_Fillet.h"

#include <ModelHighAPI_Double.h>
#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>
#include <GeomAlgoAPI_PointBuilder.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Curve.h>

static GeomAPI_Shape::ShapeType typeOfSelection(
    const std::list<ModelHighAPI_Selection>& theBaseObjects)
{
  std::string aType = theBaseObjects.empty() ? "SHAPE" : theBaseObjects.front().shapeType();
  return GeomAPI_Shape::shapeTypeByStr(aType);
}

//==================================================================================================

FeaturesAPI_Fillet::FeaturesAPI_Fillet(const std::shared_ptr<ModelAPI_Feature>& theFeature)
  : ModelHighAPI_Interface(theFeature)
{
}

//==================================================================================================

FeaturesAPI_Fillet1D::FeaturesAPI_Fillet1D(const std::shared_ptr<ModelAPI_Feature>& theFeature)
  : FeaturesAPI_Fillet(theFeature)
{
  initialize();
}

FeaturesAPI_Fillet1D::FeaturesAPI_Fillet1D(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                           const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                           const ModelHighAPI_Double& theRadius)
  : FeaturesAPI_Fillet(theFeature)
{
  if (initialize()) {
    setBase(theBaseObjects);
    fillAttribute(theRadius, myradius);

    execIfBaseNotEmpty();
  }
}

FeaturesAPI_Fillet1D::~FeaturesAPI_Fillet1D()
{
}

void FeaturesAPI_Fillet1D::setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects)
{
  mybaseWires->clear();
  mybaseVertices->clear();

  if (typeOfSelection(theBaseObjects) == GeomAPI_Shape::WIRE) {
    fillAttribute(FeaturesPlugin_Fillet1D::CREATION_BY_WIRES(), mycreationMethod);
    fillAttribute(theBaseObjects, mybaseWires);
  }
  else {
    fillAttribute(FeaturesPlugin_Fillet1D::CREATION_BY_VERTICES(), mycreationMethod);
    fillAttribute(theBaseObjects, mybaseVertices);
  }

  execIfBaseNotEmpty();
}

void FeaturesAPI_Fillet1D::setRadius(const ModelHighAPI_Double& theRadius)
{
  fillAttribute(theRadius, myradius);
  execIfBaseNotEmpty();
}

void FeaturesAPI_Fillet1D::execIfBaseNotEmpty()
{
  if (mybaseWires->size() > 0 || mybaseVertices->size() > 0)
    execute();
}

void FeaturesAPI_Fillet1D::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeSelectionListPtr anAttrObjects;
  if (creationMethod()->value() == FeaturesPlugin_Fillet1D::CREATION_BY_WIRES())
    anAttrObjects = aBase->selectionList(FeaturesPlugin_Fillet1D::WIRE_LIST_ID());
  else if (creationMethod()->value() == FeaturesPlugin_Fillet1D::CREATION_BY_VERTICES())
    anAttrObjects = aBase->selectionList(FeaturesPlugin_Fillet1D::VERTEX_LIST_ID());

  AttributeDoublePtr anAttrRadius = aBase->real(FeaturesPlugin_Fillet1D::RADIUS_ID());

  theDumper << aBase << " = model.addFillet(" << aDocName << ", " << anAttrObjects
                                                          << ", " << anAttrRadius;

  if (!aBase->data()->version().empty())
    theDumper << ", keepSubResults = True";

  theDumper << ")" << std::endl;
}

//==================================================================================================

FeaturesAPI_Fillet2D::FeaturesAPI_Fillet2D(const std::shared_ptr<ModelAPI_Feature>& theFeature)
  : FeaturesAPI_Fillet(theFeature)
{
  initialize();
}

FeaturesAPI_Fillet2D::FeaturesAPI_Fillet2D(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                           const std::list<ModelHighAPI_Selection>& theBaseObjects,
                                           const ModelHighAPI_Double& theRadius)
  : FeaturesAPI_Fillet(theFeature)
{
  if (initialize()) {
    fillAttribute(FeaturesPlugin_Fillet::CREATION_METHOD_SINGLE_RADIUS(), mycreationMethod);
    fillAttribute(theBaseObjects, mybaseObjects);
    fillAttribute(theRadius, myradius);

    execIfBaseNotEmpty();
  }
}

FeaturesAPI_Fillet2D::FeaturesAPI_Fillet2D(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                           const std::list<ModelHighAPI_Selection>& theEdgesFaces,
                                           const ModelHighAPI_Double& theRadius1,
                                           const ModelHighAPI_Double& theRadius2)
  : FeaturesAPI_Fillet(theFeature)
{
  if (initialize()) {
    fillAttribute(FeaturesPlugin_Fillet::CREATION_METHOD_VARYING_RADIUS(), mycreationMethod);
    fillAttribute(theEdgesFaces, myedgesfacesselected);
    fillAttribute(theRadius1, mystartRadius);
    fillAttribute(theRadius2, myendRadius);

    execIfBaseNotEmpty();
  }
}


FeaturesAPI_Fillet2D::FeaturesAPI_Fillet2D(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                           const ModelHighAPI_Selection & theedgeselected,
                                           const std::list<ModelHighAPI_Selection>& thepoint,
                                           const std::list<ModelHighAPI_Double>& theRadius)
  : FeaturesAPI_Fillet(theFeature)
{
  if (initialize()) {
    fillAttribute(FeaturesPlugin_Fillet::CREATION_METHOD_MULTIPLES_RADIUSES(), mycreationMethod);
    fillAttribute(FeaturesPlugin_Fillet::CREATION_METHOD_BY_POINTS(), mycreationMethodmulti);
    fillAttribute(theedgeselected, edgeselected());
    fillAttribute(thepoint, myarraypointradiusbypoint);

    GeomEdgePtr anEdge = GeomEdgePtr(new GeomAPI_Edge( edgeselected()->value()));
    GeomPointPtr first =  anEdge->firstPoint();
    GeomPointPtr last  =  anEdge->lastPoint();
    double taille = first->distance(last);

    std::shared_ptr<GeomAPI_Curve> aCurve(new GeomAPI_Curve(edgeselected()->value()));

    ListOfShape aPoints;
    std::set<GeomShapePtr> aContexts;
    for (int anIndex = 0; anIndex < myarraypointradiusbypoint->size(); ++anIndex) {
        AttributeSelectionPtr aSelection = myarraypointradiusbypoint->value(anIndex);
        GeomShapePtr aShape = aSelection->value();
        ResultPtr aContext = aSelection->context();
        aContexts.insert(aContext->shape());

        if (!aShape.get()) {
          aShape = aContext->shape();
        }
        aPoints.push_back(aShape);
    }
    myvalues()->setSize( aPoints.size() +2, 2 );
    std::list<ModelHighAPI_Double>::const_iterator aRowsRadiusIter = theRadius.begin();
    ModelAPI_AttributeTables::Value aVal;
    aVal.myDouble = 0.0;
    myvalues()->setValue(aVal, 0, 0 );
    aVal.myDouble = aRowsRadiusIter->value();
    myvalues()->setValue(aVal, 0, 1 );
    aRowsRadiusIter++;
    int aRowIndex = 1;
    ListOfShape::const_iterator aPointsIt = aPoints.begin();
    for (; aPointsIt != aPoints.end(); ++aPointsIt, aRowsRadiusIter++) {
      std::shared_ptr<GeomAPI_Pnt> aPnt = GeomAlgoAPI_PointBuilder::point(*aPointsIt);
      std::shared_ptr<GeomAPI_Pnt> aPntCurv = aCurve->project(aPnt);
      double res = (aPntCurv->distance(first) / taille);
       aVal.myDouble = res;
      myvalues()->setValue(aVal, aRowIndex, 0 );
      aVal.myDouble = aRowsRadiusIter->value();
      myvalues()->setValue(aVal, aRowIndex, 1 );
      aRowIndex++;
    }
    aVal.myDouble = 1.0;
    myvalues()->setValue(aVal, aRowIndex, 0 );
    aVal.myDouble = aRowsRadiusIter->value();
    myvalues()->setValue(aVal, aRowIndex, 1 );
    execIfBaseNotEmpty();
  }
}

FeaturesAPI_Fillet2D::FeaturesAPI_Fillet2D(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                           const std::list<ModelHighAPI_Selection>& theEdgesFaces,
                                           const std::list<ModelHighAPI_Double>& thepointCurvCood,
                                           const std::list<ModelHighAPI_Double>& theRadius)
  : FeaturesAPI_Fillet(theFeature)
{
  if (initialize()) {
    fillAttribute(FeaturesPlugin_Fillet::CREATION_METHOD_MULTIPLES_RADIUSES(), mycreationMethod);
    fillAttribute(FeaturesPlugin_Fillet::CREATION_METHOD_BY_CURVILEAR_ABSCISSA(), mycreationMethodmulti);
    fillAttribute(theEdgesFaces, myedgesfacesselected);

    int aRowIndex = 0;
    myvaluescurv()->setSize( thepointCurvCood.size(), 2 );
    std::list<ModelHighAPI_Double>::const_iterator aRowsCoodIter = thepointCurvCood.begin();
    std::list<ModelHighAPI_Double>::const_iterator aRowsRadiusIter = theRadius.begin();
    for(; aRowsCoodIter != thepointCurvCood.end();
          aRowsCoodIter++, aRowsRadiusIter++, aRowIndex++) {
      ModelAPI_AttributeTables::Value aVal;
      aVal.myDouble = aRowsCoodIter->value();
      myvaluescurv()->setValue(aVal, aRowIndex, 0 );
      aVal.myDouble = aRowsRadiusIter->value();
      myvaluescurv()->setValue(aVal, aRowIndex, 1 );
    }
    execIfBaseNotEmpty();
  }
}

FeaturesAPI_Fillet2D::~FeaturesAPI_Fillet2D()
{
}

void FeaturesAPI_Fillet2D::setBase(const std::list<ModelHighAPI_Selection>& theBaseObjects)
{
  mybaseObjects->clear();
  fillAttribute(theBaseObjects, mybaseObjects);

  execIfBaseNotEmpty();
}

void FeaturesAPI_Fillet2D::setRadius(const ModelHighAPI_Double& theRadius)
{
  fillAttribute(FeaturesPlugin_Fillet::CREATION_METHOD_SINGLE_RADIUS(), mycreationMethod);
  fillAttribute(theRadius, myradius);

  execIfBaseNotEmpty();
}

void FeaturesAPI_Fillet2D::setRadius(const ModelHighAPI_Double& theRadius1,
                                     const ModelHighAPI_Double& theRadius2)
{
  fillAttribute(FeaturesPlugin_Fillet::CREATION_METHOD_VARYING_RADIUS(), mycreationMethod);
  fillAttribute(theRadius1, mystartRadius);
  fillAttribute(theRadius2, myendRadius);

  execIfBaseNotEmpty();
}

void FeaturesAPI_Fillet2D::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeSelectionListPtr anAttrObjects =
    aBase->selectionList(FeaturesPlugin_Fillet::OBJECT_LIST_ID());


  if ( aBase->string(FeaturesPlugin_Fillet::CREATION_METHOD())->value()
        == FeaturesPlugin_Fillet::CREATION_METHOD_MULTIPLES_RADIUSES() )
  {
    AttributeSelectionPtr anAttrEdgeSelec =
    aBase->selection(FeaturesPlugin_Fillet::EDGE_SELECTED_ID());

    if( aBase->string(FeaturesPlugin_Fillet::CREATION_METHOD_MULTIPLES_RADIUSES())->value()
          == FeaturesPlugin_Fillet::CREATION_METHOD_BY_POINTS() )
    {

      AttributeSelectionListPtr anAttrPoint =
          aBase->selectionList(FeaturesPlugin_Fillet::ARRAY_POINT_RADIUS_BY_POINTS());
      AttributeTablesPtr anAttrTable =
          aBase->tables(FeaturesPlugin_Fillet::VALUES_ID());
      theDumper << aBase << " = model.addFilletMultiRadiusByPoints(" << aDocName << ", " << anAttrEdgeSelec;
      theDumper << ", " << anAttrPoint ;
      theDumper<<", [";
      for(int aRow = 0; aRow < myvalues()->rows(); aRow++) {
        if (aRow != 0){
          theDumper<<", ";
        }
        theDumper<<myvalues()->value(aRow, 1).myDouble;
      }
      theDumper<<"]";

    }else{
      AttributeSelectionListPtr anAttrEdgesFaces =
                      aBase->selectionList(FeaturesPlugin_Fillet::EDGES_FACES_LIST_ID());
      AttributeTablesPtr anAttrTable = aBase->tables(FeaturesPlugin_Fillet::VALUES_CURV_ID());
      theDumper << aBase << " = model.addFilletMultiRadiusBycurvAbs(" << aDocName << ", " << anAttrEdgesFaces;
      theDumper << ", ";
      theDumper<<"[";
      for(int aRow = 0; aRow < myvaluescurv()->rows(); aRow++) {
        if (aRow != 0)
          theDumper<<", ";
        theDumper<<myvaluescurv()->value(aRow, 0).myDouble;
      }
      theDumper<<"],";
      theDumper<<"[";
      for(int aRow = 0; aRow < myvaluescurv()->rows(); aRow++) {
        if (aRow != 0)
          theDumper<<", ";
        theDumper<<myvaluescurv()->value(aRow, 1).myDouble;
      }
      theDumper<<"]";
    }
  }else
  { 

    std::string aCreationMethod = aBase->string(FeaturesPlugin_Fillet::CREATION_METHOD())->value();

    if(aCreationMethod == FeaturesPlugin_Fillet::CREATION_METHOD_SINGLE_RADIUS()) {
       theDumper << aBase << " = model.addFillet(" << aDocName << ", " << anAttrObjects;
      AttributeDoublePtr anAttrRadius = aBase->real(FeaturesPlugin_Fillet::RADIUS_ID());
      theDumper << ", " << anAttrRadius;
    } else if(aCreationMethod == FeaturesPlugin_Fillet::CREATION_METHOD_VARYING_RADIUS()) {
      AttributeSelectionListPtr anAttrEdgesFaces =
                      aBase->selectionList(FeaturesPlugin_Fillet::EDGES_FACES_LIST_ID());
      theDumper << aBase << " = model.addFillet(" << aDocName << ", " << anAttrEdgesFaces;
      AttributeDoublePtr anAttrRadius1 = aBase->real(FeaturesPlugin_Fillet::START_RADIUS_ID());
      AttributeDoublePtr anAttrRadius2 = aBase->real(FeaturesPlugin_Fillet::END_RADIUS_ID());
      theDumper << ", " << anAttrRadius1 << ", " << anAttrRadius2;
    }
  }

  if (!aBase->data()->version().empty())
    theDumper << ", keepSubResults = True";

  theDumper << ")" << std::endl;
}

void FeaturesAPI_Fillet2D::execIfBaseNotEmpty()
{
  if (mybaseObjects->size() > 0)
    execute();
}


//==================================================================================================

FilletPtr addFillet(const std::shared_ptr<ModelAPI_Document>& thePart,
                    const std::list<ModelHighAPI_Selection>& theBaseObjects,
                    const ModelHighAPI_Double& theRadius1,
                    const ModelHighAPI_Double& theRadius2,
                    const bool keepSubResults)
{
  GeomAPI_Shape::ShapeType aType = typeOfSelection(theBaseObjects);
  bool is1D = aType == GeomAPI_Shape::WIRE || aType == GeomAPI_Shape::VERTEX;

  FeaturePtr aFeature =
      thePart->addFeature(is1D ? FeaturesAPI_Fillet1D::ID() : FeaturesAPI_Fillet2D::ID());
  if (!keepSubResults)
    aFeature->data()->setVersion("");

  FilletPtr aFillet;
  if (is1D)
    aFillet.reset(new FeaturesAPI_Fillet1D(aFeature, theBaseObjects, theRadius1));
  else if (theRadius2.value() < 0.0)
    aFillet.reset(new FeaturesAPI_Fillet2D(aFeature, theBaseObjects, theRadius1));
  else
    aFillet.reset(new FeaturesAPI_Fillet2D(aFeature, theBaseObjects, theRadius1, theRadius2));
  return aFillet;
}

FilletPtr addFilletMultiRadiusByPoints(const std::shared_ptr<ModelAPI_Document>& thePart,
                    const ModelHighAPI_Selection & theedgeselected,
                    const std::list<ModelHighAPI_Selection>& thepoint,
                    const std::list<ModelHighAPI_Double>& theRadius,
                    const bool keepSubResults)
{

  FeaturePtr aFeature = thePart->addFeature(FeaturesAPI_Fillet2D::ID());
  if (!keepSubResults)
    aFeature->data()->setVersion("");

  FilletPtr aFillet;

  aFillet.reset(new FeaturesAPI_Fillet2D(aFeature, theedgeselected, thepoint, theRadius));

  return aFillet;
}

FilletPtr addFilletMultiRadiusBycurvAbs(const std::shared_ptr<ModelAPI_Document>& thePart,
                    const std::list<ModelHighAPI_Selection>& theBaseObjects,
                    const std::list<ModelHighAPI_Double>& thepointCurvCood,
                    const std::list<ModelHighAPI_Double>& theRadius,
                    const bool keepSubResults)
{

  FeaturePtr aFeature = thePart->addFeature(FeaturesAPI_Fillet2D::ID());
  if (!keepSubResults)
    aFeature->data()->setVersion("");

  FilletPtr aFillet;

  aFillet.reset(new FeaturesAPI_Fillet2D(aFeature, theBaseObjects, thepointCurvCood, theRadius));

  return aFillet;
}