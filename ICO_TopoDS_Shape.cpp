#include "ICO_TopoDS_Shape.h"

namespace OCCTK {
namespace OCC {
namespace TopoDS {

TShape::TShape() {
	_nativeHandle = new TopoDS_Shape();
}

TShape::TShape(const TopoDS_Shape theShape) {
	_nativeHandle = new TopoDS_Shape(theShape);
}

TShape::TShape(TopoDS_Shape* theShape) {
	_nativeHandle = theShape;
}
TShape::TShape(System::IntPtr theShapeIntPtr) {
	// 将 IntPtr 转换为原生指针
	TopoDS_Shape* pShape = reinterpret_cast<TopoDS_Shape*>(theShapeIntPtr.ToPointer());
	_nativeHandle = pShape;
}

// 挪动shape的原点
void TShape::Move(Trsf^ theT) {
	gp_Trsf occT = theT->GetOCC();
	occT.SetScaleFactor(1.0);
	_nativeHandle->Move(TopLoc_Location(occT));
}

bool TShape::IsEqual(TShape^ otherShape) {
	return _nativeHandle->IsEqual(otherShape->GetOCC());
}

bool TShape::Equals(System::Object^ obj) {
	// 检查 obj 是否是同一类型
	TShape^ other = dynamic_cast<TShape^>(obj);
	if (other != nullptr) {
		return IsEqual(other);
	}
	return false;
}

TopoDS_Shape TShape::GetOCC() {
	return *_nativeHandle;
}

System::IntPtr TShape::GetPtr() {
	return System::IntPtr(_nativeHandle);
}

}
}
}