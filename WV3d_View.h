#pragma once
#include <Standard_Transient.hxx>
#include <V3d_View.hxx>

namespace OCCTK {
namespace OCC {
namespace V3d {

public ref class WV3d_View
{
public:
	WV3d_View(V3d_View theView);
	WV3d_View(Handle(V3d_View) theView);
	Handle(V3d_View)* getOCC();
private:
	Handle(V3d_View)* myView;
	Handle(Standard_Transient)* _ptransient;
};

}
}
}

