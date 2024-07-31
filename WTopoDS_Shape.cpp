#include "WTopoDS_Shape.h"

namespace OCCTK {
namespace OCC {
namespace TopoDS {

WTopoDS_Shape::WTopoDS_Shape() {
	_nativeHandle = new TopoDS_Shape();
}

WTopoDS_Shape::WTopoDS_Shape(TopoDS_Shape theShape) {
	_nativeHandle = new TopoDS_Shape(theShape);
}

WTopoDS_Shape::WTopoDS_Shape(TopoDS_Shape* theShape) {
	_nativeHandle = theShape;
}

TopoDS_Shape* WTopoDS_Shape::GetOCC() {
	return _nativeHandle;
}

}
}
}