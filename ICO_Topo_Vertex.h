#pragma once
#include <TopoDS_Vertex.hxx>
//local
#include "ICO_Topo_Shape.h"

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
};

}
}
}