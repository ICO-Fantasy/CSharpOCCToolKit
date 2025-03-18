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
    TVertex(const TopoDS_Vertex theVertex);
    TopoDS_Vertex GetOCC();
    //! 隐式转换为 TopoDS_Vertex
    static operator TopoDS_Vertex (TVertex^ s) {
        return s->GetOCC();
    }
public:
    gp::Pnt ToPnt();
};

}
}
}