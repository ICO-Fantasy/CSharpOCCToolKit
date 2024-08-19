#pragma once
#include <AIS_InteractiveObject.hxx>
#include <AIS_Shape.hxx>
#include <Standard_Transient.hxx>
#include <TopoDS_Shape.hxx>
//包装C++类到托管类
#include <NCollection_Haft.h> 
//Local
#include "ICO_TopoDS_Shape.h"
#include "ICO_InteractiveObject.h"

using namespace OCCTK::OCC::TopoDS;
using namespace System;

namespace OCCTK {
namespace OCC {
namespace AIS {

public ref class Shape :public InteractiveObject {
public:
	Shape(Handle(AIS_InteractiveObject) aInteractive);
	Shape(TopoDS_Shape aShape);
	Shape(System::IntPtr aShapePtr);
	Shape(TopoDS::Shape^ aShape);
	Shape(Handle(AIS_Shape) aAISShape);
	Shape(AIS_Shape aAISShape);
	TopoDS::Shape^ TopoShape();
	void SetColor(int R, int G, int B);
	void SetTransparency(double theFactor);
	bool HasInteractiveContext();
	virtual bool Equals(System::Object^ obj) override;
	Handle(AIS_Shape) GetOCC();
	Handle(Standard_Transient) GetStd() override;
	System::IntPtr GetIntPtr();
private:
	NCollection_Haft<Handle(AIS_Shape)> _pAISShape;
};

}
}
}


