#pragma once
#include <TopoDS_Face.hxx>
#include "ICO_Topo_Shape.h"

namespace OCCTK {
namespace OCC {
namespace Topo {

public ref class TFace :TShape {
public:
	TFace();
	TFace(const TopoDS_Face theFace);
	TFace(System::IntPtr theFaceIntPtr);
	TFace(TopoDS_Face* theFace);
	TopoDS_Face GetOCC();
};

}
}
}