#pragma once

//前向声明
class gp_Circ;
namespace OCCTK {
namespace OCC {
namespace gp {
ref class Trsf;
ref class Ax2;
value struct Pnt;
ref class Dir;
}
}
}

namespace OCCTK {
namespace OCC {
namespace gp {

public ref class Circle :System::ICloneable {
public:
	Circle(gp_Circ circle);
	Circle(gp_Circ* circle);
	Circle(Ax2^ pose, double radius);
	gp_Circ GetOCC();
	virtual System::Object^ Clone();
public:
	property Ax2^ Pose;
	property double Radius;
};

}
}
}

