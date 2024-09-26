#include "ICO_Pnt.h"
#include <gp_Pnt.hxx>
#include <cmath>

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

double Pnt::Distance(Pnt^ otherPnt) {
	return std::sqrt(
		std::pow(otherPnt->X - X, 2) +
		std::pow(otherPnt->Y - Y, 2) +
		std::pow(otherPnt->Z - Z, 2)
	);
}

}
}
}