#include "ICO_MakePipe.h"
#include <BRepOffsetAPI_MakePipe.hxx>
//local
#include "ICO_Topo_Wire.h"
#include "ICO_Topo_Shape.h"

using namespace OCCTK::OCC::Topo;

namespace OCCTK {
namespace OCC {
namespace BRepOffsetAPI {

OCCTK::OCC::BRepOffsetAPI::MakePipe::MakePipe(TWire^ wire, TShape^ shape) {
	myMaker = new BRepOffsetAPI_MakePipe(wire->GetOCC(), shape->GetOCC());
}

TShape^ MakePipe::Shape() {
	return gcnew TShape(myMaker->Shape());
}

}
}
}