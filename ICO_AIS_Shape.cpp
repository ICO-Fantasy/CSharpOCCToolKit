#include "ICO_AIS_Shape.h"
#include <AIS_InteractiveContext.hxx>
namespace OCCTK {
namespace OCC {
namespace AIS {

//WAIS_AShape::WAIS_Shape(WTopoDS_Shape^ waShape) {
//	_ptransient = new Handle_Standard_Transient(new AIS_Shape(waShape->GetShape()));
//	_pAis_Shape = (Handle(AIS_Shape)*) _ptransient;
//
//	//_aWShape = waShape;
//}

AShape::AShape(Handle(AIS_InteractiveObject) aInteractive) {
	if (aInteractive->IsKind(STANDARD_TYPE(AIS_Shape))) {
		Handle(AIS_Shape) anAISShape = Handle(AIS_Shape)::DownCast(aInteractive);
		myAShape(new AIS_Shape(*anAISShape));
	}
	throw std::exception("Type error");
}

AShape::AShape(TopoDS_Shape aShape) {
	myAShape(new AIS_Shape(aShape));
}

// 传入TopoDS_Shape的指针（不要轻易使用指针构造方法）
AShape::AShape(System::IntPtr aShapePtr) {
	// 将 IntPtr 转换为原生指针
	TopoDS_Shape* pShape = reinterpret_cast<TopoDS_Shape*>(aShapePtr.ToPointer());
	// 创建新的 AIS_Shape 对象
	myAShape(new AIS_Shape(*pShape));
}

AShape::AShape(TopoDS::TShape^ aShape) {
	myAShape(new AIS_Shape(aShape->GetOCC()));
}

AShape::AShape(Handle(AIS_Shape)aAISShape) {
	myAShape(aAISShape);
}

AShape::AShape(AIS_Shape aAISShape) {
	myAShape(new AIS_Shape(aAISShape));
}

inline void AShape::myAShape(Handle(AIS_Shape)aAISShape) {
	_pAISShape() = aAISShape;
	// 保存原始句柄
	nativeHandle() = _pAISShape();
	//todo debug 
	_pAISShape()->SetDisplayMode(1);
}

TopoDS::TShape^ AShape::TopoShape() {
	return gcnew TopoDS::TShape(_pAISShape()->Shape());
}

void AShape::SetColor(int R, int G, int B) {
	Quantity_Color theColor((double)R / 255.0, (double)G / 255.0, (double)B / 255.0, Quantity_TOC_RGB);
	_pAISShape()->SetColor(theColor);
}

void AShape::SetTransparency(double theFactor) {
	_pAISShape()->SetTransparency(theFactor);

}

bool AShape::HasInteractiveContext() {
	return _pAISShape()->HasInteractiveContext();
}

bool AShape::Equals(System::Object^ obj) {
	// 检查 obj 是否是同一类型
	AShape^ other = dynamic_cast<AShape^>(obj);
	if (other != nullptr) {
		return _pAISShape()->Shape().IsEqual(other->GetOCC()->Shape());
	}
	return false;
}

void AShape::RemoveSelf() {
	Handle(AIS_InteractiveContext) theContext = _pAISShape()->GetContext();
	if (!theContext.IsNull()) {
		theContext->Remove(_pAISShape(), false);
	}
}

Handle(AIS_Shape) AShape::GetOCC() {
	return _pAISShape();
}

Handle(Standard_Transient)AShape::GetStd() {
	return nativeHandle();
}

// 用于底层OCC对象传递，不要轻易使用
System::IntPtr AShape::GetIntPtr() {
	return System::IntPtr(&*_pAISShape());
}

}
}
}
