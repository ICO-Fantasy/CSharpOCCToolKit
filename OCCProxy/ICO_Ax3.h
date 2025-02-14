#pragma once
//值类型作为构造函数成员需要完整定义
#include <gp_Ax3.hxx>
#include "ICO_Ax1.h"
#include "ICO_Pnt.h"
#include "ICO_Dir.h"

namespace OCCTK {
namespace OCC {
namespace gp {
//前向声明
ref class Trsf;

public value struct Ax3 :System::ICloneable {
public:
    const static Ax3 Default = Ax3(::gp_Ax3());
public:
    Ax3(Pnt location, Dir zAxis);
    Ax3(Pnt location, Dir zAxis, Dir xAxis);
    Ax3(gp_Ax3 theAx3);
    Ax3(gp_Ax3* theAx3);
    gp_Ax3 GetOCC();
    virtual System::Object^ Clone();
    virtual System::String^ ToString() override;
    //! 隐式转换为 gp_Ax3
    static operator gp_Ax3 (Ax3 axis) { return axis.GetOCC(); }
public:
    //void Transform(Trsf^ theT);
    Ax3 Transformed(Trsf^ theT);
public:
    property Pnt Location;
    property Dir XDir;
    property Dir YDir {Dir get(); }
    property Dir ZDir;
    property Ax1 XAxis {Ax1 get(); }
    property Ax1 YAxis {Ax1 get(); }
    property Ax1 ZAxis {Ax1 get(); }

#pragma region 重载操作符

    bool Equals(Ax3 other, double AngularTOL, double LinearTOL) {
        return other.Location.Equals(Location, LinearTOL) &&
            other.XDir.Equals(XDir, AngularTOL) &&
            other.ZDir.Equals(ZDir, AngularTOL);
    }
    static bool operator == (Ax3 Left, Ax3 Right) { return Left.Equals(Right, ANGULAR_TOL, LINEAR_TOL); }//默认精度
    static bool operator != (Ax3 Left, Ax3 Right) { return !Left.Equals(Right, ANGULAR_TOL, LINEAR_TOL); }//默认精度

#pragma endregion
};

}
}
}