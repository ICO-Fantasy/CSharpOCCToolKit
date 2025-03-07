#include "ICO_Topo_Edge.h"
#include <TopoDS.hxx>

namespace OCCTK {
namespace OCC {
namespace Topo {

TEdge::TEdge() {
    myShape = new TopoDS_Edge();
    //DebugID = GetHashCode();
}

//TEdge::TEdge(TEdge% edge)
//{
//    myShape = &edge.GetOCC();
//}

TEdge::TEdge(const TopoDS_Edge theEdge) {
    myShape = new TopoDS_Edge(theEdge);
    //DebugID = GetHashCode();
}

TopoDS_Edge TEdge::GetOCC() {
    return TopoDS::Edge(*myShape);
}

}
}
}