#include "ICO_MakePolygon.h"
//local
#include "ICO_Pnt.h"
#include "ICO_Topo_Shape.h"
#include "ICO_Topo_Edge.h"
#include "ICO_Topo_Wire.h"
#include "ICO_Exception.h"

using namespace OCCTK::OCC::gp;
using namespace OCCTK::OCC::Topo;


namespace OCCTK {
namespace OCC {
namespace BRepBuilderAPI {

MakePolygon::MakePolygon() {
	myMaker = new BRepBuilderAPI_MakePolygon();
}

MakePolygon::MakePolygon(gp::Pnt p1, gp::Pnt p2) {
	myMaker = new BRepBuilderAPI_MakePolygon(p1, p2);
}

MakePolygon::MakePolygon(gp::Pnt p1, gp::Pnt p2, gp::Pnt p3) {
	myMaker = new BRepBuilderAPI_MakePolygon(p1, p2, p3);
}
MakePolygon::MakePolygon(gp::Pnt p1, gp::Pnt p2, gp::Pnt p3, bool close) {
	myMaker = new BRepBuilderAPI_MakePolygon(p1, p2, p3, close);
}

MakePolygon::MakePolygon(gp::Pnt p1, gp::Pnt p2, gp::Pnt p3, gp::Pnt p4) {
	myMaker = new BRepBuilderAPI_MakePolygon(p1, p2, p3, p4);
}
MakePolygon::MakePolygon(gp::Pnt p1, gp::Pnt p2, gp::Pnt p3, gp::Pnt p4, bool close) {
	myMaker = new BRepBuilderAPI_MakePolygon(p1, p2, p3, p4, close);
}

Topo::TShape^ MakePolygon::Shape() {
	try {
		return gcnew TShape(myMaker->Shape());
	}
	CATCH_AND_THROW_OCC_EXCEPTIONS
}

Topo::TEdge^ MakePolygon::Edge() {
	try {
		return gcnew TEdge(myMaker->Edge());
	}
	CATCH_AND_THROW_OCC_EXCEPTIONS
}

Topo::TWire^ MakePolygon::Wire() {
	try {
		return gcnew TWire(myMaker->Wire());
	}
	CATCH_AND_THROW_OCC_EXCEPTIONS
}

}
}
}