#pragma once
//在构造函数中使用的值对象需要直接引入
#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>

namespace OCCTK {
namespace OCC {
namespace gp {

ref class Trsf;

// 转为OCC结构
//#define STRUCT_PIN(value, managed, native)
//pin_ptr<managed> value##_pinptr(&value);
//::native* value##_ptr = reinterpret_cast<::native*>(value##_pinptr);
// 转为托管结构
//#define STRUCT_PINREF(value, managed, native)
//pin_ptr<managed> value##_pinptr(&*value);
//::native* value##_ptr = reinterpret_cast<::native*>(value##_pinptr);
// 
//使用与OCC对象的内存采用同样的内存布局（同样的声明结构和值类型），这样可以利用上述宏重新解释内存，实现对托管对象和非托管对象之间的相互转换。性能较好，维护性差，项目不采用这种方式定义
//[System::Runtime::InteropServices::StructLayout(System::Runtime::InteropServices::LayoutKind::Sequential, Pack = 8)]

public value struct Pnt :System::ICloneable {
public:
	const static Pnt Default = Pnt(0.0, 0.0, 0.0);
public:
	Pnt(double theX, double theY, double theZ);
	Pnt(gp_Pnt thePnt);
	Pnt(gp_Pnt* thePnt);
	Pnt(gp_XYZ theXYZ);
	Pnt(gp_XYZ* theXYZ);
	gp_Pnt GetOCC();
	virtual System::Object^ Clone();
	virtual System::String^ ToString() override;
	//! 隐式转换为 gp_Pnt
	static operator gp_Pnt (Pnt p) { return p.GetOCC(); }
public:
	double Distance(Pnt otherPnt);
	Pnt Transformed(Trsf^ T);
	Pnt SetX(double value);
	Pnt SetY(double value);
	Pnt SetZ(double value);
public:
	property double X;
	property double Y;
	property double Z;
	// Deconstruct 方法
	void Deconstruct([System::Runtime::InteropServices::OutAttribute] double% x,
		[System::Runtime::InteropServices::OutAttribute] double% y,
		[System::Runtime::InteropServices::OutAttribute] double% z) {
		x = X;
		y = Y;
		z = Z;
	}
#pragma region 重载操作符
	bool Equals(Pnt otherPnt, double tol);
	static bool operator == (Pnt Left, Pnt Right) { return Left.Equals(Right, 1e-6); }//默认精度为1e-6
	static bool operator != (Pnt Left, Pnt Right) { return !Left.Equals(Right, 1e-6); }//默认精度为1e-6
	static Pnt operator + (Pnt Left, Pnt Right);
	static Pnt operator - (Pnt Left, Pnt Right);
	//static Pnt operator + (Pnt Left, Vec Right);
	//static Pnt operator - (Pnt Left, Vec Right);
	//static Pnt operator * (Pnt Left, Trsf Right);
#pragma endregion
};

}
}
}