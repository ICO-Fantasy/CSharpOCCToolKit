#include "ICO_Vec.h"
namespace OCCTK {
namespace OCC {
namespace gp {

Vec::Vec() {
	myVec = new gp_Vec();
}

Vec::Vec(double X, double Y, double Z) {
	myVec = new gp_Vec(X, Y, Z);
}

Vec::Vec(gp_Vec theVec) {
	myVec = &theVec;
}

Vec::Vec(gp_Vec* theVec) {
	myVec = theVec;
}

gp_Vec Vec::GetOCC() {
	return *myVec;
}

bool Vec::IsParallel(Vec^ otherVec, double theAngularTolerance) {
	return myVec->IsParallel(otherVec->GetOCC(), theAngularTolerance);
}

}
}
}