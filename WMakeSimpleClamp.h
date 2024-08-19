﻿#pragma once
#include "MakeSimpleClamp.h"
#include "ICO_Pnt.h"
#include "ICO_Dir.h"
#include "ICO_TopoDS_Shape.h"
#include "ICO_AIS_Shape.h"
#include "ICO_StringExchange.h"
//包装C++类到托管类
#include <NCollection_Haft.h> 

using namespace System::Collections::Generic;
using namespace OCCTK;
using namespace OCCTK::OCC::gp;
typedef OCCTK::OCC::AIS::Shape AShape;
typedef OCCTK::OCC::TopoDS::Shape TShape;

namespace OCCTK {
namespace Laser {
#pragma region 结构体

public ref struct PlatePose {
	PlatePose(SimpleClamp::PlatePose theOCCPose) { myDir() = theOCCPose; }
	PlatePose(Pnt^ thePoint, Dir^ theDir) { myDir() = SimpleClamp::PlatePose(thePoint->GetOCC(), theDir->GetOCC()); }
	property Pnt^ Location {Pnt^ get() { return gcnew Pnt(myDir().point); }};
	property Dir^ Direction { Dir^ get() { return  gcnew Dir(myDir().dir); } };
public:
	SimpleClamp::PlatePose GetOCC() { return myDir(); };
private:
	NCollection_Haft<SimpleClamp::PlatePose> myDir;
};

public ref struct BasePlate {
	BasePlate(SimpleClamp::BasePlate theBasePlate) { myBP() = theBasePlate; }
	property TShape^ Shape {TShape^ get() { return gcnew TShape(myBP().shape); }};
	property double X {double get() { return myBP().X; }};
	property double Y {double get() { return myBP().Y; }};
	property double Z {double get() { return myBP().Z; }};
	property double DX {double get() { return myBP().dX; }};
	property double DY {double get() { return myBP().dY; }};
	property double OffsetX {double get() { return myBP().offsetX; }void set(double value) { myBP().offsetX = value; }};
	property double OffsetY {double get() { return myBP().offsetY; }void set(double value) { myBP().offsetY = value; }};
	property AShape^ AIS {AShape^ get() { if (myAIS == nullptr) { myAIS = gcnew AShape(Shape); } return myAIS; }};
public:
	SimpleClamp::BasePlate GetOCC() { return myBP(); };
private:
	NCollection_Haft<SimpleClamp::BasePlate> myBP;
	AShape^ myAIS;
};

public ref struct VerticalPiece {
	VerticalPiece(OCCTK::SimpleClamp::VerticalPiece theOCCPiece) {
		myPiece() = theOCCPiece;
	}
public:
	int NumberString = -1;
	property TShape^ Shape {TShape^ get() { return gcnew TShape(myPiece().Shape()); }}
	property AShape^ AIS { AShape^ get() { if (myAIS == nullptr) { myAIS = gcnew AShape(Shape); } return myAIS; }}
	property PlatePose^ Pose {PlatePose^ get() { return gcnew PlatePose(myPiece().pose); }}
	property double Order {double get() { return myPiece().order; }}
	OCCTK::SimpleClamp::VerticalPiece GetOCC() { return myPiece(); };

	virtual System::String^ ToString() override { return Order.ToString(); }
private:
	NCollection_Haft<OCCTK::SimpleClamp::VerticalPiece> myPiece;
	AShape^ myAIS;
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
	property String^ NumberString { String^ get() { return gcnew String(myPlate().numberString.ToCString()); }void set(String^ value) {
		myPlate().numberString = DataExchange::ToAsciiString(value);
	}};
	// 最终的竖板形状
	property AShape^ AIS {AShape^ get() { if (sutured) { return myAIS; } return nullptr; }};
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
	TShape^ myShape;
	AShape^ myAIS;
};

#pragma endregion

public ref class SimpleClampMaker {
public:
	static BasePlate^ MakeBasePlate_NoInteract(TShape^ InputWorkpiece, double OffsetZ, double BasePlateOffsetX, double BasePlateOffsetY);

	static VerticalPlate^ MakeVerticalPlate(TShape^ InputWorkpiece, BasePlate^ BasePlate, PlatePose^ theDirection, double theClearances, double theMinSupportLen, double theCuttingDistance);

	static VerticalPlate^ SuturePLate(VerticalPlate^ theVerticalPlate, BasePlate^ BasePlate, double theConnectHight, double theConnectThickness);

	static cli::array<List<VerticalPlate^>^>^ ConnectVerticalPLates(List<VerticalPlate^>^ toDownPlates, List<VerticalPlate^>^ toUpPlates, BasePlate^ BasePlate, double theConnectHight, double theConnectThickness, double theFilletRadius);

	static BasePlate^ SlotBasePLates(BasePlate^ BasePlate, List<VerticalPlate^>^ toDownPlates, List<VerticalPlate^>^ toUpPlates);

	static VerticalPlate^ BrandNumberVerticalPlate(VerticalPlate^ theVerticalPlate, double hight);

	static BasePlate^ BrandNumberBasePlate(BasePlate^ theBasePlate, double hight);

	//static void BrandNumber(VerticalPlate^% theVerticalPlate, double hight, int number, Wgp_Pnt^ thePoint);

	static TShape^ DeployPlates(BasePlate^ BasePlate, List<VerticalPlate^>^ MiddleToDownPlates, List<VerticalPlate^>^ MiddleToUpPlatesates);
};

}
}

