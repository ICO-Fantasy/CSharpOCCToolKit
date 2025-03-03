#include "ICO_Transform.h"
//Local
#include "ICO_Topo_Shape.h"
#include "ICO_Trsf.h"
namespace OCCTK {
namespace OCC {
namespace BRepBuilderAPI {

Transform::Transform(Topo::TShape^ theShape, gp::Trsf theTrsf) {
	myT = new BRepBuilderAPI_Transform(theShape->GetOCC(), theTrsf);
}

Topo::TShape^ Transform::Shape() {
	return gcnew Topo::TShape(myT->Shape());
}

}
}
}