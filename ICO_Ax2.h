#pragma once
#include <gp_Ax2.hxx>

//前向声明
namespace OCCTK {
namespace OCC {
namespace gp {
ref class Trsf;
}
}
}

namespace OCCTK {
namespace OCC {
namespace gp {

public ref class Ax2 {
public:
	Ax2();
	Ax2(gp_Ax2 theAx2);
	Ax2(gp_Ax2* theAx2);
	Ax2(array<double, 2>^ matrix);
	Ax2(array<array<double>^>^ matrix);
	gp_Ax2 GetOCC();
	virtual System::String^ ToString() override;

	property double X {double get() { return myAx2->Location().X(); }; void set(double value) { myAx2->SetLocation(gp_Pnt(value, Y, Z)); } };
	property double Y {double get() { return myAx2->Location().Y(); }; void set(double value) { myAx2->SetLocation(gp_Pnt(X, value, Z)); } };
	property double Z {double get() { return myAx2->Location().Z(); }; void set(double value) { myAx2->SetLocation(gp_Pnt(X, Y, value)); } };

	void Transform(Trsf^ theT);
	Ax2^ Transformed(Trsf^ theT);

protected:
	gp_Ax2* myAx2;
};

}
}
}