#pragma once

//前向声明
class gp_Circ;
namespace OCCTK {
namespace OCC {
namespace gp {
ref class Trsf;
ref class Ax2;
ref class Pnt;
ref class Dir;
}
}
}

namespace OCCTK {
namespace OCC {
namespace gp {

public ref class Circle :System::ICloneable {
public:
	Circle(Ax2^ pose, double radius);
	Circle(gp_Circ circle);
	Circle(gp_Circ* circle);
	gp_Circ GetOCC();
	virtual System::Object^ Clone();
public:
	Pnt^ Location();
	property double Radius;
protected:
	Ax2^ pose;
};

}
}
}

