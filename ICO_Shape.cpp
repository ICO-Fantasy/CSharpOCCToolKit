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

Handle(AIS_Shape) Shape::GetOCC() {
	return _pAISShape();
}

Handle(Standard_Transient)Shape::GetStd() {
	return nativeHandle();
}

}
}
}
