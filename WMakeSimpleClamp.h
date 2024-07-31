#pragma once
#include "MakeSimpleClamp.h"
#include "WTopoDS_Shape.h"
#include "Wgp_Pnt.h"
#include "Wgp_Dir.h"
#include "WAIS_Shape.h"
//包装C++类到托管类
#include <NCollection_Haft.h> 

using namespace System::Collections::Generic;
using namespace OCCTK;
using namespace OCCTK::OCC::AIS;
using namespace OCCTK::OCC::gp;
using namespace OCCTK::OCC::TopoDS;

namespace OCCTK {
namespace Laser {

public ref struct PlatePose {
	PlatePose(SimpleClamp::PlatePose theOCCPose) { myDir() = theOCCPose; }
	PlatePose(Wgp_Pnt^ thePoint, Wgp_Dir^ theDir) { myDir() = SimpleClamp::PlatePose(*thePoint->GetOCC(), *theDir->GetOCC()); }
	property Wgp_Pnt^ Point {Wgp_Pnt^ get() { return gcnew Wgp_Pnt(myDir().point); }};
	property Wgp_Dir^ Dir { Wgp_Dir^ get() { return  gcnew Wgp_Dir(myDir().pose); } };
public:
	SimpleClamp::PlatePose GetOCC() { return myDir(); };
private:
	NCollection_Haft<SimpleClamp::PlatePose> myDir;
};

public ref struct BasePlate {
	BasePlate(OCCTK::SimpleClamp::BasePlate theBasePlate) { myBP() = theBasePlate; }
	property WTopoDS_Shape^ Shape {WTopoDS_Shape^ get() { return gcnew WTopoDS_Shape(myBP().shape); }};
	property double X {double get() { return myBP().X; }};
	property double Y {double get() { return myBP().Y; }};
	property double Z {double get() { return myBP().Z; }};
	property double DX {double get() { return myBP().dX; }};
	property double DY {double get() { return myBP().dY; }};
	property double OffsetX {double get() { return myBP().offsetX; }};
	property double OffsetY {double get() { return myBP().offsetY; }};
	property WAIS_Shape^ AIS {WAIS_Shape^ get() { if (myAIS == nullptr) { myAIS = gcnew WAIS_Shape(Shape); } return myAIS; }};
public:
	OCCTK::SimpleClamp::BasePlate GetOCC() { return myBP(); };
private:
	NCollection_Haft<OCCTK::SimpleClamp::BasePlate> myBP;
	WAIS_Shape^ myAIS;
};

public ref struct VerticalPiece {
	VerticalPiece(OCCTK::SimpleClamp::VerticalPiece theOCCPiece) {
		myPiece() = theOCCPiece;
	}
public:
	int Number = -1;
	property WTopoDS_Shape^ Shape {WTopoDS_Shape^ get() { return gcnew WTopoDS_Shape(myPiece().Shape()); }};
	property WAIS_Shape^ AIS { WAIS_Shape^ get() { if (myAIS == nullptr) { myAIS = gcnew WAIS_Shape(Shape); } return myAIS; }	}
	OCCTK::SimpleClamp::VerticalPiece GetOCC() { return myPiece(); };

	virtual System::String^ ToString() override { return Number.ToString(); }
private:
	NCollection_Haft<OCCTK::SimpleClamp::VerticalPiece> myPiece;
	WAIS_Shape^ myAIS;
};

public ref struct VerticalPlate {
	VerticalPlate(OCCTK::SimpleClamp::VerticalPlate theOCCPlate) {
		myPlate = &theOCCPlate;
		for (size_t i = 0; i < myPlate->pieces.size(); ++i) {
			Pieces->Add(gcnew VerticalPiece(myPlate->pieces[i]));
		}
	}
	// 方向
	property PlatePose^ Direction {PlatePose^ get() { return gcnew PlatePose(myPlate->pose); }}
	// 避让间隙
	property double Clearances { double get() { return myPlate->clearances; } };
	// 最小支撑长度
	property double MinSupportLen { double get() { return myPlate->minSupportLen; } };
	// 切断距离
	property double CuttingDistance { double get() { return myPlate->cuttingDistance; } };
	// 连接高
	property double ConnectionHeight {void set(double theValue) { myConnectionHeight = theValue; } double get() { return myConnectionHeight; } };
	// 连接槽宽度
	property double ConnectionThickness {void set(double theValue) { myConnectionThickness = theValue; } double get() { return myConnectionThickness; } };
	// 开槽圆角半径
	property double FilletRadius {void set(double theValue) { myFilletRadius = theValue; } double get() { return myFilletRadius; } };
	// 标签编号
	property int Number { void set(int theNum) { myPlate->number = theNum; } int get() { return myPlate->number; } };
	// 最终的竖板形状
	property WAIS_Shape^ AIS {WAIS_Shape^ get() { if (sutured) { return myAIS; } return nullptr; }};
public:
	List<VerticalPiece^>^ Pieces = gcnew List<VerticalPiece^>();
	OCCTK::SimpleClamp::VerticalPlate GetOCC() { return *myPlate; };
	void SuturePLate();
	// 添加 ToString 方法，默认返回1位小数
	virtual System::String^ ToString() override {
		return Number.ToString();
	}
private:
	OCCTK::SimpleClamp::VerticalPlate* myPlate;
	double myConnectionHeight;
	double myConnectionThickness;
	double myFilletRadius;
	bool sutured = false;
	WTopoDS_Shape myShape;
	WAIS_Shape^ myAIS;
};

public ref class SimpleClampMaker {
public:
	static BasePlate^ MakeBasePlate_NoInteract(WTopoDS_Shape^ InputWorkpiece, double OffsetZ, double BasePlateOffsetX, double BasePlateOffsetY);
	static VerticalPlate^ MakeVerticalPlate(WTopoDS_Shape^ InputWorkpiece, BasePlate^ BasePlate, PlatePose^ theDirection, double theClearances, double theMinSupportLen, double theCuttingDistance);

	//static void SuturePLate(VerticalPlate^% theVerticalPlate, BasePlate^ BasePlate, double theConnectHight);
	//static List<VerticalPlate^>^ SuturePLates(List<VerticalPlate^>^ verticalPlates, BasePlate^ BasePlate, double theConnectHight, double theConnectThickness, double theFilletRadius);
	//static WTopoDS_Shape^ DeployPlates(List<VerticalPlate^>^ verticalPlates, BasePlate^ BasePlate);
	//static void BrandNumber(VerticalPlate^% theVerticalPlate, double hight, int number);
	//static void BrandNumber(VerticalPlate^% theVerticalPlate, double hight, int number, Wgp_Pnt^ thePoint);

};

}
}

