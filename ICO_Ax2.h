#pragma once
#include <gp_Ax2.hxx>
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
	gp_Ax2 GetOCC();
	virtual System::String^ ToString() override;

	void Transform(Trsf^ theT);
	Ax2^ Transformed(Trsf^ theT);

protected:
	gp_Ax2* myAx2;
};

}
}
}