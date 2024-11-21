#pragma once
#include <BRepAdaptor_Surface.hxx>
//Local
#include "ICO_BaseObject.h"

namespace OCCTK {
namespace OCC {
namespace Topo {
ref class TFace;
ref class TFace;
}
namespace gp {
ref class Pnt;
ref class Cylinder;
}
namespace GeomAbs {
enum class SurfaceType;
}
}
}


namespace OCCTK {
namespace OCC {
namespace BRepAdaptor {
public ref class Surface : BaseObject {
private:
	Handle(BRepAdaptor_Surface) mySurface() { return Handle(BRepAdaptor_Surface)::DownCast(NativeHandle); }
public:
	Surface();
	Surface(Topo::TFace^ face);
public:
	gp::Pnt^ Value(double U, double V);
	GeomAbs::SurfaceType GetType();
	gp::Cylinder^ Cylinder();
	double FirstUParameter();
	double FirstVParameter();
	double LastUParameter();
	double LastVParameter();
};

}
}
}

