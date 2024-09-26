#include "ICO_AIS_Shape.h"
#include "ICO_InteractiveObject.h"
#include <AIS_Shape.hxx>
#include<AIS_InteractiveContext.hxx>

namespace OCCTK {
namespace OCC {
namespace AIS {


InteractiveObject::InteractiveObject(Handle(AIS_InteractiveObject) theAISObject) :
	BaseObject(theAISObject) {
}
/// <summary>
/// 检查AIS类型
/// </summary>
/// <returns></returns>
bool InteractiveObject::IsShape() {
	return GetOCC()->IsKind(STANDARD_TYPE(AIS_Shape));
}
/// <summary>
/// 从上下文中删除自身
/// </summary>
void InteractiveObject::RemoveSelf() {
	Handle(AIS_InteractiveContext) theContext = Handle(AIS_InteractiveObject)::DownCast(NativeHandle)->GetContext();
	if (!theContext.IsNull()) {
		theContext->Remove(Handle(AIS_InteractiveObject)::DownCast(NativeHandle), false);
	}
}

}
}
}