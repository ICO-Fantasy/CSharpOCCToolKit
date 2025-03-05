#pragma once
#include "ICO_Topo_Shape.h"
#include <TopoDS_Face.hxx>

namespace OCCTK {
namespace OCC {
namespace Topo {

public ref class TFace :TShape {
public:
    TFace();
    TFace(TFace^ face);
    TFace(const TopoDS_Face theFace);
    TFace(System::IntPtr theFaceIntPtr);
    TopoDS_Face GetOCC();
};

}
}
}