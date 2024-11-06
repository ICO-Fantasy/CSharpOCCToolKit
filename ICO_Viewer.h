#pragma once
#include <V3d_Viewer.hxx>
//local
#include "ICO_BaseObject.h"

namespace OCCTK {
namespace OCC {
namespace V3d {
ref class View;
}
namespace OpenGL {
ref class GraphicDriver;
}
namespace Aspect {
enum class GridType;
enum class GridDrawMode;
}
}
}


namespace OCCTK {
namespace OCC {
namespace V3d {

public ref class Viewer :BaseObject {
private:
	Handle(V3d_Viewer) myViewer() { return Handle(V3d_Viewer)::DownCast(NativeHandle); }
public:
	Viewer(OpenGL::GraphicDriver^ theGraphicDriver);
	Viewer(const Handle(V3d_Viewer)& theViewer) :BaseObject(theViewer) {};
	Handle(V3d_Viewer) GetOCC() { return myViewer(); };
	void SetDefaultLight();
	void SetICOLight();

	V3d::View^ CreateView();

	void ActivateGrid(Aspect::GridType type, Aspect::GridDrawMode drawMode);
	void DeactivateGrid();
};

}
}
}

