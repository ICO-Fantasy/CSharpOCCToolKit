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

Pnt::Pnt() {
	X = 0.0;
	Y = 0.0;
	Z = 0.0;
}

Pnt::Pnt(double theX, double theY, double theZ) {
	X = theX;
	Y = theY;
	Z = theZ;
}

Pnt::Pnt(gp_Pnt thePnt) {
	X = thePnt.X();
	Y = thePnt.Y();
	Z = thePnt.Z();
}

Pnt::Pnt(gp_XYZ theXYZ) {
	X = theXYZ.X();
	Y = theXYZ.Y();
	Z = theXYZ.Z();
}

Pnt::Pnt(gp_Pnt* thePnt) {
	X = thePnt->X();
	Y = thePnt->Y();
	Z = thePnt->Z();
}

Object^ Pnt::Clone() {
	return gcnew Pnt(this->X, this->Y, this->Z);;
}

gp_Pnt Pnt::GetOCC() {
	return gp_Pnt(X, Y, Z);
}

System::String^ Pnt::ToString() {
	return X.ToString("F3") + ", " + Y.ToString("F3") + ", " + Z.ToString("F3");
}

Pnt^ Pnt::Add(Pnt^ other) {
	return gcnew Pnt(X + other->X, Y + other->Y, Z + other->Z);
}

Pnt^ Pnt::Minus(Pnt^ other) {
	return gcnew Pnt(X - other->X, Y - other->Y, Z - other->Z);
}

double Pnt::Distance(Pnt^ otherPnt) {
	return std::sqrt(
		std::pow(otherPnt->X - X, 2) +
		std::pow(otherPnt->Y - Y, 2) +
		std::pow(otherPnt->Z - Z, 2)
	);
}

Pnt^ Pnt::Transformed(Trsf^ T) {
	return gcnew Pnt(gp_Pnt(X, Y, Z).Transformed(T->GetOCC()));
}

/// <summary>
/// 返回修改了X值之后的新点
/// </summary>
/// <param name="value"></param>
/// <returns></returns>
Pnt^ Pnt::SetX(double value) {
	return gcnew Pnt(gp_Pnt(value, Y, Z));
}

/// <summary>
/// 返回修改了Y值之后的新点
/// </summary>
/// <param name="value"></param>
/// <returns></returns>
Pnt^ Pnt::SetY(double value) {
	return gcnew Pnt(gp_Pnt(X, value, Z));
}

/// <summary>
/// 返回修改了Z值之后的新点
/// </summary>
/// <param name="value"></param>
/// <returns></returns>
Pnt^ Pnt::SetZ(double value) {
	return gcnew Pnt(gp_Pnt(X, Y, value));
}

}
}
}