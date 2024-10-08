#include "ICO_Trsf.h"
#include <gp_Quaternion.hxx>
#include <gp_Trsf.hxx>
//local
#include <gp_Ax3.hxx>
#include "ICO_Ax2.h"
#include "ICO_Ax1.h"
#include "ICO_Pnt.h"
#include "ICO_Vec.h"

using namespace System;

namespace OCCTK {
namespace OCC {
namespace gp {

Trsf::Trsf() {
	myTrsf = new gp_Trsf();
}

Trsf::Trsf(const gp_Trsf theT) {
	myTrsf = new gp_Trsf(theT);
}

Trsf::Trsf(gp_Trsf* theT) {
	myTrsf = theT;
}

Trsf::Trsf(Ax2^ theAx2) {
	myTrsf = new gp_Trsf();
	myTrsf->SetTransformation(gp_Ax3(), gp_Ax3(theAx2->GetOCC()));
}

Trsf::Trsf(Ax2^ fromAx2, Ax2^ toAx2) {
	myTrsf = new gp_Trsf();
	myTrsf->SetTransformation(gp_Ax3(toAx2->GetOCC()), gp_Ax3(fromAx2->GetOCC()));
}

Trsf::Trsf(Pnt^ fromPoint, Pnt^ toPoint) {
	myTrsf = new gp_Trsf();
	myTrsf->SetTranslation(fromPoint->GetOCC(), toPoint->GetOCC());
}

gp_Trsf Trsf::GetOCC() {
	return *myTrsf;
}

Object^ Trsf::Clone() {
	return gcnew Trsf(myTrsf);
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
void Trsf::SetTranslation(Pnt^ fromPoint, Pnt^ toPoint) {
	myTrsf->SetTranslation(fromPoint->GetOCC(), toPoint->GetOCC());
}

void Trsf::SetTranslation(Vec^ vec) {
	myTrsf->SetTranslation(vec->GetOCC());
}

/// <summary>
/// 设置绕轴旋转
/// </summary>
/// <param name="axis"></param>
/// <param name="angle"></param>
void Trsf::SetRotation(Ax1^ axis, double angle) {
	myTrsf->SetRotation(axis->GetOCC(), angle);
}

/// <summary>
/// 左乘
/// </summary>
/// <param name="leftTrsf"></param>
void Trsf::PreMultiply(Trsf^ leftTrsf) {
	myTrsf->PreMultiply(leftTrsf->GetOCC());
}

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

}
}
}