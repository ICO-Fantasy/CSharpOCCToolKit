#pragma once
#include <AIS_Triangulation.hxx>
//Local
#include "ICO_InteractiveObject.h"

namespace OCCTK {
namespace Extension {
value struct Color;
}
namespace OCC {
namespace Poly {
ref class PTriangulation;
}
}
}

namespace OCCTK {
namespace OCC {
namespace AIS {

public ref class ATriangulation :public InteractiveObject {
private:
	Handle(AIS_Triangulation) myPT() { return Handle(AIS_Triangulation)::DownCast(NativeHandle); }
public:
	ATriangulation(const Handle(AIS_Triangulation) ais) : InteractiveObject(ais) {};
	ATriangulation(Poly::PTriangulation^ triangulation);
public:
	void SetColor(Extension::Color^ color);
public:
	Handle(AIS_Triangulation) GetOCC() { return myPT(); }
};

}
}
}