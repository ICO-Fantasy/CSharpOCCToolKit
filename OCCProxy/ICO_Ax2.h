﻿#pragma once
//值类型作为构造函数成员需要完整定义
#include <gp_Ax2.hxx>
#include "ICO_Pnt.h"
#include "ICO_Dir.h"
#include "ICO_Trsf.h"

namespace OCCTK {
namespace OCC {
namespace gp {
//前向声明
value struct Ax1;
ref class Trsf;

/// <summary>
/// Z方向为主方向，X方向为另一个方向，Y方向自动定义
/// </summary>
public value struct Ax2 :System::ICloneable {
public:
    const static Ax2 Default = Ax2(::gp_Ax2());
public:
    Ax2(Pnt location, Dir zAxis);
    Ax2(Pnt location, Dir zAxis, Dir XAxis);
    Ax2(gp_Ax2 theAx2);
    Ax2(Ax2 fromAx2, Trsf^ transfrom);
    Ax2(gp_Ax2* theAx2);
    Ax2(array<double, 2>^ matrix);
    Ax2(array<array<double>^>^ matrix);
    gp_Ax2 GetOCC();
    virtual System::Object^ Clone();
    virtual System::String^ ToString() override;
    //! 隐式转换为 gp_Ax2
    static operator gp_Ax2 (Ax2 axis) { return axis.GetOCC(); }
public:
    //void Transform(Trsf^ theT);
    Ax2 Transformed(Trsf^ theT);
public:
    property Pnt Location;
    property Dir XDir;
    property Dir YDir {Dir get(); }
    property Dir ZDir;
    property Ax1 XAxis {Ax1 get(); }
    property Ax1 YAxis {Ax1 get(); }
    property Ax1 ZAxis {Ax1 get(); }
};

}
}
}