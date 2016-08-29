// Copyright

// File:        GeomAlgoAPI_ParameterLaw.cpp


#include "GeomAlgoAPI_ParameterLaw.h"

//trier les include un jour...
//#include <GeomAPI_Face.h>
#include <math.h>   
#include <GeomAPI_Lin.h>
#include <Geom_Line.hxx>
#include <gp_Lin.hxx>
#include <GeomAPI_ShapeExplorer.h>
#include <GeomAlgoAPI_DFLoader.h>
#include <GeomAlgoAPI_MakeShapeList.h>
#include <GeomAlgoAPI_ShapeTools.h>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepOffsetAPI_MakePipe.hxx>
#include <GeomFill_Trihedron.hxx>
#include <BRepOffsetAPI_MakePipeShell.hxx>
#include <BRepGProp.hxx>
#include <Geom_Plane.hxx>
#include <GeomLib_IsPlanarSurface.hxx>
#include <gp_Pln.hxx>
#include <GProp_GProps.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Edge.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <GProp_PGProps.hxx>
#include <Extrema_ExtPElC.hxx>
#include "GeomAPI.h"
#include "GeomAPI_Edge.h"
#include "GeomAPI_Wire.h"
#include "GeomAPI_Pnt.h"
#include "GeomAPI_Dir.h"
#include "GeomAPI_Ax3.h"
#include "GeomAPI_Curve.h"

#include <GeomAPI.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <GeomAPI_IntCS.hxx>
#include <BRepAlgo_NormalProjection.hxx>
#include <BRep_Tool.hxx>
#include <Geom_Curve.hxx>
#include <BRepAdaptor_HCompCurve.hxx>
#include <BRepAdaptor_HCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>
#include "GeomAlgoAPI_Placement.h"
#include <BRepBuilderAPI_Transform.hxx>
#include "GeomAlgoAPI_MakeShape.h"

#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAPI_PlanarEdges.h>
#include <GeomAlgoAPI_SketchBuilder.h>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>

//#define NB_SECT 50

/// \return solid created from face or shell.
static TopoDS_Solid makeSolidFromShape(const TopoDS_Shape& theShape);

//=================================================================================================
GeomAlgoAPI_ParameterLaw::GeomAlgoAPI_ParameterLaw()
{
}

//=================================================================================================
GeomAlgoAPI_ParameterLaw::GeomAlgoAPI_ParameterLaw(std::shared_ptr<GeomAPI_Shape> theDefShape,
	std::shared_ptr<GeomAPI_Shape>   theRefShape,
	bool isReverse)
{
	build(theDefShape, theRefShape, isReverse);
}

//=================================================================================================
void GeomAlgoAPI_ParameterLaw::build(const std::shared_ptr<GeomAPI_Shape>& theDefShape,
	const std::shared_ptr<GeomAPI_Shape>&   theRefShape,
	const bool isReverse)
{
	if(!theDefShape || !theRefShape) {
		return;
	}

	// Getting Def
	TopoDS_Edge aDefEdge;
	if(theDefShape->shapeType() == GeomAPI_Shape::EDGE) {
		aDefEdge = TopoDS::Edge(theDefShape->impl<TopoDS_Shape>());
	}
	// Getting Ref
	TopoDS_Edge aRefEdge;
	if(theRefShape->shapeType() == GeomAPI_Shape::EDGE) {
		aRefEdge = TopoDS::Edge(theRefShape->impl<TopoDS_Shape>());
	}


	//COMPUTE RESULT
	TopoDS_Shape aResult;

	double defFirst, defLast, refFirst, refLast;
	Handle(Geom_Curve) aDefCurve = BRep_Tool::Curve(aDefEdge, defFirst, defLast);
	if(isReverse)
	{
		double aTemp = defFirst;
		defFirst = defLast;
		defLast = aTemp;
	}

	//recover Ref line
	Handle(Geom_Curve) aRefCurve = BRep_Tool::Curve(aRefEdge, refFirst, refLast);
	GeomAdaptor_Curve aRefAdaptorCurve = GeomAdaptor_Curve(aRefCurve);

	gp_Pnt rFP, rLP;
	gp_XYZ rF, rL;
	aRefCurve->D0(refFirst, rFP);
	aRefCurve->D0(refLast, rLP);
	rF = rFP.XYZ();
	rL = rLP.XYZ();
	rL.Subtract(rF);
	gp_Lin aRefLin = gp_Lin(rFP, rL);
	refFirst = aRefAdaptorCurve.FirstParameter();
	refLast = aRefAdaptorCurve.LastParameter();

	gp_Pnt DF;
	aDefCurve->D0(defFirst, DF);


	//to store edges for the final result
	BRepBuilderAPI_MakeEdge mkEdge;
	TopTools_ListOfShape aEL;

	//project First Point of the def curve on the ref line
	// to get the translation vector
	double Tol = 1.0e-10;
	Extrema_ExtPElC aProjDF = Extrema_ExtPElC(DF, aRefLin, Tol, refFirst, refLast);
	int nbDMin = aProjDF.NbExt();
	nbDMin++;
	int k = 1;
	while(k < nbDMin && !aProjDF.IsMin(k))
	{
		k++;
	}

	Extrema_POnCurv DFPOC = aProjDF.Point(k);
	gp_Pnt DFP = DFPOC.Value();

	//get translation vector
	// and make the translation transformation
	// then do the transformation and recover curve.
	gp_Vec aTransVec = rF - DFP.XYZ();
	gp_Trsf aTrans = gp_Trsf();
	aTrans.SetTranslation(aTransVec);

	TopoDS_Edge aTransDefEdge;
	BRepBuilderAPI_Transform* aBuilder = new BRepBuilderAPI_Transform(aDefEdge, aTrans, false);
	if(aBuilder) {
		if(aBuilder->IsDone()) {
			aTransDefEdge = TopoDS::Edge(aBuilder->Shape());
		}
	}

	aEL.Append(aTransDefEdge);
	double tdFirst, tdLast;
	Handle(Geom_Curve) aTransDefCurve = BRep_Tool::Curve(aTransDefEdge, tdFirst, tdLast);
	if(isReverse)
	{
		double aTemp = tdFirst;
		tdFirst = tdLast;
		tdLast = aTemp;
	}

	//now project last point of the transDefEdge
	// then make the needed edges for final result
	gp_Pnt TDF, TDL;
	aTransDefCurve->D0(tdLast, TDL);
	aTransDefCurve->D0(tdFirst, TDF);
	Extrema_ExtPElC aProjDL = Extrema_ExtPElC(TDL, aRefLin, Tol, refFirst, refLast);
	nbDMin = aProjDF.NbExt();
	nbDMin++;
	k = 1;
	while(k < nbDMin && !aProjDF.IsMin(k))
	{
		k++;
	}
	Extrema_POnCurv DLPOC = aProjDL.Point(k);
	gp_Pnt DLP = DLPOC.Value();

	mkEdge = BRepBuilderAPI_MakeEdge(rFP, DLP);
	if(mkEdge.IsDone())
		aEL.Append(mkEdge.Edge());
	mkEdge = BRepBuilderAPI_MakeEdge(DLP, TDL);
	if(mkEdge.IsDone())
		aEL.Append(mkEdge.Edge());
	mkEdge = BRepBuilderAPI_MakeEdge(TDF, rFP);
	if(mkEdge.IsDone())
		aEL.Append(mkEdge.Edge());

	BRepBuilderAPI_MakeWire mkWire;
	mkWire.Add(aEL);
	BRepBuilderAPI_MakeFace mkFace;
	if(mkWire.IsDone())
		mkFace = BRepBuilderAPI_MakeFace(mkWire.Wire());
	if(mkFace.IsDone())
		aResult = mkFace.Face();

	// Setting result.
	if(aResult.IsNull()) {
		return;
	}


	std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
	aShape->setImpl(new TopoDS_Shape(aResult));
	this->setShape(aShape);
	this->setDone(true);
}



//=================================================================================================
std::vector<double> GeomAlgoAPI_ParameterLaw::getVectorOfScalar(const std::shared_ptr<GeomAPI_Shape>&   theDef,
	const std::shared_ptr<GeomAPI_Shape>&   theRef,
	const bool isReverse,
	const int nbSect)
{
	std::vector<double> result;

	// Getting law
	// DEF
	TopoDS_Edge aDefEdge;
	if(theDef->shapeType() == GeomAPI_Shape::EDGE) {
		aDefEdge = TopoDS::Edge(theDef->impl<TopoDS_Shape>());
	}
	// REF
	TopoDS_Edge aRefEdge;
	if(theRef->shapeType() == GeomAPI_Shape::EDGE) {
		aRefEdge = TopoDS::Edge(theRef->impl<TopoDS_Shape>());
	}

	double d, dFirst, dLast, rFirst, rLast;

	//recover Def curve
	Handle(Geom_Curve) aDefCurve = BRep_Tool::Curve(aDefEdge, dFirst, dLast);
	if(isReverse)
	{
		//aPathCurve = aPathCurve->Reversed();
		double aTemp = dFirst;
		dFirst = dLast;
		dLast = aTemp;
	}
	d = dFirst;
	const double dStep = abs(dLast - dFirst) / (nbSect - 1);

	double coeffReverse = 1.0;
	if(isReverse)
	{
		coeffReverse = -1.0;
	}

	//recover Ref line
	Handle(Geom_Curve) aRefCurve = BRep_Tool::Curve(aRefEdge, rFirst, rLast);
	GeomAdaptor_Curve aRefAdaptorCurve = GeomAdaptor_Curve(aRefCurve);

	gp_Pnt rFP, rLP;
	gp_XYZ rF, rL;
	aRefCurve->D0(rFirst, rFP);
	aRefCurve->D0(rLast, rLP);
	rF = rFP.XYZ();
	rL = rLP.XYZ();
	rL.Subtract(rF);
	gp_Lin aRefLin = gp_Lin(rFP, rL);
	rFirst = aRefAdaptorCurve.FirstParameter();
	rLast = aRefAdaptorCurve.LastParameter();

	gp_Pnt D;

	for (int i = 0; i < nbSect; ++i)
	{	
		//get value for param
		//will calculate from law

		aDefCurve->D0(d, D);
		double Tol = 1.0e-10;
		Extrema_ExtPElC aCalcD = Extrema_ExtPElC(D, aRefLin, Tol, rFirst, rLast);
		int nbDMin = aCalcD.NbExt();
		nbDMin++;
		int k = 1;
		while(k < nbDMin && !aCalcD.IsMin(k))
		{
			k++;
		}
		double ii = aCalcD.SquareDistance(k);
		ii = sqrt(ii);
		result.push_back(ii);

		d+= (coeffReverse*dStep);
	}

	return result;
}