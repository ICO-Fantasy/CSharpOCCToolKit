#include "ICO_Vec.h"
#include <gp_Vec.hxx>
//local
#include"ICO_Pnt.h"
#include"ICO_Trsf.h"

using namespace System;

namespace OCCTK {
namespace OCC {
namespace gp {

Vec::Vec(double theX, double theY, double theZ) {
	if (std::abs(theX) < 1e-6 && std::abs(theY) < 1e-6 && std::abs(theZ) < 1e-6) {
		System::String^ str = "创建了零向量(" + theX + ", " + theY + ", " + theZ + ")";
		System::Diagnostics::Debug::WriteLine(str);
	}
	X = theX;
	Y = theY;
	Z = theZ;
}

Vec::Vec(System::ValueTuple<double, double, double> theXYZ) {
	if (std::abs(theXYZ.Item1) < 1e-6 && std::abs(theXYZ.Item2) < 1e-6 && std::abs(theXYZ.Item3) < 1e-6) {
		System::String^ str = "创建了零向量(" + theXYZ.Item1 + ", " + theXYZ.Item2 + ", " + theXYZ.Item3 + ")";
		System::Diagnostics::Debug::WriteLine(str);
	}
	X = theXYZ.Item1;
	Y = theXYZ.Item2;
	Z = theXYZ.Item3;
}

Vec::Vec(Pnt fromPnt, Pnt toPnt) {
	if (fromPnt.Distance(toPnt) < 1e-6) {
		System::String^ str = "创建了零向量(" + (toPnt.X - fromPnt.X) + ", " + (toPnt.Y - fromPnt.Y) + ", " + (toPnt.Z - fromPnt.Z) + ")";
		System::Diagnostics::Debug::WriteLine(str);
	}
	X = toPnt.X - fromPnt.X;
	Y = toPnt.Y - fromPnt.Y;
	Z = toPnt.Z - fromPnt.Z;
}

Vec::Vec(gp_Vec theVec) {
	X = theVec.X();
	Y = theVec.Y();
	Z = theVec.Z();
}

Vec::Vec(gp_Vec* theVec) {
	X = theVec->X();
	Y = theVec->Y();
	Z = theVec->Z();
}

Vec::Vec(gp_XYZ theVec) {
	X = theVec.X();
	Y = theVec.Y();
	Z = theVec.Z();
}

Vec::Vec(gp_XYZ* theVec) {
	X = theVec->X();
	Y = theVec->Y();
	Z = theVec->Z();
}

gp_Vec Vec::GetOCC() {
	return gp_Vec(X, Y, Z);
}

Object^ Vec::Clone() {
	return Vec(X, Y, Z);
}

System::String^ Vec::ToString() {
	return X.ToString("F3") + ", " + Y.ToString("F3") + ", " + Z.ToString("F3");
}

bool Vec::IsParallel(Vec otherVec, double theAngularTolerance) {
	return GetOCC().IsParallel(otherVec, theAngularTolerance);
}

Vec Vec::Reversed() {
	double newX = -X;
	double newY = -Y;
	double newZ = -Z;
	return Vec(newX, newY, newZ);
}

Vec Vec::Normalized() {
	double m = std::sqrt(X * X + Y * Y + Z * Z);
	double newX = X / m;
	double newY = Y / m;
	double newZ = Z / m;
	return Vec(newX, newY, newZ);
}

Vec Vec::Multiplied(double value) {
	double newX = X * value;
	double newY = Y * value;
	double newZ = Z * value;
	return Vec(newX, newY, newZ);
}

double Vec::Dot(Vec other) {
	return  X * other.X + Y * other.Y + Z * other.Z;
}

Vec Vec::Crossed(Vec other) {
	double newX = (this->Y * other.Z) - (this->Z * other.Y);
	double newY = (this->Z * other.X) - (this->X * other.Z);
	double newZ = (this->X * other.Y) - (this->Y * other.X);
	return Vec(newX, newY, newZ);
}

Vec Vec::CrossProduct(Vec other) {
	return Crossed(other);
}

Vec Vec::Transformed(Trsf T) {
	gp_Vec v = gp_Vec(X, Y, Z);
	v.Transform(T);
	return Vec(v.X(), v.Y(), v.Z());
}

double Vec::Magnitude() {
	return std::sqrt(X * X + Y * Y + Z * Z);
}

double Vec::Angle(Vec theOther) {
	return GetOCC().Angle(theOther);
}

double Vec::AngleWithRef(Vec theOther,Vec theVRef) {
	return GetOCC().AngleWithRef(theOther, theVRef);
}

#pragma region 重载操作符

bool Vec::Equals(Vec otherVec, double tol) {
	if (this->IsParallel(otherVec, tol)) {
		return true;
	}
	return false;
}

Vec Vec::operator+(Vec Left, Vec Right) {
	return Vec(
		Left.X + Right.X,
		Left.Y + Right.Y,
		Left.Z + Right.Z);
}
Vec Vec::operator-(Vec Left, Vec Right) {
	return Vec(
		Left.X - Right.X,
		Left.Y - Right.Y,
		Left.Z - Right.Z);
}

/// <summary>
/// 点乘
/// </summary>
/// <param name="Left"></param>
/// <param name="Right"></param>
/// <returns></returns>
double Vec::operator*(Vec Left, Vec Right) {
	return Left.Dot(Right);
}

/// <summary>
/// 叉乘
/// </summary>
/// <param name="Left"></param>
/// <param name="Right"></param>
/// <returns></returns>
Vec Vec::operator^(Vec Left, Vec Right) {
	return Left.Crossed(Right);
}

#pragma endregion

}
}
}