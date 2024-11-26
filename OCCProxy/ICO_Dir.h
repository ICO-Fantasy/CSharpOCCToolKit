#pragma once
//在构造函数中使用的值对象需要直接引入
#include <gp_Dir.hxx>
#include "ICO_Pnt.h"
#include "ICO_Vec.h"

namespace OCCTK {
namespace OCC {
namespace gp {
//前向声明
value struct Trsf;

public value struct Dir :System::ICloneable {
public:
	const static Dir Default = Dir(1.0, 0.0, 0.0);
public:
	Dir(double theX, double theY, double theZ);
	Dir(Vec theDir);
	Dir(Pnt fromPoint, Pnt toPoint);
	Dir(gp_Dir theDir);
	Dir(gp_Dir* theDir);
	gp_Dir GetOCC();
	virtual System::Object^ Clone();
	virtual System::String^ ToString() override;
	//! 隐式转换为 gp_Dir
	static operator gp_Dir (Dir v) { return v.GetOCC(); }
public:
	bool IsParallel(Dir otherDir, double theAngularTolerance);
	double Angle(Dir otherDir);
	double AngleWithRef(Dir otherDir, Dir refDir);
	void Cross(Dir other);
	Dir Crossed(Dir other);
	double Dot(Dir other);
	void Reverse();
	Dir Reversed();
	void Transform(Trsf T);
	Dir Transformed(Trsf T);
public:
	Vec ToVec(double factor);
public:
	property double X;
	property double Y;
	property double Z;
	// Deconstruct 方法
	void Deconstruct([System::Runtime::InteropServices::Out] double% x,
		[System::Runtime::InteropServices::Out] double% y,
		[System::Runtime::InteropServices::Out] double% z) {
		x = X;
		y = Y;
		z = Z;
	}
private:
	void Normalize();
};

}
}
}