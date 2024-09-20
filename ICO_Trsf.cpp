#include <gp_Quaternion.hxx>
#include <gp_Ax3.hxx>
#include "ICO_Trsf.h"
#include "ICO_Ax2.h"

namespace OCCTK {
namespace OCC {
namespace gp {

Trsf::Trsf() {
	myTrsf = new gp_Trsf();
}

Trsf::Trsf(const gp_Trsf theT) {
	myTrsf = new gp_Trsf(theT);
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