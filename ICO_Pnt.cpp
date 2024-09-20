#include "ICO_Pnt.h"
namespace OCCTK {
namespace OCC {
namespace gp {

Pnt::Pnt() {
	myPnt = new gp_Pnt();
}

Pnt::Pnt(double X, double Y, double Z) {
	myPnt = new gp_Pnt(X, Y, Z);
}

Pnt::Pnt(gp_Pnt thePnt) {
	myPnt = new gp_Pnt(thePnt);
}
Pnt::Pnt(gp_Pnt* thePnt) {
	myPnt = thePnt;
}

gp_Pnt Pnt::GetOCC() {
	return *myPnt;
}

}
}
}