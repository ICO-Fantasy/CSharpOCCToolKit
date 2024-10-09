#include "ICO_AIS_Shape.h"
#include "ICO_InteractiveObject.h"
#include <AIS_Shape.hxx>
#include<AIS_InteractiveContext.hxx>
//local
#include "ICO_InteractiveContext.h"

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
void InteractiveObject::RemoveSelf(bool update) {
	Handle(AIS_InteractiveContext) theContext = myObj()->GetContext();
	if (!theContext.IsNull()) {
		theContext->Remove(myObj(), update);
	}
}
/// <summary>
/// 是否有交互上下文
/// </summary>
/// <returns></returns>
bool InteractiveObject::HasInteractiveContext() {
	return myObj()->HasInteractiveContext();
}

/// <summary>
/// 获取当前交互上下文
/// </summary>
/// <returns></returns>
InteractiveContext^ InteractiveObject::GetContext() {
	if (myObj()->HasInteractiveContext()) {
		return gcnew InteractiveContext(myObj()->GetContext());
	}
	throw gcnew System::Exception("对象没有交互上下文");
}

}
}
}