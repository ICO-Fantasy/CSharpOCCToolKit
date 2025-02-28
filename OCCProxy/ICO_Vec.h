#pragma once
#include <cmath>
//在构造函数中使用的值对象需要直接引入
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include "ICO_Pnt.h"
//前向声明
class gp_Vec;

namespace OCCTK {
namespace OCC {
namespace gp {
//前向声明
ref class Trsf;

public value struct Vec :System::ICloneable {
public:
	const static Vec Default = Vec(1.0, 0.0, 0.0);
public:
	Vec(double theX, double theY, double theZ);
	Vec(System::ValueTuple<double, double, double> theXYZ);
	Vec(Pnt fromPnt, Pnt toPnt);
	Vec(gp_Vec theVec);
	Vec(gp_Vec* theVec);
	Vec(gp_XYZ theVec);
	Vec(gp_XYZ* theVec);
	gp_Vec GetOCC();
	virtual System::Object^ Clone();
	virtual System::String^ ToString() override;
	//! 隐式转换为 gp_Vec
	static operator gp_Vec (Vec v) { return v.GetOCC(); }

public:
	bool IsParallel(Vec otherVec, double theAngularTolerance);
	Vec Reversed();
	Vec Normalized();
	Vec Multiplied(double value);
	double Dot(Vec other);
	Vec Crossed(Vec other);
	Vec CrossProduct(Vec other);
	Vec Transformed(Trsf^ T);
	double Magnitude();
	double Angle(Vec theOther);
	double AngleWithRef(Vec theOther, Vec theVRef);

public:
	property double X;
	property double Y;
	property double Z;
	property double Length {double get() { return std::sqrt(X * X + Y * Y + Z * Z); }};
	// Deconstruct 方法
	void Deconstruct([System::Runtime::InteropServices::OutAttribute] double% x,
		[System::Runtime::InteropServices::OutAttribute] double% y,
		[System::Runtime::InteropServices::OutAttribute] double% z) {
		x = X;
		y = Y;
		z = Z;
	}
#pragma region 重载操作符
	bool Equals(Vec otherVec, double tol);
	static bool operator == (Vec Left, Vec Right) { return Left.Equals(Right, 0.0000174533); }//默认精度0.001度
	static bool operator != (Vec Left, Vec Right) { return !Left.Equals(Right, 0.0000174533); }//默认精度0.001度
	static Vec operator + (Vec Left, Vec Right);
	static Vec operator - (Vec Left, Vec Right);
	//点乘
	static double operator * (Vec Left, Vec Right);
	//叉乘
	static Vec operator ^ (Vec Left, Vec Right);
#pragma endregion
};

}
}
}