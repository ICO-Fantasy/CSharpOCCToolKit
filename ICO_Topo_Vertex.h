#pragma once
//local
#include "ICO_Topo_Shape.h"
class TopoDS_Vertex;
namespace OCCTK {
namespace OCC {
namespace gp {
ref class Pnt;
}
}
}
namespace OCCTK {
namespace OCC {
namespace Topo {

public ref class TVertex :TShape {
public:
	TVertex();
	TVertex(const TopoDS_Vertex theVertex);
	TVertex(System::IntPtr theVertexIntPtr);
	TVertex(TopoDS_Vertex* theVertex);
	TopoDS_Vertex GetOCC();
public:
	gp::Pnt^ ToPnt();
};

}
}
}