#include "ICO_AIS_Shape.h"
#include <AIS_InteractiveContext.hxx>

namespace OCCTK {
namespace OCC {
namespace AIS {

//! AShape必须保存InteractiveObject和BaseObject的句柄

AShape::AShape(const Handle(AIS_InteractiveObject) aInteractive) : InteractiveObject(aInteractive) {
	if (aInteractive->IsKind(STANDARD_TYPE(AIS_Shape))) {
		Handle(AIS_Shape) anAISShape = Handle(AIS_Shape)::DownCast(aInteractive);
		myShape() = new AIS_Shape(*anAISShape);

	}
	else {
		throw gcnew System::Exception("InteractiveObject not an AIS_Shape");
	}
}

AShape::AShape(const TopoDS_Shape& aShape) : InteractiveObject(Handle(AIS_Shape)()) {
	myShape() = new AIS_Shape(aShape);

	// 确保使用正确的 Handle
	InteractiveObject::myAISObject() = myShape();
	BaseObject::myHandle() = myShape();
}

// 传入TopoDS_Shape的指针（不要轻易使用指针构造方法）
AShape::AShape(System::IntPtr aShapePtr) : InteractiveObject(Handle(AIS_Shape)()) {
	// 将 IntPtr 转换为原生指针
	TopoDS_Shape* pShape = reinterpret_cast<TopoDS_Shape*>(aShapePtr.ToPointer());
	// 创建新的 AIS_Shape 对象
	myShape() = new AIS_Shape(*pShape);

	// 确保使用正确的 Handle
	InteractiveObject::myAISObject() = myShape();
	BaseObject::myHandle() = myShape();
}

AShape::AShape(TopoDS::TShape^ aShape) : InteractiveObject(Handle(AIS_Shape)()) {
	myShape() = new AIS_Shape(aShape->GetOCC());

	// 确保使用正确的 Handle
	InteractiveObject::myAISObject() = myShape();
	BaseObject::myHandle() = myShape();
}

AShape::AShape(const Handle(AIS_Shape) aAISShape) : InteractiveObject(aAISShape) {
	myShape() = aAISShape;
}

AShape::AShape(const AIS_Shape& aAISShape) : InteractiveObject(Handle(AIS_Shape)(&aAISShape)) {
	myShape() = new AIS_Shape(aAISShape);
}

Handle(AIS_Shape) AShape::GetOCC() {
	return myShape();
}

// 获取指针 (不要轻易使用指针传递)
System::IntPtr AShape::GetIntPtr() {
	return System::IntPtr(&*myShape());
}

TopoDS::TShape^ AShape::Shape() {
	return gcnew TopoDS::TShape(myShape()->Shape());
}

void AShape::SetColor(int R, int G, int B) {
	Quantity_Color theColor((double)R / 255.0, (double)G / 255.0, (double)B / 255.0, Quantity_TOC_RGB);
	myShape()->SetColor(theColor);
}

void AShape::SetTransparency(double theFactor) {
	myShape()->SetTransparency(theFactor);

}

bool AShape::HasInteractiveContext() {
	return myShape()->HasInteractiveContext();
}

Trsf^ AShape::LocalTransformation() {
	return gcnew Trsf(myShape()->LocalTransformation());
}

bool AShape::Equals(System::Object^ obj) {
	// 检查 obj 是否是同一类型
	AShape^ other = dynamic_cast<AShape^>(obj);
	if (other != nullptr) {
		return myShape()->Shape().IsEqual(other->GetOCC()->Shape());
	}
	return false;
}

void AShape::RemoveSelf() {
	Handle(AIS_InteractiveContext) theContext = myShape()->GetContext();
	if (!theContext.IsNull()) {
		theContext->Remove(myShape(), false);
	}
}

}
}
}
