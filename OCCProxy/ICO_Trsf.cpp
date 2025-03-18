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
#include "ICO_Exception.h"

using namespace System;

namespace OCCTK {
namespace OCC {
namespace gp {

Trsf::Trsf(const gp_Trsf theT) {
    Init(theT);
}
//
//Trsf::Trsf(array<double, 2>^ matrix) {
//    // 检查矩阵尺寸
//    if (matrix->GetLength(0) != 3 && matrix->GetLength(0) != 4) {
//        int rows = matrix->GetLength(0);
//        System::String^ str = "输入矩阵为" + rows + "行，需要3或4行";
//        throw gcnew System::Exception(str);
//    }
//    if (matrix->GetLength(1) != 4) {
//        int cols = matrix->GetLength(1);
//        System::String^ str = "输入矩阵为" + cols + "列，需要4行";
//        throw gcnew System::Exception(str);
//    }
//
//    gp_Trsf theT;
//    theT.SetValues(matrix[0, 0], matrix[0, 1], matrix[0, 2], matrix[0, 3],
//        matrix[1, 0], matrix[1, 1], matrix[1, 2], matrix[1, 3],
//        matrix[2, 0], matrix[2, 1], matrix[2, 2], matrix[2, 3]);
//    Init(theT);
//}
//
//Trsf::Trsf(array<array<double>^>^ matrix) {
//    // 检查矩阵尺寸
//    if (matrix->Length != 3 && matrix->Length != 4) {
//        int rows = matrix->Length;
//        System::String^ str = "输入矩阵为" + rows + "行，需要3或4行";
//        throw gcnew System::Exception(str);
//    }
//    if (matrix[0]->Length != 4) {
//        int cols = matrix[0]->Length;
//        System::String^ str = "输入矩阵为" + cols + "列，需要4行";
//        throw gcnew System::Exception(str);
//    }
//
//    gp_Trsf theT;
//    theT.SetValues(matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3],
//        matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3],
//        matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3]);
//    Init(theT);
//}

Trsf::Trsf(XYZ translation, Quat rotation)
{
    myTranslation = translation;
    myRotation = rotation;
    myScale = 1;
}

Trsf::Trsf(XYZ translation, Quat rotation, double scale)
{
    myTranslation = translation;
    myRotation = rotation;
    myScale = scale;
}

/// <summary>
/// 从一个坐标系变换到另一个坐标系的变换
/// </summary>
/// <param name="fromAx2"></param>
/// <param name="toAx2"></param>
Trsf::Trsf(Ax2 fromAx2, Ax2 toAx2) {
    gp_Trsf a;
    gp_Trsf b;
    a.SetDisplacement(gp_Ax3(), gp_Ax3(fromAx2));
    b.SetDisplacement(gp_Ax3(), gp_Ax3(toAx2));
    Init(a.Inverted().Multiplied(b));
}

/// <summary>
/// 从原点变换到目标坐标系
/// </summary>
Trsf::Trsf(Ax2 toAx2)
{
    gp_Trsf theT;
    theT.SetDisplacement(gp_Ax3(), gp_Ax3(toAx2));
    Init(theT);
}

/// <summary>
/// 只有平移的变换
/// </summary>
/// <param name="translation">平移部分</param>
Trsf::Trsf(Vec translation)
{
    myTranslation = translation;
    myRotation = Quat::Default;
    myScale = 1;
}

/// <summary>
/// 只有旋转的变换
/// </summary>
/// <param name="rotation">旋转部分</param>
Trsf::Trsf(Quat rotation)
{
    myTranslation = XYZ::Default;
    myRotation = rotation;
    myScale = 1;
}

gp_Trsf Trsf::GetOCC() {
    gp_Trsf t=gp_Trsf();
    if (Translation != XYZ::Default)
    {
        t.SetTranslationPart(Vec(Translation));
    }
    t.SetRotationPart(Rotation);
    //! default 构造的Trsf会出现错误
    if (Scale == 0)
    {
        myScale = 1;
    }
    t.SetScaleFactor(Scale);
    return t;
}

Object^ Trsf::Clone() {
    return Trsf(Translation, Rotation, Scale);
}

System::String^ Trsf::ToString() {
    TCollection_AsciiString theString;
    gp_XYZ p = GetOCC().TranslationPart();
    gp_Quaternion q = GetOCC().GetRotation();
    double x, y, z;
    q.GetEulerAngles(gp_Extrinsic_XYZ, x, y, z);
    System::String^ str = "(" + p.X().ToString("F1") + ", " + p.Y().ToString("F1") + ", " + p.Z().ToString("F1") + " | Extrinsic_XYZ:(" + (x / M_PI * 180.0).ToString("F1") + ", " + (y / M_PI * 180.0).ToString("F1") + ", " + (z / M_PI * 180.0).ToString("F1") + ")";
    return str;
}

///// <summary>
///// 左乘
///// </summary>
///// <param name="leftTrsf"></param>
//void Trsf::PreMultiply(Trsf leftTrsf) {
//	GetOCC().PreMultiply(leftTrsf->GetOCC());
//}

/// <summary>
/// 右乘
/// </summary>
/// <param name="rightTrsf"></param>
/// <returns></returns>
Trsf Trsf::Multiplied(Trsf rightTrsf) {
    gp_Trsf l=GetOCC();
    l.SetForm(gp_TrsfForm::gp_CompoundTrsf);
    gp_Trsf r= rightTrsf.GetOCC();
    r.SetForm(gp_TrsfForm::gp_CompoundTrsf);
    gp_Trsf debugt = l.Multiplied(r);
    return Trsf(l.Multiplied(r));
}

/// <summary>
/// 取反
/// </summary>
/// <returns></returns>
Trsf Trsf::Inverted() {
    return Trsf(GetOCC().Inverted());
}

Trsf Trsf::CreateDisplacement(Ax2 fromAx2,Ax2 toAx2)
{
    gp_Trsf t=GetOCC();
    t.SetDisplacement( gp_Ax3(fromAx2), gp_Ax3(toAx2));
    return Trsf(t);
}

Trsf Trsf::CreateTransformation(Ax2 fromAx2, Ax2 toAx2)
{
    gp_Trsf t = GetOCC();
    t.SetTransformation(gp_Ax3(fromAx2), gp_Ax3(toAx2));
    return Trsf(t);
}

Trsf Trsf::CreateTransformation(Ax2 toAx2)
{
    gp_Trsf t = GetOCC();
    t.SetTransformation( gp_Ax3(toAx2));
    return Trsf(t);
}

void Trsf::Init(gp_Trsf t) {
    if (t.Form() == gp_TrsfForm::gp_Rotation)
    {
        myTranslation = XYZ::Default;
        myRotation = Quat(t.GetRotation());
        myScale = t.ScaleFactor();
    }
    if (t.Form() == gp_TrsfForm::gp_Translation)
    {
        myTranslation = XYZ(t.TranslationPart());
        myRotation = Quat::Default;
        myScale = t.ScaleFactor();
    }
    if (t.Form() == gp_TrsfForm::gp_CompoundTrsf)
    {
        myTranslation = XYZ(t.TranslationPart());
        myRotation = Quat(t.GetRotation());
        myScale = t.ScaleFactor();
    }
}

}
}
}