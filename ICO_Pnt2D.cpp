﻿#include "ICO_Pnt2D.h"
#include <cmath>
#include <gp_Pnt2d.hxx>
//local
#include "ICO_Trsf.h"

using namespace System;

namespace OCCTK {
namespace OCC {
namespace gp {

Pnt2D::Pnt2D() {
	X = 0.0;
	Y = 0.0;
}

Pnt2D::Pnt2D(double theX, double theY) {
	X = theX;
	Y = theY;
}

Pnt2D::Pnt2D(gp_Pnt2d thePnt) {
	X = thePnt.X();
	Y = thePnt.Y();
}
Pnt2D::Pnt2D(gp_Pnt2d* thePnt) {
	X = thePnt->X();
	Y = thePnt->Y();
}

Object^ Pnt2D::Clone() {
	return gcnew Pnt2D(this->X, this->Y);;
}

gp_Pnt2d Pnt2D::GetOCC() {
	return gp_Pnt2d(X, Y);
}

System::String^ Pnt2D::ToString() {
	return X.ToString("F3") + ", " + Y.ToString("F3");
}

double Pnt2D::Distance(Pnt2D^ otherPnt) {
	return std::sqrt(
		std::pow(otherPnt->X - X, 2) +
		std::pow(otherPnt->Y - Y, 2)
	);
}

Pnt2D^ Pnt2D::Transformed(Trsf^ T) {
	return gcnew Pnt2D(gp_Pnt2d(X, Y).Transformed(T->GetOCC()));
}

/// <summary>
/// 返回修改了X值之后的新点
/// </summary>
/// <param name="value"></param>
/// <returns></returns>
Pnt2D^ Pnt2D::SetX(double value) {
	return gcnew Pnt2D(gp_Pnt2d(value, Y));
}

/// <summary>
/// 返回修改了Y值之后的新点
/// </summary>
/// <param name="value"></param>
/// <returns></returns>
Pnt2D^ Pnt2D::SetY(double value) {
	return gcnew Pnt2D(gp_Pnt2d(X, value));
}

}
}
}