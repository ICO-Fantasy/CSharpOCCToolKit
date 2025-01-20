#include "ICO_Ax3.h"
#include <gp_Ax3.hxx>
//local
#include "ICO_Trsf.h"
#include "ICO_Ax1.h"

using namespace System;

namespace OCCTK {
namespace OCC {
namespace gp {

Ax3::Ax3(Pnt location, Dir zAxis) {
	gp_Ax3 ax3(location, zAxis);
	Location = Pnt(ax3.Location());
	ZDir = Dir(ax3.Direction());
	XDir = Dir(ax3.XDirection());
}

Ax3::Ax3(Pnt location, Dir zAxis, Dir xAxis) {
	Location = location;
	ZDir = zAxis;
	XDir = xAxis;
}

Ax3::Ax3(gp_Ax3 theAx3) {
	Location = Pnt(theAx3.Location());
	ZDir = Dir(theAx3.Direction());
	XDir = Dir(theAx3.XDirection());
}

Ax3::Ax3(gp_Ax3* theAx3) {
	Location = Pnt(theAx3->Location());
	ZDir = Dir(theAx3->Direction());
	XDir = Dir(theAx3->XDirection());
}

gp_Ax3 Ax3::GetOCC() {
	return gp_Ax3(Location, ZDir, XDir);
}

Object^ Ax3::Clone() {
	return Ax3(Location, ZDir, XDir);
}

System::String^ Ax3::ToString() {
	TCollection_AsciiString theString;
	System::String^ str = "(" + Location.X.ToString("F1") + ", " + Location.Y.ToString("F1") + ", " + Location.Z.ToString("F1") + "), (" + ZDir.X.ToString("F1") + ", " + ZDir.Y.ToString("F1") + ", " + ZDir.Z.ToString("F1") + ")";
	return str;
}

//void Ax3::Transform(Trsf^ theT) {
//	gp_Ax3 ax3(Location, ZDir, XDir);
//	ax3.Transform(theT);
//	Location = Pnt(ax3.Location());
//	ZDir = Dir(ax3.Direction());
//	XDir = Dir(ax3.XDirection());
//}

Ax3 Ax3::Transformed(Trsf^ theT) {
	gp_Ax3 ax3(Location, ZDir, XDir);
	return Ax3(ax3.Transformed(theT));
}
Dir Ax3::YDir::get() {
	return Dir(GetOCC().YDirection());
}

Ax1 Ax3::XAxis::get() {
	return Ax1(Location, XDir);
}

Ax1 Ax3::YAxis::get() {
	return Ax1(Location, YDir);
}

Ax1 Ax3::ZAxis::get() {
	return Ax1(Location, ZDir);
}

}
}
}