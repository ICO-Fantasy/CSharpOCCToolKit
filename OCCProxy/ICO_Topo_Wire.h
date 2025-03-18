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
    TopoDS_Wire GetOCC();
    //! 隐式转换为 TopoDS_Wire
    static operator TopoDS_Wire (TWire^ s) {
        return s->GetOCC();
    }
};

}
}
}