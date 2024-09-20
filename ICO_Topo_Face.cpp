#include "ICO_Topo_Face.h"
#include <TopoDS.hxx>

namespace OCCTK {
namespace OCC {
namespace Topo {

TFace::TFace() {
	myShape = new TopoDS_Face();
}

TFace::TFace(const TopoDS_Face theFace) {
	myShape = new TopoDS_Face(theFace);
}

TFace::TFace(System::IntPtr theFaceIntPtr) {
	// 将 IntPtr 转换为原生指针
	TopoDS_Face* pShape = reinterpret_cast<TopoDS_Face*>(theFaceIntPtr.ToPointer());
	myShape = pShape;
}

TFace::TFace(TopoDS_Face* theFace) {
	myShape = theFace;
}

TopoDS_Face TFace::GetOCC() {
	return TopoDS::Face(*myShape);
}

}
}
}