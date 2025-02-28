#pragma once
#include<Bnd_Box.hxx>

namespace OCCTK {
namespace OCC {
namespace gp {
value struct Pnt;
}
}
}

namespace OCCTK {
namespace OCC {
namespace Bnd {

public ref class AABB {
public:
    AABB();
    AABB(Bnd_Box* box);
    property double XMin {double get() { return Get()[0]; }}
    property double YMin {double get() { return Get()[1]; }}
    property double ZMin {double get() { return Get()[2]; }}
    property double XMax {double get() { return Get()[3]; }}
    property double YMax {double get() { return Get()[4]; }}
    property double ZMax {double get() { return Get()[5]; }}
    property double Gap {double get() { return myBox->GetGap(); } void set(double value) { myBox->SetGap(value); }}
    gp::Pnt CornerMin();
    gp::Pnt CornerMax();
private:
    array<double>^ Get();
    Bnd_Box* myBox;
protected:
    // 析构函数用于清理非托管资源
    !AABB() {
        delete myBox;
        myBox = nullptr;
    }
    // 终结器（finalizer）用于垃圾回收时的清理
    ~AABB() {
        // 调用析构函数来清理非托管资源
        this->!AABB();
    }
};

}
}
}

