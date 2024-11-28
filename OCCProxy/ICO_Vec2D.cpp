#include "ICO_Vec2D.h"
#include <gp_Vec2d.hxx>
#include <cmath>
//local
#include"ICO_Pnt2D.h"

using namespace System;

namespace OCCTK {
namespace OCC {
namespace gp {

Vec2D::Vec2D(double theX, double theY) {
	X = theX;
	Y = theY;
}

Vec2D::Vec2D(Pnt2D fromPnt, Pnt2D toPnt) {
	X = toPnt.X - fromPnt.X;
	Y = toPnt.Y - fromPnt.Y;
}

Vec2D::Vec2D(gp_Vec2d theVec2D) {
	X = theVec2D.X();
	Y = theVec2D.Y();
}

Vec2D::Vec2D(gp_Vec2d* theVec2D) {
	X = theVec2D->X();
	Y = theVec2D->Y();
}

gp_Vec2d Vec2D::GetOCC() {
	return gp_Vec2d(X, Y);
}

Object^ Vec2D::Clone() {
	return Vec2D(X, Y);
}

System::String^ Vec2D::ToString() {
	return X.ToString("F3") + ", " + Y.ToString("F3");
}

bool Vec2D::IsParallel(Vec2D^ otherVec2D, double theAngularTolerance) {
	return GetOCC().IsParallel(otherVec2D->GetOCC(), theAngularTolerance);
}

void Vec2D::Normalize() {
	double m = std::sqrt(X * X + Y * Y);
	X = X / m;
	Y = Y / m;
}

Vec2D^ Vec2D::Normalized() {
	double m = std::sqrt(X * X + Y * Y);
	double newX = X / m;
	double newY = Y / m;
	return Vec2D(newX, newY);
}

//void Vec2D::Multiply(double value) {
//	X *= value;
//	Y *= value;
//}

Vec2D^ Vec2D::Multiplied(double value) {
	double newX = X * value;
	double newY = Y * value;
	return Vec2D(newX, newY);
}

double Vec2D::Crossed(Vec2D^ other) {
	return this->X * other->Y - other->X * this->Y;
}

}
}
}