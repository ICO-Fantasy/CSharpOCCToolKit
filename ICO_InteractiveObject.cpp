#include "ICO_InteractiveObject.h"
namespace OCCTK {
namespace OCC {
namespace AIS {

InteractiveObject::InteractiveObject() {
}

InteractiveObject::InteractiveObject(Handle(AIS_InteractiveObject)theAISObject) {
	myAISObject() = theAISObject;

	//保存原始句柄
	nativeHandle() = myAISObject();
}

InteractiveObject::InteractiveObject(Handle(Standard_Transient) nativeHandle) {
	myAISObject() = Handle(AIS_InteractiveObject)::DownCast(nativeHandle);

	//保存原始句柄
	this->nativeHandle() = myAISObject();
}

bool InteractiveObject::IsNull() {
	return nativeHandle().IsNull();
}

Handle(AIS_InteractiveObject) InteractiveObject::GetOCC() {
	return myAISObject();
}

Handle(Standard_Transient) InteractiveObject::GetStd() {
	return nativeHandle();
}


}
}
}