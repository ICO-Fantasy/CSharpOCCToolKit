#include "ICO_AIS_Shape.h"
#include <AIS_InteractiveContext.hxx>
#include "ICO_Topo_Shape.h"
#include "ICO_Trsf.h"

using namespace OCCTK::OCC::Topo;
using namespace OCCTK::OCC::gp;

namespace OCCTK {
namespace OCC {
namespace AIS {

AShape::AShape(const Handle(AIS_InteractiveObject) aInteractive) : InteractiveObject(aInteractive) {
    if (aInteractive->IsKind(STANDARD_TYPE(AIS_Shape))) {
        Handle(AIS_Shape) anAISShape = Handle(AIS_Shape)::DownCast(NativeHandle);
    }
    else {
        throw gcnew System::Exception("InteractiveObject not an AIS_Shape");
    }
}

AShape::AShape(const TopoDS_Shape& aShape) : InteractiveObject() {
    NativeHandle = new AIS_Shape(aShape);
}

/// <summary>
/// 传入Handle(AIS_Shape)的指针
/// </summary>
/// <param name="aShapePtr"></param>
/// <remarks>
/// !!! 不要轻易使用指针构造方法 !!!
/// </remarks>
AShape::AShape(System::IntPtr aShapePtr) : InteractiveObject() {
    // 将 IntPtr 转换为原生指针
    Handle(AIS_Shape)* pShape = reinterpret_cast<Handle(AIS_Shape)*>(aShapePtr.ToPointer());
    NativeHandle = *pShape;
}

AShape::AShape(Topo::TShape^ aShape) : InteractiveObject() {
    NativeHandle = new AIS_Shape(aShape->GetOCC());
}

/// <summary>
/// 返回正在显示的TShape
/// </summary>
/// <returns></returns>
TShape^ AShape::Shape() {
    return gcnew Topo::TShape(myShape()->Shape());
}

/// <summary>
/// 定义AShape的颜色，不建议直接使用
/// </summary>
/// <param name="R"></param>
/// <param name="G"></param>
/// <param name="B"></param>
void AShape::SetColor(int R, int G, int B) {
    Quantity_Color theColor((double)R / 255.0, (double)G / 255.0, (double)B / 255.0, Quantity_TOC_RGB);
    myShape()->SetColor(theColor);
}

/// <summary>
/// 定义AShape的透明度，不建议直接使用
/// </summary>
/// <param name="theFactor"></param>
void AShape::SetTransparency(double theFactor) {
    myShape()->SetTransparency(theFactor);
}

void AShape::SetShape(Topo::TShape^ newShape) {
    myShape()->SetShape(newShape->GetOCC());
}

/// <summary>
/// 检查两个AISShape的TShape是否相等
/// </summary>
/// <param name="obj"></param>
/// <returns></returns>
bool AShape::Equals(System::Object^ obj) {
    // 检查 obj 是否是同一类型
    AShape^ other = dynamic_cast<AShape^>(obj);
    if (other != nullptr) {
        return myShape()->Shape().IsEqual(other->GetOCC()->Shape());
    }
    return false;
}

}
}
}
