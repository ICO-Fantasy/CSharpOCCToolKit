#include "ICO_Trsf.h"
#include <Eigen\Geometry>
#include <gp_Mat.hxx>
#include <gp_Quaternion.hxx>
#include <gp_Trsf.hxx>
#include <gp_TrsfForm.hxx>
//local
#include <gp_Ax3.hxx>
#include "ICO_Ax2.h"
#include "ICO_Ax1.h"
#include "ICO_Pnt.h"
#include "ICO_Vec.h"
#include "ICO_Dir.h"
#include "ICO_Quaternion.h"
#include "ICO_TrsfForm.h"

#define InitFromgpTrsf(theT)                                    \
    Translation = Vec((theT).TranslationPart());                \
    Form = TrsfForm((theT).Form());                             \
    Scale = (theT).ScaleFactor();                               \
    {                                                           \
        gp_Mat mat = (theT).HVectorialPart();                   \
        SO3 = SO3Matrix(                                        \
            System::ValueTuple<double, double, double>(         \
                mat.Value(1, 1), mat.Value(2, 1), mat.Value(3, 1)), \
            System::ValueTuple<double, double, double>(         \
                mat.Value(1, 2), mat.Value(2, 2), mat.Value(3, 2)), \
            System::ValueTuple<double, double, double>(         \
                mat.Value(1, 3), mat.Value(2, 3), mat.Value(3, 3))  \
        );                                                      \
    }


namespace OCCTK {
namespace OCC {
namespace gp {

#pragma region 构造函数

Trsf::Trsf(const gp_Trsf& theT) {
	InitFromgpTrsf(theT)
}

Trsf::Trsf(gp_Trsf* theT) {
	InitFromgpTrsf(*theT)
}

Trsf::Trsf(Ax2 theAx2) {
	gp_Trsf theT = gp_Trsf();
	theT.SetTransformation(gp_Ax3(), gp_Ax3(theAx2));
	InitFromgpTrsf(theT)
}

Trsf::Trsf(array<double, 2>^ matrix) {
	// 检查矩阵尺寸
	if (matrix->GetLength(0) != 3 && matrix->GetLength(0) != 4) {
		int rows = matrix->GetLength(0);
		System::String^ str = "输入矩阵为" + rows + "行，需要3或4行";
		throw gcnew System::Exception(str);
	}
	if (matrix->GetLength(1) != 4) {
		int cols = matrix->GetLength(1);
		System::String^ str = "输入矩阵为" + cols + "列，需要4行";
		throw gcnew System::Exception(str);
	}
	gp_Trsf theT = gp_Trsf();
	theT.SetValues(matrix[0, 0], matrix[0, 1], matrix[0, 2], matrix[0, 3],
		matrix[1, 0], matrix[1, 1], matrix[1, 2], matrix[1, 3],
		matrix[2, 0], matrix[2, 1], matrix[2, 2], matrix[2, 3]);
	InitFromgpTrsf(theT)
}

Trsf::Trsf(array<array<double>^>^ matrix) {
	// 检查矩阵尺寸
	if (matrix->Length != 3 && matrix->Length != 4) {
		int rows = matrix->Length;
		System::String^ str = "输入矩阵为" + rows + "行，需要3或4行";
		throw gcnew System::Exception(str);
	}
	if (matrix[0]->Length != 4) {
		int cols = matrix[0]->Length;
		System::String^ str = "输入矩阵为" + cols + "列，需要4行";
		throw gcnew System::Exception(str);
	}
	gp_Trsf theT = gp_Trsf();
	theT.SetValues(matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3],
		matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3],
		matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3]);
	InitFromgpTrsf(theT)
}

Trsf::Trsf(Vec translation, Quat rotation) {
	gp_Trsf theT = gp_Trsf();
	theT.SetTranslationPart(translation);
	theT.SetRotationPart(rotation);
	InitFromgpTrsf(theT)
}

Trsf::Trsf(Ax2 fromAx2, Ax2 toAx2) {
	gp_Trsf theT = gp_Trsf();
	theT.SetTransformation(gp_Ax3(toAx2), gp_Ax3(fromAx2));
	InitFromgpTrsf(theT)
}

Trsf::Trsf(Pnt fromPoint, Pnt toPoint) {
	gp_Trsf theT = gp_Trsf();
	theT.SetTranslation(fromPoint, toPoint);
	InitFromgpTrsf(theT)
}

#pragma endregion

gp_Trsf Trsf::GetOCC() {
	gp_Trsf theT = gp_Trsf();
	theT.SetScaleFactor(Scale);
	theT.SetForm(gp_TrsfForm(Form));
	theT.SetTranslationPart(Translation);
	theT.SetRotationPart(Rotation);
	return theT;
}

System::Object^ Trsf::Clone() {
	return Trsf(GetOCC());
}

System::String^ Trsf::ToString() {
	gp_Quaternion q = Rotation.GetOCC();
	double x, y, z;
	q.GetEulerAngles(gp_Intrinsic_XYZ, x, y, z);
	System::String^ str = "(" + Translation.X.ToString("F1") + ", " + Translation.Y.ToString("F1") + ", " + Translation.Z.ToString("F1") + "), Intrinsic_xyz:(" + (x / M_PI * 180.0).ToString("F1") + ", " + (y / M_PI * 180.0).ToString("F1") + ", " + (z / M_PI * 180.0).ToString("F1") + ")";
	return str;
}
#pragma region 方法

/// <summary>
/// 设置平移
/// </summary>
/// <param name="fromPoint"></param>
/// <param name="toPoint"></param>
void Trsf::SetTranslation(Pnt fromPoint, Pnt toPoint) {
	Form = TrsfForm::Translation;
	Scale = 1;
	SO3 = Quat().GetMatrix();
	Translation = Vec(fromPoint, toPoint);
}

/// <summary>
/// 设置为平移变换
/// </summary>
/// <param name="vec"></param>
void Trsf::SetTranslation(Vec vec) {
	Form = TrsfForm::Translation;
	Scale = 1;
	SO3 = Quat().GetMatrix();
	Translation = vec;
}

/// <summary>
/// 设置变换的平移部分
/// </summary>
/// <param name="vec"></param>
void Trsf::SetTranslationPart(Vec vec) {
	Translation = vec;
}

/// <summary>
/// 设置变换的旋转部分
/// </summary>
/// <param name="quat"></param>
void Trsf::SetRotationPart(Quat quat) {
	SO3 = quat.GetMatrix();
}

/// <summary>
/// 设置为旋转变换
/// </summary>
/// <param name="axis"></param>
/// <param name="angle"></param>
void Trsf::SetRotation(Quat quat) {
	gp_Trsf theT;
	theT.SetRotation(quat);
	InitFromgpTrsf(theT)
}

/// <summary>
/// 设置变换为绕轴角旋转
/// </summary>
/// <param name="axis">旋转轴</param>
/// <param name="angle">旋转角</param>
void Trsf::SetRotation(Ax1 axis, double angle) {
	gp_Trsf theT;
	theT.SetRotation(axis, angle);
	InitFromgpTrsf(theT)
}

/// <summary>
/// 左乘
/// </summary>
/// <param name="leftTrsf"></param>
void Trsf::PreMultiply(Trsf leftTrsf) {
	gp_Trsf theT;
	theT.PreMultiply(leftTrsf);
	InitFromgpTrsf(theT)
}

/// <summary>
/// 右乘
/// </summary>
/// <param name="rightTrsf"></param>
/// <returns></returns>
Trsf Trsf::Multiplied(Trsf rightTrsf) {
	gp_Trsf theT;
	return Trsf(theT.Multiplied(rightTrsf));
}

/// <summary>
/// 取反
/// </summary>
/// <returns></returns>
Trsf Trsf::Inverted() {
	gp_Trsf theT;
	return Trsf(theT.Inverted());
}

#pragma endregion

Quat Trsf::Rotation::get() {
	return Quat(SO3);
}

}
}
}