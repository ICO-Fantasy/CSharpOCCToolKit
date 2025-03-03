#pragma once
//在构造函数中使用的值对象需要直接引入
#include <gp_Dir.hxx>
#include "ICO_Pnt.h"
#include "ICO_Vec.h"
#include "ICO_Precision.h"

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
    Dir(System::ValueTuple<double, double, double> theXYZ);
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
    //void Cross(Dir other);
    Dir Crossed(Dir other);
    double Dot(Dir other);
    //void Reverse();
    Dir Reversed();
    //void Transform(Trsf T);
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

#pragma region 重载操作符

    bool Equals(Dir otherDir, double AngularTol) {
        return this->IsParallel(otherDir, AngularTol);
    }
    static bool operator == (Dir Left, Dir Right) { return Left.Equals(Right, ANGULAR_TOL); }//默认精度
    static bool operator != (Dir Left, Dir Right) { return !Left.Equals(Right, ANGULAR_TOL); }//默认精度
    static Dir operator + (Dir Left, Dir Right) {
        return Dir(
            Left.X + Right.X,
            Left.Y + Right.Y,
            Left.Z + Right.Z);
    }
    static Dir operator - (Dir Left, Dir Right) {
        return Dir(
            Left.X - Right.X,
            Left.Y - Right.Y,
            Left.Z - Right.Z);
    }
    /// <summary>
    /// 点乘
    /// </summary>
    static double operator * (Dir Left, Dir Right) { return Left.Dot(Right); }
    /// <summary>
    /// 叉乘
    /// </summary>
    static Dir operator ^ (Dir Left, Dir Right) {
        return Left.Crossed(Right);
    }

#pragma endregion

private:
    void Normalize();
};

}
}
}