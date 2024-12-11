#include "ICO_Graphic3d_Camera.h"
#include <Graphic3d_Camera.hxx>
//local
#include "ICO_BaseObject.h"

namespace OCCTK {
namespace OCC {
namespace Graphic3d {

Camera::Camera(const Handle(Graphic3d_Camera)& camera) :BaseObject() {
	NativeHandle = camera;
}

Camera::Camera(Camera^ camera) :BaseObject() {
	NativeHandle = camera->GetOCC();
}



}
}
}