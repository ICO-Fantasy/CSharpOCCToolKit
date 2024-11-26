#include "ICO_Pln.h"
#include <gp_Pln.hxx>
//local
#include "ICO_Trsf.h"
#include "ICO_Ax1.h"
#include "ICO_Ax3.h"
#include "ICO_Dir.h"
#include "ICO_pnt.h"

using namespace System;

namespace OCCTK {
namespace OCC {
namespace gp {

Pln::Pln() {
	myPln = new gp_Pln();
}

Pln::Pln(gp_Pln pln) {
	myPln = new gp_Pln(pln);
}

Pln::Pln(gp_Pln* pln) {
	myPln = pln;
}

Pln::Pln(Ax3^ axis) {
	myPln = new gp_Pln(axis->GetOCC());
}

Pln::Pln(Pnt location, Dir direction) {
	myPln = new gp_Pln(location, direction);
}

gp_Pln Pln::GetOCC() {
	return *myPln;
}

Object^ Pln::Clone() {
	return gcnew Pln(myPln);
}

double Pln::Distance(Pnt point) {
	return myPln->Distance(point);
}

System::ValueTuple<double, double, double, double> Pln::Coefficients() {
	double a, b, c, d;
	myPln->Coefficients(a, b, c, d);
	return System::ValueTuple<double, double, double, double>(a, b, c, d);
}

Ax1^ Pln::Axis::get() { return gcnew Ax1(myPln->Axis()); }

}
}
}