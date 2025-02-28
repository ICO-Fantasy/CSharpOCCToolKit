#pragma once

//ǰ������
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
    //! ��ʽת��Ϊ ( w, p, r)
    static operator System::ValueTuple<double, double, double>(WPR wpr) { return System::ValueTuple<double, double, double>(wpr.W, wpr.P, wpr.R); }
public:
    property double W;
    property double P;
    property double R;
    // Deconstruct ����
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
