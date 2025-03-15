#pragma once
#include<gp_XYZ.hxx>

//ǰ������
namespace OCCTK {
namespace OCC {
namespace gp {
value struct Vec;
value struct Pnt;
value struct Dir;
}
}
}


namespace OCCTK {
namespace OCC {
namespace gp {

public value struct XYZ :System::ICloneable {
public:
    const static XYZ Default = XYZ(0.0, 0.0, 0.0);
public:
    XYZ(double x, double y, double z);
    XYZ(gp_XYZ xyz);
    gp_XYZ GetOCC();
    virtual System::Object^ Clone();
    virtual System::String^ ToString() override;
    //! ��ʽת��Ϊ gp_XYZ
    static operator gp_XYZ (XYZ xyz) { return xyz.GetOCC(); }
private:
    double myX;
    double myY;
    double myZ;
public:
    property double X {double get() { return myX; }};
    property double Y {double get() { return myY; }};
    property double Z {double get() { return myZ; }};
    // �⹹��ֵ
    void Deconstruct([System::Runtime::InteropServices::Out] double% x,
        [System::Runtime::InteropServices::Out] double% y,
        [System::Runtime::InteropServices::Out] double% z) {
        x = X;
        y = Y;
        z = Z;
    }

#pragma region ���ز�����

    bool Equals(XYZ other) {
        return (
            X == other.X &&
            Y == other.Y &&
            Z == other.Z);
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
