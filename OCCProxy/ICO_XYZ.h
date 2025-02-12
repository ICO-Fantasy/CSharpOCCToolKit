#pragma once
#include<gp_XYZ.hxx>

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

public value struct XYZ :System::ICloneable {
public:
    XYZ(double x, double y, double z);
    XYZ(gp_XYZ xyz);
    gp_XYZ GetOCC();
    virtual System::Object^ Clone();
    virtual System::String^ ToString() override;
    //! 隐式转换为 gp_XYZ
    static operator gp_XYZ (XYZ xyz) { return xyz.GetOCC(); }
    //! 隐式转换为 ( x, y, z)
    static operator System::ValueTuple<double, double, double>(XYZ xyz) { return System::ValueTuple<double, double, double>(xyz.X, xyz.Y, xyz.Z); }
public:
    property double X;
    property double Y;
    property double Z;
    // Deconstruct 方法
    void Deconstruct([System::Runtime::InteropServices::OutAttribute] double% x,
        [System::Runtime::InteropServices::OutAttribute] double% y,
        [System::Runtime::InteropServices::OutAttribute] double% z) {
        x = X;
        y = Y;
        z = Z;
    }
};

}
}
}
