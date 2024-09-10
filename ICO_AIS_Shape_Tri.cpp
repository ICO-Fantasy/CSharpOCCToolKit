#include <BRepPrimAPI_MakeCone.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <Bnd_Box.hxx>
#include <Geom_Axis2Placement.hxx>
#include <BRepBndLib.hxx>
#include <AIS_Trihedron.hxx>
#include "ICO_AIS_Shape_Tri.h"
#include "ICO_Color.h"

static double calculateArrowLength(const TopoDS_Shape theShape) {
	Bnd_Box b;
	BRepBndLib::Add(theShape, b);
	double xMin, yMin, zMin, xMax, yMax, zMax;
	b.Get(xMin, yMin, zMin, xMax, yMax, zMax);
	double dx = xMax - xMin;
	double 	dy = yMax - yMin;
	double 	dz = zMax - zMin;
	return std::max({ dx, dy, dz }) * 1.3;
}

namespace OCCTK {
namespace Extension {

//AShapeTri::AShapeTri(OCC::AIS::AShape^ theAIS) {
//	myAISTri() = new AIS_MultipleConnectedInteractive();
//	myAISObject() = myAISTri();
//	nativeHandle() = myAISTri();
//	myAIS() = theAIS->GetOCC();
//
//	myAISTri()->Connect(myAIS());
//
//	gp_Ax2 ax2 = gp_Ax2();
//	ax2.Transform(theAIS->GetOCC()->LocalTransformation());
//	Handle(Geom_Axis2Placement) gax2 = new Geom_Axis2Placement(ax2);
//	myAISTri()->Connect(new AIS_Trihedron(gax2));
//
//	//myLen = calculateArrowLength(myAIS()->Shape());
//	//if (myLen == 0.0) { return; }
//	//MakeArrow(gp_Dir(1, 0, 0), myLen, (gcnew Color())->XAxis);
//	//MakeArrow(gp_Dir(0, 1, 0), myLen, (gcnew Color())->YAxis);
//	//MakeArrow(gp_Dir(0, 0, 1), myLen, (gcnew Color())->ZAxis);
//}

AShapeTri::AShapeTri(OCC::TopoDS::TShape^ theShape) :InteractiveObject(Handle(AIS_Shape)()) {
	myAISTri() = new AIS_MultipleConnectedInteractive();

	// 确保使用正确的 Handle
	InteractiveObject::myAISObject() = myAISTri();
	BaseObject::myHandle() = myAISTri();

	//绑定AIS对象
	myAIS() = new AIS_Shape(theShape->GetOCC());
	myAISTri()->Connect(myAIS());

	//绑定坐标轴
	gp_Ax2 ax2 = gp_Ax2();
	ax2.Transform(theShape->GetOCC().Location().Transformation());
	myTri() = new AIS_Trihedron(new Geom_Axis2Placement(ax2));
	myTri()->SetWidth(1.0);
	myTri()->SetDisplayMode(AIS_Shaded);
	myAISTri()->Connect(myTri());

	//myLen = calculateArrowLength(myAIS()->Shape());
	//if (myLen == 0.0) { return; }
	//MakeArrow(gp_Dir(1, 0, 0), myLen, (gcnew Color())->XAxis);
	//MakeArrow(gp_Dir(0, 1, 0), myLen, (gcnew Color())->YAxis);
	//MakeArrow(gp_Dir(0, 0, 1), myLen, (gcnew Color())->ZAxis);
}

Handle(AIS_InteractiveObject) AShapeTri::GetOCC() {
	return myAISTri();
}

inline void AShapeTri::MakeArrow(const gp_Dir theDir, const double arrowLength, Quantity_Color theColor) {
	gp_Pnt sP = gp_Pnt();
	gp_Dir D = theDir.Transformed(myAIS()->LocalTransformation());
	sP.Transform(myAIS()->LocalTransformation());
	double arrowAngle = 18.0;
	double coneScale = 2;
	double arrowRadius = arrowLength / 60.0;
	double bottomRadius = arrowRadius * coneScale;
	double arrowHigh = bottomRadius / std::tan(arrowAngle / 180.0 * M_PI);
	gp_Pnt cone_start = sP.Translated(gp_Vec(D).Scaled(arrowLength - arrowHigh));
	gp_Ax2 axis = gp_Ax2(cone_start, D);
	TopoDS_Shape cone = BRepPrimAPI_MakeCone(axis, bottomRadius, 0.0, arrowHigh).Shape();
	Handle(AIS_Shape) coneAIS = new AIS_Shape(cone);
	coneAIS->SetColor(theColor);
	axis = gp_Ax2(sP, D);
	TopoDS_Shape cylinder = BRepPrimAPI_MakeCylinder(axis, arrowRadius, arrowLength - arrowHigh).Shape();
	Handle(AIS_Shape) cylinderAIS = new AIS_Shape(cylinder);
	cylinderAIS->SetColor(theColor);
	myAISTri()->Connect(coneAIS);
	myAISTri()->Connect(cylinderAIS);
}



}
}