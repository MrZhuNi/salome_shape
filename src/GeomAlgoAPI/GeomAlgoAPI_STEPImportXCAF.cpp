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

#include <GeomAlgoAPI_STEPImportXCAF.h>

#include <TDF_ChildIDIterator.hxx>
#include <TDF_Label.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_Comment.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>

#include <IFSelect_ReturnStatus.hxx>
#include <Interface_EntityIterator.hxx>
#include <Interface_Graph.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Interface_Static.hxx>
#include <STEPControl_Reader.hxx>
#include <StepBasic_Product.hxx>
#include <StepBasic_ProductDefinition.hxx>
#include <StepBasic_ProductDefinitionFormation.hxx>
#include <StepGeom_GeometricRepresentationItem.hxx>
#include <StepShape_TopologicalRepresentationItem.hxx>
#include <StepRepr_DescriptiveRepresentationItem.hxx>
#include <StepRepr_ProductDefinitionShape.hxx>
#include <StepRepr_PropertyDefinitionRepresentation.hxx>
#include <StepRepr_Representation.hxx>
#include <TransferBRep.hxx>
#include <Transfer_Binder.hxx>
#include <Transfer_TransientProcess.hxx>
#include <XSControl_TransferReader.hxx>
#include <XSControl_WorkSession.hxx>

#include <BRep_Builder.hxx>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>

#include <XCAFApp_Application.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <OSD_Exception.hxx>
#include <Locale_Convert.h>

#include <TDocStd_Document.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_MaterialTool.hxx> 
#include <Quantity_Color.hxx>
#include <TopoDS.hxx>
#include <STEPConstruct.hxx>
#include <STEPConstruct_Tool.hxx>
#include <StepBasic_ProductDefinitionRelationship.hxx>
#include <StepRepr_NextAssemblyUsageOccurrence.hxx>
#include <XCAFDoc_Location.hxx>

#include <TColStd_SequenceOfAsciiString.hxx>

#include <Standard_Failure.hxx>
#include <Standard_ErrorHandler.hxx> // CAREFUL ! position of this file is critic : see Lucien PIGNOLONI / OCC


//=============================================================================
/*!
 *  GetShape()
 */
//=============================================================================

TopoDS_Shape GetShape(const Handle(Standard_Transient)        &theEnti,
                        const Handle(Transfer_TransientProcess) &theTP)
{
  TopoDS_Shape            aResult;
  Handle(Transfer_Binder) aBinder = theTP->Find(theEnti);

  if (aBinder.IsNull()) {
    return aResult;
  }

  aResult = TransferBRep::ShapeResult(aBinder);

  return aResult;
}

// ----------------------------------------------------------------------------

std::shared_ptr<GeomAPI_Shape> readAttributes( STEPCAFControl_Reader &reader,
                                               std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                                               const bool  anMaterials,
                                               std::map< std::wstring, std::list<std::wstring>> &theMaterialShape,
                                               const std::string &format)
{
  // dummy XCAF Application to handle the STEP XCAF Document
  Handle(XCAFApp_Application) dummy_app = XCAFApp_Application::GetApplication();
  // XCAF Document to contain the STEP/IGES file itself
  Handle(TDocStd_Document) doc;
   // check if a file is already open under this handle, if so, close it to
  // prevent segfaults when trying to create a new document
  if(dummy_app->NbDocuments() > 0) {
    dummy_app->GetDocument(1, doc);
    dummy_app->Close(doc);
  }

  dummy_app->NewDocument( TCollection_ExtendedString("MDTV-CAF"), doc);
  // transfer STEP/IGES into the document, and get the main label
  reader.Transfer(doc);
  TDF_Label mainLabel = doc->Main();
  Handle_XCAFDoc_ShapeTool shapeTool = XCAFDoc_DocumentTool::ShapeTool(mainLabel);
  Handle_XCAFDoc_ColorTool colorTool = XCAFDoc_DocumentTool::ColorTool(mainLabel);
  Handle(XCAFDoc_MaterialTool) materialTool = XCAFDoc_DocumentTool::MaterialTool(mainLabel);
  // traverse the labels recursively to set attributes on shapes
  setShapeAttributes(shapeTool, colorTool, materialTool, mainLabel,
                     TopLoc_Location(),theResultBody,theMaterialShape,false);

     
  std::shared_ptr<GeomAPI_Shape> ageom =  setgeom(shapeTool,mainLabel);
 
  STEPControl_Reader aReader = reader.ChangeReader();   
  
  // BEGIN: reading materials of sub-shapes from file
  if ( anMaterials )
  {
    TopTools_IndexedMapOfShape anIndices;
    TopExp::MapShapes(ageom->impl<TopoDS_Shape>(), anIndices);

    Handle(Interface_InterfaceModel) Model = aReader.WS()->Model();
    Handle(XSControl_TransferReader) TR = aReader.WS()->TransferReader();
    if (!TR.IsNull()) {
      Handle(Transfer_TransientProcess) TP = TR->TransientProcess();

      Standard_Integer nb = Model->NbEntities();

      for (Standard_Integer ie = 1; ie <= nb; ie++) {
        Handle(Standard_Transient) enti = Model->Value(ie);

        // Store materials.
        StoreMaterial(theResultBody,enti, anIndices, TP, mainLabel,theMaterialShape);
      }
    }
  }

  return ageom;                                  
}

std::shared_ptr<GeomAPI_Shape> setgeom(const Handle(XCAFDoc_ShapeTool) &shapeTool,
                                       const TDF_Label &label)
{
  BRep_Builder B;
  TopoDS_Compound compound;
  B.MakeCompound(compound);

  TDF_LabelSequence frshapes;
  shapeTool->GetShapes(frshapes);

  std::shared_ptr<GeomAPI_Shape> aGeomShape(new GeomAPI_Shape);

  if (frshapes.Length() == 0) {
      aGeomShape->setImpl(new TopoDS_Shape());
      return aGeomShape;
  } else if (frshapes.Length() == 1) {
    TopoDS_Shape shape = shapeTool->GetShape(frshapes.Value(1));
    aGeomShape->setImpl(new TopoDS_Shape(shape));
    return aGeomShape;
  } else {
    for (Standard_Integer i=1; i<frshapes.Length(); i++) {
      TopoDS_Shape S = shapeTool->GetShape(frshapes.Value(i));

      TDF_Label aLabel = shapeTool->FindShape(S, Standard_False);
      if ( (!aLabel.IsNull()) && (shapeTool->IsShape(aLabel)) ) {
        if (shapeTool->IsFree(aLabel) ) {
          if (S.IsNull()) {
            continue;
          }
          else {
            B.Add(compound, S);
          }
        }
      }
    }
    TopoDS_Shape shape = compound;
    aGeomShape->setImpl(new TopoDS_Shape(shape));
    return aGeomShape;
  }
}

void setShapeAttributes(const Handle(XCAFDoc_ShapeTool) &shapeTool,
                                    const Handle(XCAFDoc_ColorTool) &colorTool,
                                    const Handle(XCAFDoc_MaterialTool) &materialTool,
                                    const TDF_Label &label,
                                    const TopLoc_Location &loc,
                                    std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                                    std::map< std::wstring, std::list<std::wstring>> &theMaterialShape,
                                    bool isRef)
{
  std::wstring shapeName;
  Handle(TDataStd_Name) n;

  if(label.FindAttribute(TDataStd_Name::GetID(), n)) {
    TCollection_ExtendedString name = n->Get();

    shapeName =  Locale::Convert::toWString(TCollection_AsciiString(name).ToCString()) ;
  }
 
  TopLoc_Location partLoc = loc;
  Handle(XCAFDoc_Location) l;
  if(label.FindAttribute(XCAFDoc_Location::GetID(), l)) {
    if(isRef)
      partLoc = partLoc * l->Get();
    else
      partLoc = l->Get();
  }

  TDF_Label ref;
  if(shapeTool->IsReference(label) && shapeTool->GetReferredShape(label, ref)) {

    setShapeAttributes( shapeTool, colorTool, materialTool, ref,
                        partLoc,theResultBody,theMaterialShape,true);
  }
 
  if( shapeTool->IsSimpleShape(label) && (isRef || shapeTool->IsFree(label))) {

    TopoDS_Shape shape = shapeTool->GetShape(label);

    std::shared_ptr<GeomAPI_Shape> aShapeGeom(new GeomAPI_Shape);
    if (!loc.IsIdentity()){
        shape.Move(loc);
    }
    aShapeGeom->setImpl(new TopoDS_Shape(shape));
    shapeName = theResultBody->addShapeName(aShapeGeom, shapeName);


    shape.Location(isRef ? loc : partLoc);
    int dim =
      (shape.ShapeType() == TopAbs_VERTEX) ?
        0 :
        (shape.ShapeType() == TopAbs_EDGE || shape.ShapeType() == TopAbs_WIRE) ?
        1 :
        (shape.ShapeType() == TopAbs_FACE ||
         shape.ShapeType() == TopAbs_SHELL) ? 2 :3;

    Handle(TCollection_HAsciiString) matName;
    Handle(TCollection_HAsciiString) matDescription;
    Standard_Real matDensity;
    Handle(TCollection_HAsciiString) matDensName;
    Handle(TCollection_HAsciiString) matDensValType;

    if(materialTool->GetMaterial(label, matName, matDescription, matDensity,
                                 matDensName, matDensValType)) {
      std::wstring nameMaterial = Locale::Convert::toWString(matName->ToCString()); 
  
      theMaterialShape[nameMaterial].push_back(shapeName);
    }


    Quantity_Color col;
    if(colorTool->GetColor(label, XCAFDoc_ColorGen, col)) {
      double r = col.Red(), g = col.Green(), b = col.Blue();
      std::vector<int> ColRGB = {int(r*255),int(g*255),int(b*255)};
      theResultBody->addShapeColor(shapeName, ColRGB);
    }
    else if(colorTool->GetColor(label, XCAFDoc_ColorSurf, col)) {
      double r = col.Red(), g = col.Green(), b = col.Blue();
      std::vector<int> ColRGB = {int(r*255),int(g*255),int(b*255)};
      theResultBody->addShapeColor(shapeName, ColRGB);
    }
    else if(colorTool->GetColor(label, XCAFDoc_ColorCurv, col)) {
     double r = col.Red(), g = col.Green(), b = col.Blue();
     std::vector<int> ColRGB = {int(r*255),int(g*255),int(b*255)};
      theResultBody->addShapeColor(shapeName, ColRGB);
    }
    // check explicit coloring of boundary entities
    if(dim == 3) {
      TopExp_Explorer xp2(shape, TopAbs_FACE);
      while(xp2.More()) {
        if(colorTool->GetColor(xp2.Current(), XCAFDoc_ColorGen, col) ||
           colorTool->GetColor(xp2.Current(), XCAFDoc_ColorSurf, col) ||
           colorTool->GetColor(xp2.Current(), XCAFDoc_ColorCurv, col)) {
          double r = col.Red(), g = col.Green(), b = col.Blue();
          TopoDS_Face face = TopoDS::Face(xp2.Current());
          std::vector<int> ColRGB = {int(r*255),int(g*255),int(b*255)};
          std::wstringstream aNameFace; 
          TopoDS_Shape shapeface = xp2.Current();
          if (!loc.IsIdentity()){
                  shapeface.Move(loc);
          }
          aShapeGeom->setImpl(new TopoDS_Shape(shapeface));
          theResultBody->addShapeColor(
          theResultBody->addShapeName(aShapeGeom , aNameFace.str()), ColRGB);
        }
        xp2.Next();
      }
    }
    if(dim == 2) {
      TopExp_Explorer xp1(shape, TopAbs_EDGE);
      while(xp1.More()) {
        if(colorTool->GetColor(xp1.Current(), XCAFDoc_ColorGen, col) ||
           colorTool->GetColor(xp1.Current(), XCAFDoc_ColorSurf, col) ||
           colorTool->GetColor(xp1.Current(), XCAFDoc_ColorCurv, col)) {
           double r = col.Red(), g = col.Green(), b = col.Blue();
           std::vector<int> ColRGB = {int(r*255),int(g*255),int(b*255)};
           std::wstringstream aNameEdge; 
           aNameEdge << L"Edge_"<< shapeName;
           aShapeGeom->setImpl(new TopoDS_Shape(xp1.Current() ));
           theResultBody->addShapeColor(
           theResultBody->addShapeName(aShapeGeom , aNameEdge.str()), ColRGB);
        }
        xp1.Next();
      }
    }
  }
  else {
    int indiceChild = 1;  

    if (!shapeTool->IsReference(label)){
      TopoDS_Shape shape = shapeTool->GetShape(label);

      std::shared_ptr<GeomAPI_Shape> aShapeGeom(new GeomAPI_Shape);
      if (!loc.IsIdentity()){
          shape.Move(loc);
      }
      aShapeGeom->setImpl(new TopoDS_Shape(shape));
      shapeName = theResultBody->addShapeName(aShapeGeom, shapeName);
    }
    for(TDF_ChildIterator it(label); it.More(); it.Next()) { 

      setShapeAttributes( shapeTool, colorTool, materialTool,
                         it.Value(), partLoc,theResultBody,theMaterialShape, isRef);
      indiceChild++;
    }
  }
}


//=============================================================================
  /*!
   *  StoreMaterial()
   */
  //=============================================================================

  void StoreMaterial( std::shared_ptr<ModelAPI_ResultBody>    theResultBody,
                      const Handle(Standard_Transient)        &theEnti,
                      const TopTools_IndexedMapOfShape        &theIndices,
                      const Handle(Transfer_TransientProcess) &theTP,
                      const TDF_Label                         &theShapeLabel,
                      std::map< std::wstring, std::list<std::wstring>> &theMaterialShape )
  {
    // Treat Product Definition Shape only.
    Handle(StepRepr_ProductDefinitionShape) aPDS =
      Handle(StepRepr_ProductDefinitionShape)::DownCast(theEnti);
    Handle(StepBasic_ProductDefinition)     aProdDef;

    if(aPDS.IsNull() == Standard_False) {
      // Product Definition Shape ==> Product Definition
      aProdDef = aPDS->Definition().ProductDefinition();
    }

    if (aProdDef.IsNull() == Standard_False) {
      // Product Definition ==> Property Definition
      const Interface_Graph    &aGraph = theTP->Graph();
      Interface_EntityIterator  aSubs  = aGraph.Sharings(aProdDef);
      TopoDS_Shape              aShape;

      for(aSubs.Start(); aSubs.More(); aSubs.Next()) {
        Handle(StepRepr_PropertyDefinition) aPropD =
          Handle(StepRepr_PropertyDefinition)::DownCast(aSubs.Value());

        if(aPropD.IsNull() == Standard_False) {
          // Property Definition ==> Representation.
          Interface_EntityIterator aSubs1 = aGraph.Sharings(aPropD);

          for(aSubs1.Start(); aSubs1.More(); aSubs1.Next()) {
            Handle(StepRepr_PropertyDefinitionRepresentation) aPDR =
              Handle(StepRepr_PropertyDefinitionRepresentation)::
              DownCast(aSubs1.Value());

            if(aPDR.IsNull() == Standard_False) {
              // Property Definition ==> Material Name.
              Handle(StepRepr_Representation) aRepr = aPDR->UsedRepresentation();

              if(aRepr.IsNull() == Standard_False) {
                Standard_Integer ir;

                for(ir = 1; ir <= aRepr->NbItems(); ir++) {
                  Handle(StepRepr_RepresentationItem) aRI = aRepr->ItemsValue(ir);
                  Handle(StepRepr_DescriptiveRepresentationItem) aDRI =
                    Handle(StepRepr_DescriptiveRepresentationItem)::DownCast(aRI);

                  if(aDRI.IsNull() == Standard_False) {
                    // Get shape from Product Definition
                    Handle(TCollection_HAsciiString) aMatName = aDRI->Name();
                    if(aMatName.IsNull() == Standard_False) {
                      TCollection_ExtendedString
                        aMatNameExt (aMatName->ToCString());

                      if (aShape.IsNull()) {
                        //Get the shape.
                        aShape = GetShape(aProdDef, theTP);
                        if (aShape.IsNull()) {
                          return;
                        }
                      }

                      // as PRODUCT can be included in the main shape
                      // several times, we look here for all iclusions.
                      Standard_Integer isub, nbSubs = theIndices.Extent();

                      for (isub = 1; isub <= nbSubs; isub++) {
                        TopoDS_Shape aSub = theIndices.FindKey(isub);

                        if (aSub.IsPartner(aShape)) {
                          std::shared_ptr<GeomAPI_Shape> aShapeGeom(new GeomAPI_Shape);
                          aShapeGeom->setImpl(new TopoDS_Shape(aSub));
                          std::wstring nom = theResultBody->findShapeName(aShapeGeom);
                          std::wstring matName= Locale::Convert::toWString(aMatName->ToCString()); 
                          theMaterialShape[matName].push_back(nom);

                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }

