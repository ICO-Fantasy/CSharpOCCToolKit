#include "WAIS_Shape.h"
namespace OCCTK::OCC::AIS
{
//WAIS_Shape::WAIS_Shape(WTopoDS_Shape^ waShape) {
//	_ptransient = new Handle_Standard_Transient(new AIS_Shape(waShape->GetShape()));
//	_pAis_Shape = (Handle(AIS_Shape)*) _ptransient;
//
//	//_aWShape = waShape;
//}
WAIS_Shape::WAIS_Shape(TopoDS_Shape aShape) {
	_ptransient = new Handle_Standard_Transient(new AIS_Shape(aShape));
	_pAis_Shape = (Handle(AIS_Shape)*) _ptransient;
}
WAIS_Shape::WAIS_Shape(WTopoDS_Shape^ aShape)
{
	_ptransient = new Handle_Standard_Transient(new AIS_Shape(*aShape->GetOCC()));
	_pAis_Shape = (Handle(AIS_Shape)*) _ptransient;
}
WAIS_Shape::WAIS_Shape(Handle(AIS_Shape) aAISShape) {
	_ptransient = new Handle_Standard_Transient(aAISShape);
	_pAis_Shape = (Handle(AIS_Shape)*) _ptransient;
}
WAIS_Shape::WAIS_Shape(AIS_Shape aAISShape) {
	Handle(AIS_Shape) aAISShapeHadle = new AIS_Shape(aAISShape);
	_ptransient = new Handle_Standard_Transient(aAISShapeHadle);
	_pAis_Shape = (Handle(AIS_Shape)*) _ptransient;
}
WTopoDS_Shape^ WAIS_Shape::Shape()
{
	return gcnew WTopoDS_Shape((*_pAis_Shape)->Shape());
}
void WAIS_Shape::SetColor(int R, int G, int B)
{
	Quantity_Color theColor((double)R / 255.0, (double)G / 255.0, (double)B / 255.0, Quantity_TOC_RGB);
	(*_pAis_Shape)->SetColor(theColor);
}
void WAIS_Shape::SetTransparency(double theFactor)
{
	(*_pAis_Shape)->SetTransparency(theFactor);
}
Handle(AIS_Shape)* WAIS_Shape::GetOCC() {
	return _pAis_Shape;
}
Handle(Standard_Transient)* WAIS_Shape::GetStd() {
	return _ptransient;
}
}
