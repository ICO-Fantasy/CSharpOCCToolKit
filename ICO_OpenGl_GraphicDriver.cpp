#include "ICO_OpenGl_GraphicDriver.h"
#include <Aspect_DisplayConnection.hxx>
#include <OpenGl_GraphicDriver.hxx>
//local
#include "ICO_Viewer.h"
using namespace OCCTK::OCC::V3d;

namespace OCCTK {
namespace OCC {
namespace OpenGL {

GraphicDriver::GraphicDriver() {
	Handle(Aspect_DisplayConnection) aDisplayConnection;
	NativeHandle = new OpenGl_GraphicDriver(aDisplayConnection);
}
GraphicDriver::GraphicDriver(Handle(OpenGl_GraphicDriver) theGraphicDriver) {
	NativeHandle = theGraphicDriver;
}

Handle(OpenGl_GraphicDriver) GraphicDriver::GetOCC() {
	return myGraphicDriver();
}

}
}
}