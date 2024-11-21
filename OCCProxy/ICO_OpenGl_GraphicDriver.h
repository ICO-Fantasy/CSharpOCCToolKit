#pragma once
#include<OpenGl_GraphicDriver.hxx>
//local
#include"ICO_BaseObject.h"

namespace OCCTK {
namespace OCC {
namespace OpenGL {

public ref class GraphicDriver :BaseObject {
private:
	Handle(OpenGl_GraphicDriver) myGraphicDriver() { return Handle(OpenGl_GraphicDriver)::DownCast(NativeHandle); }
public:
	GraphicDriver();
	GraphicDriver(Handle(OpenGl_GraphicDriver) theGraphicDriver);
	Handle(OpenGl_GraphicDriver) GetOCC();
};
}
}
}
