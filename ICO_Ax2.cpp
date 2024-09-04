#include "ICO_Ax2.h"
#include "ICO_Trsf.h"
#include <gp_Ax3.hxx>

namespace OCCTK {
namespace OCC {
namespace gp {

Ax2::Ax2() {
	myAx2 = new gp_Ax2();
}

Ax2::Ax2(gp_Ax2 theAx2) {
	myAx2 = new gp_Ax2(theAx2);
}

Ax2::Ax2(gp_Ax2* theAx2) {
	myAx2 = theAx2;
}

gp_Ax2 Ax2::GetOCC() {
	return *myAx2;
}

System::String^ Ax2::ToString() {
	TCollection_AsciiString theString;
	gp_Pnt p = myAx2->Location();
	auto t = gp_Trsf();
	t.SetTransformation(gp_Ax3(), gp_Ax3(*myAx2));
	Trsf^ wt = gcnew Trsf(t);
	//gp_Dir d = myAx2->Axis().Direction();
	//System::String^ str = "(" + p.X().ToString("F1") + ", " + p.Y().ToString("F1") + ", " + p.Z().ToString("F1") + "), (" + d.X().ToString("F1") + ", " + d.Y().ToString("F1") + ", " + d.Z().ToString("F1") + ")";
	return wt->ToString();
}

void Ax2::Transform(Trsf^ theT) {
	myAx2->Transform(theT->GetOCC());
}

Ax2^ Ax2::Transformed(Trsf^ theT) {
	return gcnew Ax2(myAx2->Transformed(theT->GetOCC()));
}

}
}
}