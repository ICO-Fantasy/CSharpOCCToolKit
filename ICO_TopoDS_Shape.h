#pragma once
#include <TopoDS_Shape.hxx>
//Local
#include "ICO_Trsf.h"
using namespace OCCTK::OCC::gp;
using namespace System;

namespace OCCTK {
namespace OCC {
namespace TopoDS {

public ref class Shape {
public:
	Shape();
	Shape(TopoDS_Shape theShape);
	Shape(System::IntPtr theShapeIntPtr);
	Shape(TopoDS_Shape* theShape);
	void Move(Trsf^ theT);
	TopoDS_Shape GetOCC();
	System::IntPtr GetPtr();
private:
	TopoDS_Shape* _nativeHandle;
};

}
}
}


