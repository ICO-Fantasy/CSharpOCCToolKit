#include "WAIS_InteractiveObject.h"
namespace OCCTK {
namespace OCC {
namespace AIS
{

WAIS_InteractiveObject::WAIS_InteractiveObject() {
}

WAIS_InteractiveObject::WAIS_InteractiveObject(Handle(Standard_Transient) nativeHandle) {
	this->nativeHandle() = nativeHandle;
}

bool WAIS_InteractiveObject::IsNull() {
	return this->nativeHandle().IsNull();
}


}
}
}