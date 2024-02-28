#include "MakeBox.h"
#include<BRepPrimAPI_MakeBox.hxx>
#include <TopoDS_Shape.hxx>
#include <Standard_Handle.hxx>
#include "Shape.h"
using namespace OCCTK::OCC;
namespace OCCTK::OCC::BRepPrimAPI
{
MakeBox::MakeBox() {
	myOCCClass() = new BRepPrimAPI_MakeBox();
}
MakeBox::MakeBox(const double dx, const double dy, const double dz) {
	myOCCClass() = new BRepPrimAPI_MakeBox(dx, dy, dz);
	TopoDS_Shape aShape = myOCCClass()->Shape();
	myShape() = Handle(aShape);
}
//TopoDS::Shape MakeBox::Shape() {
//	myShape() = myOCCClass()->Shape();
//	return myShape;
//}
Handle(BRepPrimAPI_MakeBox) MakeBox::GetOCC()
{
	return myOCCClass();
}
}