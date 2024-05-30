#include "WTopoDS_Shape.h"

namespace OCCTK::OCC::TopoDS {
WTopoDS_Shape::WTopoDS_Shape() {
	_nativeHandle = new TopoDS_Shape();
}
WTopoDS_Shape::WTopoDS_Shape(TopoDS_Shape theShape)
{
	_nativeHandle = new TopoDS_Shape(theShape);
}
TopoDS_Shape* WTopoDS_Shape::GetOCC()
{
	return _nativeHandle;
}
}