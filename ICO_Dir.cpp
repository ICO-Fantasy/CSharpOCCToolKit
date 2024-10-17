#include <gp_Dir.hxx>
#include <cmath>
//Local
#include "ICO_Dir.h"
#include "ICO_Vec.h"

using namespace System;

namespace OCCTK {
namespace OCC {
namespace gp {

Dir::Dir() {
	X = 1.0;
	Y = 0.0;
	Z = 0.0;
	Normalize();
}

Dir::Dir(double theX, double theY, double theZ) {
	X = theX;
	Y = theY;
	Z = theZ;
	Normalize();
}

Dir::Dir(Vec^ theDir) {
	X = theDir->X;
	Y = theDir->Y;
	Z = theDir->Z;
	Normalize();
}

Dir::Dir(gp_Dir theDir) {
	X = theDir.X();
	Y = theDir.Y();
	Z = theDir.Z();
	Normalize();
}

Dir::Dir(gp_Dir* theDir) {
	X = theDir->X();
	Y = theDir->Y();
	Z = theDir->Z();
	Normalize();
}

bool Dir::IsParallel(Dir^ otherDir, double theAngularTolerance) {
	return GetOCC().IsParallel(otherDir->GetOCC(), theAngularTolerance);
}

/// <summary>
/// 获取 [0,π] 范围内的角度
/// </summary>
/// <param name="otherDir"></param>
/// <returns></returns>
double Dir::Angle(Dir^ otherDir) {
	return GetOCC().Angle(otherDir->GetOCC());
}

/// <summary>
/// 获取 [-π,π] 范围内的角度，正负取决于参考方向
/// </summary>
/// <param name="otherDir"></param>
/// <param name="refDir"></param>
/// <returns></returns>
double Dir::AngleWithRef(Dir^ otherDir, Dir^ refDir) {
	return GetOCC().AngleWithRef(otherDir->GetOCC(), refDir->GetOCC());
}

void Dir::Cross(Dir^ other) {
	X = (this->Y * other->Z) - (this->Z * other->Y);
	Y = (this->Z * other->X) - (this->X * other->Z);
	Z = (this->X * other->Y) - (this->Y * other->X);
	Normalize();
}

Dir^ Dir::Crossed(Dir^ other) {
	double crossX = (this->Y * other->Z) - (this->Z * other->Y);
	double crossY = (this->Z * other->X) - (this->X * other->Z);
	double crossZ = (this->X * other->Y) - (this->Y * other->X);
	return gcnew Dir(crossX, crossY, crossZ);
}

double Dir::Dot(Dir^ other) {
	return GetOCC().Dot(other->GetOCC());
}

void Dir::Normalize() {
	double m = std::sqrt(X * X + Y * Y + Z * Z);
	X = X / m;
	Y = Y / m;
	Z = Z / m;
}

gp_Dir Dir::GetOCC() {
	return gp_Dir(X, Y, Z);
}

Object^ Dir::Clone() {
	return gcnew Dir(X, Y, Z);
}

System::String^ Dir::ToString() {
	return X.ToString("F3") + ", " + Y.ToString("F3") + ", " + Z.ToString("F3");
}

}
}
}