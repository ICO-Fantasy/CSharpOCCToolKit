#include "ICO_Vec2D.h"
#include <cmath>
#include <gp_Vec2d.hxx>
//local
#include"ICO_Pnt2D.h"

using namespace System;

namespace OCCTK {
namespace OCC {
namespace gp {

Vec2D::Vec2D(double theX, double theY) {
    x = theX;
    y = theY;
}

Vec2D::Vec2D(System::ValueTuple<double, double> theXY) {
    x = theXY.Item1;
    y = theXY.Item2;
}

Vec2D::Vec2D(Pnt2D fromPnt, Pnt2D toPnt) {
    x = toPnt.X - fromPnt.X;
    y = toPnt.Y - fromPnt.Y;
}

Vec2D::Vec2D(gp_Vec2d theVec2D) {
    x = theVec2D.X();
    y = theVec2D.Y();
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

bool Vec2D::IsParallel(Vec2D otherVec2D, double theAngularTolerance) {
    return GetOCC().IsParallel(otherVec2D.GetOCC(), theAngularTolerance);
}

Vec2D Vec2D::Normalized() {
    double m = std::sqrt(X * X + Y * Y);
    double newX = X / m;
    double newY = Y / m;
    return Vec2D(newX, newY);
}

Vec2D Vec2D::Multiplied(double value) {
    double newX = X * value;
    double newY = Y * value;
    return Vec2D(newX, newY);
}

double Vec2D::Dot(Vec2D other) {
    return GetOCC().Dot(other);
}

double Vec2D::Crossed(Vec2D other) {
    return GetOCC().Crossed(other);
}

}
}
}