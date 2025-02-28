#pragma once
//在构造函数中使用的值对象需要直接引入
#include <gp_Vec2d.hxx>
#include "ICO_Precision.h"

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
    Vec2D(System::ValueTuple<double, double> theXY);
    Vec2D(Pnt2D fromPnt, Pnt2D toPnt);
    Vec2D(gp_Vec2d theVec);
    Vec2D(gp_Vec2d* theVec);
    gp_Vec2d GetOCC();
    virtual System::Object^ Clone();
    virtual System::String^ ToString() override;
    //! 隐式转换为 gp_Vec2d
    static operator gp_Vec2d (Vec2D v) { return v.GetOCC(); }
public:
    bool IsParallel(Vec2D otherVec, double theAngularTolerance);
    Vec2D Normalized();
    Vec2D Multiplied(double value);
    double Dot(Vec2D other);
    double Crossed(Vec2D other);
public:
    property double X;
    property double Y;
    // Deconstruct 方法
    void Deconstruct([System::Runtime::InteropServices::OutAttribute] double% x,
        [System::Runtime::InteropServices::OutAttribute] double% y) {
        x = X;
        y = Y;
    }

#pragma region 重载操作符

    bool Equals(Vec2D otherVec2D, double angularTOL) {
        return this->IsParallel(otherVec2D, angularTOL);
    }
    static bool operator == (Vec2D left, Vec2D right) { return left.Equals(right, ANGULAR_TOL); }//默认精度
    static bool operator != (Vec2D left, Vec2D right) { return !left.Equals(right, ANGULAR_TOL); }//默认精度
    static Vec2D operator + (Vec2D left, Vec2D right) {
        return Vec2D(
            left.X + right.X,
            left.Y + right.Y);
    }
    static Vec2D operator - (Vec2D left, Vec2D right) {
        return Vec2D(
            left.X - right.X,
            left.Y - right.Y);
    }

    /// <summary>
    /// 点乘
    /// </summary>
    static double operator * (Vec2D left, Vec2D right) {
        return left.Dot(right);
    }

    /// <summary>
    /// 叉乘
    /// </summary>
    static double operator ^ (Vec2D left, Vec2D right) {
        return left.Crossed(right);
    }

#pragma endregion

};

}
}
}