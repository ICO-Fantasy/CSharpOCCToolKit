#include "ICO_Topo_Vertex.h"
#include <TopoDS.hxx>

namespace OCCTK {
namespace OCC {
namespace Topo {

TVertex::TVertex() {
	myShape = new TopoDS_Vertex();
}

TVertex::TVertex(const TopoDS_Vertex theVertex) {
	myShape = new TopoDS_Vertex(theVertex);
}

TVertex::TVertex(System::IntPtr theVertexIntPtr) {
	// 将 IntPtr 转换为原生指针
	TopoDS_Vertex* pShape = reinterpret_cast<TopoDS_Vertex*>(theVertexIntPtr.ToPointer());
	myShape = pShape;
}

TVertex::TVertex(TopoDS_Vertex* theVertex) {
	myShape = theVertex;
}

TopoDS_Vertex TVertex::GetOCC() {
	return TopoDS::Vertex(*myShape);
}

}
}
}