#include "WAIS_Shape.h"
namespace OCCTK::OCC::AIS
{

//WAIS_Shape::WAIS_Shape(WTopoDS_Shape^ waShape) {
//	_ptransient = new Handle_Standard_Transient(new AIS_Shape(waShape->GetShape()));
//	_pAis_Shape = (Handle(AIS_Shape)*) _ptransient;
//
//	//_aWShape = waShape;
//}

WAIS_Shape::WAIS_Shape(Handle(AIS_InteractiveObject) aInteractive)
{
	if (aInteractive->IsKind(STANDARD_TYPE(AIS_Shape)))
	{
		Handle(AIS_Shape) anAISShape = Handle(AIS_Shape)::DownCast(aInteractive);
		_pAISShape() = new AIS_Shape(*anAISShape);
		;
		// 保存原始句柄
		nativeHandle() = aInteractive;
	}
}

WAIS_Shape::WAIS_Shape(TopoDS_Shape aShape) {
	_pAISShape() = new AIS_Shape(aShape);
	// 保存原始句柄
	nativeHandle() = _pAISShape();
}

WAIS_Shape::WAIS_Shape(WTopoDS_Shape^ aShape) {
	_pAISShape() = new AIS_Shape(*aShape->GetOCC());
	// 保存原始句柄
	nativeHandle() = _pAISShape();
}

WAIS_Shape::WAIS_Shape(Handle(AIS_Shape)aAISShape) {
	_pAISShape() = aAISShape;
	// 保存原始句柄
	nativeHandle() = _pAISShape();
}

WAIS_Shape::WAIS_Shape(AIS_Shape aAISShape) {
	_pAISShape() = new AIS_Shape(aAISShape);
	// 保存原始句柄
	nativeHandle() = _pAISShape();
}

WTopoDS_Shape^ WAIS_Shape::Shape() {
	return gcnew WTopoDS_Shape(_pAISShape()->Shape());
}

void WAIS_Shape::SetColor(int R, int G, int B) {
	Quantity_Color theColor((double)R / 255.0, (double)G / 255.0, (double)B / 255.0, Quantity_TOC_RGB);
	_pAISShape()->SetColor(theColor);
}

void WAIS_Shape::SetTransparency(double theFactor) {
	_pAISShape()->SetTransparency(theFactor);

}

bool WAIS_Shape::HasInteractiveContext() {
	return _pAISShape()->HasInteractiveContext();
}

Handle(AIS_Shape) WAIS_Shape::GetOCC() {
	return _pAISShape();
}

Handle(Standard_Transient)WAIS_Shape::GetStd() {
	return nativeHandle();
}

}
