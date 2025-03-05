#pragma once
#include "ICO_Topo_Shape.h"
#include <TopoDS_Vertex.hxx>

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
    TVertex(TVertex^ vertex);
    TVertex(const TopoDS_Vertex theVertex);
    TVertex(TopoDS_Vertex* theVertex);
    TopoDS_Vertex GetOCC();
public:
    gp::Pnt ToPnt();
};

}
}
}