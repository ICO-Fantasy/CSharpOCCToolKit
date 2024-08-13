#pragma once
#include "WAIS_InteractiveObject.h"
#include <AIS_Trihedron.hxx>
#include <Standard_Handle.hxx>
//包装C++类到托管类
#include <NCollection_Haft.h> 
//Local
#include "WAIS_Shape.h"
#include "WColor.h"
#include "Wgp_Trsf.h"
#include <Quantity_Color.hxx>

namespace OCCTK {
namespace OCC {
namespace AIS {

public ref class WAIS_Trihedron :public WAIS_InteractiveObject {
public:
	WAIS_Trihedron(float axisSize);
	WAIS_Trihedron(WAIS_Shape^ theAIS, float axisSize);
	WAIS_Trihedron(gp::Wgp_Trsf^ theTrsf, float axisSize);
	void SetArrowLength(double value);
	void SetArrowWidth(double value);
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

