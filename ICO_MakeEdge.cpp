#include "ICO_MakeEdge.h"
#include <Standard_Failure.hxx>
#include <BRepBuilderAPI_EdgeError.hxx>
#include <gp_Circ.hxx>
//Local
#include "ICO_Topo_Shape.h"
#include "ICO_Topo_Edge.h"
#include "ICO_Geom_Curve.h"
#include "ICO_Topo_Vertex.h"
#include "ICO_Pnt.h"
#include "ICO_Circle.h"

using namespace OCCTK::OCC::Topo;
using namespace OCCTK::OCC::Geom;
using namespace OCCTK::OCC::gp;

namespace OCCTK {
namespace OCC {
namespace BRepBuilderAPI {

MakeEdge::MakeEdge() {
	myMaker = new BRepBuilderAPI_MakeEdge();
}

MakeEdge::MakeEdge(Pnt^ p1, Pnt^ p2) {
	myMaker = new BRepBuilderAPI_MakeEdge(p1->GetOCC(), p2->GetOCC());
}

MakeEdge::MakeEdge(gp::Circle^ circle) {
	myMaker = new BRepBuilderAPI_MakeEdge(circle->GetOCC());
}

MakeEdge::MakeEdge(gp::Circle^ circle, gp::Pnt^ p1, gp::Pnt^ p2) {
	myMaker = new BRepBuilderAPI_MakeEdge(circle->GetOCC(), p1->GetOCC(), p2->GetOCC());
}

MakeEdge::MakeEdge(Topo::TVertex^ p1, Topo::TVertex^ p2) {
	myMaker = new BRepBuilderAPI_MakeEdge(p1->GetOCC(), p2->GetOCC());
}

MakeEdge::MakeEdge(Geom::Curve^ curve, double p1, double p2) {
	myMaker = new BRepBuilderAPI_MakeEdge(curve->GetOCC(), p1, p2);
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

bool MakeEdge::IsDone() {
	return myMaker->IsDone();
}

}
}
}