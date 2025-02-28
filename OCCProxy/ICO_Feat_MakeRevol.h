#pragma once
#include <BRepFeat_MakeRevol.hxx>

//前向声明
namespace OCCTK {
namespace OCC {
namespace gp {
ref class Trsf;
value struct Ax1;
value struct Pnt;
}
namespace Topo {
ref class TShape;
ref class TVertex;
ref class TEdge;
ref class TWire;
ref class TFace;
}
namespace Geom {
ref class Curve;
}
}
}

namespace OCCTK {
namespace OCC {
namespace BRepFeat {

public ref class MakeRevol {
public:
    MakeRevol();
    MakeRevol(Topo::TShape^ sBase, Topo::TShape^ pBase, Topo::TFace^ skFace, gp::Ax1 axis, int fuse, bool modify);
public:
    Topo::TShape^ Shape();
    //! 隐式转换为 TShape
    static operator Topo::TShape ^ (MakeRevol^ makeRevol) {
        return makeRevol->Shape();  // 隐式调用 Edge 方法
    }
private:
    BRepFeat_MakeRevol* myMaker;
protected:
    // 析构函数用于清理非托管资源
    !MakeRevol() {
        delete myMaker;
        myMaker = nullptr;
    }

    // 终结器（finalizer）用于垃圾回收时的清理
    ~MakeRevol() {
        // 调用析构函数来清理非托管资源
        this->!MakeRevol();
    }
};

}
}
}

