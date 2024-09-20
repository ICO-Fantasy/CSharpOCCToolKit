#pragma once
#include <TopoDS_Edge.hxx>
//local
#include "ICO_Topo_Shape.h"

namespace OCCTK {
namespace OCC {
namespace Topo {

public ref class TEdge :TShape {
public:
	TEdge();
	TEdge(const TopoDS_Edge theEdge);
	TEdge(System::IntPtr theEdgeIntPtr);
	TEdge(TopoDS_Edge* theEdge);
	TopoDS_Edge GetOCC();
};

}
}
}