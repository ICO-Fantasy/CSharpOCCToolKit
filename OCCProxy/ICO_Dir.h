﻿#pragma once
//#include <Eigen/Dense>
//在构造函数中使用的值对象需要直接引入
#include <gp_Dir.hxx>
#include "ICO_Pnt.h"
#include "ICO_Vec.h"
namespace Eigen {
typedef Vector3d;
}

namespace OCCTK {
namespace OCC {
namespace gp {
//前向声明
ref class Trsf;

public value struct Dir :System::ICloneable {
public:
	const static Dir XDir = Dir(1.0, 0.0, 0.0);
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
	void Transform(Trsf^ T);
	Dir Transformed(Trsf^ T);
public:
	Vec ToVec(double factor);
	Eigen::Vector3d ToVector3d();
public:
	property double X;
	property double Y;
	property double Z;
private:
	void Normalize();
};

}
}
}