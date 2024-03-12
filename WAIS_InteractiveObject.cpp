#include "WAIS_InteractiveObject.h"
namespace OCCTK::OCC::AIS {
InteractiveObject::InteractiveObject(Handle(Standard_Transient)* nativeHandle)
{
	this->nativeHandle = nativeHandle;
}
bool InteractiveObject::IsNull() {
	if (nativeHandle->IsNull())
		return true;
	else
		return false;
}
Handle(Standard_Transient)* InteractiveObject::GetOCC() {
	return nativeHandle;
}
}