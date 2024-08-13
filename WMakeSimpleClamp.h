#pragma once
#include "MakeSimpleClamp.h"
#include "Wgp_Pnt.h"
#include "Wgp_Dir.h"
#include "WTopoDS_Shape.h"
#include "WAIS_Shape.h"
#include "StringExchange.h"
//包装C++类到托管类
#include <NCollection_Haft.h> 

using namespace System::Collections::Generic;
using namespace OCCTK;
using namespace OCCTK::OCC::AIS;
using namespace OCCTK::OCC::gp;
using namespace OCCTK::OCC::TopoDS;

namespace OCCTK {
namespace Laser {
#pragma region 结构体

public ref struct PlatePose {
	PlatePose(SimpleClamp::PlatePose theOCCPose) { myDir() = theOCCPose; }
	PlatePose(Wgp_Pnt^ thePoint, Wgp_Dir^ theDir) { myDir() = SimpleClamp::PlatePose(*thePoint->GetOCC(), *theDir->GetOCC()); }
	property Wgp_Pnt^ Point {Wgp_Pnt^ get() { return gcnew Wgp_Pnt(myDir().point); }};
	property Wgp_Dir^ Dir { Wgp_Dir^ get() { return  gcnew Wgp_Dir(myDir().dir); } };
public:
	SimpleClamp::PlatePose GetOCC() { return myDir(); };
private:
	NCollection_Haft<SimpleClamp::PlatePose> myDir;
};

public ref struct BasePlate {
	BasePlate(SimpleClamp::BasePlate theBasePlate) { myBP() = theBasePlate; }
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
	SimpleClamp::BasePlate GetOCC() { return myBP(); };
private:
	NCollection_Haft<SimpleClamp::BasePlate> myBP;
	WAIS_Shape^ myAIS;
};

public ref struct VerticalPiece {
	VerticalPiece(OCCTK::SimpleClamp::VerticalPiece theOCCPiece) {
		myPiece() = theOCCPiece;
	}
public:
	int NumberString = -1;
	property WTopoDS_Shape^ Shape {WTopoDS_Shape^ get() { return gcnew WTopoDS_Shape(myPiece().Shape()); }}
	property WAIS_Shape^ AIS { WAIS_Shape^ get() { if (myAIS == nullptr) { myAIS = gcnew WAIS_Shape(Shape); } return myAIS; }}
	property PlatePose^ Pose {PlatePose^ get() { return gcnew PlatePose(myPiece().pose); }}
	property double Order {double get() { return myPiece().order; }}
	OCCTK::SimpleClamp::VerticalPiece GetOCC() { return myPiece(); };

	virtual System::String^ ToString() override { return NumberString.ToString(); }
private:
	NCollection_Haft<OCCTK::SimpleClamp::VerticalPiece> myPiece;
	WAIS_Shape^ myAIS;
};

public ref struct VerticalPlate {
	VerticalPlate(OCCTK::SimpleClamp::VerticalPlate theOCCPlate) {
		myPlate() = theOCCPlate;
		for (size_t i = 0; i < myPlate().pieces.size(); ++i) {
			Pieces->Add(gcnew VerticalPiece(myPlate().pieces[i]));
		}
	}
	// 方向
	property PlatePose^ Pose {PlatePose^ get() { return gcnew PlatePose(myPlate().pose); }}
	// 避让间隙
	property double Clearances { double get() { return myPlate().clearances; } };
	// 最小支撑长度
	property double MinSupportLen { double get() { return myPlate().minSupportLen; } };
	// 切断距离
	property double CuttingDistance { double get() { return myPlate().cuttingDistance; } };
	// 避让高度
	property double AvoidanceHeight { double get() { return myPlate().avoidanceHeight; } };
	// 连接槽宽度
	property double ConnectionThickness { double get() { return myPlate().connectionThickness; } };
	// 开槽圆角半径
	property double FilletRadius { double get() { return myPlate().filletRadius; } };
	// 标签编号
	property String^ NumberString { String^ get() { return gcnew String(myPlate().numberString.ToCString()); }void set(String^ value) { myPlate().numberString = DataExchange::ToAsciiString(value); }};
	// 最终的竖板形状
	property WAIS_Shape^ AIS {WAIS_Shape^ get() { if (sutured) { return myAIS; } return nullptr; }};
	// 用于判断是否已连接
	property bool Sutured {bool get() { return sutured; } };
public:
	List<VerticalPiece^>^ Pieces = gcnew List<VerticalPiece^>();
	OCCTK::SimpleClamp::VerticalPlate GetOCC() { return myPlate(); };
	// 添加 ToString 方法
	virtual System::String^ ToString() override {
		return NumberString;
	}
private:
	NCollection_Haft<SimpleClamp::VerticalPlate> myPlate;
internal:
	bool sutured = false;
	WTopoDS_Shape^ myShape;
	WAIS_Shape^ myAIS;
};

#pragma endregion

public ref class SimpleClampMaker {
public:
	static BasePlate^ MakeBasePlate_NoInteract(WTopoDS_Shape^ InputWorkpiece, double OffsetZ, double BasePlateOffsetX, double BasePlateOffsetY);

	static VerticalPlate^ MakeVerticalPlate(WTopoDS_Shape^ InputWorkpiece, BasePlate^ BasePlate, PlatePose^ theDirection, double theClearances, double theMinSupportLen, double theCuttingDistance);

	static VerticalPlate^ SuturePLate(VerticalPlate^ theVerticalPlate, BasePlate^ BasePlate, double theConnectHight, double theConnectThickness);
	static cli::array<List<VerticalPlate^>^>^ ConnectVerticalPLates(List<VerticalPlate^>^ toDownPlates, List<VerticalPlate^>^ toUpPlates, BasePlate^ BasePlate, double theConnectHight, double theConnectThickness, double theFilletRadius);
	static BasePlate^ SlotBasePLates(BasePlate^ BasePlate, List<VerticalPlate^>^ toDownPlates, List<VerticalPlate^>^ toUpPlates);

	static VerticalPlate^ BrandNumber(VerticalPlate^ theVerticalPlate, double hight);
	//static void BrandNumber(VerticalPlate^% theVerticalPlate, double hight, int number, Wgp_Pnt^ thePoint);

	static WTopoDS_Shape^ DeployPlates(BasePlate^ BasePlate, List<VerticalPlate^>^ MiddleToDownPlates, List<VerticalPlate^>^ MiddleToUpPlatesates);
};

}
}

