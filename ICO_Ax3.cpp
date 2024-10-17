#include "ICO_Ax3.h"
//local
#include "ICO_Trsf.h"
#include "ICO_Ax1.h"

using namespace System;

namespace OCCTK {
namespace OCC {
namespace gp {

Ax3::Ax3() {
	myAx3 = new gp_Ax3();
}

Ax3::Ax3(gp_Ax3 theAx3) {
	myAx3 = new gp_Ax3(theAx3);
}

Ax3::Ax3(gp_Ax3* theAx3) {
	myAx3 = theAx3;
}

gp_Ax3 Ax3::GetOCC() {
	return *myAx3;
}

Object^ Ax3::Clone() {
	return gcnew Ax3(myAx3);
}

System::String^ Ax3::ToString() {
	TCollection_AsciiString theString;
	gp_Pnt p = myAx3->Location();
	gp_Dir d = myAx3->Axis().Direction();
	System::String^ str = "(" + p.X().ToString("F1") + ", " + p.Y().ToString("F1") + ", " + p.Z().ToString("F1") + "), (" + d.X().ToString("F1") + ", " + d.Y().ToString("F1") + ", " + d.Z().ToString("F1") + ")";
	return str;
}

void Ax3::Transform(Trsf^ theT) {
	myAx3->Transform(theT->GetOCC());
}

Ax3^ Ax3::Transformed(Trsf^ theT) {
	return gcnew Ax3(myAx3->Transformed(theT->GetOCC()));
}

Ax1^ Ax3::Axis() {
	return gcnew Ax1(myAx3->Axis());
}

}
}
}