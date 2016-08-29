// Copyright

// File:        GeomAlgoAPI_PipeApprox.cpp


#include "GeomAlgoAPI_PipeApprox.h"

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

#include "GeomConvert_ApproxCurve.hxx"
#include "Geom_BSplineCurve.hxx"
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
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
#include <Adaptor3d_Curve.hxx>
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

#include <TopAbs_Orientation.hxx>

//the define are only for the pipe not using ParamLaws
#define THRUSECT
#define NB_SECT 50

//based on the GeomAlgoAPI_Revolution

/// \return solid created from face or shell.
static TopoDS_Solid makeSolidFromShape(const TopoDS_Shape& theShape);

/// to make wire from a face after transformation, to use in thruSections
static TopoDS_Wire makeWireFromTransf(const TopoDS_Shape &theSourceShape,
	gp_Trsf  theTrsf);

//=================================================================================================
// Constructors
//=================================================================================================
//=================================================================================================
GeomAlgoAPI_PipeApprox::GeomAlgoAPI_PipeApprox()
{
}

//=================================================================================================
GeomAlgoAPI_PipeApprox::GeomAlgoAPI_PipeApprox(std::shared_ptr<GeomAPI_Shape> theProfileShape,
	std::shared_ptr<GeomAPI_Shape>   thePathShape)
{
	build(theProfileShape, thePathShape);
}

//=================================================================================================
GeomAlgoAPI_PipeApprox::GeomAlgoAPI_PipeApprox(std::shared_ptr<GeomAPI_Shape> theProfileShape,
	std::shared_ptr<GeomAPI_Shape>   thePathShape,
	std::shared_ptr<GeomAPI_Shape>  theBiNormal)
{
	build(theProfileShape, thePathShape, theBiNormal);
}

//=================================================================================================
GeomAlgoAPI_PipeApprox::GeomAlgoAPI_PipeApprox(std::shared_ptr<GeomAPI_Shape> theProfileShape,
	std::shared_ptr<GeomAPI_Shape>   thePathShape,
	std::shared_ptr<GeomAPI_Shape>   theDef,
	std::shared_ptr<GeomAPI_Shape>   theRef)
{
	build(theProfileShape, thePathShape, theDef, theRef);
}


//=================================================================================================
GeomAlgoAPI_PipeApprox::GeomAlgoAPI_PipeApprox(ListOfShape theProfileShapeList,
	std::shared_ptr<GeomAPI_Shape>   thePath,
	bool isReverse,
	int nbSect,
	bool withFrenet)
{
	build(theProfileShapeList, thePath, isReverse, nbSect, withFrenet);
}

//=================================================================================================
GeomAlgoAPI_PipeApprox::GeomAlgoAPI_PipeApprox(ListOfShape theProfileShapeList,
	std::shared_ptr<GeomAPI_Shape>   thePath,
	std::shared_ptr<GeomAPI_Shape>   theBinormal,
	bool isReverse,
	int nbSect)
{
	build(theProfileShapeList, thePath, theBinormal, isReverse, nbSect);
}


//=================================================================================================
// BUILD() when doing simple pipe with ThruSect
//=================================================================================================

//=================================================================================================
void GeomAlgoAPI_PipeApprox::build(const std::shared_ptr<GeomAPI_Shape>& theProfileShape,
	const std::shared_ptr<GeomAPI_Shape>&   thePathShape)
{
	if(!theProfileShape || !thePathShape) {
		return;
	}

	// Getting profile.
	TopoDS_Face aProfileFace = TopoDS::Face(theProfileShape->impl<TopoDS_Shape>());

	// Getting path
	const TopoDS_Shape& aPathShape = thePathShape->impl<TopoDS_Shape>();
	TopoDS_Edge aPathEdge;
	if(thePathShape->shapeType() == GeomAPI_Shape::EDGE) {
		aPathEdge = TopoDS::Edge(thePathShape->impl<TopoDS_Shape>());
	}

	//COMPUTE RESULT
	TopoDS_Shape aResult;
	double u, aFirst, aLast;

	//recover curve
	aPathEdge.Oriented(TopAbs_FORWARD );
	Handle(Geom_Curve) aPathCurve = BRep_Tool::Curve(aPathEdge, aFirst, aLast);

	u = aFirst;
	const double uStep = (aLast - aFirst) / (NB_SECT - 1);

#ifdef THRUSECT
	const Standard_Boolean isSolid = Standard_True;
	const Standard_Boolean isRuled = Standard_False;
	const Standard_Real    pres3d = 1.0e-02;

	//building thrusection algo)
	BRepOffsetAPI_ThruSections* aGenerator = new BRepOffsetAPI_ThruSections(isSolid, isRuled, pres3d);
	if(!aGenerator) {
		return;
	}
	aGenerator->SetSmoothing(Standard_True);
	aGenerator->SetMaxDegree(5);
	aGenerator->CheckCompatibility(Standard_False);
#else
	std::shared_ptr<GeomAPI_Shape> aCompoundResult;
	std::list<std::shared_ptr<GeomAPI_Shape> > aCompoundResultList;
#endif

	//iterating on sections
	TopoDS_Wire aWire;
	gp_Trsf aTrsf = gp_Trsf();
	gp_Pnt P;
	gp_Vec T, N, Vx;

	aPathCurve->D2(u, P, T, N);
	Vx = T ^ N; //Vx should be in the plane of (P,D1,D2) so that (P,D1,Vx) is orthogonal
	Vx = Vx ^ T;

	gp_Ax3 gp_Ax3_0(P, T, Vx);

	for (int i = 0; i < NB_SECT; ++i)
	{	
		u += uStep;
		aPathCurve->D2(u, P, T, N);
		Vx = T ^ N;
		Vx = Vx ^ T;

		aTrsf.SetDisplacement(gp_Ax3_0, gp_Ax3(P, T, Vx));

		aWire = makeWireFromTransf(aProfileFace, aTrsf);


		// Add section to skinner
#ifdef THRUSECT
		aGenerator->AddWire(aWire);
#else
		std::shared_ptr<GeomAPI_Shape> aForCompoundShape(new GeomAPI_Shape);
		aForCompoundShape->setImpl(new TopoDS_Shape(aWire));
		aCompoundResultList.push_back(aForCompoundShape);
#endif
	}
#ifdef THRUSECT
	aGenerator->Build();

	if (!aGenerator->IsDone()){
		return;
	}

	aResult = aGenerator->Shape();

	// Setting naming.
	// JE SAIS PAS
	std::shared_ptr<GeomAPI_Shape> aFromShape(new GeomAPI_Shape), aToShape(new GeomAPI_Shape);
	aFromShape->setImpl(new TopoDS_Shape(aGenerator->FirstShape()));
	aToShape->setImpl(new TopoDS_Shape(aGenerator->LastShape()));
	this->addFromShape(aFromShape);
	this->addToShape(aToShape);

	// Setting result.
	if(aResult.IsNull()) {
		return;
	}
	std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
	aShape->setImpl(new TopoDS_Shape(aResult));
	this->setShape(aShape);
	this->setDone(true);
#else
	aCompoundResult = GeomAlgoAPI_CompoundBuilder::compound(aCompoundResultList);
	this->setShape(aCompoundResult);
	this->setDone(true);
#endif
}

//=================================================================================================
void GeomAlgoAPI_PipeApprox::build(const std::shared_ptr<GeomAPI_Shape>& theProfileShape,
	const std::shared_ptr<GeomAPI_Shape>&   thePathShape,
	const std::shared_ptr<GeomAPI_Shape>&   theBiNormal)
{
	if(!theProfileShape || !thePathShape || !theBiNormal) {
		return;
	}

	// Getting profile.
	TopoDS_Face aProfileFace = TopoDS::Face(theProfileShape->impl<TopoDS_Shape>());

	// Getting path

	const TopoDS_Shape& aPathShape = thePathShape->impl<TopoDS_Shape>();
	TopoDS_Edge aPathEdge;
	if(thePathShape->shapeType() == GeomAPI_Shape::EDGE) {
		aPathEdge = TopoDS::Edge(thePathShape->impl<TopoDS_Shape>());
	}

	// Getting binormal

	TopoDS_Shape aBiNormalShape = theBiNormal->impl<TopoDS_Shape>();

	TopoDS_Edge aBiNormalEdge = TopoDS::Edge(aBiNormalShape);
	double aFirst, aLast;
	Handle(Geom_Curve) aBiNormalCurve = BRep_Tool::Curve(aBiNormalEdge, aFirst, aLast);
	Handle(Geom_Line) aBiNormalLine = Handle(Geom_Line)::DownCast(aBiNormalCurve);

	gp_Dir aBiNormalDir = aBiNormalLine->Lin().Direction();


	//COMPUTE RESULT
	TopoDS_Shape aResult;

	double u;
	//recover curve
	aPathEdge.Oriented(TopAbs_FORWARD );

	Handle(Geom_Curve) aPathCurve = BRep_Tool::Curve(aPathEdge, aFirst, aLast);

	u = aFirst;
	const double uStep = (aLast - aFirst) / (NB_SECT - 1);

#ifdef THRUSECT
	const Standard_Boolean isSolid = Standard_True;
	const Standard_Boolean isRuled = Standard_False;
	const Standard_Real    pres3d = 1.0e-02;

	//building thrusection algo)
	BRepOffsetAPI_ThruSections* aGenerator = new BRepOffsetAPI_ThruSections(isSolid, isRuled, pres3d);
	if(!aGenerator) {
		return;
	}
	aGenerator->SetSmoothing(Standard_True);
	aGenerator->SetMaxDegree(5);
	aGenerator->CheckCompatibility(Standard_False);
#else
	std::shared_ptr<GeomAPI_Shape> aCompoundResult;
	std::list<std::shared_ptr<GeomAPI_Shape> > aCompoundResultList;
#endif

	//iterating on sections
	TopoDS_Wire aWire;
	gp_Trsf aTrsf = gp_Trsf();
	gp_Pnt P;
	gp_Vec T, Vx;

	aPathCurve->D1(u, P, T);
	//Vx should be in the plane of (P,D1,D2) so that (P,D1,Vx) is orthogonal
	//if D2 undefined, replace it with "binormal"
	Vx = T ^ aBiNormalDir;
	Vx = Vx ^ T;

	gp_Ax3 gp_Ax3_0(P, T, Vx);

	for (int i = 0; i < NB_SECT; ++i)
	{	
		u += uStep;
		aPathCurve->D1(u, P, T);
		Vx = T ^ aBiNormalDir;
		Vx = Vx ^ T;

		aTrsf.SetDisplacement(gp_Ax3_0, gp_Ax3(P, T, Vx));

		aWire = makeWireFromTransf(aProfileFace, aTrsf);


		// Add section to skinner
#ifdef THRUSECT
		aGenerator->AddWire(aWire);
#else
		std::shared_ptr<GeomAPI_Shape> aForCompoundShape(new GeomAPI_Shape);
		aForCompoundShape->setImpl(new TopoDS_Shape(aWire));
		aCompoundResultList.push_back(aForCompoundShape);
#endif
	}
#ifdef THRUSECT
	aGenerator->Build();

	if (!aGenerator->IsDone()){
		return;
	}

	aResult = aGenerator->Shape();

	// Setting naming.
	// JE SAIS PAS
	std::shared_ptr<GeomAPI_Shape> aFromShape(new GeomAPI_Shape), aToShape(new GeomAPI_Shape);
	aFromShape->setImpl(new TopoDS_Shape(aGenerator->FirstShape()));
	aToShape->setImpl(new TopoDS_Shape(aGenerator->LastShape()));
	this->addFromShape(aFromShape);
	this->addToShape(aToShape);

	// Setting result.
	if(aResult.IsNull()) {
		return;
	}
	std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
	aShape->setImpl(new TopoDS_Shape(aResult));
	this->setShape(aShape);
	this->setDone(true);
#else
	aCompoundResult = GeomAlgoAPI_CompoundBuilder::compound(aCompoundResultList);
	this->setShape(aCompoundResult);
	this->setDone(true);
#endif
}

//=================================================================================================
// BUILD() when using a law directly defined in the feature (pipeapprox)
//=================================================================================================

//=================================================================================================
void GeomAlgoAPI_PipeApprox::build(const std::shared_ptr<GeomAPI_Shape>& theProfileShape,
	const std::shared_ptr<GeomAPI_Shape>&   thePathShape,
	const std::shared_ptr<GeomAPI_Shape>&   theDef,
	const std::shared_ptr<GeomAPI_Shape>&   theRef)
{
	if(!theProfileShape || !thePathShape || !theDef || !theRef) {
		return;
	}

	// Getting profile.
	TopoDS_Face aProfileFace = TopoDS::Face(theProfileShape->impl<TopoDS_Shape>());

	// Getting path
	//const TopoDS_Shape& aPathShape = thePathShape->impl<TopoDS_Shape>();
	TopoDS_Edge aPathEdge;
	if(thePathShape->shapeType() == GeomAPI_Shape::EDGE) {
		aPathEdge = TopoDS::Edge(thePathShape->impl<TopoDS_Shape>());
	}

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

	//COMPUTE RESULT
	TopoDS_Shape aResult;

	double u, aFirst, aLast;
	double d, dFirst, dLast, rFirst, rLast;
	//recover curve
	aPathEdge.Oriented(TopAbs_FORWARD );
	Handle(Geom_Curve) aPathCurve = BRep_Tool::Curve(aPathEdge, aFirst, aLast);

	u = aFirst;
	const double uStep = (aLast - aFirst) / (NB_SECT - 1);

	//recover Def curve
	Handle(Geom_Curve) aDefCurve = BRep_Tool::Curve(aDefEdge, dFirst, dLast);
	d = dFirst;
	const double dStep = (dLast - dFirst) / (NB_SECT - 1);

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

#ifdef THRUSECT
	const Standard_Boolean isSolid = Standard_True;
	const Standard_Boolean isRuled = Standard_False;
	const Standard_Real    pres3d = 1.0e-02;

	//building thrusection algo)
	BRepOffsetAPI_ThruSections* aGenerator = new BRepOffsetAPI_ThruSections(isSolid, isRuled, pres3d);
	if(!aGenerator) {
		return;
	}
	aGenerator->SetSmoothing(Standard_True);
	aGenerator->SetMaxDegree(5);
	aGenerator->CheckCompatibility(Standard_False);
#else
	std::shared_ptr<GeomAPI_Shape> aCompoundResult;
	std::list<std::shared_ptr<GeomAPI_Shape> > aCompoundResultList;
#endif

	//iterating on sections
	TopoDS_Wire aWire;
	gp_Trsf aTrsf = gp_Trsf();
	gp_Trsf bTrsf = gp_Trsf();
	gp_Pnt P, D, C;
	gp_Vec T, N, Vx;

	aPathCurve->D2(u, P, T, N);
	//Vx should be in the plane of (P,D1,D2) so that (P,D1,Vx) is orthogonal
	Vx = T ^ N;
	Vx = Vx ^ T;

	gp_Ax3 gp_Ax3_0(P, T, Vx);

	//get center for scaling
	GProp_PGProps aCConstr = GProp_PGProps();
	TopExp_Explorer aVertexExp(aProfileFace, TopAbs_VERTEX);
	for (; aVertexExp.More(); aVertexExp.Next()) {
		TopoDS_Vertex aVS = TopoDS::Vertex(aVertexExp.Current());
		gp_Pnt aP = BRep_Tool::Pnt(aVS);
		aCConstr.AddPoint(aP);
	}
	C = aCConstr.CentreOfMass();

	for (int i = 0; i < NB_SECT; ++i)
	{	
		u += uStep;
		aPathCurve->D2(u, P, T, N);
		Vx = T ^ N;
		Vx = Vx ^ T;

		//set scale
		//will calculate from law
		d+= dStep;
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
		//for the scale maybe get init "length" to divide ii by it?

		bTrsf.SetScale(C, ii);
		BRepBuilderAPI_Transform* aBuilder = new BRepBuilderAPI_Transform(aProfileFace, bTrsf, true);

		TopoDS_Shape aScaledPorfileFace = aBuilder->Shape();

		aTrsf.SetDisplacement(gp_Ax3_0, gp_Ax3(P, T, Vx));
		aWire = makeWireFromTransf(aScaledPorfileFace, aTrsf);


		// Add section to skinner
#ifdef THRUSECT
		aGenerator->AddWire(aWire);
#else
		std::shared_ptr<GeomAPI_Shape> aForCompoundShape(new GeomAPI_Shape);
		aForCompoundShape->setImpl(new TopoDS_Shape(aWire));
		aCompoundResultList.push_back(aForCompoundShape);
#endif
	}
#ifdef THRUSECT
	aGenerator->Build();

	if (!aGenerator->IsDone()){
		return;
	}

	aResult = aGenerator->Shape();

	// Setting naming.
	// JE SAIS PAS
	std::shared_ptr<GeomAPI_Shape> aFromShape(new GeomAPI_Shape), aToShape(new GeomAPI_Shape);
	aFromShape->setImpl(new TopoDS_Shape(aGenerator->FirstShape()));
	aToShape->setImpl(new TopoDS_Shape(aGenerator->LastShape()));
	this->addFromShape(aFromShape);
	this->addToShape(aToShape);

	// Setting result.
	if(aResult.IsNull()) {
		return;
	}
	std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
	aShape->setImpl(new TopoDS_Shape(aResult));
	this->setShape(aShape);
	this->setDone(true);
#else
	aCompoundResult = GeomAlgoAPI_CompoundBuilder::compound(aCompoundResultList);
	this->setShape(aCompoundResult);
	this->setDone(true);
#endif
}

//=================================================================================================
// BUILD() when using paramLaws
//=================================================================================================

//=================================================================================================

void GeomAlgoAPI_PipeApprox::build(const ListOfShape& theProfileShapeList,
	const std::shared_ptr<GeomAPI_Shape>&   thePathShape,
	const bool isReverse,
	const int nbSect,
	const bool withFrenet)
{
	if(!theProfileShapeList.size() || !thePathShape) {
		return;
	}

	double coeffReverse = 1.0;
	if(isReverse)
	{
		coeffReverse = -1.0;
	}

	//To COMPUTE the RESULT
	TopoDS_Shape aResult;

	BRepOffsetAPI_ThruSections* aGenerator;

	const Standard_Boolean isSolid = Standard_True;
	const Standard_Boolean isRuled = Standard_False;
	const Standard_Real    pres3d = 1.0e-02;

	aGenerator = new BRepOffsetAPI_ThruSections(isSolid, isRuled, pres3d);
	if(!aGenerator) {
		return;
	}
	aGenerator->SetSmoothing(Standard_True);
	aGenerator->SetMaxDegree(5);
	aGenerator->CheckCompatibility(Standard_False);

	//used for dump
	std::shared_ptr<GeomAPI_Shape> aCompoundResult;
	std::shared_ptr<GeomAPI_Shape> aCompoundPointResult;
	std::list<std::shared_ptr<GeomAPI_Shape> > aCompoundResultList;
	std::list<std::shared_ptr<GeomAPI_Shape> > aCompoundPointList;

	// Getting path
	double u, aFirst, aLast, uStep;
	TopoDS_Edge aPathEdge;
	TopoDS_Wire aPathWire;
	Handle(Geom_Curve) aPathCurve;

	if(thePathShape->shapeType() == GeomAPI_Shape::EDGE) {
		aPathEdge = TopoDS::Edge(thePathShape->impl<TopoDS_Shape>());
		//recover curve

		aPathCurve = BRep_Tool::Curve(aPathEdge, aFirst, aLast);
		if(isReverse)
		{
			double aTemp = aFirst;
			aFirst = aLast;
			aLast = aTemp;
		}

		u = aFirst;
		uStep = abs(aLast - aFirst) / (nbSect - 1);

		//iterating on sections
		TopoDS_Wire aWire;
		gp_Trsf aTrsf = gp_Trsf();
		gp_Pnt P;
		gp_Vec T, N, Vx;
		double Tol = 1.0e-10;

		//Vx should be in the plane of (P,D1,D2) so that (P,D1,Vx) is orthogonal
		aPathCurve->D2(u, P, T, N);
		if(!withFrenet && (N.X() == 0) && (N.Y() == 0) && (N.Z() == 0))
		{
			//no 2nd derivative to help find a normal oriented towards the curvature
			//use random orthogonal vecotr to T
			N.SetX(-T.Y());
			N.SetY(T.X());
			N.SetZ(0.0);

			Vx = N ^ T;
			Vx = T ^ Vx;

		}
		else
		{
			//use standard Frenet

			//need to add error management...
			Vx = N ^ T;
			Vx = T ^ Vx;
			//currently force "crash", which means need for binormal, 
		}	

		gp_Ax3 gp_Ax3_0(P, T, Vx);


		for(ListOfShape::const_reverse_iterator anIter = theProfileShapeList.crbegin(); anIter != theProfileShapeList.crend(); anIter++) {
			const TopoDS_Face aProfile = TopoDS::Face((*anIter)->impl<TopoDS_Shape>());
			if(withFrenet)
			{
				//normal frenet frame
				aPathCurve->D2(u, P, T, N);
				Vx = N ^ T;
				Vx = T ^ Vx;
			}
			else
			{
				//iterative placement of the frames
				//based on  http://webhome.cs.uvic.ca/~blob/courses/305/notes/pdf/ref-frames.pdf
				//might need to be checked
				//process from lines 816-837 is valid (tested and debugged) 
				//and work based on global transformation from start to actual and not iterative
				gp_Vec aux;
				aPathCurve->D1(u, P, aux);
				if (T.IsParallel(aux, Tol))
				{
					if(T.IsOpposite(aux, Tol))
					{
						Vx.Reverse();
					}
				}
				else
				{
					gp_Vec cross(T);
					cross.Cross(aux);
					double alpha = T.AngleWithRef(aux, cross);
					gp_Ax1 axe(gp::Origin(), cross.XYZ());

					//have T and Vx for the new frame
					Vx.Rotate(axe, alpha);
				}
				T = aux;
			}

			aTrsf.SetDisplacement(gp_Ax3_0, gp_Ax3(P, T, Vx));
			aWire = makeWireFromTransf(aProfile, aTrsf);


			aGenerator->AddWire(aWire);

			//for dump
			std::shared_ptr<GeomAPI_Shape> aForCompoundShape(new GeomAPI_Shape);
			aForCompoundShape->setImpl(new TopoDS_Shape(aWire));
			aCompoundResultList.push_back(aForCompoundShape);
			//

			u += (coeffReverse*uStep);
		}
	}else if(thePathShape->shapeType() == GeomAPI_Shape::WIRE){
		aPathWire = TopoDS::Wire(thePathShape->impl<TopoDS_Shape>());
		BRepTools::Write(aPathWire, "C:/Users/occ/Desktop/pathDump.brep");

		Handle(BRepAdaptor_HCompCurve) aPathCompCurve = new (BRepAdaptor_HCompCurve) (aPathWire);

		Standard_Real aTol = Precision::Confusion();
		GeomAbs_Shape aContinuity = GeomAbs_C2 /*highest supported continuity*/;
		Standard_Integer aMaxSeg = 10000, /*max number of spans*/
			aMaxDeg = 9; /*max degree, consistent with settings in Algo*/
		GeomConvert_ApproxCurve anApproxPath (aPathCompCurve, aTol, aContinuity, aMaxSeg, aMaxDeg);
		Handle(Geom_BSplineCurve) aBSplinePath = anApproxPath.Curve();
		BRepBuilderAPI_MakeEdge * aBE = new BRepBuilderAPI_MakeEdge(aBSplinePath);
		TopoDS_Edge aBSE = aBE->Edge();
		BRepTools::Write(aBSE, "C:/Users/occ/Desktop/pathBspline.brep");
		aFirst = aBSplinePath->FirstParameter();
		aLast = aBSplinePath->LastParameter();

		if(isReverse)
		{
			double aTemp = aFirst;
			aFirst = aLast;
			aLast = aTemp;
		}

		u = aFirst;
		uStep = abs(aLast - aFirst) / (nbSect - 1);

		//iterating on sections
		TopoDS_Wire aWire;
		gp_Trsf aTrsf = gp_Trsf();
		gp_Trsf bTrsf = gp_Trsf();
		gp_Pnt P;
		gp_Vec T, N, Vx;
		gp_Ax1 axe0;
		double Tol = 1.0e-10;

		//Vx should be in the plane of (P,D1,D2) so that (P,D1,Vx) is orthogonal
		aBSplinePath->D2(u, P, T, N);

		if(!withFrenet && (N.X() == 0) && (N.Y() == 0) && (N.Z() == 0))
		{
			//no 2nd derivative to help find a normal oriented towards the curvature
			//use random orthogonal vecotr to T
			if(T.X() == 0)
			{
				N.SetX(0.0);
				N.SetY(-T.Z());
				N.SetZ(T.Y());
			}else if (T.Y() == 0)
			{
				N.SetX(-T.Z());
				N.SetY(0.0);
				N.SetZ(T.X());
			}
			else
			{
				N.SetX(-T.Y());
				N.SetY(T.X());
				N.SetZ(0.0);
			}
			//can first derivative be null ? assume not
			Vx = N ^ T;
			axe0 = gp_Ax1(gp::Origin(), Vx.XYZ());
			Vx = T ^ Vx;
		}
		else
		{
			//use standard Frenet
			//no checks to force "crash" if withFrenet, which means need for binormal, to add error
			Vx = N ^ T;
			axe0 = gp_Ax1(gp::Origin(), Vx.XYZ());
			Vx = T ^ Vx;
		}	
		const gp_Ax3 gp_Ax3_0(P, T, Vx);
		const gp_Pnt P0(P);
		const gp_Vec T0(T);
		double alpha = 0.0;

		for(ListOfShape::const_reverse_iterator anIter = theProfileShapeList.crbegin(); anIter != theProfileShapeList.crend(); anIter++) {
			const TopoDS_Face aProfile = TopoDS::Face((*anIter)->impl<TopoDS_Shape>());
			if(withFrenet)
			{
				//normal frenet frame
				aBSplinePath->D2(u, P, T, N);
				Vx = N ^ T;
				Vx = T ^ Vx;

				aTrsf.SetDisplacement(gp_Ax3_0, gp_Ax3(P, T, Vx));
				aWire = makeWireFromTransf(aProfile, aTrsf);
			}
			else
			{//this works better, not iterative anymore but global from initial place to actual
				gp_Pnt Paux;
				gp_Vec Taux;
				aBSplinePath->D1(u, Paux, Taux);

				bTrsf = gp_Trsf();
				if (T0.IsParallel(Taux, Tol))
				{
					if(T0.IsOpposite(Taux, Tol))
					{
						bTrsf.SetMirror(axe0);
					}
				}
				else
				{
					gp_Vec cross(Taux);
					cross.Cross(T);
					double angle = T0.AngleWithRef(Taux, cross);
					bTrsf.SetRotation(axe0, angle);
				}

				gp_Vec VT(Paux.XYZ() - P0.XYZ());
				bTrsf.SetTranslationPart(VT);

				aWire = makeWireFromTransf(aProfile, bTrsf);
			}

			aGenerator->AddWire(aWire);

			//for dump
			std::shared_ptr<GeomAPI_Shape> aForCompoundShape(new GeomAPI_Shape);
			aForCompoundShape->setImpl(new TopoDS_Shape(aWire));
			aCompoundResultList.push_back(aForCompoundShape);

			//ForDump
			BRepBuilderAPI_MakeVertex coucou(P);
			std::shared_ptr<GeomAPI_Shape> aForCompoundPoint(new GeomAPI_Shape);
			aForCompoundPoint->setImpl(new TopoDS_Shape(coucou.Vertex()));
			aCompoundPointList.push_back(aForCompoundPoint);
			//

			u += (coeffReverse*uStep);
		}
	}

	aGenerator->Build();



	aResult = aGenerator->Shape();

	// Setting naming.
	// JE SAIS PAS

	std::shared_ptr<GeomAPI_Shape> aFromShape(new GeomAPI_Shape), aToShape(new GeomAPI_Shape);
	aFromShape->setImpl(new TopoDS_Shape(aGenerator->LastShape()));
	aToShape->setImpl(new TopoDS_Shape(aGenerator->FirstShape()));
	this->addFromShape(aFromShape);
	this->addToShape(aToShape);

	// Setting result.
	if(aResult.IsNull()) {
		return;
	}
	std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
	aShape->setImpl(new TopoDS_Shape(aResult));
	this->setShape(aShape);
	this->setDone(true);

	//dump
	aCompoundResult = GeomAlgoAPI_CompoundBuilder::compound(aCompoundResultList);
	TopoDS_Compound aComp = TopoDS::Compound(aCompoundResult->impl<TopoDS_Shape>());
	BRepTools::Write(aComp, "C:/Users/occ/Desktop/forDump.brep");

	aCompoundPointResult = GeomAlgoAPI_CompoundBuilder::compound(aCompoundPointList);
	TopoDS_Compound aComp2 = TopoDS::Compound(aCompoundResult->impl<TopoDS_Shape>());
	BRepTools::Write(aComp2, "C:/Users/occ/Desktop/forDumpPoint.brep");
}


//=================================================================================================
void GeomAlgoAPI_PipeApprox::build(const ListOfShape& theProfileShapeList,
	const std::shared_ptr<GeomAPI_Shape>&   thePathShape,
	const std::shared_ptr<GeomAPI_Shape>&   theBinormal,
	const bool isReverse,
	const int nbSect)
{
	//METHOD MIGHT NEED TO BE DEBUGGED/MODIFIED BASED ON BUILD WITHOUT BINORMAL
	if(!theProfileShapeList.size() || !thePathShape || !theBinormal) {
		return;
	}

	double coeffReverse = 1.0;
	if(isReverse)
	{
		coeffReverse = -1.0;
	}

	//COMPUTE RESULT
	TopoDS_Shape aResult;

	BRepOffsetAPI_ThruSections* aGenerator; 

	const Standard_Boolean isSolid = Standard_True;
	const Standard_Boolean isRuled = Standard_False;
	const Standard_Real    pres3d = 1.0e-02;

	aGenerator = new BRepOffsetAPI_ThruSections(isSolid, isRuled, pres3d);
	if(!aGenerator) {
		return;
	}
	aGenerator->SetSmoothing(Standard_True);
	aGenerator->SetMaxDegree(5);
	aGenerator->CheckCompatibility(Standard_False);


	// Getting binormal
	TopoDS_Shape aBiNormalShape = theBinormal->impl<TopoDS_Shape>();
	TopoDS_Edge aBiNormalEdge = TopoDS::Edge(aBiNormalShape);
	double aFirst, aLast;

	Handle(Geom_Curve) aBiNormalCurve = BRep_Tool::Curve(aBiNormalEdge, aFirst, aLast);
	Handle(Geom_Line) aBiNormalLine = Handle(Geom_Line)::DownCast(aBiNormalCurve);

	gp_Dir aBiNormalDir = aBiNormalLine->Lin().Direction();

	// Getting path
	double u, uStep;
	TopoDS_Edge aPathEdge;
	TopoDS_Wire aPathWire;
	Handle(Geom_Curve) aPathCurve;

	if(thePathShape->shapeType() == GeomAPI_Shape::EDGE) {
		aPathEdge = TopoDS::Edge(thePathShape->impl<TopoDS_Shape>());


		//recover curve

		aPathCurve = BRep_Tool::Curve(aPathEdge, aFirst, aLast);

		if(isReverse)
		{
			double aTemp = aFirst;
			aFirst = aLast;
			aLast = aTemp;
		}

		u = aFirst;
		const double uStep = abs(aLast - aFirst) / (nbSect - 1);

		//iterating on sections
		TopoDS_Wire aWire;
		gp_Trsf aTrsf = gp_Trsf();
		gp_Pnt P;
		gp_Vec T, Vx;

		aPathCurve->D1(u, P, T);
		//Vx should be in the plane of (P,D1,D2) so that (P,D1,Vx) is orthogonal
		//if D2 undefined, replace it with "binormal"
		Vx = T ^ aBiNormalDir;
		Vx = Vx ^ T;

		gp_Ax3 gp_Ax3_0(P, T, Vx);

		int cpt = nbSect - 1;
		for(ListOfShape::const_reverse_iterator anIter = theProfileShapeList.crbegin(); anIter != theProfileShapeList.crend(); anIter++) {
			const TopoDS_Face aProfile = TopoDS::Face((*anIter)->impl<TopoDS_Shape>());

			aPathCurve->D1(u, P, T);
			Vx = T ^ aBiNormalDir;
			Vx = Vx ^ T;
			aTrsf.SetDisplacement(gp_Ax3_0, gp_Ax3(P, T, Vx));

			aWire = makeWireFromTransf(aProfile, aTrsf);


			aGenerator->AddWire(aWire);

			u += (coeffReverse*uStep);
			cpt--;
		}
	}else if(thePathShape->shapeType() == GeomAPI_Shape::WIRE)
	{
		aPathWire = TopoDS::Wire(thePathShape->impl<TopoDS_Shape>());
		Handle(BRepAdaptor_HCompCurve) aPathCompCurve = new (BRepAdaptor_HCompCurve) (aPathWire);

		Standard_Real aTol = Precision::Confusion();
		GeomAbs_Shape aContinuity = GeomAbs_C2 /*highest supported continuity*/;
		Standard_Integer aMaxSeg = 10000, /*max number of spans*/
			aMaxDeg = 9; /*max degree, consistent with settings in Algo*/
		GeomConvert_ApproxCurve anApproxPath (aPathCompCurve, aTol, aContinuity, aMaxSeg, aMaxDeg);
		Handle(Geom_BSplineCurve) aBSplinePath = anApproxPath.Curve();

		aFirst = aBSplinePath->FirstParameter();
		aLast = aBSplinePath->LastParameter();


		gp_Ax3 gp_Ax3_0;


		if(isReverse)
		{
			double aTemp = aFirst;
			aFirst = aLast;
			aLast = aTemp;
		}

		u = aFirst;
		uStep = abs(aLast-aFirst) / (nbSect - 1);
		TopoDS_Wire aWire;
		gp_Trsf aTrsf = gp_Trsf();
		gp_Pnt P;
		gp_Vec T,Vx;

		aPathCurve->D1(u, P, T);
		Vx = T ^ aBiNormalDir;
		Vx = Vx ^ T;

		gp_Ax3_0 = gp_Ax3(P, T, Vx);
		for(ListOfShape::const_reverse_iterator anIter = theProfileShapeList.crbegin(); anIter != theProfileShapeList.crend(); anIter++) {
			const TopoDS_Face aProfile = TopoDS::Face((*anIter)->impl<TopoDS_Shape>());

			aPathCurve->D1(u, P, T);
			Vx = T ^ aBiNormalDir;
			Vx = Vx ^ T;
			aTrsf.SetDisplacement(gp_Ax3_0, gp_Ax3(P, T, Vx));
			aWire = makeWireFromTransf(aProfile, aTrsf);


			aGenerator->AddWire(aWire);


			u += (coeffReverse*uStep);
		}

	}
	aGenerator->Build();

	if (!aGenerator->IsDone()){
		return;
	}

	aResult = aGenerator->Shape();

	// Setting naming.
	// JE SAIS PAS
	std::shared_ptr<GeomAPI_Shape> aFromShape(new GeomAPI_Shape), aToShape(new GeomAPI_Shape);
	aFromShape->setImpl(new TopoDS_Shape(aGenerator->LastShape()));
	aToShape->setImpl(new TopoDS_Shape(aGenerator->FirstShape()));
	this->addFromShape(aFromShape);
	this->addToShape(aToShape);

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
TopoDS_Wire makeWireFromTransf(const TopoDS_Shape &theSourceShape,
	gp_Trsf  theTrsf)
{
	TopoDS_Wire aResult;
	if(theSourceShape.IsNull()) {
		return aResult;
	}

	BRepBuilderAPI_Transform* aBuilder = new BRepBuilderAPI_Transform(theSourceShape, theTrsf, true);
	if(!aBuilder) {
		return aResult;
	}

	if(aBuilder->IsDone() != Standard_True) {
		return aResult;
	}

	BRepBuilderAPI_MakeWire mkWire;

	TopExp_Explorer aEdgeExp(aBuilder->Shape(), TopAbs_EDGE);
	for (; aEdgeExp.More(); aEdgeExp.Next()) {
		TopoDS_Edge aES = TopoDS::Edge(aEdgeExp.Current());
		mkWire.Add(aES);
	}
	aResult = mkWire.Wire();

	return aResult;
}