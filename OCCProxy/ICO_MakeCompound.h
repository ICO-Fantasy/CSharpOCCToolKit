#pragma once
#include <BRep_Builder.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Shape.hxx>
#include <vector>
//包装C++类到托管类
#include <NCollection_Haft.h> 
//local
#include"ICO_Topo_Shape.h"

namespace OCCTK {
namespace Tool {

public ref class CompoundMaker {
public:
    CompoundMaker();
    CompoundMaker(List<OCC::Topo::TShape^>^ theShapeList);
    OCC::Topo::TShape^ Shape();
    //! 隐式转换为 TShape
    static operator OCC::Topo::TShape ^ (CompoundMaker^ maker) {
        return maker->Shape();  // 隐式调用 Shape 方法
    }
    void Add(OCC::Topo::TShape^ theShape);
private:
    std::vector<TopoDS_Shape>* myShapeList = new std::vector<TopoDS_Shape>;
    BRep_Builder* myBuilder;
private:
    // 析构函数用于清理非托管资源
    !CompoundMaker() {
        delete myBuilder;
        myBuilder = nullptr;

        delete myShapeList;
        myShapeList = nullptr;
    }


    // 终结器（finalizer）用于垃圾回收时的清理
    ~CompoundMaker() {
        // 调用析构函数来清理非托管资源
        this->!CompoundMaker();
    }
};

}
}