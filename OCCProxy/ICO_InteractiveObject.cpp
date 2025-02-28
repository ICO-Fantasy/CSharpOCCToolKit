#include "ICO_InteractiveObject.h"
#include<AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
#include <Graphic3d_ZLayerId.hxx>
//local
#include "ICO_InteractiveContext.h"
#include "ICO_Trsf.h"
#include "ICO_ZLayerId.h"

namespace OCCTK {
namespace OCC {
namespace AIS {

InteractiveObject::InteractiveObject(Handle(AIS_InteractiveObject) theAISObject) :
    BaseObject(theAISObject) {
}

/// <summary>
/// 设置自身的变换（从原始状态到当前显示状态的变换）
/// </summary>
/// <param name="trsf"></param>
void InteractiveObject::SetLocalTransformation(gp::Trsf^ trsf) {
    GetOCC()->SetLocalTransformation(trsf);
}

void InteractiveObject::SetZLayer(Graphic3d::ZLayerId id) {
    GetOCC()->SetZLayer(Graphic3d_ZLayerId((int)id));
}

void InteractiveObject::SetZLayer(int Zlayerid) {
    GetOCC()->SetZLayer(Zlayerid);
}

/// <summary>
/// 返回自身的变换（从原始状态到当前显示状态的变换）
/// </summary>
/// <returns></returns>
gp::Trsf^ InteractiveObject::LocalTransformation() {
    return gcnew gp::Trsf(GetOCC()->LocalTransformation());
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
    if (!myObj()->HasInteractiveContext()) {
        return;
    }
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