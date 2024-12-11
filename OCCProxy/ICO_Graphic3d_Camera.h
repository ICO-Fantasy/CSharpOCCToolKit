#pragma once
#include <Graphic3d_Camera.hxx>
//local
#include "ICO_BaseObject.h"

namespace OCCTK {
namespace OCC {
namespace Graphic3d {

public ref class Camera :BaseObject {
private:
	Handle(Graphic3d_Camera) myCamera() { return Handle(Graphic3d_Camera)::DownCast(NativeHandle); }
public:
	Camera(const Handle(Graphic3d_Camera)& camera);
	Camera(Camera^ camera);
	Handle(Graphic3d_Camera) GetOCC() { return myCamera(); };
};

}
}
}