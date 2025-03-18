#pragma once
#include "ICO_Topo_Shape.h"
#include <TopoDS_Face.hxx>

namespace OCCTK {
namespace OCC {
namespace Topo {

public ref class TFace :TShape {
public:
    TFace();
    TFace(const TopoDS_Face theFace);
    TFace(System::IntPtr theFaceIntPtr);
    TopoDS_Face GetOCC();
    //! 隐式转换为 TopoDS_Face
    static operator TopoDS_Face (TFace^ s) {
        return s->GetOCC();
    }
};

}
}
}