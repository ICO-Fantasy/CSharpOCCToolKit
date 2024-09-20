#pragma once
#include <TopoDS_Wire.hxx>
//local
#include "ICO_Topo_Shape.h"

namespace OCCTK {
namespace OCC {
namespace Topo {

public ref class TWire :TShape {
public:
	TWire();
	TWire(const TopoDS_Wire theWire);
	TWire(System::IntPtr theWireIntPtr);
	TWire(TopoDS_Wire* theWire);
	TopoDS_Wire GetOCC();
};

}
}
}