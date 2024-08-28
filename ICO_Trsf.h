#pragma once
#include <gp_Trsf.hxx>
//Local
#include "ICO_Ax2.h"
#include "ICO_Pnt.h"

namespace OCCTK {
namespace OCC {
namespace gp {

public ref class Trsf {
public:
	Trsf();
	Trsf(Ax2^ theAx2);
	Trsf(Ax2^ fromAx2, Ax2^ toAx2);
	Trsf(Pnt^ fromPoint, Pnt^ toPoint);
	void SetTranslation(Pnt^ fromPoint, Pnt^ toPoint);
	gp_Trsf GetOCC();
private:
	gp_Trsf* _nativeHandle;
};

}
}
}

