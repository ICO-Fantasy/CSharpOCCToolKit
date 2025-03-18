#pragma once

//前向声明
namespace OCCTK {
namespace OCC {
namespace gp {
value struct Quat;
}
}
}

class gp_Quaternion;

namespace OCCTK {
namespace OCC {
namespace gp {

public value struct WPR :System::ICloneable {
public:
    WPR(double w, double p, double r);
    virtual System::Object^ Clone();
    virtual System::String^ ToString() override;
    //! 隐式转换为 gp_Vec
    static operator Quat (WPR wpr);
    static operator gp_Quaternion (WPR wpr);
private:
    double myW;
    double myP;
    double myR;
public:
    property double W {double get() { return myW; }};
    property double P {double get() { return myP; }};
    property double R {double get() { return myR; }};
    // Deconstruct 方法
    void Deconstruct([System::Runtime::InteropServices::OutAttribute] double% w,
        [System::Runtime::InteropServices::OutAttribute] double% p,
        [System::Runtime::InteropServices::OutAttribute] double% r) {
        w = W;
        p = P;
        r = R;
    }
};

}
}
}
