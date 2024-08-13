#pragma once
#include <AIS_ViewCube.hxx>
#include <Standard_Handle.hxx>
#include "WAIS_InteractiveObject.h"
//包装C++类到托管类
#include <NCollection_Haft.h> 

namespace OCCTK {
namespace OCC {
namespace AIS {

public ref class WAIS_ViewCube :public WAIS_InteractiveObject {
public:
	WAIS_ViewCube(float axisSize);
	Handle(AIS_ViewCube) GetOCC();
	Handle(Standard_Transient) GetStd() override;

private:
	NCollection_Haft<Handle(AIS_ViewCube)> myViewCube;
};

}
}
}

