#pragma once

class gp_QuaternionSLerp;
namespace OCCTK {
namespace OCC {
namespace gp {
//前向声明
value struct Quat;

public ref class SlerpCalculator
{
public:
    SlerpCalculator(Quat start, Quat end);
public:
    Quat Interpolate(double percentage);
private:
    gp_QuaternionSLerp* mySlerpCalculator;
protected:
    // 析构函数用于清理非托管资源
    !SlerpCalculator() {
        delete mySlerpCalculator;
        mySlerpCalculator = nullptr;
    }

    // 终结器（finalizer）用于垃圾回收时的清理
    ~SlerpCalculator() {
        // 调用析构函数来清理非托管资源
        this->!SlerpCalculator();
    }
};

}
}
}
