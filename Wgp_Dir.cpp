#include "Wgp_Dir.h"
#include <gp_Dir.hxx>

namespace OCCTK {
namespace OCC {
namespace gp {

Wgp_Dir::Wgp_Dir() {
	myDir = new gp_Dir();
}

Wgp_Dir::Wgp_Dir(double X, double Y, double Z)
{
	myDir = new gp_Dir(X, Y, Z);
}

Wgp_Dir::Wgp_Dir(gp_Dir theDir) {
	myDir = new gp_Dir(theDir);
}

Wgp_Dir::Wgp_Dir(gp_Dir* theDir) {
	myDir = theDir;
}

bool Wgp_Dir::IsParallel(Wgp_Dir^ otherDir, double theAngularTolerance) {
	return myDir->IsParallel(*otherDir->GetOCC(), theAngularTolerance);
}

gp_Dir* Wgp_Dir::GetOCC() {
	return myDir;
}

double Wgp_Dir::X()
{
	return myDir->X();
}

double Wgp_Dir::Y()
{
	return myDir->Y();
}

double Wgp_Dir::Z()
{
	return myDir->Z();
}

}
}
}