#pragma once
#include <V3d_Viewer.hxx>
#include <OpenGl_GraphicDriver.hxx>
//包装C++类到托管类
#include <NCollection_Haft.h> 
//local
#include "ICO_Aspect_GridType.h"
#include "ICO_Aspect_GridDrawMode.h"

namespace OCCTK {
namespace OCC {
namespace V3d {

public ref class Viewer {
public:
	Viewer(Handle(OpenGl_GraphicDriver) theGraphicDriver);
	Viewer(Handle(V3d_Viewer) theViewer);
	Handle(V3d_Viewer) GetOCC();
	void SetDefaultLight();
	void ActivateGrid(Aspect::GridType type, Aspect::GridDrawMode drawMode);
	void DeactivateGrid();
private:
	NCollection_Haft<Handle(V3d_Viewer)> myViewer;
};

}
}
}

