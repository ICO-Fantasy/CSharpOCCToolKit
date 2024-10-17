#pragma once
#include <BRepAdaptor_Curve.hxx>
//Local
#include "ICO_BaseObject.h"


namespace OCCTK {
namespace OCC {
namespace Topo {
ref class TEdge;
ref class TFace;
}
namespace gp {
ref class Pnt;
ref class Circle;
}
namespace GeomAbs {
enum class CurveType;
}
}
}


namespace OCCTK {
namespace OCC {
namespace BRepAdaptor {
public ref class Curve : BaseObject {
private:
	Handle(BRepAdaptor_Curve) myCure() { return Handle(BRepAdaptor_Curve)::DownCast(NativeHandle); }
public:
	Curve();
	Curve(Topo::TEdge^ edge);
	Curve(Topo::TEdge^ edge, Topo::TFace^ face);
public:
	gp::Pnt^ Value(double UVValue);
	double FirstParameter();
	double LastParameter();
	GeomAbs::CurveType GetType();
	gp::Circle^ Circle();
};

}
}
}

