#pragma once
#include <AIS_ViewCube.hxx>
#include <Standard_Handle.hxx>
//包装C++类到托管类
#include <NCollection_Haft.h> 
//Local
#include "ICO_InteractiveObject.h"

namespace OCCTK {
namespace OCC {
namespace AIS {

public ref class ViewCube :public InteractiveObject {
public:
	ViewCube(double axisSize);
	Handle(AIS_ViewCube) GetOCC();
	Handle(Standard_Transient) GetStd() override;

private:
	NCollection_Haft<Handle(AIS_ViewCube)> myViewCube;
};

}
}
}

