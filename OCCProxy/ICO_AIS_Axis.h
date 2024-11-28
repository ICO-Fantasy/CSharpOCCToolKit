#pragma once
#include <AIS_InteractiveObject.hxx>
#include <AIS_Axis.hxx>
//Local
#include "ICO_InteractiveObject.h"

namespace OCCTK {
namespace OCC {
namespace gp {
ref class Trsf;
value struct Ax1;
}
}
}

namespace OCCTK {
namespace OCC {
namespace AIS {

public ref class AAxis :public InteractiveObject {
private:
	Handle(AIS_Axis) myAxis() { return Handle(AIS_Axis)::DownCast(NativeHandle); }
public:
	AAxis(const Handle(AIS_Axis) aAISAxis) : InteractiveObject(aAISAxis) {};
	AAxis(gp::Ax1 aAxis);
	AAxis(gp::Ax1 aAxis, double len);
public:
	void SetWidth(double size);
	void UnsetWidth();
public:
	Handle(AIS_Axis) GetOCC() { return myAxis(); }
};

}
}
}


