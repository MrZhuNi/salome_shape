// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include <Model_BodyBuilder.h>

#include <Model_Data.h>
#include <Model_Document.h>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Iterator.hxx>
#include <TNaming_Tool.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_Integer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_ChildIDIterator.hxx>
#include <TDF_Reference.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapIteratorOfDataMapOfShapeShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopExp.hxx>
#include <BRepTools.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools_History.hxx>
#include <GeomAPI_Shape.h>
#include <GeomAPI_ShapeExplorer.h>
#include <GeomAlgoAPI_MakeShape.h>
#include <GeomAlgoAPI_SortListOfShapes.h>
#include <Config_PropManager.h>
// DEB
//#include <TCollection_AsciiString.hxx>
//#include <TDF_Tool.hxx>
//#define DEB_IMPORT 1

static const int INVALID_TAG            = -1;
static const int GENERATED_VERTICES_TAG = 1;
static const int GENERATED_EDGES_TAG    = 2;
static const int GENERATED_FACES_TAG    = 3;
static const int MODIFIED_VERTICES_TAG  = 4;
static const int MODIFIED_EDGES_TAG     = 5;
static const int MODIFIED_FACES_TAG     = 6;
static const int DELETED_TAG            = 7;
static const int PRIMITIVES_START_TAG   = 11;

static int getGenerationTag(const TopoDS_Shape& theShape) {
  TopAbs_ShapeEnum aShapeType = theShape.ShapeType();
  switch (aShapeType) {
    case TopAbs_VERTEX: return GENERATED_VERTICES_TAG;
    case TopAbs_EDGE:   return GENERATED_EDGES_TAG;
    case TopAbs_FACE:   return GENERATED_FACES_TAG;
  }

  return INVALID_TAG;
}

static int getModificationTag(const TopoDS_Shape& theShape) {
  TopAbs_ShapeEnum aShapeType = theShape.ShapeType();
  switch (aShapeType) {
    case TopAbs_VERTEX: return MODIFIED_VERTICES_TAG;
    case TopAbs_EDGE:   return MODIFIED_EDGES_TAG;
    case TopAbs_FACE:   return MODIFIED_FACES_TAG;
  }

  return INVALID_TAG;
}

static TopAbs_ShapeEnum convertShapeType(const GeomAPI_Shape::ShapeType theType) {
  switch (theType) {
    case GeomAPI_Shape::VERTEX:    return TopAbs_VERTEX;
    case GeomAPI_Shape::EDGE:      return TopAbs_EDGE;
    case GeomAPI_Shape::WIRE:      return TopAbs_WIRE;
    case GeomAPI_Shape::FACE:      return TopAbs_FACE;
    case GeomAPI_Shape::SHELL:     return TopAbs_SHELL;
    case GeomAPI_Shape::SOLID:     return TopAbs_SOLID;
    case GeomAPI_Shape::COMPSOLID: return TopAbs_COMPSOLID;
    case GeomAPI_Shape::COMPOUND:  return TopAbs_COMPOUND;
    case GeomAPI_Shape::SHAPE:     return TopAbs_SHAPE;
  }
  return TopAbs_SHAPE;
}

static bool isAlreadyStored(const TNaming_Builder* theBuilder,
                            const TopoDS_Shape& theOldShape,
                            const TopoDS_Shape& theNewShape)
{
  for (TNaming_Iterator aNamingIt(theBuilder->NamedShape());
    aNamingIt.More();
    aNamingIt.Next())
  {
    if (aNamingIt.NewShape().IsSame(theNewShape)
      && aNamingIt.OldShape().IsSame(theOldShape))
    {
      return true;
    }
  }

  return false;
}

Model_BodyBuilder::Model_BodyBuilder(ModelAPI_Object* theOwner)
: ModelAPI_BodyBuilder(theOwner),
  myPrimitiveTag(PRIMITIVES_START_TAG),
  myDividedIndex(1),
  myVIndex(1),
  myEIndex(1),
  myFIndex(1)
{
}

// Converts evolution of naming shape to selection evelution and back to avoid
// naming support on the disabled results. Deeply in the labels tree, recursively.
static void evolutionToSelectionRec(TDF_Label theLab, const bool theFlag) {
  std::list<std::pair<TopoDS_Shape, TopoDS_Shape> > aShapePairs; // to store old and new shapes
  Handle(TNaming_NamedShape) aName;
  int anEvolution = -1;
  if (theLab.FindAttribute(TNaming_NamedShape::GetID(), aName)) {
    TNaming_Evolution aNSEvol = aName->Evolution();
    if ((aNSEvol == TNaming_SELECTED && theFlag) ||
        (aNSEvol != TNaming_SELECTED && !theFlag)) { // nothing to do, it is already correct
      return;
    }
    anEvolution = (int)(aNSEvol);
    if (!theFlag) {
      Handle(TDataStd_Integer) anAttrEvol;
      if (theLab.FindAttribute(TDataStd_Integer::GetID(), anAttrEvol)) {
        anEvolution = anAttrEvol->Get();
      }
    } else {
      TDataStd_Integer::Set(theLab, anEvolution);
    }

    for(TNaming_Iterator anIter(aName); anIter.More(); anIter.Next()) {
      // iterator goes in reversed order relatively to the Builder, to, make the list reversed
      aShapePairs.push_front(std::pair<TopoDS_Shape, TopoDS_Shape>
        (anIter.OldShape(), anIter.NewShape()));
    }

    // create new
    TNaming_Builder aBuilder(theLab);
    TNaming_Evolution anEvol = (TNaming_Evolution)(anEvolution);
    std::list<std::pair<TopoDS_Shape, TopoDS_Shape> >::iterator aPairsIter = aShapePairs.begin();
    for(; aPairsIter != aShapePairs.end(); aPairsIter++) {
      if (theFlag) { // disabled => make selection
        if (anEvolution == TNaming_DELETE) // issue 2274 : don't put too many same null shapes
          aBuilder.Select(aPairsIter->first, aPairsIter->first);
        else if (anEvolution == TNaming_PRIMITIVE)
          aBuilder.Select(aPairsIter->second, aPairsIter->second);
        else
          aBuilder.Select(aPairsIter->second, aPairsIter->first);
      } else if (anEvol == TNaming_GENERATED) {
        aBuilder.Generated(aPairsIter->first, aPairsIter->second);
      } else if (anEvol == TNaming_MODIFY) {
        aBuilder.Modify(aPairsIter->first, aPairsIter->second);
      } else if (anEvol == TNaming_DELETE) {
        aBuilder.Delete(aPairsIter->first);
      } else if (anEvol == TNaming_PRIMITIVE) {
        aBuilder.Generated(aPairsIter->second);
      } else if (anEvol == TNaming_SELECTED) {
        aBuilder.Select(aPairsIter->second, aPairsIter->first);
      }
    }
  }
  // recursive call for all sub-labels
  TDF_ChildIterator anIter(theLab, Standard_False);
  for(; anIter.More(); anIter.Next()) {
    evolutionToSelectionRec(anIter.Value(), theFlag);
  }
}

void Model_BodyBuilder::evolutionToSelection(const bool theFlag)
{
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(data());
  if (!aData || !aData->isValid()) // unknown case
    return;
  TDF_Label& aShapeLab = aData->shapeLab();
  evolutionToSelectionRec(aShapeLab, theFlag);
}

void Model_BodyBuilder::store(const GeomShapePtr& theShape,
                              const bool theIsStoreSameShapes)
{
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(data());
  if (aData) {
    TDF_Label& aShapeLab = aData->shapeLab();
    // clean builders
    clean();
    // store the new shape as primitive
    TNaming_Builder aBuilder(aShapeLab);
    if (!theShape)
      return;  // bad shape
    TopoDS_Shape aShape = theShape->impl<TopoDS_Shape>();
    if (aShape.IsNull())
      return;  // null shape inside

    if(!theIsStoreSameShapes) {
      Handle(TNaming_NamedShape) aNS = TNaming_Tool::NamedShape(aShape, aShapeLab);
      if(!aNS.IsNull() && !aNS->IsEmpty()) {
        // This shape is already in document, store reference instead of shape;
        const TDF_Label aFoundLabel = aNS->Label();
        TDF_Reference::Set(aShapeLab, aFoundLabel);
        aShapeLab.ForgetAttribute(TNaming_NamedShape::GetID());
        return;
      }
    }

    aBuilder.Generated(aShape);
    // register name
    aShapeLab.ForgetAttribute(TDF_Reference::GetID());
    if(!aBuilder.NamedShape()->IsEmpty()) {
      Handle(TDataStd_Name) anAttr;
      if(aBuilder.NamedShape()->Label().FindAttribute(TDataStd_Name::GetID(),anAttr)) {
        std::string aName (TCollection_AsciiString(anAttr->Get()).ToCString());
        if(!aName.empty()) {
          std::shared_ptr<Model_Document> aDoc =
            std::dynamic_pointer_cast<Model_Document>(document());
          aDoc->addNamingName(aBuilder.NamedShape()->Label(), aName);
        }
      }
    }
  }
}

void Model_BodyBuilder::storeGenerated(const GeomShapePtr& theFromShape,
  const GeomShapePtr& theToShape)
{
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(data());
  if (aData) {
    TDF_Label& aShapeLab = aData->shapeLab();
    // clean builders
    clean();
    // store the new shape as primitive
    TNaming_Builder aBuilder(aShapeLab);
    if (!theFromShape || !theToShape)
      return;  // bad shape
    TopoDS_Shape aShapeBasis = theFromShape->impl<TopoDS_Shape>();
    if (aShapeBasis.IsNull())
      return;  // null shape inside
    TopoDS_Shape aShapeNew = theToShape->impl<TopoDS_Shape>();
    if (aShapeNew.IsNull())
      return;  // null shape inside
    aBuilder.Generated(aShapeBasis, aShapeNew);
    // register name
    if(!aBuilder.NamedShape()->IsEmpty()) {
      Handle(TDataStd_Name) anAttr;
      if(aBuilder.NamedShape()->Label().FindAttribute(TDataStd_Name::GetID(),anAttr)) {
        std::string aName (TCollection_AsciiString(anAttr->Get()).ToCString());
        if(!aName.empty()) {
          std::shared_ptr<Model_Document> aDoc =
            std::dynamic_pointer_cast<Model_Document>(document());
          aDoc->addNamingName(aBuilder.NamedShape()->Label(), aName);
        }
      }
    }
  }
}

TNaming_Builder* Model_BodyBuilder::builder(const int theTag)
{
  std::map<int, TNaming_Builder*>::iterator aFind = myBuilders.find(theTag);
  if (aFind == myBuilders.end()) {
    std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(data());
    myBuilders[theTag] = new TNaming_Builder(
      theTag == 0 ? aData->shapeLab() : aData->shapeLab().FindChild(theTag));
    aFind = myBuilders.find(theTag);
  }
  return aFind->second;
}

void Model_BodyBuilder::storeModified(const GeomShapePtr& theOldShape,
                                      const GeomShapePtr& theNewShape,
                                      const bool theIsCleanStored)
{
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(data());
  if (aData) {
    TDF_Label& aShapeLab = aData->shapeLab();
    // clean builders
    if (theIsCleanStored) clean();
    // store the new shape as primitive
    TNaming_Builder* aBuilder = builder(0);
    if (!theOldShape || !theNewShape)
      return;  // bad shape
    TopoDS_Shape aShapeOld = theOldShape->impl<TopoDS_Shape>();
    if (aShapeOld.IsNull())
      return;  // null shape inside
    TopoDS_Shape aShapeNew = theNewShape->impl<TopoDS_Shape>();
    if (aShapeNew.IsNull())
      return;  // null shape inside
    aBuilder->Modify(aShapeOld, aShapeNew);
    if(!aBuilder->NamedShape()->IsEmpty()) {
      Handle(TDataStd_Name) anAttr;
      if(aBuilder->NamedShape()->Label().FindAttribute(TDataStd_Name::GetID(),anAttr)) {
        std::string aName (TCollection_AsciiString(anAttr->Get()).ToCString());
        if(!aName.empty()) {
          std::shared_ptr<Model_Document> aDoc =
            std::dynamic_pointer_cast<Model_Document>(document());
          aDoc->addNamingName(aBuilder->NamedShape()->Label(), aName);
        }
      }
    }
  }
}

void  Model_BodyBuilder::storeWithoutNaming(const GeomShapePtr& theShape)
{
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(data());
  if (aData) {
    clean();
    if (!theShape.get())
      return; // bad shape
    TopoDS_Shape aShape = theShape->impl<TopoDS_Shape>();
    if (aShape.IsNull())
      return;  // null shape inside
    TNaming_Builder aBuilder(aData->shapeLab());
    aBuilder.Select(aShape, aShape);
  }
}

void Model_BodyBuilder::clean()
{
  TDF_Label aLab = std::dynamic_pointer_cast<Model_Data>(data())->shapeLab();
  if (aLab.IsNull())
    return;
  std::map<int, TNaming_Builder*>::iterator aBuilder = myBuilders.begin();
  for(; aBuilder != myBuilders.end(); aBuilder++) {
    delete aBuilder->second;
    // clear also shapes on cleaned sub-labels (#2241)
    Handle(TNaming_NamedShape) aNS;
    if (aLab.FindChild(aBuilder->first).FindAttribute(TNaming_NamedShape::GetID(), aNS)) {
      aNS->Clear();
    }
  }
  myBuilders.clear();
  // remove the old reference (if any)
  aLab.ForgetAttribute(TDF_Reference::GetID());
  myPrimitiveTag = PRIMITIVES_START_TAG;
  myDividedIndex = 1;
  myVIndex = 1;
  myEIndex = 1;
  myFIndex = 1;
}

Model_BodyBuilder::~Model_BodyBuilder()
{
  clean();
}

void Model_BodyBuilder::buildName(const int theTag, const std::string& theName)
{
  std::shared_ptr<Model_Document> aDoc = std::dynamic_pointer_cast<Model_Document>(document());
  std::string aName = theName;
  switch (theTag) {
    case GENERATED_VERTICES_TAG: aName += aName.empty() ? "Generated_Vertex" : "_GV"; break;
    case GENERATED_EDGES_TAG:    aName += aName.empty() ? "Generated_Edge"   : "_GE"; break;
    case GENERATED_FACES_TAG:    aName += aName.empty() ? "Generated_Face"   : "_GF"; break;
    case MODIFIED_VERTICES_TAG:  aName += aName.empty() ? "Modified_Vertex"  : "_MV"; break;
    case MODIFIED_EDGES_TAG:     aName += aName.empty() ? "Modified_Edge"    : "_ME"; break;
    case MODIFIED_FACES_TAG:     aName += aName.empty() ? "Modified_Face"    : "_MF"; break;
  }

  TDataStd_Name::Set(builder(theTag)->NamedShape()->Label(), aName.c_str());
}
void Model_BodyBuilder::generated(const GeomShapePtr& theNewShape,
                                  const std::string& theName)
{
  TopoDS_Shape aShape = theNewShape->impl<TopoDS_Shape>();
  builder(myPrimitiveTag)->Generated(aShape);
  if (!theName.empty()) {
    buildName(myPrimitiveTag, theName);
  }

  ++myPrimitiveTag;
}

void Model_BodyBuilder::generated(const GeomShapePtr& theOldShape,
                                  const GeomShapePtr& theNewShape,
                                  const std::string& theName)
{
  TopoDS_Shape anOldShape = theOldShape->impl<TopoDS_Shape>();
  TopoDS_Shape aNewShape = theNewShape->impl<TopoDS_Shape>();
  TopAbs_ShapeEnum aNewShapeType = aNewShape.ShapeType();
  int aTag;
  if (aNewShapeType == TopAbs_WIRE || aNewShapeType == TopAbs_SHELL) {
    // TODO: This is a workaround. New shape should be only vertex, edge or face.
    TopAbs_ShapeEnum anExplodeShapeType = aNewShapeType == TopAbs_WIRE ? TopAbs_EDGE : TopAbs_FACE;
    aTag = TopAbs_WIRE ? GENERATED_EDGES_TAG : GENERATED_FACES_TAG;
    for (TopExp_Explorer anExp(aNewShape, anExplodeShapeType); anExp.More(); anExp.Next()) {
      builder(aTag)->Generated(anOldShape, anExp.Current());
      buildName(aTag, theName);
    }
  } else {
    aTag = getGenerationTag(aNewShape);
    if (aTag == INVALID_TAG) return;
    builder(aTag)->Generated(anOldShape, aNewShape);
    buildName(aTag, theName);
  }
}

void Model_BodyBuilder::modified(const GeomShapePtr& theOldShape,
                                 const GeomShapePtr& theNewShape,
                                 const std::string& theName)
{
  TopoDS_Shape anOldShape = theOldShape->impl<TopoDS_Shape>();
  TopoDS_Shape aNewShape = theNewShape->impl<TopoDS_Shape>();
  int aTag = getModificationTag(aNewShape);
  if (aTag == INVALID_TAG) return;
  builder(aTag)->Modify(anOldShape, aNewShape);
  buildName(aTag, theName);
}

void Model_BodyBuilder::deleted(const GeomShapePtr& theOldShape)
{
  TopoDS_Shape aShape = theOldShape->impl<TopoDS_Shape>();
  builder(DELETED_TAG)->Delete(aShape);
}

void Model_BodyBuilder::loadDeletedShapes(const GeomMakeShapePtr& theAlgo,
                                          const GeomShapePtr& theOldShape,
                                          const GeomAPI_Shape::ShapeType theShapeTypeToExplore,
                                          const GeomShapePtr& theShapesToExclude)
{
  TopTools_MapOfShape anAlreadyProcessedShapes;
  GeomShapePtr aResultShape = shape();
  for (GeomAPI_ShapeExplorer anExp(theOldShape, theShapeTypeToExplore);
       anExp.more();
       anExp.next())
  {
    GeomShapePtr anOldSubShape = anExp.current();
    const TopoDS_Shape& anOldSubShape_ = anOldSubShape->impl<TopoDS_Shape>();
    if (!anAlreadyProcessedShapes.Add(anOldSubShape_)
        || !theAlgo->isDeleted(anOldSubShape)
        || aResultShape->isSubShape(anOldSubShape, false)
        || (theShapesToExclude.get() && theShapesToExclude->isSubShape(anOldSubShape, false)))
    {
      continue;
    }

    ListOfShape aNewShapes;
    if (BRepTools_History::IsSupportedType(anOldSubShape_)) { // to avoid crash in #2572
      theAlgo->modified(anOldSubShape, aNewShapes);
    }

    if (aNewShapes.size() == 0
        || (aNewShapes.size() == 1 && aNewShapes.front()->isSame(anOldSubShape))) {
      builder(DELETED_TAG)->Delete(anOldSubShape_);
    }
  }
}

static void removeBadShapes(ListOfShape& theShapes)
{
  ListOfShape::iterator anIt = theShapes.begin();
  while (anIt != theShapes.end()) {
    TopoDS_Shape aNewShape = (*anIt)->impl<TopoDS_Shape>();
    bool aSkip = aNewShape.IsNull()
      || (aNewShape.ShapeType() == TopAbs_EDGE && BRep_Tool::Degenerated(TopoDS::Edge(aNewShape)));
    if (aSkip) {
      ListOfShape::iterator aRemoveIt = anIt++;
      theShapes.erase(aRemoveIt);
    } else {
      ++anIt;
    }
  }
}

// Keep only the shapes with minimal shape type
static void keepTopLevelShapes(ListOfShape& theShapes, const TopoDS_Shape& theRoot,
  const GeomShapePtr& theResultShape = GeomShapePtr())
{
  GeomAPI_Shape::ShapeType aKeepShapeType = GeomAPI_Shape::SHAPE;
  ListOfShape::iterator anIt = theShapes.begin();
  while (anIt != theShapes.end()) {
    TopoDS_Shape aNewShape = (*anIt)->impl<TopoDS_Shape>();
    bool aSkip = aNewShape.IsNull() ||
      (aNewShape.ShapeType() == TopAbs_EDGE && BRep_Tool::Degenerated(TopoDS::Edge(aNewShape)));
    if (aSkip || theRoot.IsSame(aNewShape) || (theResultShape &&
        (!theResultShape->isSubShape(*anIt, false) || theResultShape->isSame(*anIt)))) {
      ListOfShape::iterator aRemoveIt = anIt++;
      theShapes.erase(aRemoveIt);
    } else {
      GeomAPI_Shape::ShapeType aType = (*anIt)->shapeType();
      if (aType < aKeepShapeType) {
        // found a shape with lesser shape type => remove all previous shapes
        aKeepShapeType = aType;
        theShapes.erase(theShapes.begin(), anIt);
        ++anIt;
      } else if (aType > aKeepShapeType) {
        // shapes with greater shape type should be removed from the list
        ListOfShape::iterator aRemoveIt = anIt++;
        theShapes.erase(aRemoveIt);
      } else
        ++anIt;
    }
  }
}

// returns an ancestor shape-type thaty used for naming-definition of the sub-type
TopAbs_ShapeEnum typeOfAncestor(const TopAbs_ShapeEnum theSubType) {
  if (theSubType == TopAbs_VERTEX)
    return TopAbs_EDGE;
  if (theSubType == TopAbs_EDGE)
    return TopAbs_FACE;
  return TopAbs_VERTEX; // bad case
}

void Model_BodyBuilder::loadModifiedShapes(const GeomMakeShapePtr& theAlgo,
                                           const GeomShapePtr& theOldShape,
                                           const GeomAPI_Shape::ShapeType theShapeTypeToExplore,
                                           const std::string& theName)
{
  GeomShapePtr aResultShape = shape();
  GeomShapePtr aShapeToExplore = theOldShape;
  if (theAlgo->isNewShapesCollected(theOldShape, theShapeTypeToExplore)) {
    // use optimized set of old shapes for this
    GeomShapePtr aCompound = theAlgo->oldShapesForNew(theOldShape,
                                                      aResultShape,
                                                      theShapeTypeToExplore);
    if (aCompound.get()) aShapeToExplore = aCompound;
  }

  TopTools_MapOfShape anAlreadyProcessedShapes;
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(data());
  for (GeomAPI_ShapeExplorer anOldShapeExp(aShapeToExplore, theShapeTypeToExplore);
       anOldShapeExp.more();
       anOldShapeExp.next())
  {
    GeomShapePtr anOldSubShape = anOldShapeExp.current();
    const TopoDS_Shape& anOldSubShape_ = anOldSubShape->impl<TopoDS_Shape>();

    // There is no sense to write history if shape already processed
    // or old shape does not exist in the document.
    bool anOldSubShapeAlreadyProcessed = !anAlreadyProcessedShapes.Add(anOldSubShape_);
    bool anOldSubShapeNotInTree = TNaming_Tool::NamedShape(anOldSubShape_, aData->shapeLab())
                                  .IsNull();
    if (anOldSubShapeAlreadyProcessed
        || anOldSubShapeNotInTree)
    {
      continue;
    }

    // Get new shapes.
    ListOfShape aNewShapes;
    theAlgo->modified(anOldSubShape, aNewShapes);

    for (ListOfShape::const_iterator aNewShapesIt = aNewShapes.cbegin();
         aNewShapesIt != aNewShapes.cend();
         ++aNewShapesIt)
    {
      GeomShapePtr aNewShape = *aNewShapesIt;
      const TopoDS_Shape& aNewShape_ = aNewShape->impl<TopoDS_Shape>();
      bool isGenerated = anOldSubShape_.ShapeType() != aNewShape_.ShapeType();

      bool aNewShapeIsSameAsOldShape = anOldSubShape->isSame(aNewShape);
      bool aNewShapeIsNotInResultShape = !aResultShape->isSubShape(aNewShape, false);
      if (aNewShapeIsSameAsOldShape
          || aNewShapeIsNotInResultShape)
      {
        continue;
      }

      TNaming_Builder* aBuilder;
      if (aResultShape->isSame(aNewShape)) {
        // keep the modification evolution on the root level (2241 - history propagation issue)
        aBuilder = builder(0);
        TDF_Label aShapeLab = aBuilder->NamedShape()->Label();
        Handle(TDF_Reference) aRef;
        if (aShapeLab.FindAttribute(TDF_Reference::GetID(), aRef)) {
          // Store only in case if it does not have reference.
          continue;
        }

        // Check if new shape was already stored.
        if (isAlreadyStored(aBuilder, anOldSubShape_, aNewShape_)) continue;

        if (!aBuilder->NamedShape().IsNull() &&
            ((isGenerated && aBuilder->NamedShape()->Evolution() != TNaming_GENERATED)
              || (!isGenerated && aBuilder->NamedShape()->Evolution() != TNaming_MODIFY)))
        {
          myBuilders.erase(0); // clear old builder to avoid different evolutions crash
          aBuilder = builder(0);
        }
      } else {
        int aTag = isGenerated ? getGenerationTag(aNewShape_)
                               : getModificationTag(aNewShape_);
        aBuilder = builder(aTag);

        // Check if new shape was already stored.
        if (isAlreadyStored(aBuilder, anOldSubShape_, aNewShape_)) continue;

        buildName(aTag, theName);
      }

      isGenerated ? aBuilder->Generated(anOldSubShape_, aNewShape_)
                  : aBuilder->Modify(anOldSubShape_, aNewShape_);
    }
  }
}

void Model_BodyBuilder::loadAndOrientGeneratedShapes (
  GeomAlgoAPI_MakeShape* theMS,
  GeomShapePtr  theShapeIn,
  const int  theKindOfShape,
  const int  theTag,
  const std::string& theName,
  GeomAPI_DataMapOfShapeShape& theSubShapes)
{
  TopoDS_Shape aShapeIn = theShapeIn->impl<TopoDS_Shape>();
  TopTools_MapOfShape aView;
  bool isBuilt = !theName.empty();
  TopExp_Explorer aShapeExplorer (aShapeIn, (TopAbs_ShapeEnum)theKindOfShape);
  for (; aShapeExplorer.More(); aShapeExplorer.Next ()) {
    const TopoDS_Shape& aRoot = aShapeExplorer.Current ();
    if (!aView.Add(aRoot)) continue;
    //if (TNaming_Tool::NamedShape(aRoot, builder(theTag)->NamedShape()->Label()).IsNull())
    //  continue; // there is no sense to write history if old shape does not exist in the document
    ListOfShape aList;
    GeomShapePtr aRShape(new GeomAPI_Shape());
    aRShape->setImpl((new TopoDS_Shape(aRoot)));
    theMS->generated(aRShape, aList);
    keepTopLevelShapes(aList, aRoot);
    std::list<GeomShapePtr >::const_iterator
      anIt = aList.begin(), aLast = aList.end();
    for (; anIt != aLast; anIt++) {
      TopoDS_Shape aNewShape = (*anIt)->impl<TopoDS_Shape>();
      if (theSubShapes.isBound(*anIt)) {
        GeomShapePtr aMapShape(theSubShapes.find(*anIt));
        aNewShape.Orientation(aMapShape->impl<TopoDS_Shape>().Orientation());
      }
      if (!aRoot.IsSame (aNewShape)) {
        builder(theTag)->Generated(aRoot,aNewShape);
        if(isBuilt)
          buildName(theTag, theName);
      }
      TopAbs_ShapeEnum aGenShapeType = aNewShape.ShapeType();
      if(aGenShapeType == TopAbs_WIRE || aGenShapeType == TopAbs_SHELL) {
        TopAbs_ShapeEnum anExplodeShapeType =
          aGenShapeType == TopAbs_WIRE ? TopAbs_EDGE : TopAbs_FACE;
        const TDF_Label aLabel = builder(theTag)->NamedShape()->Label();
        int aTag = 1;
        std::shared_ptr<Model_Document> aDoc =
          std::dynamic_pointer_cast<Model_Document>(document());
        for(TopExp_Explorer anExp(aNewShape, anExplodeShapeType); anExp.More(); anExp.Next()) {
          TDF_Label aChildLabel = aLabel.FindChild(aTag);
          TNaming_Builder aBuilder(aChildLabel);
          aBuilder.Generated(aRoot, anExp.Current());
          TCollection_AsciiString aChildName =
            TCollection_AsciiString((theName + "_").c_str()) + aTag;
          TDataStd_Name::Set(aChildLabel, aChildName.ToCString());
          aTag++;
        }
      }
    }
  }
}

//=======================================================================
int getDangleShapes(const TopoDS_Shape&           theShapeIn,
  const TopAbs_ShapeEnum        theGeneratedFrom,
  TopTools_DataMapOfShapeShape& theDangles)
{
  theDangles.Clear();
  TopTools_IndexedDataMapOfShapeListOfShape subShapeAndAncestors;
  TopAbs_ShapeEnum GeneratedTo;
  if (theGeneratedFrom == TopAbs_FACE) GeneratedTo = TopAbs_EDGE;
  else if (theGeneratedFrom == TopAbs_EDGE) GeneratedTo = TopAbs_VERTEX;
  else return Standard_False;
  TopExp::MapShapesAndAncestors(theShapeIn, GeneratedTo, theGeneratedFrom, subShapeAndAncestors);
  for (Standard_Integer i = 1; i <= subShapeAndAncestors.Extent(); i++) {
    const TopoDS_Shape& mayBeDangle = subShapeAndAncestors.FindKey(i);
    const TopTools_ListOfShape& ancestors = subShapeAndAncestors.FindFromIndex(i);
    if (ancestors.Extent() == 1) theDangles.Bind(ancestors.First(), mayBeDangle);
  }
  return theDangles.Extent();
}

//=======================================================================
void loadGeneratedDangleShapes(
  const TopoDS_Shape&      theShapeIn,
  const TopAbs_ShapeEnum   theGeneratedFrom,
  TNaming_Builder *        theBuilder)
{
  TopTools_DataMapOfShapeShape dangles;
  if (!getDangleShapes(theShapeIn, theGeneratedFrom, dangles)) return;
  TopTools_DataMapIteratorOfDataMapOfShapeShape itr(dangles);
  for (; itr.More(); itr.Next())
    theBuilder->Generated(itr.Key(), itr.Value());
}

//=======================================================================
void Model_BodyBuilder::loadNextLevels(GeomShapePtr theShape,
  const std::string& theName, int&  theTag)
{
  if(theShape->isNull()) return;
  TopoDS_Shape aShape = theShape->impl<TopoDS_Shape>();
  std::string aName;
  if (aShape.ShapeType() == TopAbs_SOLID) {
    TopExp_Explorer expl(aShape, TopAbs_FACE);
    for (; expl.More(); expl.Next()) {
      builder(theTag)->Generated(expl.Current());
      TCollection_AsciiString aStr(theTag);
      aName = theName + aStr.ToCString();
      buildName(theTag, aName);
      theTag++;
    }
  }
  else if (aShape.ShapeType() == TopAbs_SHELL || aShape.ShapeType() == TopAbs_FACE) {
    // load faces and all the free edges
    TopTools_IndexedMapOfShape Faces;
    TopExp::MapShapes(aShape, TopAbs_FACE, Faces);
    if (Faces.Extent() > 1 || (aShape.ShapeType() == TopAbs_SHELL && Faces.Extent() == 1)) {
      TopExp_Explorer expl(aShape, TopAbs_FACE);
      for (; expl.More(); expl.Next()) {
        builder(theTag)->Generated(expl.Current());
        TCollection_AsciiString aStr(theTag);
        aName = theName + aStr.ToCString();
        buildName(theTag, aName);
        theTag++;
      }
    }
    TopTools_IndexedDataMapOfShapeListOfShape anEdgeAndNeighbourFaces;
    TopExp::MapShapesAndAncestors(aShape, TopAbs_EDGE, TopAbs_FACE, anEdgeAndNeighbourFaces);
    for (Standard_Integer i = 1; i <= anEdgeAndNeighbourFaces.Extent(); i++)
    {
      const TopTools_ListOfShape& aLL = anEdgeAndNeighbourFaces.FindFromIndex(i);
      if (aLL.Extent() < 2) {
        if (BRep_Tool::Degenerated(TopoDS::Edge(anEdgeAndNeighbourFaces.FindKey(i))))
          continue;
        builder(theTag)->Generated(anEdgeAndNeighbourFaces.FindKey(i));
        TCollection_AsciiString aStr(theTag);
        aName = theName + aStr.ToCString();
        buildName(theTag, aName);
        theTag++;
      } else {
        TopTools_ListIteratorOfListOfShape anIter(aLL);
        const TopoDS_Face& aFace = TopoDS::Face(anIter.Value());
        anIter.Next();
        if(aFace.IsEqual(anIter.Value())) {
          builder(theTag)->Generated(anEdgeAndNeighbourFaces.FindKey(i));
          TCollection_AsciiString aStr(theTag);
          aName = theName + aStr.ToCString();
          buildName(theTag, aName);
          theTag++;
        }
      }
    }
  } else if (aShape.ShapeType() == TopAbs_WIRE) {
    TopTools_IndexedMapOfShape Edges;
    BRepTools::Map3DEdges(aShape, Edges);
    if (Edges.Extent() == 1) {
      builder(theTag++)->Generated(Edges.FindKey(1));
      TopExp_Explorer expl(aShape, TopAbs_VERTEX);
      for (; expl.More(); expl.Next()) {
        builder(theTag)->Generated(expl.Current());
        TCollection_AsciiString aStr(theTag);
        aName = theName + aStr.ToCString();
        buildName(theTag, aName);
        theTag++;
      }
    } else {
      TopExp_Explorer expl(aShape, TopAbs_EDGE);
      for (; expl.More(); expl.Next()) {
        builder(theTag)->Generated(expl.Current());
        TCollection_AsciiString aStr(theTag);
        aName = theName + aStr.ToCString();
        buildName(theTag, aName);
        theTag++;
      }
      // and load generated vertices.
      TopTools_DataMapOfShapeShape generated;
      if (getDangleShapes(aShape, TopAbs_EDGE, generated))
      {
        TNaming_Builder* pBuilder = builder(theTag++);
        loadGeneratedDangleShapes(aShape, TopAbs_EDGE, pBuilder);
      }
    }
  } else if (aShape.ShapeType() == TopAbs_EDGE) {
    TopExp_Explorer expl(aShape, TopAbs_VERTEX);
    for (; expl.More(); expl.Next()) {
      builder(theTag)->Generated(expl.Current());
      TCollection_AsciiString aStr(theTag);
      aName = theName + aStr.ToCString();
      buildName(theTag, aName);
      theTag++;
    }
  }
}

//=======================================================================
int findAmbiguities(const TopoDS_Shape&           theShapeIn,
  TopTools_ListOfShape&   theList)
{
  theList.Clear();
  // edges -> ancestor faces list
  TopTools_IndexedDataMapOfShapeListOfShape aSubShapeAndAncestors;
  TopExp::MapShapesAndAncestors(theShapeIn, TopAbs_EDGE, TopAbs_FACE, aSubShapeAndAncestors);
  // keeps the shapes which are already in the resulting list
  TopTools_MapOfShape alreadyThere;
  // map from faces identifier (combination of hash-codes) to list of edges produced such ID
  NCollection_DataMap<int, NCollection_List<TopoDS_Shape> > aFacesIDs;

  TopTools_IndexedDataMapOfShapeListOfShape::Iterator anAncestorsIter(aSubShapeAndAncestors);
  for (; anAncestorsIter.More(); anAncestorsIter.Next()) {
    const TopTools_ListOfShape& ancestors = anAncestorsIter.Value();
    if (ancestors.Extent() < 2)
      continue;
    Standard_Integer anID = 0;
    for(TopTools_ListIteratorOfListOfShape aFaceIt(ancestors); aFaceIt.More(); aFaceIt.Next()) {
      anID ^= HashCode(aFaceIt.ChangeValue(), 1990657); // Pierpont prime
    }
    if (aFacesIDs.IsBound(anID)) { // there found same edge, check they really have same faces
      const NCollection_List<TopoDS_Shape>& aSameFaces1 =
        aSubShapeAndAncestors.FindFromKey(anAncestorsIter.Key());
      NCollection_List<TopoDS_Shape>::Iterator aSameEdge(aFacesIDs.ChangeFind(anID));
      for(; aSameEdge.More(); aSameEdge.Next()) {
        const NCollection_List<TopoDS_Shape>& aSameFaces2 =
          aSubShapeAndAncestors.FindFromKey(aSameEdge.Value());
        if (aSameFaces2.Extent() != aSameFaces1.Extent()) // the number of faces is different
          break;

        NCollection_List<TopoDS_Shape>::Iterator aFaceIter1(aSameFaces1);
        for(; aFaceIter1.More(); aFaceIter1.Next()) {
          NCollection_List<TopoDS_Shape>::Iterator aFaceIter2(aSameFaces2);
          for(; aFaceIter2.More(); aFaceIter2.Next()) {
            if (aFaceIter1.Value().IsSame(aFaceIter2.Value()))
              break;
          }
          if (!aFaceIter2.More()) // aFaceIter1 contains a face, which is not in aFaceIter2
            break;
        }
        if (!aFaceIter1.More()) { // all the faces are same => put to the result
          if (alreadyThere.Add(aSameEdge.Value()))
            theList.Append(aSameEdge.Value());
          if (alreadyThere.Add(anAncestorsIter.Key()))
            theList.Append(anAncestorsIter.Key());
        }
      }
    } else { // ID is unique, just add this edge
      aFacesIDs.Bind(anID, NCollection_List<TopoDS_Shape>());
    }
    aFacesIDs.ChangeFind(anID).Append(anAncestorsIter.Key()); // add to the list anyway
  }
  return theList.Extent();
}

//=======================================================================
void Model_BodyBuilder::loadFirstLevel(
  GeomShapePtr theShape, const std::string& theName, int&  theTag)
{
  if(theShape->isNull()) return;
  TopoDS_Shape aShape = theShape->impl<TopoDS_Shape>();
  std::string aName;
  if (aShape.ShapeType() == TopAbs_COMPOUND || aShape.ShapeType() == TopAbs_COMPSOLID) {
    TopoDS_Iterator itr(aShape);
    for (; itr.More(); itr.Next()) {
      builder(theTag)->Generated(itr.Value());
      TCollection_AsciiString aStr(theTag);
      aName = theName + aStr.ToCString();
      buildName(theTag, aName);
      if(!theName.empty()) buildName(theTag, aName);
      theTag++;
      if (itr.Value().ShapeType() == TopAbs_COMPOUND ||
        itr.Value().ShapeType() == TopAbs_COMPSOLID)
      {
        GeomShapePtr itrShape(new GeomAPI_Shape());
        itrShape->setImpl(new TopoDS_Shape(itr.Value()));
        loadFirstLevel(itrShape, theName, theTag);
      } else {
        GeomShapePtr itrShape(new GeomAPI_Shape());
        itrShape->setImpl(new TopoDS_Shape(itr.Value()));
        loadNextLevels(itrShape, theName, theTag);
      }
    }
  } else {
    GeomShapePtr itrShape(new GeomAPI_Shape());
    itrShape->setImpl(new TopoDS_Shape(aShape));
    loadNextLevels(itrShape, theName, theTag);
  }
  TopTools_ListOfShape   aList;
  if(findAmbiguities(aShape, aList)) {
    TopTools_ListIteratorOfListOfShape it(aList);
    for (; it.More(); it.Next(),theTag++) {
      builder(theTag)->Generated(it.Value());
      TCollection_AsciiString aStr(theTag);
      aName = theName + aStr.ToCString();
      buildName(theTag, aName);
    }
  }
}

//=======================================================================
void Model_BodyBuilder::loadDisconnectedEdges(
  GeomShapePtr theShape, const std::string& theName, int&  theTag)
{
  if(theShape->isNull()) return;
  TopoDS_Shape aShape = theShape->impl<TopoDS_Shape>();
  TopTools_DataMapOfShapeListOfShape edgeNaborFaces;
  TopTools_ListOfShape empty;
  TopExp_Explorer explF(aShape, TopAbs_FACE);
  for (; explF.More(); explF.Next()) {
    const TopoDS_Shape& aFace = explF.Current();
    TopExp_Explorer explV(aFace, TopAbs_EDGE);
    for (; explV.More(); explV.Next()) {
      const TopoDS_Shape& anEdge = explV.Current();
      if (!edgeNaborFaces.IsBound(anEdge)) edgeNaborFaces.Bind(anEdge, empty);
      Standard_Boolean faceIsNew = Standard_True;
      TopTools_ListIteratorOfListOfShape itrF(edgeNaborFaces.Find(anEdge));
      for (; itrF.More(); itrF.Next()) {
        if (itrF.Value().IsSame(aFace)) {
          faceIsNew = Standard_False;
          break;
        }
      }
      if (faceIsNew)
        edgeNaborFaces.ChangeFind(anEdge).Append(aFace);
    }
  }

  TopTools_MapOfShape anEdgesToDelete;
  TopExp_Explorer anEx(aShape,TopAbs_EDGE);
  std::string aName;
  for(;anEx.More();anEx.Next()) {
    Standard_Boolean aC0 = Standard_False;
    TopoDS_Shape anEdge1 = anEx.Current();
    if (edgeNaborFaces.IsBound(anEdge1)) {
      const TopTools_ListOfShape& aList1 = edgeNaborFaces.Find(anEdge1);
      if (aList1.Extent()<2) continue;
      TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itr(edgeNaborFaces);
      for (; itr.More(); itr.Next()) {
        TopoDS_Shape anEdge2 = itr.Key();
        if(anEdgesToDelete.Contains(anEdge2)) continue;
        if (anEdge1.IsSame(anEdge2)) continue;
        const TopTools_ListOfShape& aList2 = itr.Value();
        // compare lists of the neighbour faces of edge1 and edge2
        if (aList1.Extent() == aList2.Extent()) {
          Standard_Integer aMatches = 0;
          for(TopTools_ListIteratorOfListOfShape aLIter1(aList1);aLIter1.More();aLIter1.Next())
            for(TopTools_ListIteratorOfListOfShape aLIter2(aList2);aLIter2.More();aLIter2.Next())
              if (aLIter1.Value().IsSame(aLIter2.Value())) aMatches++;
          if (aMatches == aList1.Extent()) {
            aC0=Standard_True;
            builder(theTag)->Generated(anEdge2);
            anEdgesToDelete.Add(anEdge2);
            TCollection_AsciiString aStr(theTag);
            aName = theName + aStr.ToCString();
            buildName(theTag, aName);
            theTag++;
          }
        }
      }
      TopTools_MapIteratorOfMapOfShape itDelete(anEdgesToDelete);
      for(;itDelete.More();itDelete.Next())
        edgeNaborFaces.UnBind(itDelete.Key());
      edgeNaborFaces.UnBind(anEdge1);
    }
    if (aC0) {
      builder(theTag)->Generated(anEdge1);
      TCollection_AsciiString aStr(theTag);
      aName = theName + aStr.ToCString();
      buildName(theTag, aName);
      theTag++;
    }
  }
}

void Model_BodyBuilder::loadDisconnectedVertexes(GeomShapePtr theShape,
                                                 const std::string& theName, int&  theTag)
{
  if(theShape->isNull()) return;
  TopoDS_Shape aShape = theShape->impl<TopoDS_Shape>();
  TopTools_DataMapOfShapeListOfShape vertexNaborEdges;
  TopTools_ListOfShape empty;
  TopExp_Explorer explF(aShape, TopAbs_EDGE);
  for (; explF.More(); explF.Next()) {
    const TopoDS_Shape& anEdge = explF.Current();
    TopExp_Explorer explV(anEdge, TopAbs_VERTEX);
    for (; explV.More(); explV.Next()) {
      const TopoDS_Shape& aVertex = explV.Current();
      if (!vertexNaborEdges.IsBound(aVertex)) vertexNaborEdges.Bind(aVertex, empty);
      Standard_Boolean faceIsNew = Standard_True;
      TopTools_ListIteratorOfListOfShape itrF(vertexNaborEdges.Find(aVertex));
      for (; itrF.More(); itrF.Next()) {
        if (itrF.Value().IsSame(anEdge)) {
          faceIsNew = Standard_False;
          break;
        }
      }
      if (faceIsNew) {
        vertexNaborEdges.ChangeFind(aVertex).Append(anEdge);
      }
    }
  }
  std::string aName;
  TopTools_DataMapIteratorOfDataMapOfShapeListOfShape itr(vertexNaborEdges);
  for (; itr.More(); itr.Next()) {
    const TopTools_ListOfShape& naborEdges = itr.Value();
    if (naborEdges.Extent() < 2) {
      builder(theTag)->Generated(itr.Key());
      TCollection_AsciiString aStr(theTag);
      aName = theName + aStr.ToCString();
      buildName(theTag, aName);
      theTag++;
    }
  }
}

GeomShapePtr Model_BodyBuilder::shape()
{
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(data());
  if (aData && aData->isValid()) {
    TDF_Label aShapeLab = aData->shapeLab();
    Handle(TDF_Reference) aRef;
    if (aShapeLab.FindAttribute(TDF_Reference::GetID(), aRef)) {
      aShapeLab = aRef->Get();
    }
    Handle(TNaming_NamedShape) aName;
    if (aShapeLab.FindAttribute(TNaming_NamedShape::GetID(), aName)) {
      TopoDS_Shape aShape = aName->Get();
      if (!aShape.IsNull()) {
        GeomShapePtr aRes(new GeomAPI_Shape);
        aRes->setImpl(new TopoDS_Shape(aShape));
        return aRes;
      }
    }
  }
  return GeomShapePtr();
}

bool Model_BodyBuilder::isLatestEqual(const GeomShapePtr& theShape)
{
  if (theShape.get()) {
    TopoDS_Shape aShape = theShape->impl<TopoDS_Shape>();
    std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(data());
    if (aData) {
      TDF_Label& aShapeLab = aData->shapeLab();
      Handle(TNaming_NamedShape) aName;
      if (aShapeLab.FindAttribute(TNaming_NamedShape::GetID(), aName)) {
        TopoDS_Shape aLatest = TNaming_Tool::CurrentShape(aName);
        if (aLatest.IsNull())
          return false;
        if (aLatest.IsEqual(aShape))
          return true;
        // check sub-shapes for comp-solids:
        for (TopExp_Explorer anExp(aShape, aLatest.ShapeType()); anExp.More(); anExp.Next()) {
          if (aLatest.IsEqual(anExp.Current()))
            return true;
        }
      }
    }
  }
  return false;
}
