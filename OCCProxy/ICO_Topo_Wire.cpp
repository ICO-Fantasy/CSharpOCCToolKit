#include "ICO_Topo_Wire.h"
#include <TopoDS.hxx>

namespace OCCTK {
namespace OCC {
namespace Topo {

TWire::TWire() {
    myShape = new TopoDS_Wire();
}

TWire::TWire(TWire^ wire)
{
    myShape = new TopoDS_Shape(*wire->myShape);
}

TWire::TWire(const TopoDS_Wire theWire) {
    myShape = new TopoDS_Wire(theWire);
}

TWire::TWire(TopoDS_Wire* theWire) {
    myShape = theWire;
}

TopoDS_Wire TWire::GetOCC() {
    return TopoDS::Wire(*myShape);
}

}
}
}