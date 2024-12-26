#include "ICO_Pnt.h"
#include <cmath>
#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>
#include <Geom_CartesianPoint.hxx>
//local
#include "ICO_Trsf.h"

using namespace System;

namespace OCCTK {
namespace OCC {
namespace gp {

Pnt::Pnt(double theX, double theY, double theZ) {
	X = theX;
	Y = theY;
	Z = theZ;
}

Pnt::Pnt(System::ValueTuple<double, double, double> theXYZ) {
	X = theXYZ.Item1;
	Y = theXYZ.Item2;
	Z = theXYZ.Item3;
}

Pnt::Pnt(gp_Pnt thePnt) {
	X = thePnt.X();
	Y = thePnt.Y();
	Z = thePnt.Z();
}

Pnt::Pnt(gp_Pnt* thePnt) {
	X = thePnt->X();
	Y = thePnt->Y();
	Z = thePnt->Z();
}

Pnt::Pnt(gp_XYZ theXYZ) {
	X = theXYZ.X();
	Y = theXYZ.Y();
	Z = theXYZ.Z();
}

Pnt::Pnt(gp_XYZ* theXYZ) {
	X = theXYZ->X();
	Y = theXYZ->Y();
	Z = theXYZ->Z();
}

Object^ Pnt::Clone() {
	return Pnt(this->X, this->Y, this->Z);;
}

gp_Pnt Pnt::GetOCC() {
	return gp_Pnt(X, Y, Z);
}

System::String^ Pnt::ToString() {
	return X.ToString("F3") + ", " + Y.ToString("F3") + ", " + Z.ToString("F3");
}

double Pnt::Distance(Pnt otherPnt) {
	return std::sqrt(
		std::pow(otherPnt.X - X, 2) +
		std::pow(otherPnt.Y - Y, 2) +
		std::pow(otherPnt.Z - Z, 2)
	);
}

Pnt Pnt::Transformed(Trsf^ T) {
	return Pnt(gp_Pnt(X, Y, Z).Transformed(T));
}

/// <summary>
/// 返回修改了X值之后的新点
/// </summary>
/// <param name="value"></param>
/// <returns></returns>
Pnt Pnt::SetX(double value) {
	return Pnt(gp_Pnt(value, Y, Z));
}

/// <summary>
/// 返回修改了Y值之后的新点
/// </summary>
/// <param name="value"></param>
/// <returns></returns>
Pnt Pnt::SetY(double value) {
	return Pnt(gp_Pnt(X, value, Z));
}

/// <summary>
/// 返回修改了Z值之后的新点
/// </summary>
/// <param name="value"></param>
/// <returns></returns>
Pnt Pnt::SetZ(double value) {
	return Pnt(gp_Pnt(X, Y, value));
}

#pragma region 重载操作符

bool Pnt::Equals(Pnt otherPnt, double tol) {
	if (this->Distance(otherPnt) <= tol) {
		return true;
	}
	return false;
}

Pnt Pnt::operator + (Pnt Left, Pnt Right) {
	return Pnt(
		Left.X + Right.X,
		Left.Y + Right.Y,
		Left.Z + Right.Z);
}

Pnt Pnt::operator - (Pnt Left, Pnt Right) {
	return Pnt(
		Left.X - Right.X,
		Left.Y - Right.Y,
		Left.Z - Right.Z);
}
//
//Pnt Pnt::operator + (Pnt Left, Vec Right)
//{
//	return Left.Translated(Right);
//}
//
//Pnt Pnt::operator - (Pnt Left, Vec Right)
//{
//	return Left.Translated(Right.Reversed());
//}
//
//Pnt Pnt::operator * (Pnt Left, Trsf Right)
//{
//	return Left.Transformed(Right);
//}

#pragma endregion

}
}
}