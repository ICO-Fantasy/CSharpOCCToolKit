﻿#include "ICO_AIS_Shape.h"
#include "ICO_InteractiveObject.h"
#include <AIS_Shape.hxx>
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

}
}
}