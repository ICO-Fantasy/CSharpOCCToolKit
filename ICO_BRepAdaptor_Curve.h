#pragma once
#include <BRepAdaptor_Curve.hxx>
//包装C++类到托管类
#include <NCollection_Haft.h> 

namespace OCCTK {
namespace OCC {
namespace Topo {
ref class TEdge;
ref class TFace;
}
namespace gp {
ref class Pnt;
}
}
}


namespace OCCTK {
namespace OCC {
namespace BRepAdaptor {

public ref class Curve {
public:
	Curve();
	Curve(Topo::TEdge^ edge);
	Curve(Topo::TEdge^ edge, Topo::TFace^ face);
public:
	gp::Pnt^ Value(double UVValue);
	double FirstParameter();
	double LastParameter();
private:
	NCollection_Haft<Handle(BRepAdaptor_Curve)> myCure;
};

}
}
}

