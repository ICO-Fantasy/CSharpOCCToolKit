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
    TFace(TopoDS_Face* theFace);
    TopoDS_Face GetOCC();
};

}
}
}