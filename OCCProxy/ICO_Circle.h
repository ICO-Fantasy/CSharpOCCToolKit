#pragma once
//值类型作为构造函数成员需要完整定义
#include "ICO_Ax2.h"
//前向声明
class gp_Circ;

namespace OCCTK {
namespace OCC {
namespace gp {
//前向声明
ref class Trsf;

public ref class Circle :System::ICloneable {
public:
	Circle(gp_Circ circle);
	Circle(gp_Circ* circle);
	Circle(Ax2 pose, double radius);
	gp_Circ GetOCC();
	virtual System::Object^ Clone();
public:
	property Ax2 Pose;
	property double Radius;
};

}
}
}

