#include "ICO_Feat_MakeRevol.h"
//local
#include "ICO_Topo_Shape.h"
#include "ICO_Topo_Face.h"
#include "ICO_Ax1.h"
#include "ICO_Exception.h"
#include "ICO_BRepPrimAPI_MakeRevol.h"
using namespace OCCTK::OCC::Topo;
using namespace OCCTK::OCC::gp;

namespace OCCTK {
namespace OCC {
namespace BRepFeat {

MakeRevol::MakeRevol() {
	myMaker = new BRepFeat_MakeRevol();
}

MakeRevol::MakeRevol(Topo::TShape^ sBase, Topo::TShape^ pBase, Topo::TFace^ skFace, gp::Ax1 axis, int fuse, bool modify) {
	myMaker = new BRepFeat_MakeRevol(sBase->GetOCC(), pBase->GetOCC(), skFace->GetOCC(), axis, fuse, modify);
}

Topo::TShape^ MakeRevol::Shape() {
	try {
		return gcnew TShape(myMaker->Shape());
	}
	CATCH_AND_THROW_OCC_EXCEPTIONS
}

}
}
}