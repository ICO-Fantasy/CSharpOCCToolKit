#pragma once
#include "WAIS_InteractiveObject.h"
#include "WTopoDS_Shape.h"
#include <AIS_InteractiveObject.hxx>
#include <AIS_Shape.hxx>
#include <Standard_Transient.hxx>
#include <TopoDS_Shape.hxx>
//包装C++类到托管类
#include <NCollection_Haft.h> 

using namespace OCCTK::OCC::TopoDS;

namespace OCCTK {
namespace OCC {
namespace AIS
{

public ref class WAIS_Shape :public WAIS_InteractiveObject {
public:
	WAIS_Shape(Handle(AIS_InteractiveObject) aInteractive);
	WAIS_Shape(TopoDS_Shape aShape);
	WAIS_Shape(WTopoDS_Shape^ aShape);
	WAIS_Shape(Handle(AIS_Shape) aAISShape);
	WAIS_Shape(AIS_Shape aAISShape);
	WTopoDS_Shape^ Shape();
	void SetColor(int R, int G, int B);
	void SetTransparency(double theFactor);
	bool HasInteractiveContext();
	Handle(AIS_Shape) GetOCC();
	Handle(Standard_Transient) GetStd() override;
private:
	NCollection_Haft<Handle(AIS_Shape)> _pAISShape;
};

}
}
}


