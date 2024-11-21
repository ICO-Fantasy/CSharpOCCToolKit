#pragma once
#include <Geom_Curve.hxx>
//Local
#include "ICO_BaseObject.h"

namespace OCCTK {
namespace OCC {
ref class BaseObject;
}
}

namespace OCCTK {
namespace OCC {
namespace Geom {

public ref class Curve :BaseObject {
private:
	Handle(Geom_Curve) myCurve() { return Handle(Geom_Curve)::DownCast(NativeHandle); }
public:
	Curve(Handle(Geom_Curve) curve) :BaseObject(curve) {}
	Handle(Geom_Curve) GetOCC();
};

}
}
}

