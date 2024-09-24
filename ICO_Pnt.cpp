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

System::String^ Pnt::ToString() {
	System::String^ str = myPnt->X().ToString("F3") + ", " + myPnt->Y().ToString("F3") + ", " + myPnt->Z().ToString("F3");
	return str;
}

double Pnt::Distance(Pnt^ otherPnt) {
	return myPnt->Distance(otherPnt->GetOCC());
}

}
}
}