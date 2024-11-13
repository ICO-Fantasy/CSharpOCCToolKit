#include "ICO_MakeFace.h"
#include <gp_Pln.hxx>
#include <Standard_Failure.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_FaceError.hxx>
//Local
#include "ICO_Topo_Shape.h"
#include "ICO_Topo_Wire.h"
#include "ICO_Topo_Face.h"
#include "ICO_Pln.h"

using namespace OCCTK::OCC::Topo;
using namespace OCCTK::OCC::gp;

namespace OCCTK {
namespace OCC {
namespace BRepBuilderAPI {

MakeFace::MakeFace() {
	myMaker = new BRepBuilderAPI_MakeFace();
}

MakeFace::MakeFace(Topo::TWire^ wire) {
	myMaker = new BRepBuilderAPI_MakeFace(wire->GetOCC());
}

MakeFace::MakeFace(Pln^ plane) {
	myMaker = new BRepBuilderAPI_MakeFace(plane->GetOCC());
}

MakeFace::MakeFace(Pln^ plane, double Umin, double Umax, double Vmin, double Vmax) {
	myMaker = new BRepBuilderAPI_MakeFace(plane->GetOCC(), Umax, Umin, Vmin, Vmax);
}

TShape^ MakeFace::Shape() {
	try {
		return gcnew TShape(myMaker->Shape());
	}
	catch (Standard_Failure e) {
		throw gcnew System::Exception(gcnew System::String(e.GetMessageString()));
	}
}

TFace^ MakeFace::Face() {
	try {
		return gcnew TFace(myMaker->Face());
	}
	catch (Standard_Failure e) {
		throw gcnew System::Exception(gcnew System::String(e.GetMessageString()));
	}
}

void MakeFace::Add(Topo::TWire^ wire) {
	myMaker->Add(wire->GetOCC());
}

bool MakeFace::Error() {
	BRepBuilderAPI_FaceError state = myMaker->Error();
	if (state == BRepBuilderAPI_FaceDone) {
		return true;
	}
	if (state == BRepBuilderAPI_NoFace) {
		throw gcnew System::Exception(gcnew System::String("No Face"));
	}
	if (state == BRepBuilderAPI_NotPlanar) {
		throw gcnew System::Exception(gcnew System::String("Not Planar"));
	}
	if (state == BRepBuilderAPI_CurveProjectionFailed) {
		throw gcnew System::Exception(gcnew System::String("Curve Projection Failed"));
	}
	if (state == BRepBuilderAPI_ParametersOutOfRange) {
		throw gcnew System::Exception(gcnew System::String("Parameters Out Of Range"));
	}
	return false;
}


}
}
}