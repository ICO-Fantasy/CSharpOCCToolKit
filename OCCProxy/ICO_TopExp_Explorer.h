#pragma once

//前向声明
class TopExp_Explorer;
namespace OCCTK::OCC {
namespace Topo {
ref class TShape;
}
namespace TopoAbs {
enum class ShapeEnum;
}
}

namespace OCCTK::OCC::TopExp
{

public ref class Explorer
{
private:
    TopExp_Explorer* myExp;
private:
    // 析构函数用于清理非托管资源
    !Explorer() {
        delete myExp;
        myExp = nullptr;
    }

    // 终结器（finalizer）用于垃圾回收时的清理
    ~Explorer() {
        // 调用析构函数来清理非托管资源
        this->!Explorer();
    }
public:
    Explorer(Topo::TShape^ shape, TopoAbs::ShapeEnum toFind);
    Explorer(Topo::TShape^ shape, TopoAbs::ShapeEnum toFind, TopoAbs::ShapeEnum toAvoid);
    bool More();
    void Next();
    Topo::TShape^ Value();
    Topo::TShape^ Current();
    void ReInit();
    Topo::TShape^ ExploredShape();
    int Depth();
    void Clear();
};
}