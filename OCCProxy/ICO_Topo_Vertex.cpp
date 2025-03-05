#include "ICO_Topo_Vertex.h"
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
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

TVertex::TVertex(TVertex^ vertex)
{
    myShape = new TopoDS_Vertex(vertex->GetOCC());
}

TVertex::TVertex(const TopoDS_Vertex theVertex) {
    myShape = new TopoDS_Vertex(theVertex);
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