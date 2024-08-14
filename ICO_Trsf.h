#pragma once
#include <gp_Trsf.hxx>
//Local
#include "ICO_Ax2.h"

namespace OCCTK {
namespace OCC {
namespace gp {

public ref class Trsf {
public:
	Trsf();
	Trsf(Ax2^ theAx2);
	Trsf(Ax2^ fromAx2, Ax2^ toAx2);
	gp_Trsf GetOCC();
private:
	gp_Trsf* _nativeHandle;
};

}
}
}

