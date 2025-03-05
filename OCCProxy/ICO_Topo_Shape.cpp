#include "ICO_Topo_Shape.h"
#include <Standard_TypeMismatch.hxx>
#include <TopAbs_Orientation.hxx>
#include<TopAbs_ShapeEnum.hxx>
#include <TopLoc_Location.hxx>
#include<TopoDS.hxx>
#include<TopoDS_Shape.hxx>
//local
#include "ICO_Trsf.h"
#include "ICO_Ax2.h"
#include "ICO_Topo_Vertex.h"
#include "ICO_Topo_Edge.h"
#include "ICO_Topo_Face.h"
#include "ICO_Orientation.h"

using namespace OCCTK::OCC::TopoAbs;
using namespace OCCTK::OCC::gp;


namespace OCCTK {
namespace OCC {
namespace Topo {

TShape::TShape() {
    myShape = new TopoDS_Shape();
}

TShape::TShape(TShape^ shape)
{
    myShape = new TopoDS_Shape(*shape->myShape);
}

TShape::TShape(const TopoDS_Shape theShape) {
    myShape = new TopoDS_Shape(theShape);
}

/// <summary>
/// 使用 TopoDS_Shape 指针创建
/// </summary>
/// <param name="theEdgeIntPtr"></param>
/// <remarks>
/// !!! 不要轻易使用指针构造方法 !!!
/// </remarks>
TShape::TShape(System::IntPtr theShapeIntPtr) {
    // 将 IntPtr 转换为原生指针
    TopoDS_Shape* pShape = reinterpret_cast<TopoDS_Shape*>(theShapeIntPtr.ToPointer());
    myShape = pShape;
}

bool TShape::IsEqual(TShape^ otherShape) {
    return myShape->IsEqual(otherShape->GetOCC());
}

bool TShape::Equals(System::Object^ obj) {
    // 检查 obj 是否是同一类型
    TShape^ other = dynamic_cast<TShape^>(obj);
    if (other != nullptr) {
        return IsEqual(other);
    }
    return false;
}

TopoDS_Shape TShape::GetOCC() {
    return *myShape;
}

TopoAbs::ShapeEnum TShape::ShapeType() {
    return TopoAbs::ShapeEnum(myShape->ShapeType());
}

/// <summary>
/// 尝试将基类TShape转换为TVertex
/// </summary>
/// <returns></returns>
TVertex^ TShape::AsVertex() {
    if (myShape->ShapeType() != TopAbs_VERTEX) {
        throw gcnew System::Exception("Shape Mismatch is not TVertex");
    }
    try {
        return gcnew TVertex(TopoDS::Vertex(*myShape));
    }
    catch (Standard_TypeMismatch e) {
        throw gcnew System::Exception(System::String::Format("Type Mismatch: \"{0}\"", (char)e.GetMessageString()));
    }
}

/// <summary>
/// 尝试将基类TShape转换为TEdge
/// </summary>
/// <returns></returns>
TEdge^ TShape::AsEdge() {
    if (myShape->ShapeType() != TopAbs_EDGE) {
        throw gcnew System::Exception("Shape Mismatch is not TEdge");
    }
    try {
        return gcnew TEdge(TopoDS::Edge(*myShape));
    }
    catch (Standard_TypeMismatch e) {
        throw gcnew System::Exception(System::String::Format("Type Mismatch: \"{0}\"", (char)e.GetMessageString()));
    }
}

/// <summary>
/// 尝试将基类TShape转换为TFace
/// </summary>
/// <returns></returns>
TFace^ TShape::AsFace() {
    if (myShape->ShapeType() != TopAbs_FACE) {
        throw gcnew System::Exception("Shape Mismatch is not TFace");
    }
    try {
        return gcnew TFace(TopoDS::Face(*myShape));
    }
    catch (Standard_TypeMismatch e) {
        throw gcnew System::Exception(System::String::Format("Type Mismatch: \"{0}\"", (char)e.GetMessageString()));
    }
}

/// <summary>
/// 反转形状反向
/// </summary>
void TShape::Reverse() {
    myShape->Reverse();
}

/// <summary>
/// 将 Shape 的 Location 右乘 T
/// </summary>
/// <param name="theT"></param>
void TShape::Move(gp::Trsf theT) {
    gp_Trsf occT = theT;
    occT.SetScaleFactor(1.0);
    myShape->Move(TopLoc_Location(occT));
}

/// <summary>
/// 返回从世界原点到Shape原点的坐标变换
/// </summary>
/// <returns></returns>
gp::Trsf TShape::Location() {
    return  gp::Trsf(myShape->Location().Transformation());
}

void TShape::Location(Ax2 newOrigin) {
    auto t = Trsf(Ax2(), newOrigin);
    myShape->Location(TopLoc_Location(t));
}

TShape^ TShape::Located(Ax2 newOrigin) {
    auto t = Trsf(Ax2(), newOrigin);
    return gcnew TShape(myShape->Located(TopLoc_Location(t)));
}

int TShape::HashCode(int upperBound) {
    return myShape->HashCode(upperBound);
}


TopoAbs::Orientation TShape::Orientation::get() {
    return TopoAbs::Orientation(myShape->Orientation());
}

void TShape::Orientation::set(TopoAbs::Orientation orientation) {
    myShape->Orientation(TopAbs_Orientation(orientation));
}

}
}
}