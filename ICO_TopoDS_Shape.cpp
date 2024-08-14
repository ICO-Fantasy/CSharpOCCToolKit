#include "ICO_TopoDS_Shape.h"

namespace OCCTK {
namespace OCC {
namespace TopoDS {

Shape::Shape() {
	_nativeHandle = new TopoDS_Shape();
}

Shape::Shape(TopoDS_Shape theShape) {
	_nativeHandle = new TopoDS_Shape(theShape);
}

Shape::Shape(TopoDS_Shape* theShape) {
	_nativeHandle = theShape;
}

// 挪动shape的原点
void Shape::Move(Trsf^ theT) {
	gp_Trsf occT = theT->GetOCC();
	occT.SetScaleFactor(1.0);
	_nativeHandle->Move(TopLoc_Location(occT));
}

TopoDS_Shape Shape::GetOCC() {
	return *_nativeHandle;
}

}
}
}