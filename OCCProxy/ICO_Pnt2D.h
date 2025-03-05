#pragma once
//在构造函数中使用的对象需要直接引入
#include <gp_Pnt2d.hxx>
//local
#include "ICO_Precision.h"

namespace OCCTK {
namespace OCC {
namespace gp {
//前向声明
value struct Trsf;

public value struct Pnt2D :System::ICloneable {
public:
    const static Pnt2D Default = Pnt2D(0.0, 0.0);
public:
    Pnt2D(double theX, double theY);
    Pnt2D(gp_Pnt2d thePnt);
    virtual System::Object^ Clone();
    virtual System::String^ ToString() override;
    gp_Pnt2d GetOCC();
    //! 隐式转换为 gp_Pnt2d
    static operator gp_Pnt2d (Pnt2D p) { return p.GetOCC(); }
public:
    double Distance(Pnt2D otherPnt);
    Pnt2D Transformed(Trsf T);
    Pnt2D SetX(double value);
    Pnt2D SetY(double value);
private:
    double x;
    double y;
public:
    property double X {double get() { return x; }};
    property double Y {double get() { return y; }};

#pragma region 重载操作符

    bool Equals(Pnt2D otherPnt, double tol) {
        return this->Distance(otherPnt) <= tol;
    }
    static bool operator == (Pnt2D Left, Pnt2D Right) { return Left.Equals(Right, LINEAR_TOL); }//默认精度
    static bool operator != (Pnt2D Left, Pnt2D Right) { return !Left.Equals(Right, LINEAR_TOL); }//默认精度
    static Pnt2D operator + (Pnt2D Left, Pnt2D Right) {
        return Pnt2D(
            Left.X + Right.X,
            Left.Y + Right.Y);
    }
    static Pnt2D operator - (Pnt2D Left, Pnt2D Right) {
        return Pnt2D(
            Left.X - Right.X,
            Left.Y - Right.Y);
    }
    //static Pnt2D operator + (Pnt2D Left, Vec Right);
    //static Pnt2D operator - (Pnt2D Left, Vec Right);
    //static Pnt2D operator * (Pnt2D Left, Trsf Right);

#pragma endregion
};

}
}
}