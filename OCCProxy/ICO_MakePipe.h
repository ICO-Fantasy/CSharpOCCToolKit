#pragma once
#include<BRepOffsetAPI_MakePipe.hxx>

//前向声明
namespace OCCTK {
namespace OCC {
namespace Topo {
ref class TWire;
ref class TShape;
}
}
}

namespace OCCTK {
namespace OCC {
namespace BRepOffsetAPI {

public ref class MakePipe {
public:
    MakePipe(Topo::TWire^ wire, Topo::TShape^ shape);
public:
    Topo::TShape^ Shape();
    //! 隐式转换为 TShape
    static operator Topo::TShape ^ (MakePipe^ makePipe) {
        return makePipe->Shape();  // 隐式调用 Edge 方法
    }
private:
    BRepOffsetAPI_MakePipe* myMaker;
protected:
    // 析构函数用于清理非托管资源
    !MakePipe() {
        delete myMaker;
        myMaker = nullptr;
    }

    // 终结器（finalizer）用于垃圾回收时的清理
    ~MakePipe() {
        // 调用析构函数来清理非托管资源
        this->!MakePipe();
    }
};

}
}
}


