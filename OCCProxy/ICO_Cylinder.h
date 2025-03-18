#pragma once
//值类型作为构造函数成员需要完整定义
#include "ICO_Ax3.h"
//前向声明
class gp_Cylinder;

namespace OCCTK {
namespace OCC {
namespace gp {
//前向声明
value struct Ax1;

public ref class Cylinder :System::ICloneable {
public:
    Cylinder(Ax3 pose, double radius);
    Cylinder(gp_Cylinder cylinder);
    gp_Cylinder GetOCC();
    virtual System::Object^ Clone();
private:
    Ax3 pose;
    double radius;
public:
    property Ax1 Axis {Ax1 get() { return pose.ZAxis; }; };
    property double Radius { double get() { return radius; } };
};

}
}
}

