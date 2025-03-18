#pragma once
//值类型作为构造函数成员需要完整定义
#include "ICO_Dir.h"
#include "ICO_Pnt.h"
#include "ICO_Precision.h"
#include <gp_Ax1.hxx>

namespace OCCTK {
namespace OCC {
namespace gp {
//前向声明
value struct Trsf;

public value struct Ax1 :System::ICloneable {
public:
    const static Ax1 Default = Ax1(::gp_Ax1());
public:
    Ax1(Pnt location, Dir direction);
    Ax1(Pnt fromPnt, Pnt toPnt);
    Ax1(gp_Ax1 theAx1);
    gp_Ax1 GetOCC();
    virtual System::Object^ Clone();
    virtual System::String^ ToString() override;
    //! 隐式转换为 gp_Ax1
    static operator gp_Ax1 (Ax1 axis) { return axis.GetOCC(); }
public:
    Ax1 Transformed(Trsf theT);
    Ax1 Reversed();
    bool IsCoaxial(Ax1 other, double AngularTOL, double LinearTOL);
    bool IsCollinear(Ax1 other, double AngularTOL);
private:
    Pnt myLocation;
    Dir myDirection;
public:
    property Pnt Location { Pnt get() { return myLocation; } };
    property Dir Direction {Dir get() { return myDirection; } };

#pragma region 重载操作符

    bool Equals(Ax1 other, double angularTOL, double linearTOL) {
        return Location == other.Location && IsCoaxial(other, angularTOL, linearTOL);
    }
    static bool operator == (Ax1 left, Ax1 right) { return left.Equals(right, ANGULAR_TOL, LINEAR_TOL); }//默认精度
    static bool operator != (Ax1 left, Ax1 right) { return !left.Equals(right, ANGULAR_TOL, LINEAR_TOL); }//默认精度

#pragma endregion
};

}
}
}