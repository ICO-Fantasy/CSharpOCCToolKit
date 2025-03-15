#pragma once
//构造函数成员需要完整定义
#include "ICO_Dir.h"
#include "ICO_Pnt.h"
#include "ICO_XYZ.h"
#include "..\Extension\ICO_WPR.h"
#include "ICO_Precision.h"
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Trsf.hxx>

namespace OCCTK {
namespace OCC {
namespace gp {
//前向声明
value struct Ax1;
value struct Trsf;

/// <summary>
/// Z方向为主方向，X方向为另一个方向，Y方向自动定义
/// </summary>
public value struct Ax2 :System::ICloneable {
public:
    const static Ax2 Default = Ax2(::gp_Ax2());
public:
    Ax2(gp_Ax2 theAx2);
    Ax2(gp_Ax3 theAx3);
    Ax2(Pnt location, Dir zAxis);
    Ax2(Pnt location, Dir zDir, Dir XDir);
    //Ax2(array<double, 2>^ matrix);
    //Ax2(array<array<double>^>^ matrix);
    gp_Ax2 GetOCC();
    virtual System::Object^ Clone();
    virtual System::String^ ToString() override;
    //! 隐式转换为 gp_Ax2
    static operator gp_Ax2 (Ax2 axis) { return axis.GetOCC(); }
public:
    Ax2 LocalTransformed(Trsf theT);
    Ax2 LocalTransformed(XYZ xyz,WPR wpr);
    Ax2 Transformed(Trsf theT);
private:
    Pnt myLocation;
    Dir myZDir;
    Dir myXDir;
public:
    property Pnt Location {Pnt get() { return myLocation; }};
    property Dir XDir {Dir get() { return myXDir; }};
    property Dir YDir {Dir get(); }
    property Dir ZDir {Dir get() { return myZDir; }};
    property Ax1 XAxis {Ax1 get(); }
    property Ax1 YAxis {Ax1 get(); }
    property Ax1 ZAxis {Ax1 get(); }

#pragma region 重载操作符

    bool Equals(Ax2 other, double AngularTOL, double LinearTOL) {
        return other.Location.Equals(Location, LinearTOL) &&
            other.XDir.Equals(XDir, AngularTOL) &&
            other.ZDir.Equals(ZDir, AngularTOL);
    }
    static bool operator == (Ax2 Left, Ax2 Right) { return Left.Equals(Right, ANGULAR_TOL, LINEAR_TOL); }//默认精度
    static bool operator != (Ax2 Left, Ax2 Right) { return !Left.Equals(Right, ANGULAR_TOL, LINEAR_TOL); }//默认精度

#pragma endregion

};

}
}
}