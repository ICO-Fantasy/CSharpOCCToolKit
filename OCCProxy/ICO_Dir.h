#pragma once
//在构造函数中使用的值对象需要直接引入
#include "ICO_Pnt.h"
#include "ICO_Precision.h"
#include "ICO_Vec.h"
#include <gp_Dir.hxx>

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
    gp_Dir GetOCC();
    virtual System::Object^ Clone();
    virtual System::String^ ToString() override;
    //! 隐式转换为 gp_Dir
    static operator gp_Dir (Dir v) { return v.GetOCC(); }
public:
    bool IsParallel(Dir otherDir, double theAngularTolerance);
    double Angle(Dir otherDir);
    double AngleWithRef(Dir otherDir, Dir refDir);
    Dir Crossed(Dir other);
    double Dot(Dir other);
    Dir Reversed();
    Dir Transformed(Trsf T);
public:
    Vec ToVec(double factor);
private:
    double x;
    double y;
    double z;
public:
    property double X {double get() { return x; }};
    property double Y {double get() { return y; }};
    property double Z {double get() { return z; }};

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