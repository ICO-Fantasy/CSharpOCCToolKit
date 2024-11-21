#pragma once
#include <AIS_ViewCube.hxx>
//Local
#include "ICO_InteractiveObject.h"

namespace OCCTK {
namespace OCC {
namespace AIS {

public ref class ViewCube :public InteractiveObject {
private:
	Handle(AIS_ViewCube) myViewCube() { return Handle(AIS_ViewCube)::DownCast(NativeHandle); }
public:
	ViewCube();
	ViewCube(double axesRadius);
	Handle(AIS_ViewCube) GetOCC() { return myViewCube(); };

private:
	void SetDefault();
	double myAxisSize = 5.0;
};

}
}
}

