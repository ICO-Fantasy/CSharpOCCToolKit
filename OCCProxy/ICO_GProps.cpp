#include "ICO_GProps.h"
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
//local
#include "ICO_Pnt.h"

using namespace OCCTK::OCC::gp;

namespace OCCTK {
namespace OCC {
namespace GProp {

GProps::GProps() {
	myGProps = new GProp_GProps();
}

GProps::GProps(GProp_GProps g) {
	myGProps = new GProp_GProps(g);
}

GProps::GProps(GProp_GProps* intptr) {
	myGProps = intptr;
}

GProp_GProps GProps::GetOCC() {
	return *myGProps;
}

double GProps::Mass() {
	return myGProps->Mass();
}

Pnt GProps::CentreOfMass() {
	return Pnt(myGProps->CentreOfMass());
}

/// <summary>
/// 3×3的惯性矩阵
/// </summary>
/// <returns></returns>
array<double, 2>^ GProps::MatrixOfInertia() {
	auto mat = myGProps->MatrixOfInertia();
	array<double, 2>^ matrix = gcnew array<double, 2>(3, 3);
	for (int i = 1; i < 4; i++) {
		for (int j = 1; j < 4; j++) {
			matrix[i - 1, j - 1] = mat.Value(i, j);
		}
	}
	return matrix;
}

}
}
}