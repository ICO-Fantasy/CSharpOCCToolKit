﻿#pragma once
#include "ICO_InteractiveObject.h"
#include <AIS_Trihedron.hxx>
#include <Standard_Handle.hxx>
//包装C++类到托管类
#include <NCollection_Haft.h> 
//Local
#include "ICO_AIS_Shape.h"
#include "ICO_Color.h"
#include "ICO_Trsf.h"

namespace OCCTK {
namespace OCC {
namespace AIS {

public ref class Trihedron :public InteractiveObject {
private:
	Handle(AIS_Trihedron) myTrihedron() { return Handle(AIS_Trihedron)::DownCast(NativeHandle); }
public:
	Trihedron(double axisSize);
	Trihedron(AShape^ theAIS, double axisSize);
	Trihedron(gp::Trsf^ theTrsf, double axisSize);
	void SetArrowLength(double value);
	void SetArrowWidth(double value);
	void SetAspect(int theX, int theY);
	Handle(AIS_Trihedron) GetOCC() { return myTrihedron(); };

private:
	void SetAxis();
	double ArrowLength = 100.0;
	double ArrowWidth = 2.5;
};

}
}
}

