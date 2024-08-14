#include "ICO_Ax2.h"

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
	auto d = myAx2->Angle(gp_Ax2());
	theString += "(";
	theString += TCollection_AsciiString(p.X());
	theString += ", ";
	theString += TCollection_AsciiString(p.Y());
	theString += ", ";
	theString += TCollection_AsciiString(p.Z());
	theString += ")";
	theString += ", ";
	theString += TCollection_AsciiString(d);
	return gcnew System::String(theString.ToCString());
}

}
}
}