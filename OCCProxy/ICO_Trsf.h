#pragma once
//#include <Eigen\Geometry>
//在构造函数中使用的值对象需要直接引入
#include <gp_Trsf.hxx>
#include "ICO_Pnt.h"
#include "ICO_Vec.h"
#include "ICO_Ax2.h"
#include "ICO_Quaternion.h"
//前向声明
namespace Eigen {
typedef Isometry3d;
}

typedef System::ValueTuple<System::ValueTuple<double, double, double>,
	System::ValueTuple<double, double, double>,
	System::ValueTuple<double, double, double>> SO3Matrix;


namespace OCCTK {
namespace OCC {
namespace gp {
//前向声明
value struct Ax1;
enum class TrsfForm;

public value struct Trsf :System::ICloneable {
public:
	const static Trsf Motionless = Trsf(Vec(0, 0, 0), Quat());
public:
	Trsf(const gp_Trsf& theT);
	Trsf(gp_Trsf* theT);
	Trsf(Ax2 theAx2);
	Trsf(array<double, 2>^ matrix);
	Trsf(array<array<double>^>^ matrix);
	Trsf(Vec translation, Quat rotation);
	Trsf(Ax2 fromAx2, Ax2 toAx2);
	Trsf(Pnt fromPoint, Pnt toPoint);
	gp_Trsf GetOCC();
	//Eigen::Isometry3d* GetEigenTransfrom();//todo 直接使用Eigen进行齐次变换的定义
	virtual System::Object^ Clone();
	virtual System::String^ ToString() override;
	//! 隐式转换为 gp_Trsf
	static operator gp_Trsf (Trsf t) { return t.GetOCC(); }
public:
	void SetTranslation(Pnt fromPoint, Pnt toPoint);
	void SetTranslation(Vec vec);
	void SetTranslationPart(Vec vec);
	void SetRotationPart(Quat quat);
	void SetRotation(Quat quat);
	void SetRotation(Ax1 axis, double angle);
	void PreMultiply(Trsf leftTrsf);
	Trsf Multiplied(Trsf rightTrsf);
	Trsf Inverted();
public:
	property Quat Rotation {Quat get(); };
public:
	property double Scale;
	property TrsfForm Form;
	property SO3Matrix SO3;
	property Vec Translation;
};

}
}
}

