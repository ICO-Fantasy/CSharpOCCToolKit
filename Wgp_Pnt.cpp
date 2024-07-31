#include "Wgp_Pnt.h"
namespace OCCTK {
namespace OCC {
namespace gp {

Wgp_Pnt::Wgp_Pnt() {
	_nativeHandle = new gp_Pnt();
}

Wgp_Pnt::Wgp_Pnt(double X, double Y, double Z)
{
	_nativeHandle = new gp_Pnt(X, Y, Z);
}

Wgp_Pnt::Wgp_Pnt(gp_Pnt thePnt) {
	_nativeHandle = new gp_Pnt(thePnt);
}
Wgp_Pnt::Wgp_Pnt(gp_Pnt* thePnt) {
	_nativeHandle = new gp_Pnt(*thePnt);
}

gp_Pnt* Wgp_Pnt::GetOCC() {
	return _nativeHandle;
}

double Wgp_Pnt::X()
{
	return _nativeHandle->X();
}

double Wgp_Pnt::Y()
{
	return _nativeHandle->Y();
}

double Wgp_Pnt::Z()
{
	return _nativeHandle->Z();
}

}
}
}