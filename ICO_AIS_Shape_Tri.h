#pragma once
#include <AIS_InteractiveObject.hxx>
#include <AIS_MultipleConnectedInteractive.hxx>
#include <AIS_Shape.hxx>
#include <Quantity_Color.hxx>
//包装C++类到托管类
#include <NCollection_Haft.h> 
//Local
#include "ICO_AIS_Shape.h"
#include "ICO_InteractiveObject.h"
#include "ICO_TopoDS_Shape.h"
#include <AIS_Trihedron.hxx>

namespace OCCTK {
namespace Extension {

public ref class AShapeTri :public OCC::AIS::InteractiveObject {
public:
	AShapeTri(OCC::TopoDS::TShape^ theShape);
	Handle(AIS_InteractiveObject) GetOCC();
private:
	inline void MakeArrow(const gp_Dir theDir, const double arrowLength, Quantity_Color theColor);
private:
	double myLen = 0.0;
	NCollection_Haft<Handle(AIS_MultipleConnectedInteractive)> myAISTri;
	NCollection_Haft<Handle(AIS_Shape)> myAIS;
	NCollection_Haft<Handle(AIS_Trihedron)> myTri;

};

}
}