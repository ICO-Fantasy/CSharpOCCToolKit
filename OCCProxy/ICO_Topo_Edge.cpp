#include "ICO_Topo_Edge.h"
#include <TopoDS.hxx>

namespace OCCTK {
namespace OCC {
namespace Topo {

TEdge::TEdge() {
	myShape = new TopoDS_Edge();
}

TEdge::TEdge(const TopoDS_Edge theEdge) {
	myShape = new TopoDS_Edge(theEdge);
}

TEdge::TEdge(System::IntPtr theEdgeIntPtr) {
	// 将 IntPtr 转换为原生指针
	TopoDS_Edge* pShape = reinterpret_cast<TopoDS_Edge*>(theEdgeIntPtr.ToPointer());
	myShape = pShape;
}

TEdge::TEdge(TopoDS_Edge* theEdge) {
	myShape = theEdge;
}

TopoDS_Edge TEdge::GetOCC() {
	return TopoDS::Edge(*myShape);
}

}
}
}