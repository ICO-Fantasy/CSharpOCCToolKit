#include "ICO_Vec.h"
#include <gp_Vec.hxx>

using namespace System;

namespace OCCTK {
namespace OCC {
namespace gp {

Vec::Vec() {
	X = 1.0;
	Y = 0.0;
	Z = 0.0;
}

Vec::Vec(double theX, double theY, double theZ) {
	X = theX;
	Y = theY;
	Z = theZ;
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

gp_Vec Vec::GetOCC() {
	return gp_Vec(X, Y, Z);
}

Object^ Vec::Clone() {
	return gcnew Vec(X, Y, Z);
}

System::String^ Vec::ToString() {
	return X.ToString("F3") + ", " + Y.ToString("F3") + ", " + Z.ToString("F3");
}

bool Vec::IsParallel(Vec^ otherVec, double theAngularTolerance) {
	return GetOCC().IsParallel(otherVec->GetOCC(), theAngularTolerance);
}

void Vec::Cross(Vec^ other) {
	X = (this->Y * other->Z) - (this->Z * other->Y);
	Y = (this->Z * other->X) - (this->X * other->Z);
	Z = (this->X * other->Y) - (this->Y * other->X);
}

Vec^ Vec::Crossed(Vec^ other) {
	double crossX = (this->Y * other->Z) - (this->Z * other->Y);
	double crossY = (this->Z * other->X) - (this->X * other->Z);
	double crossZ = (this->X * other->Y) - (this->Y * other->X);
	return gcnew Vec(crossX, crossY, crossZ);
}

}
}
}