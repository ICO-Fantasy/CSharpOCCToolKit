#pragma once
#include <AIS_InteractiveObject.hxx>
#include <AIS_Shape.hxx>
#include <TopoDS_Shape.hxx>
//包装C++类到托管类
#include <NCollection_Haft.h> 
//Local
#include "ICO_InteractiveObject.h"

namespace OCCTK {
namespace OCC {
namespace gp {
ref class Trsf;
ref class Ax2;
}
namespace Topo {
ref class TShape;
}
}
}

namespace OCCTK {
namespace OCC {
namespace AIS {

public ref class AShape :public InteractiveObject {
private:
	Handle(AIS_Shape) myShape() { return Handle(AIS_Shape)::DownCast(NativeHandle); }
public:
	AShape(const Handle(AIS_InteractiveObject) aInteractive);
	AShape(const Handle(AIS_Shape) aAISShape) : InteractiveObject(aAISShape) {};
	AShape(const TopoDS_Shape& aShape);
	AShape(System::IntPtr aShapePtr);
	AShape(Topo::TShape^ aShape);
public:
	Handle(AIS_Shape) GetOCC() { return myShape(); };
	System::IntPtr GetIntPtr();
public:
	Topo::TShape^ Shape();
	void SetColor(int R, int G, int B);
	void SetTransparency(double theFactor);
	bool HasInteractiveContext();
	gp::Trsf^ LocalTransformation();
	void RemoveSelf();
	virtual bool Equals(System::Object^ obj) override;
};

}
}
}


