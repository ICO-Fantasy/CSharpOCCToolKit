#include "ICO_AIS_Shape.h"
#include <AIS_InteractiveContext.hxx>
namespace OCCTK {
namespace OCC {
namespace AIS {

//! AShape必须保存InteractiveObject和BaseObject的句柄

AShape::AShape(Handle(AIS_InteractiveObject) aInteractive) {
	if (aInteractive->IsKind(STANDARD_TYPE(AIS_Shape))) {
		Handle(AIS_Shape) anAISShape = Handle(AIS_Shape)::DownCast(aInteractive);
		myAIS() = new AIS_Shape(*anAISShape);

		// 保存原始句柄
		myAISObject() = myAIS();
		nativeHandle() = myAIS();
	}
}

AShape::AShape(TopoDS_Shape aShape) {
	myAIS() = new AIS_Shape(aShape);

	// 保存原始句柄
	myAISObject() = myAIS();
	nativeHandle() = myAIS();
}

// 传入TopoDS_Shape的指针（不要轻易使用指针构造方法）
AShape::AShape(System::IntPtr aShapePtr) {
	// 将 IntPtr 转换为原生指针
	TopoDS_Shape* pShape = reinterpret_cast<TopoDS_Shape*>(aShapePtr.ToPointer());
	// 创建新的 AIS_Shape 对象
	myAIS() = new AIS_Shape(*pShape);

	// 保存原始句柄
	myAISObject() = myAIS();
	nativeHandle() = myAIS();
}

AShape::AShape(TopoDS::TShape^ aShape) {
	myAIS() = new AIS_Shape(aShape->GetOCC());

	// 保存原始句柄
	myAISObject() = myAIS();
	nativeHandle() = myAIS();
}

AShape::AShape(Handle(AIS_Shape)aAISShape) {
	myAIS() = aAISShape;

	// 保存原始句柄
	myAISObject() = myAIS();
	nativeHandle() = myAIS();
}

AShape::AShape(AIS_Shape aAISShape) {
	myAIS() = new AIS_Shape(aAISShape);

	// 保存原始句柄
	myAISObject() = myAIS();
	nativeHandle() = myAIS();
}

Handle(AIS_Shape) AShape::GetOCC() {
	return myAIS();
}

Handle(Standard_Transient)AShape::GetStd() {
	return nativeHandle();
}

// 用于底层OCC对象传递，不要轻易使用
System::IntPtr AShape::GetIntPtr() {
	return System::IntPtr(&*myAIS());
}

TopoDS::TShape^ AShape::Shape() {
	return gcnew TopoDS::TShape(myAIS()->Shape());
}

void AShape::SetColor(int R, int G, int B) {
	Quantity_Color theColor((double)R / 255.0, (double)G / 255.0, (double)B / 255.0, Quantity_TOC_RGB);
	myAIS()->SetColor(theColor);
}

void AShape::SetTransparency(double theFactor) {
	myAIS()->SetTransparency(theFactor);

}

bool AShape::HasInteractiveContext() {
	return myAIS()->HasInteractiveContext();
}

Trsf^ AShape::LocalTransformation() {
	return gcnew Trsf(myAIS()->LocalTransformation());
}

bool AShape::Equals(System::Object^ obj) {
	// 检查 obj 是否是同一类型
	AShape^ other = dynamic_cast<AShape^>(obj);
	if (other != nullptr) {
		return myAIS()->Shape().IsEqual(other->GetOCC()->Shape());
	}
	return false;
}

void AShape::RemoveSelf() {
	Handle(AIS_InteractiveContext) theContext = myAIS()->GetContext();
	if (!theContext.IsNull()) {
		theContext->Remove(myAIS(), false);
	}
}

}
}
}
