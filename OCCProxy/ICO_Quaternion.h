#pragma once
//在构造函数中使用的值对象需要直接引入
#include "ICO_Vec.h"

//前向声明
class gp_Quaternion;
namespace OCCTK {
namespace OCC {
namespace gp {
//前向声明
enum class EulerSequence;
value struct Ax1;
value struct Vec;

public ref class Quat :System::ICloneable {
public:
	Quat();
	Quat(double theX, double theY, double theZ, double theW);
	Quat(double alpha, double beta, double gamma, EulerSequence sequence);
	Quat(Ax1 axis, double angle);
	Quat(Vec vec, double angle);
	Quat(gp_Quaternion theQuat);
	Quat(gp_Quaternion* theQuat);
	gp_Quaternion GetOCC();
	virtual System::Object^ Clone();
	virtual System::String^ ToString() override;
#pragma region 重载操作符
	//// 重载 + 操作符
	//static Quat^ operator+(Quat^ a, Quat^ b) {
	//	// 创建一个新的 Quat 对象，表示相加的结果
	//	return gcnew Quat(a->X + b->X, a->Y + b->Y, a->Z + b->Z);
	//}
	//// 重载 - 操作符
	//static Quat^ operator-(Quat^ a, Quat^ b) {
	//	// 创建一个新的 Quat 对象，表示相减的结果
	//	return gcnew Quat(a->X - b->X, a->Y - b->Y, a->Z - b->Z);
	//}
#pragma endregion
public:
	System::Tuple<double, double, double>^ ToEuler(EulerSequence sequence);
public:
	property double X;
	property double Y;
	property double Z;
	property double W;
};

}
}
}