#pragma once
#include <TopoDS_Vertex.hxx>
#include "ICO_Topo_Shape.h"

class TopoDS_Vertex;
namespace OCCTK {
namespace OCC {
//namespace Topo {
//ref class TShape;
//}
namespace gp {
value struct Pnt;
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
    TVertex(TopoDS_Vertex* theVertex);
    TopoDS_Vertex GetOCC();
public:
    gp::Pnt ToPnt();
};

}
}
}