#pragma once

namespace OCCTK {
namespace OCC {
namespace Topo {
ref class TShape;
}
}
}

namespace OCCTK {
namespace IO {

public ref class BrepExchanger {
public:
    BrepExchanger(OCC::Topo::TShape^ theShape);
    BrepExchanger(System::String^ filePath);
public:
    void ReadFile(System::String^ filePath);
    bool SaveFile(System::String^ filePath);
    OCC::Topo::TShape^ Shape();
    //! 隐式转换为 TShape
    static operator OCC::Topo::TShape ^ (BrepExchanger^ T) {
        return T->Shape();  // 隐式调用 Shape 方法
    }
private:
    OCC::Topo::TShape^ myShape;
};

}
}