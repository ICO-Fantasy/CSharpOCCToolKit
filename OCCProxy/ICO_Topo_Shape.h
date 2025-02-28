#pragma once
#include <cstdint>//用来取int32的最大值
#include "ICO_Orientation.h"
#include "ICO_ShapeEnum.h"

class TopoDS_Shape;
namespace OCCTK {
namespace OCC {
namespace gp {
value struct Trsf;
value struct Ax2;
}
namespace Topo {
ref class TEdge;
ref class TVertex;
ref class TFace;
}
}
}

namespace OCCTK {
namespace OCC {
namespace Topo {

public ref class TShape {
public:
    TShape();
    TShape(const TopoDS_Shape theShape);
    TShape(System::IntPtr theShapeIntPtr);
    TShape(TopoDS_Shape* theShape);
    bool IsEqual(TShape^ otherShape);
#pragma region 重载操作符
    // 重载 == 操作符
    static bool operator ==(TShape^ left, TShape^ right) {
        // 检查是否有任何一个对象为 null
        if (Object::ReferenceEquals(left, nullptr) || Object::ReferenceEquals(right, nullptr))
            return Object::ReferenceEquals(left, right);
        return left->IsEqual(right);
    }
    // 为了确保兼容，同时重写 Equals
    virtual bool Equals(System::Object^ obj) override;
    // 重写 GetHashCode
    virtual int GetHashCode() override {
        return HashCode(INT32_MAX);
    }
    // 重载 != 操作符
    static bool operator !=(TShape^ left, TShape^ right) {
        return !(left == right); // 利用重载的 == 操作符
    }
#pragma endregion
    TopoDS_Shape GetOCC();
    System::IntPtr GetPtr();
    int HashCode(int upperBound);
public:
    TopoAbs::ShapeEnum ShapeType();
    TVertex^ AsVertex();
    TEdge^ AsEdge();
    TFace^ AsFace();
public:
    void Reverse();
    void Move(gp::Trsf theT);
    gp::Trsf Location();
    void Location(gp::Ax2 newOrigin);
    TShape^ Located(gp::Ax2 newOrigin);
    property TopoAbs::Orientation Orientation {TopoAbs::Orientation get(); void set(TopoAbs::Orientation orientation); }
protected:
    TopoDS_Shape* myShape;
protected:
    // 析构函数用于清理非托管资源
    !TShape() {
        delete myShape;
        myShape = nullptr;
    }

    // 终结器（finalizer）用于垃圾回收时的清理
    ~TShape() {
        // 调用析构函数来清理非托管资源
        this->!TShape();
    }
};

}
}
}