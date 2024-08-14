#include "ICO_InteractiveObject.h"
namespace OCCTK {
namespace OCC {
namespace AIS {

InteractiveObject::InteractiveObject() {
}

InteractiveObject::InteractiveObject(Handle(AIS_InteractiveObject)theAISObject) {
	myAISObject() = theAISObject;
	nativeHandle() = myAISObject();
}

InteractiveObject::InteractiveObject(Handle(Standard_Transient) nativeHandle) {
	myAISObject() = Handle(AIS_InteractiveObject)::DownCast(nativeHandle);
	this->nativeHandle() = myAISObject();
}

bool InteractiveObject::IsNull() {
	return this->nativeHandle().IsNull();
}

Handle(AIS_InteractiveObject) InteractiveObject::GetOCC() {
	return Handle(AIS_InteractiveObject)::DownCast(nativeHandle());
}

Handle(Standard_Transient) InteractiveObject::GetStd() {
	return nativeHandle();
}


}
}
}