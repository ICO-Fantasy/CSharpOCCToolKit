#pragma once
//值类型作为构造函数成员需要完整定义
#include "ICO_Ax2.h"

class BRepPrimAPI_MakeCylinder;
namespace OCCTK {
namespace OCC {
//前向声明
namespace Topo {
ref class TShape;
}
namespace BRepPrimAPI {

public ref class MakeCylinder {
public:
    MakeCylinder(double r, double h);
    MakeCylinder(double r, double h, double angle);
    MakeCylinder(gp::Ax2 theAxis, double r, double h);
    MakeCylinder(gp::Ax2 theAxis, double r, double h, double angle);
    Topo::TShape^ Shape();
    //! 隐式转换为 TShape
    static operator Topo::TShape ^ (MakeCylinder^ maker) {
        return maker->Shape();  // 隐式调用 Shape 方法
    }
private:
    BRepPrimAPI_MakeCylinder* myMaker;
protected:
    // 析构函数用于清理非托管资源
    !MakeCylinder() {
        delete myMaker;
        myMaker = nullptr;
    }

    // 终结器（finalizer）用于垃圾回收时的清理
    ~MakeCylinder() {
        // 调用析构函数来清理非托管资源
        this->!MakeCylinder();
    }
};

}
}
}