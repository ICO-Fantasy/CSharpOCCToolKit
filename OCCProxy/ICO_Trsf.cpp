#include "ICO_Trsf.h"
#include <gp_Quaternion.hxx>
#include <gp_Trsf.hxx>
//local
#include <gp_Ax3.hxx>
#include "ICO_Ax2.h"
#include "ICO_Ax1.h"
#include "ICO_Pnt.h"
#include "ICO_Vec.h"
#include "ICO_XYZ.h"
#include "..\Extension\ICO_WPR.h"
#include "ICO_Quaternion.h"
#include <TCollection_AsciiString.hxx>
#include <gp_XYZ.hxx>
#include <gp_EulerSequence.hxx>

using namespace System;

namespace OCCTK {
namespace OCC {
namespace gp {

Trsf::Trsf() {
    myTrsf = new gp_Trsf();
}

Trsf::Trsf(Trsf^ theT) {
    myTrsf = new gp_Trsf(theT);
}

Trsf::Trsf(const gp_Trsf theT) {
    myTrsf = new gp_Trsf(theT);
}

Trsf::Trsf(gp_Trsf* theT) {
    myTrsf = theT;
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
    myTrsf = new gp_Trsf();
    myTrsf->SetValues(matrix[0, 0], matrix[0, 1], matrix[0, 2], matrix[0, 3],
        matrix[1, 0], matrix[1, 1], matrix[1, 2], matrix[1, 3],
        matrix[2, 0], matrix[2, 1], matrix[2, 2], matrix[2, 3]);
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
    myTrsf = new gp_Trsf();
    myTrsf->SetValues(matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3],
        matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3],
        matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3]);
}

Trsf::Trsf(Vec translation, WPR rotation)
{
    myTrsf = new gp_Trsf();
    myTrsf->SetTranslationPart(translation);
    gp_Quaternion quat;
    quat.SetEulerAngles(gp_Intrinsic_XYZ, rotation.W, rotation.P, rotation.R);
    myTrsf->SetRotationPart(quat);
}

Trsf::Trsf(Vec translation, Quat rotation)
{
    myTrsf = new gp_Trsf();
    myTrsf->SetTranslationPart(translation);
    myTrsf->SetRotationPart(rotation);
}

Trsf::Trsf(Ax2 fromAx2, Ax2 toAx2) {
    myTrsf = new gp_Trsf();
    myTrsf->SetTransformation(gp_Ax3(fromAx2), gp_Ax3(toAx2));
}

/// <summary>
/// 从原点变换到目标坐标系
/// </summary>
Trsf::Trsf(Ax2 toAx2)
{

    myTrsf = new gp_Trsf();
    myTrsf->SetTransformation(gp_Ax3(), gp_Ax3(toAx2));
}

/// <summary>
/// 设置从点到点的平移变换
/// </summary>
/// <param name="fromPoint"></param>
/// <param name="toPoint"></param>
Trsf::Trsf(Pnt fromPoint, Pnt toPoint) {
    myTrsf = new gp_Trsf();
    myTrsf->SetTranslation(fromPoint, toPoint);
}

gp_Trsf Trsf::GetOCC() {
    return *myTrsf;
}

Object^ Trsf::Clone() {
    gp_Trsf newTrsf = gp_Trsf(*myTrsf);
    return gcnew Trsf(newTrsf);
}

System::String^ Trsf::ToString() {
    TCollection_AsciiString theString;
    gp_XYZ p = myTrsf->TranslationPart();
    gp_Quaternion q = myTrsf->GetRotation();
    double x, y, z;
    q.GetEulerAngles(gp_Intrinsic_XYZ, x, y, z);
    System::String^ str = "(" + p.X().ToString("F1") + ", " + p.Y().ToString("F1") + ", " + p.Z().ToString("F1") + "), Intrinsic_xyz:(" + (x / M_PI * 180.0).ToString("F1") + ", " + (y / M_PI * 180.0).ToString("F1") + ", " + (z / M_PI * 180.0).ToString("F1") + ")";
    return str;
}

/// <summary>
/// 设置平移
/// </summary>
/// <param name="fromPoint"></param>
/// <param name="toPoint"></param>
void Trsf::SetTranslation(Pnt fromPoint, Pnt toPoint) {
    myTrsf->SetTranslation(fromPoint, toPoint);
}

/// <summary>
/// 设置为平移变换
/// </summary>
/// <param name="vec"></param>
void Trsf::SetTranslation(Vec vec) {
    myTrsf->SetTranslation(vec);
}

/// <summary>
/// 设置变换的平移部分
/// </summary>
/// <param name="vec"></param>
void Trsf::SetTranslationPart(Vec vec) {
    myTrsf->SetTranslationPart(vec);
}

/// <summary>
/// 设置变换的旋转部分
/// </summary>
/// <param name="quat"></param>
void Trsf::SetRotationPart(Quat quat) {
    myTrsf->SetRotationPart(quat);
}

/// <summary>
/// 设置为旋转变换
/// </summary>
/// <param name="axis"></param>
/// <param name="angle"></param>
void Trsf::SetRotation(Quat quat) {
    myTrsf->SetRotation(quat);
}

/// <summary>
/// 设置变换为绕轴角旋转
/// </summary>
/// <param name="axis">旋转轴</param>
/// <param name="angle">旋转角</param>
void Trsf::SetRotation(Ax1 axis, double angle) {
    myTrsf->SetRotation(axis, angle);
}

///// <summary>
///// 左乘
///// </summary>
///// <param name="leftTrsf"></param>
//void Trsf::PreMultiply(Trsf^ leftTrsf) {
//	myTrsf->PreMultiply(leftTrsf->GetOCC());
//}

/// <summary>
/// 右乘
/// </summary>
/// <param name="rightTrsf"></param>
/// <returns></returns>
Trsf^ Trsf::Multiplied(Trsf^ rightTrsf) {
    return gcnew Trsf(myTrsf->Multiplied(rightTrsf->GetOCC()));
}

/// <summary>
/// 取反
/// </summary>
/// <returns></returns>
Trsf^ Trsf::Inverted() {
    return gcnew Trsf(myTrsf->Inverted());
}

XYZ Trsf::Translation::get() {
    gp_XYZ xyz = myTrsf->TranslationPart();
    return XYZ(xyz);
}

Quat Trsf::Rotation::get() {
    return Quat(myTrsf->GetRotation());
}

Trsf^ Trsf::operator*(Trsf^ Left, Trsf^ Right) {
    return Left->Multiplied(Right);
}

Trsf^ Trsf::operator-(Trsf^ Left) {
    return Left->Inverted();
}

}
}
}