#pragma once
#include <TopoDS_Shape.hxx>
//Local
#include "ICO_Trsf.h"
#include "ICO_Ax2.h"

using namespace OCCTK::OCC::gp;
using namespace System;

namespace OCCTK {
namespace OCC {
namespace TopoDS {

public ref class TShape {
public:
	TShape();
	TShape(const TopoDS_Shape theShape);
	TShape(System::IntPtr theShapeIntPtr);
	TShape(TopoDS_Shape* theShape);
	bool IsEqual(TShape^ otherShape);
	virtual bool Equals(System::Object^ obj) override;
	TopoDS_Shape GetOCC();
	System::IntPtr GetPtr();
	void Move(Trsf^ theT);
	Trsf^ Location();
	void Location(Ax2^ newOrigin);
	TShape^ Located(Ax2^ newOrigin);
private:
	TopoDS_Shape* myShape;
};

}
}
}


