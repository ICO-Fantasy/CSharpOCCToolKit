#include "ICO_Ax2.h"
#include <gp_Ax3.hxx>
#include <gp_Ax2.hxx>
//local
#include "ICO_Pnt.h"
#include "ICO_Dir.h"
#include "ICO_Ax1.h"
#include "ICO_Trsf.h"

using namespace System;

namespace OCCTK {
namespace OCC {
namespace gp {

Ax2::Ax2() {
	Location = Pnt();
	ZDir = Dir(0, 0, 1);
	XDir = Dir(1, 0, 0);
}
/// <summary>
/// X方向任意
/// </summary>
/// <param name="theLocation"></param>
/// <param name="theZAxis"></param>
Ax2::Ax2(Pnt theLocation, Dir theZAxis) {
	gp_Ax2 theAx2 = gp_Ax2(theLocation, theZAxis);
	Location = Pnt(theAx2.Location());
	ZDir = Dir(theAx2.Direction());
	XDir = Dir(theAx2.XDirection());
}

Ax2::Ax2(Pnt theLocation, Dir theZAxis, Dir theXAxis) {
	Location = theLocation;
	ZDir = theZAxis;
	XDir = theXAxis;
}

Ax2::Ax2(gp_Ax2 theAx2) {
	Location = Pnt(theAx2.Location());
	ZDir = Dir(theAx2.Direction());
	XDir = Dir(theAx2.XDirection());
}

Ax2::Ax2(gp_Ax2* theAx2) {
	Location = Pnt(theAx2->Location());
	ZDir = Dir(theAx2->Direction());
	XDir = Dir(theAx2->XDirection());
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
	Location = Pnt(newAx2.Location());
	ZDir = Dir(newAx2.Direction());
	XDir = Dir(newAx2.XDirection());
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
	Location = Pnt(newAx2.Location());
	ZDir = Dir(newAx2.Direction());
	XDir = Dir(newAx2.XDirection());
}

gp_Ax2 Ax2::GetOCC() {
	return gp_Ax2(Location, ZDir, XDir);
}

Object^ Ax2::Clone() {
	return gcnew Ax2(Location, ZDir, XDir);
}

System::String^ Ax2::ToString() {
	System::String^ str = "(" + Location.ToString() + ") Z: (" + ZDir.ToString() + ")X: (" + XDir.ToString() + ")";
	return str;
}

void Ax2::Transform(Trsf^ theT) {
	gp_Ax2 newAxis2 = GetOCC().Transformed(theT->GetOCC());
	Location = Pnt(newAxis2.Location());
	ZDir = Dir(newAxis2.Direction());
	XDir = Dir(newAxis2.XDirection());
}


Ax2^ Ax2::Transformed(Trsf^ theT) {
	return gcnew Ax2(GetOCC().Transformed(theT->GetOCC()));
}

Dir Ax2::YDir::get() {
	return Dir(GetOCC().YDirection());
}

Ax1^ Ax2::XAxis::get() {
	return gcnew Ax1(Location, XDir);
}

Ax1^ Ax2::YAxis::get() {
	return gcnew Ax1(Location, YDir);
}

Ax1^ Ax2::ZAxis::get() {
	return gcnew Ax1(Location, ZDir);
}

}
}
}