#pragma once
#include<gp_XYZ.hxx>

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

public value struct XYZ :System::ICloneable {
public:
    XYZ(double x, double y, double z);
    XYZ(gp_XYZ xyz);
    gp_XYZ GetOCC();
    virtual System::Object^ Clone();
    virtual System::String^ ToString() override;
    //! ��ʽת��Ϊ gp_XYZ
    static operator gp_XYZ (XYZ xyz) { return xyz.GetOCC(); }
    //! ��ʽת��Ϊ ( x, y, z)
    static operator System::ValueTuple<double, double, double>(XYZ xyz) { return System::ValueTuple<double, double, double>(xyz.X, xyz.Y, xyz.Z); }
public:
    property double X;
    property double Y;
    property double Z;
    // Deconstruct ����
    void Deconstruct([System::Runtime::InteropServices::OutAttribute] double% x,
        [System::Runtime::InteropServices::OutAttribute] double% y,
        [System::Runtime::InteropServices::OutAttribute] double% z) {
        x = X;
        y = Y;
        z = Z;
    }

#pragma region ���ز�����

    bool Equals(XYZ other) {
        return (
            X == X &&
            Y == Y &&
            Z == Z);
    }
    static bool operator == (XYZ left, XYZ right) { return left.Equals(right); }//�޾���
    static bool operator != (XYZ left, XYZ right) { return !left.Equals(right); }//�޾���
    static XYZ operator + (XYZ left, XYZ right) {
        return XYZ(
            left.X + right.X,
            left.Y + right.Y,
            left.Z + right.Z);
    }
    static XYZ operator - (XYZ left, XYZ right) {
        return XYZ(
            left.X - right.X,
            left.Y - right.Y,
            left.Z - right.Z);
    }

#pragma endregion

};

}
}
}
