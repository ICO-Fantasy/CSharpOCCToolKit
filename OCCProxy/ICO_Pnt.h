#pragma once
//在构造函数中使用的值对象需要直接引入
#include "ICO_Precision.h"
#include "ICO_XYZ.h"
#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>

namespace OCCTK {
namespace OCC {
namespace gp {

//前向声明
value struct Trsf;
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
    Pnt(System::ValueTuple<double, double, double> theXYZ);
    Pnt(gp_Pnt thePnt);
    Pnt(gp_XYZ theXYZ);
    Pnt(XYZ theXYZ);
    gp_Pnt GetOCC();
    virtual System::Object^ Clone();
    virtual System::String^ ToString() override;
    //! 隐式转换为 gp_Pnt
    static operator gp_Pnt (Pnt p) { return p.GetOCC(); }
public:
    double Distance(Pnt otherPnt);
    Pnt Transformed(Trsf T);
    Pnt SetX(double value);
    Pnt SetY(double value);
    Pnt SetZ(double value);
private:
    double x;
    double y;
    double z;
public:
    property double X {double get() { return x; }};
    property double Y {double get() { return y; }};
    property double Z {double get() { return z; }};
    // 解构赋值
    void Deconstruct([System::Runtime::InteropServices::Out] double% x,
        [System::Runtime::InteropServices::Out] double% y,
        [System::Runtime::InteropServices::Out] double% z) {
        x = X;
        y = Y;
        z = Z;
    }

#pragma region 重载操作符

    bool Equals(Pnt otherPnt, double tol) {
        return this->Distance(otherPnt) <= tol;
    }

    /// <summary>
    /// 使用默认精度进行比较
    /// </summary>
    static bool operator == (Pnt Left, Pnt Right) { return Left.Equals(Right, LINEAR_TOL); }//默认精度
    static bool operator != (Pnt Left, Pnt Right) { return !Left.Equals(Right, LINEAR_TOL); }//默认精度
    static Pnt operator + (Pnt Left, Pnt Right) {
        return Pnt(
            Left.X + Right.X,
            Left.Y + Right.Y,
            Left.Z + Right.Z);
    }
    static Pnt operator - (Pnt Left, Pnt Right) {
        return Pnt(
            Left.X - Right.X,
            Left.Y - Right.Y,
            Left.Z - Right.Z);
    }
    //static Pnt operator + (Pnt Left, Vec Right);
    //static Pnt operator - (Pnt Left, Vec Right);
    //static Pnt operator * (Pnt Left, Trsf Right);

#pragma endregion

};

}
}
}