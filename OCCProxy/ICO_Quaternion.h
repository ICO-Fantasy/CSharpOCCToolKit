#pragma once
//在构造函数中使用的值对象需要直接引入
#include "..\\Extension\ICO_WPR.h"
#include "ICO_Ax1.h"
#include "ICO_EulerSequence.h"
#include "ICO_Vec.h"
#include <gp_Quaternion.hxx>

typedef System::ValueTuple<System::ValueTuple<double, double, double>,
    System::ValueTuple<double, double, double>,
    System::ValueTuple<double, double, double>> SO3Matrix;

namespace OCCTK {
namespace OCC {
namespace gp {

public value struct Quat :System::ICloneable {
public:
    const static Quat Default = Quat(0, 0, 0, 1);
public:
    Quat(double theX, double theY, double theZ, double theW);
    Quat(double alpha, double beta, double gamma, EulerSequence sequence);
    Quat(SO3Matrix matrix);
    Quat(WPR rotation);
    Quat(Ax1 axis, double angle);
    Quat(Vec vec, double angle);
    Quat(gp_Quaternion theQuat);
    Quat(Vec fromVec, Vec toVec);
    gp_Quaternion GetOCC();
    virtual System::Object^ Clone();
    virtual System::String^ ToString() override;
    //! 隐式转换为 gp_Quaternion
    static operator gp_Quaternion (Quat q) { return q.GetOCC(); }
public:
    System::ValueTuple<double, double, double> ToEuler(EulerSequence sequence);
    SO3Matrix GetMatrix();

private:
    double x;
    double y;
    double z;
    double w;
public:
    property double X {double get() { return x; }};
    property double Y {double get() { return y; }};
    property double Z {double get() { return z; }};
    property double W {double get() { return w; }};

#pragma region 重载操作符

#pragma endregion
private:
    void Init(gp_Quaternion q);
};

}
}
}