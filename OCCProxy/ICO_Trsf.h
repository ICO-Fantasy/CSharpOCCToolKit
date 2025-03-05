#pragma once
//在构造函数中使用的值对象需要直接引入
#include "ICO_Ax2.h"
#include "ICO_Quaternion.h"
#include "ICO_Vec.h"
#include <gp_Trsf.hxx>

namespace OCCTK {
namespace OCC {
namespace gp {

//+ Trsf并非gp_Trsf的完整实现，不包含镜像的变换类型
public value struct Trsf :System::ICloneable {
public:
    const static Trsf Default = Trsf(Vec::Default, Quat::Default);
public:
    Trsf(gp_Trsf theT);
    Trsf(array<double, 2>^ matrix);
    Trsf(array<array<double>^>^ matrix);
    Trsf(Vec translation, Quat rotation);
    Trsf(Vec translation, Quat rotation, double scalce);
    Trsf(Ax2 fromAx2, Ax2 toAx2);
    Trsf(Ax2 toAx2);
    Trsf(Vec translation);
    Trsf(Quat rotation);
    gp_Trsf GetOCC();
    virtual System::Object^ Clone();
    virtual System::String^ ToString() override;
    //! 隐式转换为 gp_Trsf
    static operator gp_Trsf (Trsf t) { return t.GetOCC(); }
public:
    Trsf Multiplied(Trsf rightTrsf);
    Trsf Inverted();
private:
    Vec translation;
    Quat rotation;
    double scale;
#pragma region 重载操作符
public:
    property Vec Translation {Vec get() { return translation; }}
    property Quat Rotation {Quat get() { return rotation; }}
    property double Scale {double get() { return scale; }}
#pragma region 重载操作符

    static Trsf operator * (Trsf Left, Trsf Right) {
        return Trsf(Left.Multiplied(Right));
    }

    static Trsf operator - (Trsf Left) {
        return Trsf(Left.GetOCC().Inverted());
    }
    //bool Equals(Trsf other, double AngularTOL, double LinearTOL) {    }
    //static bool operator == (Trsf Left, Trsf Right) { return Left.Equals(Right, ANGULAR_TOL, LINEAR_TOL); }//默认精度
    //static bool operator != (Trsf Left, Trsf Right) { return !Left.Equals(Right, ANGULAR_TOL, LINEAR_TOL); }//默认精度

#pragma endregion
private:
    void Init(gp_Trsf t);
};

}
}
}
