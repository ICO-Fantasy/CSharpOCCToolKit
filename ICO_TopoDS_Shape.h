#pragma once
#include <TopoDS_Shape.hxx>
//Local
#include "ICO_Trsf.h"
using namespace OCCTK::OCC::gp;

namespace OCCTK {
namespace OCC {
namespace TopoDS {

public ref class Shape {
public:
	Shape();
	Shape(TopoDS_Shape theShape);
	Shape(TopoDS_Shape* theShape);
	void Move(Trsf^ theT);
	TopoDS_Shape GetOCC();
private:
	TopoDS_Shape* _nativeHandle;
};

}
}
}


