#include "ICO_Transform.h"

namespace OCCTK {
namespace OCC {
namespace BRepBuilderAPI {

Transform::Transform(TopoDS::TShape^ theShape, Trsf^ theTrsf) {
	gp_Trsf theT = theTrsf->GetOCC();
	myT = new BRepBuilderAPI_Transform(theShape->GetOCC(), theTrsf->GetOCC());
}

TopoDS::TShape^ Transform::Shape() {
	return gcnew TopoDS::TShape(myT->Shape());
}

}
}
}