﻿#pragma once
#include <cmath>
//在构造函数中使用的值对象需要直接引入
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include "ICO_Pnt.h"
#include "ICO_XYZ.h"
#include "ICO_Precision.h"
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
    Vec(XYZ theXYZ);
    gp_Vec GetOCC();
    virtual System::Object^ Clone();
    virtual System::String^ ToString() override;
    //! 隐式转换为 gp_Vec
    static operator gp_Vec (Vec v) { return v.GetOCC(); }
public:
    bool IsParallel(Vec other, double theAngularTolerance);
    Vec Reversed();
    void Normalize();
    Vec Normalized();
    void Multiply(double value);
    Vec Multiplied(double value);
    double Dot(Vec other);
    void Cross(Vec other);
    Vec Crossed(Vec other);
    Vec CrossProduct(Vec other);
    void Transform(Trsf^ T);
    Vec Transformed(Trsf^ T);
    double Magnitude();
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

    bool Equals(Vec other, double angularTOL) {
        return this->IsParallel(other, angularTOL);
    }
    static bool operator == (Vec left, Vec right) { return left.Equals(right, ANGULAR_TOL); }//默认精度
    static bool operator != (Vec left, Vec right) { return !left.Equals(right, ANGULAR_TOL); }//默认精度
    static Vec operator + (Vec left, Vec right) {
        return Vec(
            left.X + right.X,
            left.Y + right.Y,
            left.Z + right.Z);
    }
    static Vec operator - (Vec left, Vec right) {
        return Vec(
            left.X - right.X,
            left.Y - right.Y,
            left.Z - right.Z);
    }
    /// <summary>
    /// 点乘
    /// </summary>
    static double operator * (Vec left, Vec right) {
        return left.Dot(right);
    }
    /// <summary>
    /// 叉乘
    /// </summary>
    static Vec operator ^ (Vec left, Vec right) {
        return left.Crossed(right);
    }

#pragma endregion

};

}
}
}