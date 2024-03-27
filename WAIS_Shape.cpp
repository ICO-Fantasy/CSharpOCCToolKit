
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
	//_aWShape = gcnew WTopoDS_Shape(aShape);
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
Handle(AIS_Shape)* WAIS_Shape::GetOCC() {
	return _pAis_Shape;
}
Handle(Standard_Transient)* WAIS_Shape::GetStd() {
	return _ptransient;
}
}
