#pragma once
#include <TopoDS_Shape.hxx>
//Local
#include "ICO_Trsf.h"
using namespace OCCTK::OCC::gp;
using namespace System;

namespace OCCTK {
namespace OCC {
namespace TopoDS {

public ref class TShape {
public:
	TShape();
	TShape(TopoDS_Shape theShape);
	TShape(System::IntPtr theShapeIntPtr);
	TShape(TopoDS_Shape* theShape);
	void Move(Trsf^ theT);
	bool IsEqual(TShape^ otherShape);
	virtual bool Equals(System::Object^ obj) override;
	TopoDS_Shape GetOCC();
	System::IntPtr GetPtr();
private:
	TopoDS_Shape* _nativeHandle;
};

}
}
}


