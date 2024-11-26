#pragma once
//在构造函数中使用的对象需要直接引入
#include <gp_Pnt2d.hxx>
namespace OCCTK {
namespace OCC {
namespace gp {
//前向声明
value struct Trsf;

public value struct Pnt2D :System::ICloneable {
public:
	const static Pnt2D Origin = Pnt2D(0.0, 0.0);
public:
	Pnt2D(double theX, double theY);
	Pnt2D(gp_Pnt2d thePnt);
	Pnt2D(gp_Pnt2d* thePnt);
	virtual System::Object^ Clone();
	virtual System::String^ ToString() override;
	gp_Pnt2d GetOCC();
	//! 隐式转换为 gp_Pnt2d
	static operator gp_Pnt2d (Pnt2D p) { return p.GetOCC(); }
public:
	double Distance(Pnt2D otherPnt);
	Pnt2D Transformed(Trsf T);
	Pnt2D SetX(double value);
	Pnt2D SetY(double value);
public:
	property double X;
	property double Y;
};

}
}
}