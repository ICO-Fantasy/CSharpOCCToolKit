#pragma once
#include <TopoDS_Shape.hxx>
#include <AIS_Shape.hxx>
#include "WAIS_InteractiveObject.h"

namespace OCCTK::OCC::AIS
{
public ref class WAIS_Shape {
public:
	WAIS_Shape(TopoDS_Shape aShape);
	WAIS_Shape(Handle(AIS_Shape) aAISShape);
	WAIS_Shape(AIS_Shape aAISShape);
	Handle(AIS_Shape)* GetOCC();
	Handle(Standard_Transient)* GetStd();
private:
	Handle(AIS_Shape)* _pAis_Shape;
	Handle(Standard_Transient)* _ptransient;
};
}


