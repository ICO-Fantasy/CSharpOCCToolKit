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

Ax2::Ax2(array<double, 2>^ matrix) {
	if (matrix->GetLength(0) != 4 || matrix->GetLength(1) != 4) {
		throw gcnew System::ArgumentException("The matrix must be 4x4.");
	}
	myAx2 = new gp_Ax2();
	gp_Trsf T = gp_Trsf();
	T.SetValues(
		matrix[0, 0], matrix[0, 1], matrix[0, 2], matrix[0, 3],
		matrix[1, 0], matrix[1, 1], matrix[1, 2], matrix[1, 3],
		matrix[2, 0], matrix[2, 1], matrix[2, 2], matrix[2, 3]
	);
	myAx2->Transform(T);
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
	myAx2 = new gp_Ax2();
	gp_Trsf T = gp_Trsf();
	T.SetValues(
		matrix[0][0], matrix[0][1], matrix[0][2], matrix[0][3],
		matrix[1][0], matrix[1][1], matrix[1][2], matrix[1][3],
		matrix[2][0], matrix[2][1], matrix[2][2], matrix[2][3]
	);
	myAx2->Transform(T);
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