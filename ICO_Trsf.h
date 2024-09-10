#pragma once
#include <gp_Trsf.hxx>
//Local
#include "ICO_Pnt.h"

//前向声明
namespace OCCTK {
namespace OCC {
namespace gp {
ref class Ax2;
}
}
}

namespace OCCTK {
namespace OCC {
namespace gp {

public ref class Trsf {
public:
	Trsf();
	Trsf(gp_Trsf theT);
	Trsf(Ax2^ theAx2);
	Trsf(Ax2^ fromAx2, Ax2^ toAx2);
	Trsf(Pnt^ fromPoint, Pnt^ toPoint);
	gp_Trsf GetOCC();
	virtual System::String^ ToString() override;
	void SetTranslation(Pnt^ fromPoint, Pnt^ toPoint);
	Trsf^ Multiplied(Trsf^ rightTrsf);

private:
	gp_Trsf* myTrsf;
};

}
}
}

