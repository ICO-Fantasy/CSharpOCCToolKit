﻿#pragma once

class BRepGProp_Face;

namespace OCCTK {
namespace OCC {
//前向声明
namespace Topo {
ref class TFace;
}
namespace Gprop {
ref class GProps;
}
namespace gp {
value struct Pnt;
value struct Vec;
}

namespace BRepGProp {

public ref class BRepGlobalProperties_Face {
public:
    BRepGlobalProperties_Face(Topo::TFace^ face);
public:
    gp::Vec Normal(double U, double V);
private:
    BRepGProp_Face* myGPFace;
protected:
    // 析构函数用于清理非托管资源
    !BRepGlobalProperties_Face() {
        delete myGPFace;
        myGPFace = nullptr;
    }

    // 终结器（finalizer）用于垃圾回收时的清理
    ~BRepGlobalProperties_Face() {
        // 调用析构函数来清理非托管资源
        this->!BRepGlobalProperties_Face();
    }
};

}
}
}