#include "WAIS_InteractiveObject.h"
namespace OCCTK {
namespace OCC {
namespace AIS {

WAIS_InteractiveObject::WAIS_InteractiveObject() {
}

WAIS_InteractiveObject::WAIS_InteractiveObject(Handle(AIS_InteractiveObject)theAISObject)
{
	myAISObject() = theAISObject;
	nativeHandle() = myAISObject();
}

WAIS_InteractiveObject::WAIS_InteractiveObject(Handle(Standard_Transient) nativeHandle) {
	myAISObject() = Handle(AIS_InteractiveObject)::DownCast(nativeHandle);
	this->nativeHandle() = myAISObject();
}

bool WAIS_InteractiveObject::IsNull() {
	return this->nativeHandle().IsNull();
}

Handle(AIS_InteractiveObject) WAIS_InteractiveObject::GetOCC()
{
	return Handle(AIS_InteractiveObject)::DownCast(nativeHandle());
}

Handle(Standard_Transient) WAIS_InteractiveObject::GetStd()
{
	return nativeHandle();
}


}
}
}