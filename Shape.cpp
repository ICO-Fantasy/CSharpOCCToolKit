#pragma once
#include "Shape.h"
#include <TopoDS_Shape.hxx>
#include <Standard_Handle.hxx>
namespace OCCTK::OCC::TopoDS {
Shape::Shape()
{
	throw gcnew System::NotImplementedException();
}
Shape::Shape(TopoDS_Shape aShape)
{
	Handle(TopoDS_Shape) aShapeHandle = new TopoDS_Shape(aShape);
	myOCCClass() = aShapeHandle;
}
Handle(TopoDS_Shape) Shape::GetOCC() {
	return myOCCClass();
}
}
