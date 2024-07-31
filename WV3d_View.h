#pragma once
#include <Standard_Transient.hxx>
#include <V3d_View.hxx>
//包装C++类到托管类
#include <NCollection_Haft.h> 

namespace OCCTK {
namespace OCC {
namespace V3d {

public ref class WV3d_View
{
public:
	WV3d_View(V3d_View theView);
	WV3d_View(Handle(V3d_View) theView);
	Handle(V3d_View) getOCC();
private:
	NCollection_Haft<Handle(V3d_View)> myView;
};

}
}
}

