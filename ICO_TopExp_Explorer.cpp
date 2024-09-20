#include "ICO_TopExp_Explorer.h"
#include <TopAbs_ShapeEnum.hxx>
//local
#include "ICO_ShapeEnum.h"
#include "ICO_Topo_Shape.h"

using namespace OCCTK::OCC::Topo;
using namespace OCCTK::OCC::TopoAbs;

namespace OCCTK {
namespace OCC {
namespace TopExp {

Explorer::Explorer() {
	myExplorer = new TopExp_Explorer();
}

Explorer::Explorer(TShape^ shape, ShapeEnum toFind) {
	myExplorer = new TopExp_Explorer(shape->GetOCC(), TopAbs_ShapeEnum(toFind));
}

Explorer::Explorer(Topo::TShape^ shape, TopoAbs::ShapeEnum toFind, TopoAbs::ShapeEnum toAvoid) {
	myExplorer = new TopExp_Explorer(shape->GetOCC(), TopAbs_ShapeEnum(toFind), TopAbs_ShapeEnum(toAvoid));
}

bool Explorer::More() {
	return myExplorer->More();
}

void Explorer::Next() {
	myExplorer->Next();
}

Topo::TShape^ Explorer::Current() {
	return gcnew TShape(myExplorer->Current());
}

}
}
}