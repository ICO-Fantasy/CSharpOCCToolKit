#pragma once
#include <gp_Ax3.hxx>
//local
#include "ICO_Trsf.h"

namespace OCCTK {
namespace OCC {
namespace gp {

public ref class Ax3 {
public:
	Ax3();
	Ax3(gp_Ax3 theAx3);
	Ax3(gp_Ax3* theAx3);
	gp_Ax3 GetOCC();
	virtual System::String^ ToString() override;

	void Transform(Trsf^ theT);
	Ax3^ Transformed(Trsf^ theT);

protected:
	gp_Ax3* myAx3;
};

}
}
}