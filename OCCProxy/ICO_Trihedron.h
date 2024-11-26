#pragma once
#include "ICO_InteractiveObject.h"
#include <AIS_Trihedron.hxx>
#include <Standard_Handle.hxx>
//包装C++类到托管类
#include <NCollection_Haft.h> 
//Local
#include "ICO_AIS_Shape.h"
#include "ICO_Trsf.h"
#include "ICO_DatumParts.h"
#include "..\Extension\ICO_Color.h"

namespace OCCTK {
namespace OCC {
namespace AIS {

public ref class Trihedron :public InteractiveObject {
private:
	Handle(AIS_Trihedron) myTrihedron() { return Handle(AIS_Trihedron)::DownCast(NativeHandle); }
public:
	Trihedron(double axisSize);
	Trihedron(AShape^ theAIS, double axisSize);
	Trihedron(gp::Trsf theTrsf, double axisSize);
	Handle(AIS_Trihedron) GetOCC() { return myTrihedron(); };
public:
	void SetArrowLength(double value);
	void SetArrowWidth(double value);
	void SetArrowWidth(DatumParts axis, double value);
	void SetColor(DatumParts axis, Extension::Color^ color);
	void SetAspect(int theX, int theY);
private:
	void SetDefaultAxis();
	double ArrowLength = 100.0;
	double ArrowWidth = 2.5;
};

}
}
}

