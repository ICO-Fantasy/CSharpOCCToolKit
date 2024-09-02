#pragma once
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
public:
	Trihedron(float axisSize);
	Trihedron(AShape^ theAIS, float axisSize);
	Trihedron(gp::Trsf^ theTrsf, float axisSize);
	void SetArrowLength(double value);
	void SetArrowWidth(double value);
	void SetRightLower();
	Handle(AIS_Trihedron) GetOCC();
	Handle(Standard_Transient) GetStd() override;

private:
	void SetAxis();
	double ArrowLength = 100.0;
	double ArrowWidth = 2.5;
	NCollection_Haft<Handle(AIS_Trihedron)> myTrihedron;
};

}
}
}

