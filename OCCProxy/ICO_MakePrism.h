#pragma once

namespace OCCTK {
namespace OCC {
namespace gp {
value struct Pnt;
value struct Ax2;
value struct Vec;
}
namespace Topo {
ref class TShape;
}
}
}
class BRepPrimAPI_MakePrism;
namespace OCCTK {
namespace OCC {
namespace BRepPrimAPI {

public ref class MakePrism {
public:
    MakePrism(Topo::TShape^ shape, gp::Vec vec);
    MakePrism(Topo::TShape^ shape, gp::Vec vec, bool copy);
    MakePrism(Topo::TShape^ shape, gp::Vec vec, bool copy , bool canonize);
    Topo::TShape^ Shape();
    //! 隐式转换为 TShape
    static operator Topo::TShape ^ (MakePrism^ mp) {
        return mp->Shape();  // 隐式调用 Shape 方法
    }
private:
    BRepPrimAPI_MakePrism* myMaker;
protected:
    // 析构函数用于清理非托管资源
    !MakePrism() {
        delete myMaker;
        myMaker = nullptr;
    }

    // 终结器（finalizer）用于垃圾回收时的清理
    ~MakePrism() {
        // 调用析构函数来清理非托管资源
        this->!MakePrism();
    }
};

}
}
}