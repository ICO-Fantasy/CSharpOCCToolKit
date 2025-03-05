#pragma once
//值类型作为构造函数成员需要完整定义
#include "ICO_Ax2.h"
//前向声明
class gp_Circ;

namespace OCCTK {
namespace OCC {
namespace gp {
//前向声明
value struct Trsf;

public value struct Circle :System::ICloneable {
public:
    Circle(gp_Circ circle);
    Circle(Ax2 pose, double radius);
    gp_Circ GetOCC();
    virtual System::Object^ Clone();
private:
    Ax2 pose;
    double radius;
public:
    property Ax2 Pose {Ax2 get() { return pose; }}
    property double Radius {double get() { return radius; }}
};

}
}
}

