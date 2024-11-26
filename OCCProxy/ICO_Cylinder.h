#pragma once

//前向声明
class gp_Cylinder;
namespace OCCTK {
namespace OCC {
namespace gp {
ref class Trsf;
ref class Ax1;
ref class Ax3;
value struct Pnt;
ref class Dir;
}
}
}

namespace OCCTK {
namespace OCC {
namespace gp {

public ref class Cylinder :System::ICloneable {
public:
	Cylinder(Ax3^ pose, double radius);
	Cylinder(gp_Cylinder cylinder);
	Cylinder(gp_Cylinder* cylinder);
	gp_Cylinder GetOCC();
	virtual System::Object^ Clone();
public:
	Ax1^ Axis();
	double Radius();
protected:
	Ax3^ pose;
	double radius;
};

}
}
}

