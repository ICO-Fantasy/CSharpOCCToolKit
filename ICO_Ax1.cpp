#include "ICO_Ax1.h"
//local
#include "ICO_Dir.h"
#include "ICO_Pnt.h"
#include "ICO_Trsf.h"
#include <gp_Ax3.hxx>

using namespace System;

namespace OCCTK {
namespace OCC {
namespace gp {

Ax1::Ax1() {
	Location = gcnew Pnt();
	Direction = gcnew Dir;
}

Ax1::Ax1(Pnt^ location, Dir^ direction) {
	Location = location;
	Direction = direction;
}

Ax1::Ax1(gp_Ax1 theAx1) {
	Location = gcnew Pnt(theAx1.Location());
	Direction = gcnew Dir(theAx1.Direction());
}

Ax1::Ax1(gp_Ax1* theAx1) {
	Location = gcnew Pnt(theAx1->Location());
	Direction = gcnew Dir(theAx1->Direction());
}

gp_Ax1 Ax1::GetOCC() {
	return gp_Ax1(Location->GetOCC(), Direction->GetOCC());
}

Object^ Ax1::Clone() {
	return gcnew Ax1(Location, Direction);
}

System::String^ Ax1::ToString() {
	return "(" + Location->X.ToString("F1") + ", " + Location->Y.ToString("F1") + ", " + Location->Z.ToString("F1") + "), (" + Direction->X.ToString("F1") + ", " + Direction->Y.ToString("F1") + ", " + Direction->Z.ToString("F1") + ")";
}

void Ax1::Transform(Trsf^ theT) {
	gp_Ax1 a = gp_Ax1(Location->GetOCC(), Direction->GetOCC()).Transformed(theT->GetOCC());
	Location = gcnew Pnt(a.Location());
	Direction = gcnew Dir(a.Direction());
}

Ax1^ Ax1::Transformed(Trsf^ theT) {
	return gcnew Ax1(gp_Ax1(Location->GetOCC(), Direction->GetOCC()).Transformed(theT->GetOCC()));
}

void Ax1::Reverse() {
	Direction->Reverse();
}

Ax1^ Ax1::Reversed() {
	return gcnew Ax1(Location, Direction->Reversed());
}

bool Ax1::IsCoaxial(Ax1^ other, double AngularTOL, double LinearTOL) {
	return GetOCC().IsCoaxial(other->GetOCC(), AngularTOL, LinearTOL);
}

}
}
}