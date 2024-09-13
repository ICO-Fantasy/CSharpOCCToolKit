#include "ICO_TopoDS_Shape.h"

namespace OCCTK {
namespace OCC {
namespace TopoDS {

TShape::TShape() {
	myShape = new TopoDS_Shape();
}

TShape::TShape(const TopoDS_Shape theShape) {
	myShape = new TopoDS_Shape(theShape);
}

TShape::TShape(TopoDS_Shape* theShape) {
	myShape = theShape;
}
TShape::TShape(System::IntPtr theShapeIntPtr) {
	// 将 IntPtr 转换为原生指针
	TopoDS_Shape* pShape = reinterpret_cast<TopoDS_Shape*>(theShapeIntPtr.ToPointer());
	myShape = pShape;
}

bool TShape::IsEqual(TShape^ otherShape) {
	return myShape->IsEqual(otherShape->GetOCC());
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
	return *myShape;
}

System::IntPtr TShape::GetPtr() {
	return System::IntPtr(myShape);
}

/// <summary>
/// 将 Shape 的 Location 右乘 T
/// </summary>
/// <param name="theT"></param>
void TShape::Move(Trsf^ theT) {
	gp_Trsf occT = theT->GetOCC();
	occT.SetScaleFactor(1.0);
	myShape->Move(TopLoc_Location(occT));
}

/// <summary>
/// 返回从世界原点到Shape原点的坐标变换
/// </summary>
/// <returns></returns>
Trsf^ TShape::Location() {
	return gcnew Trsf(myShape->Location().Transformation());
}

void TShape::Location(Ax2^ newOrigin) {
	auto t = gcnew Trsf(newOrigin);
	myShape->Location(TopLoc_Location(t->GetOCC()));
}

TShape^ TShape::Located(Ax2^ newOrigin) {
	auto t = gcnew Trsf(newOrigin);
	return gcnew TShape(myShape->Located(TopLoc_Location(t->GetOCC())));
}


}
}
}