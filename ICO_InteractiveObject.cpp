#include "ICO_AIS_Shape.h"
#include "ICO_InteractiveObject.h"
#include <AIS_Shape.hxx>
namespace OCCTK {
namespace OCC {
namespace AIS {

InteractiveObject::InteractiveObject(Handle(AIS_InteractiveObject) theAISObject) :
	BaseObject(theAISObject) {
	myAISObject() = theAISObject;

}

bool InteractiveObject::IsNull() {
	return myAISObject().IsNull();
}

bool InteractiveObject::IsShape() {
	return myAISObject()->IsKind(STANDARD_TYPE(AIS_Shape));
}

//转换前应该先做类型检查
AShape^ InteractiveObject::AsShape() {
	return gcnew AShape(myAISObject());
}

Handle(AIS_InteractiveObject) InteractiveObject::GetOCC() {
	return myAISObject();
}

Handle(Standard_Transient) InteractiveObject::GetStd() {
	return myHandle();
}


}
}
}