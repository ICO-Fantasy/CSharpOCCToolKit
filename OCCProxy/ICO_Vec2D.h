#pragma once
//在构造函数中使用的值对象需要直接引入
#include <gp_Vec2d.hxx>
//前向声明
class gp_Vec2d;

namespace OCCTK {
namespace OCC {
namespace gp {
//前向声明
value struct Pnt2D;

public value struct Vec2D :System::ICloneable {
public:
	const static Vec2D Default = Vec2D(1.0, 0.0);
public:
	Vec2D(double theX, double theY);
	Vec2D(Pnt2D fromPnt, Pnt2D toPnt);
	Vec2D(gp_Vec2d theVec);
	Vec2D(gp_Vec2d* theVec);
	gp_Vec2d GetOCC();
	virtual System::Object^ Clone();
	virtual System::String^ ToString() override;
public:
	bool IsParallel(Vec2D^ otherVec, double theAngularTolerance);
	void Normalize();
	Vec2D^ Normalized();
	void Multiply(double value);
	Vec2D^ Multiplied(double value);
	double Crossed(Vec2D^ other);
public:
	property double X;
	property double Y;
	// Deconstruct 方法
	void Deconstruct([System::Runtime::InteropServices::OutAttribute] double% x,
		[System::Runtime::InteropServices::OutAttribute] double% y) {
		x = X;
		y = Y;
	}
};

}
}
}