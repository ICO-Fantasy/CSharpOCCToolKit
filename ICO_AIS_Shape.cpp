#include "ICO_AIS_Shape.h"

namespace OCCTK {
namespace OCC {
namespace AIS {

//WAIS_Shape::WAIS_Shape(WTopoDS_Shape^ waShape) {
//	_ptransient = new Handle_Standard_Transient(new AIS_Shape(waShape->GetShape()));
//	_pAis_Shape = (Handle(AIS_Shape)*) _ptransient;
//
//	//_aWShape = waShape;
//}

Shape::Shape(Handle(AIS_InteractiveObject) aInteractive) {
	if (aInteractive->IsKind(STANDARD_TYPE(AIS_Shape))) {
		Handle(AIS_Shape) anAISShape = Handle(AIS_Shape)::DownCast(aInteractive);
		_pAISShape() = new AIS_Shape(*anAISShape);
		;
		// 保存原始句柄
		nativeHandle() = aInteractive;
	}
}

Shape::Shape(TopoDS_Shape aShape) {
	_pAISShape() = new AIS_Shape(aShape);
	// 保存原始句柄
	nativeHandle() = _pAISShape();
}

// 传入TopoDS_Shape的指针（不要轻易使用指针构造方法）
Shape::Shape(System::IntPtr aShapePtr) {
	// 将 IntPtr 转换为原生指针
	TopoDS_Shape* pShape = reinterpret_cast<TopoDS_Shape*>(aShapePtr.ToPointer());
	// 创建新的 AIS_Shape 对象
	_pAISShape() = new AIS_Shape(*pShape);

	// 保存原始句柄
	nativeHandle() = _pAISShape();
}

Shape::Shape(TopoDS::Shape^ aShape) {
	_pAISShape() = new AIS_Shape(aShape->GetOCC());
	// 保存原始句柄
	nativeHandle() = _pAISShape();
}

Shape::Shape(Handle(AIS_Shape)aAISShape) {
	_pAISShape() = aAISShape;
	// 保存原始句柄
	nativeHandle() = _pAISShape();
}

Shape::Shape(AIS_Shape aAISShape) {
	_pAISShape() = new AIS_Shape(aAISShape);
	// 保存原始句柄
	nativeHandle() = _pAISShape();
}

TopoDS::Shape^ Shape::TopoShape() {
	return gcnew TopoDS::Shape(_pAISShape()->Shape());
}

void Shape::SetColor(int R, int G, int B) {
	Quantity_Color theColor((double)R / 255.0, (double)G / 255.0, (double)B / 255.0, Quantity_TOC_RGB);
	_pAISShape()->SetColor(theColor);
}

void Shape::SetTransparency(double theFactor) {
	_pAISShape()->SetTransparency(theFactor);

}

bool Shape::HasInteractiveContext() {
	return _pAISShape()->HasInteractiveContext();
}

bool Shape::Equals(System::Object^ obj) {
	// 检查 obj 是否是同一类型
	Shape^ other = dynamic_cast<Shape^>(obj);
	if (other != nullptr) {
		return _pAISShape()->Shape().IsEqual(other->GetOCC()->Shape());
	}
	return false;
}

Handle(AIS_Shape) Shape::GetOCC() {
	return _pAISShape();
}

Handle(Standard_Transient)Shape::GetStd() {
	return nativeHandle();
}

// 用于底层OCC对象传递，不要轻易使用
System::IntPtr Shape::GetIntPtr() {
	return System::IntPtr(&*_pAISShape());
}

}
}
}
