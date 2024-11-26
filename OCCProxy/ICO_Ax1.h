#pragma once
//值类型作为构造函数成员需要完整定义
#include <gp_Ax1.hxx>
#include "ICO_Pnt.h"
#include "ICO_Dir.h"

namespace OCCTK {
namespace OCC {
namespace gp {
//前向声明
value struct Trsf;

public value struct Ax1 :System::ICloneable {
public:
	const static Ax1 Default = Ax1(::gp_Ax1());
public:
	Ax1(Pnt location, Dir direction);
	Ax1(Pnt fromPnt, Pnt toPnt);
	Ax1(gp_Ax1 theAx1);
	Ax1(gp_Ax1* theAx1);
	gp_Ax1 GetOCC();
	virtual System::Object^ Clone();
	virtual System::String^ ToString() override;
	//! 隐式转换为 gp_Ax1
	static operator gp_Ax1 (Ax1 axis) { return axis.GetOCC(); }
public:
	void Transform(Trsf theT);
	Ax1 Transformed(Trsf theT);
	void Reverse();
	Ax1 Reversed();
	bool IsCoaxial(Ax1 other, double AngularTOL, double LinearTOL);
	bool IsCollinear(Ax1 other, double AngularTOL);
public:
	property Pnt Location;
	property Dir Direction;
	// Deconstruct 方法
	void Deconstruct([System::Runtime::InteropServices::OutAttribute] Pnt% location,
		[System::Runtime::InteropServices::OutAttribute] Dir% direction) {
		location = Location;
		direction = Direction;
	}
};

}
}
}