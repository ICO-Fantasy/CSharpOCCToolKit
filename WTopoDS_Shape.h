#pragma once
#include <TopoDS_Shape.hxx>
namespace OCCTK {
namespace OCC {
namespace TopoDS {
public ref class WTopoDS_Shape
{
public:
	WTopoDS_Shape();
	WTopoDS_Shape(TopoDS_Shape theShape);
	TopoDS_Shape* GetOCC();
private:
	TopoDS_Shape* _nativeHandle;
};
}
}
}


