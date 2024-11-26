#pragma once
//值类型作为构造函数成员需要完整定义
#include "ICO_Pnt.h"
#include "ICO_Dir.h"
//前向声明
class gp_Ax1;

namespace OCCTK {
namespace OCC {
namespace gp {
//前向声明
ref class Trsf;
value struct Dir;

public ref class Ax1 :System::ICloneable {
public:
	Ax1();
	Ax1(Pnt location, Dir direction);
	Ax1(Pnt fromPnt, Pnt toPnt);
	Ax1(gp_Ax1 theAx1);
	Ax1(gp_Ax1* theAx1);
	gp_Ax1 GetOCC();
	virtual System::Object^ Clone();
	virtual System::String^ ToString() override;
public:
	void Transform(Trsf^ theT);
	Ax1^ Transformed(Trsf^ theT);
	void Reverse();
	Ax1^ Reversed();
	bool IsCoaxial(Ax1^ other, double AngularTOL, double LinearTOL);
	bool IsCollinear(Ax1^ other, double AngularTOL);
public:
	property Pnt Location;
	property Dir Direction;
};

}
}
}