#pragma once

namespace OCCTK {
namespace Extension {
value struct Color;
ref class XShape;
}
namespace OCC {
namespace Topo {
ref class TShape;
}
}
}

namespace OCCTK {
namespace IO {

public ref class STEPExchanger {
public:
    STEPExchanger(OCC::Topo::TShape^ theShape);
    STEPExchanger(System::String^ filePath);
    Extension::XShape^ Shape();
    //! 隐式转换为 TShape
    static operator Extension::XShape ^ (STEPExchanger^ Ex) {
        return Ex->Shape();
    }
public:
    bool SaveToSingleShape(System::String^ filePath);
private:
    void ReadFile(System::String^ filePath);
private:
    Extension::XShape^ myShape;
};


}
}

