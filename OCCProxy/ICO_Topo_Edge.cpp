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

TopoDS_Edge TEdge::GetOCC() {
    return TopoDS::Edge(*myShape);
}

}
}
}