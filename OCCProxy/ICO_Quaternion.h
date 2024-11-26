#pragma once
//在构造函数中使用的值对象需要直接引入
#include <gp_Quaternion.hxx>
#include "ICO_Vec.h"
#include "ICO_Ax1.h"
#include "ICO_EulerSequence.h"

typedef System::ValueTuple<System::ValueTuple<double, double, double>,
	System::ValueTuple<double, double, double>,
	System::ValueTuple<double, double, double>> SO3Matrix;

namespace OCCTK {
namespace OCC {
namespace gp {

public value struct Quat :System::ICloneable {
public:
	const static Quat Motionless = Quat(0, 0, 0, 1);
public:
	Quat(double theX, double theY, double theZ, double theW);
	Quat(double alpha, double beta, double gamma, EulerSequence sequence);
	Quat(SO3Matrix matrix);
	Quat(Ax1 axis, double angle);
	Quat(Vec vec, double angle);
	Quat(gp_Quaternion theQuat);
	Quat(gp_Quaternion* theQuat);
	gp_Quaternion GetOCC();
	virtual System::Object^ Clone();
	virtual System::String^ ToString() override;
	//! 隐式转换为 gp_Quaternion
	static operator gp_Quaternion (Quat q) { return q.GetOCC(); }
public:
	System::Tuple<double, double, double>^ ToEuler(EulerSequence sequence);
	SO3Matrix GetMatrix();

public:
	property double X;
	property double Y;
	property double Z;
	property double W;
#pragma region 重载操作符

#pragma endregion
};

}
}
}