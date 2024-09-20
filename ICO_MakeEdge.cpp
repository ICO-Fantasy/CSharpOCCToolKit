#include "ICO_MakeEdge.h"
#include <Standard_Failure.hxx>
#include <BRepBuilderAPI_EdgeError.hxx>
//Local
#include "ICO_Topo_Shape.h"
#include "ICO_Topo_Edge.h"
#include "ICO_Topo_Vertex.h"
#include "ICO_Pnt.h"

using namespace OCCTK::OCC::Topo;
using namespace OCCTK::OCC::gp;

namespace OCCTK {
namespace OCC {
namespace BRepBuilderAPI {

MakeEdge::MakeEdge() {
	myMaker = new BRepBuilderAPI_MakeEdge();
}

MakeEdge::MakeEdge(Pnt^ P1, Pnt^ P2) {
	myMaker = new BRepBuilderAPI_MakeEdge(P1->GetOCC(), P2->GetOCC());
}

MakeEdge::MakeEdge(Topo::TVertex^ P1, Topo::TVertex^ P2) {
	myMaker = new BRepBuilderAPI_MakeEdge(P1->GetOCC(), P2->GetOCC());
}

TShape^ MakeEdge::Shape() {
	try {
		return gcnew TShape(myMaker->Shape());
	}
	catch (Standard_Failure e) {
		throw gcnew System::Exception(gcnew System::String(e.GetMessageString()));
	}
}

TEdge^ MakeEdge::Edge() {
	try {
		return gcnew TEdge(myMaker->Edge());
	}
	catch (Standard_Failure e) {
		throw gcnew System::Exception(gcnew System::String(e.GetMessageString()));
	}
}

bool MakeEdge::Error() {
	BRepBuilderAPI_EdgeError state = myMaker->Error();
	if (state == BRepBuilderAPI_EdgeDone) {
		return true;
	}
	if (state == BRepBuilderAPI_PointProjectionFailed) {
		throw gcnew System::Exception(gcnew System::String("Point Projection Failed"));
	}
	if (state == BRepBuilderAPI_ParameterOutOfRange) {
		throw gcnew System::Exception(gcnew System::String("Parameter Out Of Range"));
	}
	if (state == BRepBuilderAPI_DifferentPointsOnClosedCurve) {
		throw gcnew System::Exception(gcnew System::String("Different Points On Closed Curve"));
	}
	if (state == BRepBuilderAPI_PointWithInfiniteParameter) {
		throw gcnew System::Exception(gcnew System::String("Point With Infinite Parameter"));
	}
	if (state == BRepBuilderAPI_DifferentsPointAndParameter) {
		throw gcnew System::Exception(gcnew System::String("Differents Point And Parameter"));
	}
	if (state == BRepBuilderAPI_LineThroughIdenticPoints) {
		throw gcnew System::Exception(gcnew System::String("Line Through Identic Points"));
	}
	return false;
}

}
}
}