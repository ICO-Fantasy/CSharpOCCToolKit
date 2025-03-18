#pragma once

class BRepBuilderAPI_MakeFace;
namespace OCCTK {
namespace OCC {
namespace gp {
value struct Pln;
value struct Circle;
}
namespace Topo {
ref class TShape;
ref class TFace;
ref class TWire;
}
}
}

namespace OCCTK {
namespace OCC {
namespace BRepBuilderAPI {

public ref class MakeFace {
public:
    MakeFace();
    MakeFace(Topo::TWire^ wire);
    MakeFace(gp::Pln plane);
    MakeFace(gp::Pln plane, double Umin, double Umax, double Vmin, double Vmax);
public:
    Topo::TShape^ Shape();
    Topo::TFace^ Face();
    //! 隐式转换为 TFace
    static operator Topo::TFace ^ (MakeFace^ makeFace) {
        return makeFace->Face();  // 隐式调用 Face 方法
    }
public:
    void Add(Topo::TWire^ wire);
    bool Error();
    bool IsDone();
private:
    BRepBuilderAPI_MakeFace* myMaker;
protected:
    // 终结器（finalizer）用于垃圾回收时的清理
    !MakeFace() {
        delete myMaker;
        myMaker = nullptr;
    }

    // 析构函数用于清理非托管资源
    ~MakeFace() {
        // 调用析构函数来清理非托管资源
        this->!MakeFace();
    }
};

}
}
}