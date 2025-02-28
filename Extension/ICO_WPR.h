#pragma once

//前向声明
namespace OCCTK {
namespace OCC {
namespace gp {
}
}
}


namespace OCCTK {
namespace OCC {
namespace gp {

public value struct WPR :System::ICloneable {
public:
    WPR(double w, double p, double r);
    virtual System::Object^ Clone();
    virtual System::String^ ToString() override;
    //! 隐式转换为 ( w, p, r)
    static operator System::ValueTuple<double, double, double>(WPR wpr) { return System::ValueTuple<double, double, double>(wpr.W, wpr.P, wpr.R); }
public:
    property double W;
    property double P;
    property double R;
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
