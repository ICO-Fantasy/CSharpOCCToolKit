#include "ICO_Ax2.h"
#include <gp_Ax3.hxx>
#include <gp_Ax2.hxx>
//local
#include "ICO_Pnt.h"
#include "ICO_Dir.h"
#include "ICO_Trsf.h"

using namespace System;

namespace OCCTK {
namespace OCC {
namespace gp {

Ax2::Ax2() {
	Location = gcnew Pnt();
	ZDir = gcnew Dir();
	XDir = gcnew Dir();
}
/// <summary>
/// X方向任意
/// </summary>
/// <param name="theLocation"></param>
/// <param name="theZAxis"></param>
Ax2::Ax2(Pnt^ theLocation, Dir^ theZAxis) {
	gp_Ax2 theAx2 = gp_Ax2(theLocation->GetOCC(), theZAxis->GetOCC());
	Location = gcnew Pnt(theAx2.Location());
	ZDir = gcnew Dir(theAx2.Direction());
	XDir = gcnew Dir(theAx2.XDirection());
}

Ax2::Ax2(Pnt^ theLocation, Dir^ theZAxis, Dir^ theXAxis) {
	Location = theLocation;
	ZDir = theZAxis;
	XDir = theXAxis;
}

Ax2::Ax2(gp_Ax2 theAx2) {
	Location = gcnew Pnt(theAx2.Location());
	ZDir = gcnew Dir(theAx2.Direction());
	XDir = gcnew Dir(theAx2.XDirection());
}

Ax2::Ax2(gp_Ax2* theAx2) {
	Location = gcnew Pnt(theAx2->Location());
	ZDir = gcnew Dir(theAx2->Direction());
	XDir = gcnew Dir(theAx2->XDirection());
}

Ax2::Ax2(array<double, 2>^ matrix) {
	if (matrix->GetLength(0) != 4 || matrix->GetLength(1) != 4) {
		throw gcnew System::ArgumentException("The matrix must be 4x4.");
	}
	gp_Ax2 newAx2 = gp_Ax2();
	gp_Trsf T = gp_Trsf();
	T.SetValues(
		matrix[0, 0], matrix[0, 1], matrix[0, 2], matrix[0, 3],
		matrix[1, 0], matrix[1, 1], matrix[1, 2], matrix[1, 3],
		matrix[2, 0], matrix[2, 1], matrix[2, 2], matrix[2, 3]
	);
	newAx2.Transform(T);
	Location = gcnew Pnt(newAx2.Location());
	ZDir = gcnew Dir(newAx2.Direction());
	XDir = gcnew Dir(newAx2.XDirection());
}

Ax2::Ax2(array<array<double>^>^ matrix) {
	if (matrix->Length != 4) {
		throw gcnew System::ArgumentException("The matrix must be 4x4.");
	}
	for each (auto var in matrix) {
		if (var->Length != 4) {
			throw gcnew System::ArgumentException("The matrix must be 4x4.");
		}
	}
	gp_Ax2 newAx2 = gp_Ax2();
	gp_Trsf T = gp_Trsf();
	T.SetValues(
		matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3],
		matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3],
		matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3]
	);
	newAx2.Transform(T);
	Location = gcnew Pnt(newAx2.Location());
	ZDir = gcnew Dir(newAx2.Direction());
	XDir = gcnew Dir(newAx2.XDirection());
}

gp_Ax2 Ax2::GetOCC() {
	return gp_Ax2(Location->GetOCC(), ZDir->GetOCC(), XDir->GetOCC());
}

Object^ Ax2::Clone() {
	return gcnew Ax2(Location, ZDir, XDir);
}

System::String^ Ax2::ToString() {
	TCollection_AsciiString theString;
	gp_Pnt p = Location->GetOCC();
	auto t = gp_Trsf();
	t.SetTransformation(gp_Ax3(), gp_Ax3(this->GetOCC()));
	Trsf^ wt = gcnew Trsf(t);
	return wt->ToString();
}

void Ax2::Transform(Trsf^ theT) {
	gp_Ax2 newAxis2 = GetOCC().Transformed(theT->GetOCC());
	Location = gcnew Pnt(newAxis2.Location());
	ZDir = gcnew Dir(newAxis2.Direction());
	XDir = gcnew Dir(newAxis2.XDirection());
}


Ax2^ Ax2::Transformed(Trsf^ theT) {
	return gcnew Ax2(GetOCC().Transformed(theT->GetOCC()));
}

}
}
}