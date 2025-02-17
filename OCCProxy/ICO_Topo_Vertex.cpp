#include "ICO_Topo_Vertex.h"
#include <TopoDS_Vertex.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>
//local
#include "ICO_Pnt.h"
#include "ICO_Topo_Shape.h"
#include "ICO_Exception.h"

using namespace OCCTK::OCC::gp;

namespace OCCTK {
namespace OCC {
namespace Topo {

TVertex::TVertex() {
    myShape = new TopoDS_Vertex();
}

TVertex::TVertex(const TopoDS_Vertex theVertex) {
    myShape = new TopoDS_Vertex(theVertex);
}

TVertex::TVertex(TopoDS_Vertex* theVertex) {
    myShape = theVertex;
}

TopoDS_Vertex TVertex::GetOCC() {
    return TopoDS::Vertex(*myShape);
}

gp::Pnt TVertex::ToPnt() {
    try {
        return Pnt(BRep_Tool::Pnt(GetOCC()));
    }
    CATCH_AND_THROW_OCC_EXCEPTIONS
}

}
}
}