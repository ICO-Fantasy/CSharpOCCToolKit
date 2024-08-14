#pragma once
#include <V3d_View.hxx>
//包装C++类到托管类
#include <NCollection_Haft.h> 

namespace OCCTK {
namespace OCC {
namespace V3d {

public ref class View {
public:
	View(V3d_View theView);
	View(Handle(V3d_View) theView);
	Handle(V3d_View) getOCC();
private:
	NCollection_Haft<Handle(V3d_View)> myView;
};

}
}
}

