#pragma once
#include <TopoDS_Shape.hxx>
#include <AIS_Shape.hxx>
#include "WTopoDS_Shape.h"
#include "WAIS_InteractiveObject.h"
using namespace OCCTK::OCC::TopoDS;

namespace OCCTK::OCC::AIS
{
public ref class WAIS_Shape {
public:
	WAIS_Shape(TopoDS_Shape aShape);
	WAIS_Shape(WTopoDS_Shape^ aShape);
	WAIS_Shape(Handle(AIS_Shape) aAISShape);
	WAIS_Shape(AIS_Shape aAISShape);
	WTopoDS_Shape^ Shape();
	void SetColor(int R, int G, int B);
	void SetTransparency(double theFactor);
	Handle(AIS_Shape)* GetOCC();
	Handle(Standard_Transient)* GetStd();
private:
	Handle(AIS_Shape)* _pAis_Shape;
	Handle(Standard_Transient)* _ptransient;
};
}


