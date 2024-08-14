#include "ICO_Pnt.h"
namespace OCCTK {
namespace OCC {
namespace gp {

Pnt::Pnt() {
	_nativeHandle = new gp_Pnt();
}

Pnt::Pnt(double X, double Y, double Z) {
	_nativeHandle = new gp_Pnt(X, Y, Z);
}

Pnt::Pnt(gp_Pnt thePnt) {
	_nativeHandle = new gp_Pnt(thePnt);
}
Pnt::Pnt(gp_Pnt* thePnt) {
	_nativeHandle = new gp_Pnt(*thePnt);
}

gp_Pnt Pnt::GetOCC() {
	return *_nativeHandle;
}

double Pnt::X() {
	return _nativeHandle->X();
}

double Pnt::Y() {
	return _nativeHandle->Y();
}

double Pnt::Z() {
	return _nativeHandle->Z();
}

}
}
}