#include <gp_Dir.hxx>
//Local
#include "ICO_Dir.h"

namespace OCCTK {
namespace OCC {
namespace gp {

Dir::Dir() {
	myDir = new gp_Dir();
}

Dir::Dir(double X, double Y, double Z) {
	myDir = new gp_Dir(X, Y, Z);
}

Dir::Dir(gp_Dir theDir) {
	myDir = new gp_Dir(theDir);
}

Dir::Dir(gp_Dir* theDir) {
	myDir = theDir;
}

bool Dir::IsParallel(Dir^ otherDir, double theAngularTolerance) {
	return myDir->IsParallel(otherDir->GetOCC(), theAngularTolerance);
}

gp_Dir Dir::GetOCC() {
	return *myDir;
}

double Dir::X() {
	return myDir->X();
}

double Dir::Y() {
	return myDir->Y();
}

double Dir::Z() {
	return myDir->Z();
}

}
}
}