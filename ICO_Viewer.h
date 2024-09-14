#pragma once
#include <V3d_Viewer.hxx>
#include <OpenGl_GraphicDriver.hxx>
//包装C++类到托管类
#include <NCollection_Haft.h> 
//local
#include "ICO_Aspect_GridType.h"
#include "ICO_BaseObject.h"
#include "ICO_View.h"
#include "ICO_Aspect_GridDrawMode.h"

namespace OCCTK {
namespace OCC {
namespace V3d {

public ref class Viewer :BaseObject {
private:
	Handle(V3d_Viewer) myViewer() { return Handle(V3d_Viewer)::DownCast(NativeHandle); }
public:
	Viewer(Handle(OpenGl_GraphicDriver) theGraphicDriver);
	Viewer(const Handle(V3d_Viewer)& theViewer) :BaseObject(theViewer) {};
	Handle(V3d_Viewer) GetOCC() { return myViewer(); };
	void SetDefaultLight();

	V3d::View^ CreateView();

	void ActivateGrid(Aspect::GridType type, Aspect::GridDrawMode drawMode);
	void DeactivateGrid();
};

}
}
}

