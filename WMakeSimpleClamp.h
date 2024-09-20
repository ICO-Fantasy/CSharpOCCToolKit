#pragma once
#include "MakeSimpleClamp.h"
#include "ICO_Pnt.h"
#include "ICO_Dir.h"
#include "ICO_Topo_Shape.h"
#include "ICO_AIS_Shape.h"
#include "ICO_StringExchange.h"
//包装C++类到托管类
#include <NCollection_Haft.h> 

//业务逻辑不会被其它头文件引用，可以使用using namespace
using namespace OCCTK::OCC::gp;
using namespace OCCTK::OCC::Topo;
using namespace OCCTK::OCC::AIS;

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
	property OCC::Topo::TShape^ Shape {OCC::Topo::TShape^ get() { return gcnew OCC::Topo::TShape(myBP().Shape()); }};
	property double X {double get() { return myBP().X; }};
	property double Y {double get() { return myBP().Y; }};
	property double Z {double get() { return myBP().Z; }};
	property double DX {double get() { return myBP().dX; }};
	property double DY {double get() { return myBP().dY; }};
	property double OffsetX {double get() { return myBP().offsetX; } void set(double value) { myBP().offsetX = value; }};
	property double OffsetY {double get() { return myBP().offsetY; } void set(double value) { myBP().offsetY = value; }};
	property OCC::AIS::AShape^ AIS {OCC::AIS::AShape^ get() { if (myAIS == nullptr) { myAIS = gcnew OCC::AIS::AShape(Shape); } return myAIS; }};
public:
	Trsf^ Translation;
	void UpdateAIS() {
		if (myAIS != nullptr) {
			myAIS->RemoveSelf();
		}
		myAIS = gcnew OCC::AIS::AShape(Shape);
	}
	SimpleClamp::BasePlate GetOCC() { return myBP(); };
private:
	NCollection_Haft<SimpleClamp::BasePlate> myBP;
	OCC::AIS::AShape^ myAIS;
};

public ref struct VerticalPiece {
	VerticalPiece(OCCTK::SimpleClamp::VerticalPiece theOCCPiece) {
		myPiece() = theOCCPiece;
	}
public:
	int NumberString = -1;
	property OCC::Topo::TShape^ Shape {OCC::Topo::TShape^ get() { return gcnew OCC::Topo::TShape(myPiece().Shape()); }}
	property OCC::AIS::AShape^ AIS { OCC::AIS::AShape^ get() { if (myAIS == nullptr) { myAIS = gcnew OCC::AIS::AShape(Shape); } return myAIS; }}
	property PlatePose^ Pose {PlatePose^ get() { return gcnew PlatePose(myPiece().pose); }}
	property double Order {double get() { return myPiece().order; }}
	property double Length {double get() { return myPiece().Length(); }}
	property cli::array<double>^ Start {cli::array<double>^ get() { return gcnew cli::array<double>(3) { myPiece().myEdge.start.X(), myPiece().myEdge.start.Y(), myPiece().myEdge.start.Z() }; }}
	property cli::array<double>^ Middle {cli::array<double>^ get() { return gcnew cli::array<double>(3) { myPiece().myEdge.middle.X(), myPiece().myEdge.middle.Y(), myPiece().myEdge.middle.Z() }; }}
	property cli::array<double>^ End {cli::array<double>^ get() { return gcnew cli::array<double>(3) { myPiece().myEdge.end.X(), myPiece().myEdge.end.Y(), myPiece().myEdge.end.Z() }; }}
	OCCTK::SimpleClamp::VerticalPiece GetOCC() { return myPiece(); };

	virtual System::String^ ToString() override { return Order.ToString("F1"); }
private:
	NCollection_Haft<OCCTK::SimpleClamp::VerticalPiece> myPiece;
	OCC::AIS::AShape^ myAIS;
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
	//是否有辅助板
	property bool Auxiliary {bool get() { return myPlate().auxiliary; }}
	//辅助板的高和宽设置
	property double AuxiliaryHight {double get() { return myPlate().auxiliaryHight; }void set(double value) { myPlate().auxiliaryHight = value; }}
	property double AuxiliaryWidth {double get() { return myPlate().auxiliaryWidth; }void set(double value) { myPlate().auxiliaryWidth = value; }}
	// 最终的竖板形状
	//property AShape^ AIS {AShape^ get() { if (myAIS == nullptr) { myAIS = gcnew AShape(myPlate().shape); }return myAIS; }};
	property OCC::AIS::AShape^ AIS {OCC::AIS::AShape^ get() { if (sutured) { return myAIS; } return nullptr; }};
	property OCC::Topo::TShape^ Shape {OCC::Topo::TShape^ get() { if (sutured) { if (numbered) { return myNumberedShape; } else { return myShape; } } return nullptr; }};
	// 是否已连接
	property bool Sutured {bool get() { return sutured; } };
	// 是否已烙印编号
	property bool Numbered {bool get() { return numbered; } };
	// debug
	property Pnt^ Start {Pnt^ get() { return gcnew Pnt(myPlate().start); }};
	property Pnt^ End {Pnt^ get() { return gcnew Pnt(myPlate().end); }};

public:
	List<VerticalPiece^>^ Pieces = gcnew List<VerticalPiece^>();
	void RemovePiece(VerticalPiece^ thePiece) {
		std::vector<SimpleClamp::VerticalPiece>T;
		for each (auto p in  myPlate().pieces) {
			if (p == thePiece->GetOCC()) {
				continue;
			}
			T.push_back(p);
		}
		myPlate().pieces = T;
	}
	SimpleClamp::VerticalPlate GetOCC() { return myPlate(); };
	// 添加 ToString 方法
	virtual System::String^ ToString() override {
		return NumberString;
	}
private:
	NCollection_Haft<SimpleClamp::VerticalPlate> myPlate;
internal:
	bool sutured = false;
	bool numbered = false;
	OCC::Topo::TShape^ myShape;
	OCC::Topo::TShape^ myNumberedShape;
	OCC::AIS::AShape^ myAIS;
};

#pragma endregion

public ref class SimpleClampMaker {
public:
	static BasePlate^ MakeBasePlate_NoInteract(OCC::Topo::TShape^ InputWorkpiece, double OffsetZ, double BasePlateOffsetX, double BasePlateOffsetY);

	static VerticalPlate^ MakeVerticalPlate(OCC::Topo::TShape^ InputWorkpiece, BasePlate^ BasePlate, PlatePose^ theDirection, double theClearances, double theMinSupportLen, double theCuttingDistance);

	static VerticalPlate^ SuturePLate(VerticalPlate^ theVerticalPlate, BasePlate^ BasePlate, double theConnectHight, double theConnectThickness);

	static cli::array<List<VerticalPlate^>^>^ ConnectVerticalPLates(List<VerticalPlate^>^ toDownPlates, List<VerticalPlate^>^ toUpPlates, BasePlate^ BasePlate, double theConnectHight, double theConnectThickness, double theFilletRadius);

	static BasePlate^ SlotBasePLates(BasePlate^ BasePlate, List<VerticalPlate^>^ toDownPlates, List<VerticalPlate^>^ toUpPlates);

	static VerticalPlate^ BrandNumberVerticalPlate(VerticalPlate^ theVerticalPlate, double hight);

	static BasePlate^ BrandNumberBasePlate(BasePlate^ theBasePlate, double hight);

	//static void BrandNumber(VerticalPlate^% theVerticalPlate, double hight, int number, Wgp_Pnt^ thePoint);

	static OCC::Topo::TShape^ DeployPlates(BasePlate^ BasePlate, List<VerticalPlate^>^ MiddleToDownPlates, List<VerticalPlate^>^ MiddleToUpPlatesates);

	static auto TestError(int value);
};

}
}

