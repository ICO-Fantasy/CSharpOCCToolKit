#include "ICO_Transform.h"

namespace OCCTK {
namespace OCC {
namespace BRepBuilderAPI {

Transform::Transform(TopoDS::Shape^ theShape, Trsf^ theTrsf) {
	gp_Trsf theT = theTrsf->GetOCC();
	myT = new BRepBuilderAPI_Transform(theShape->GetOCC(), theTrsf->GetOCC());
}

TopoDS::Shape^ Transform::Shape() {
	return gcnew TopoDS::Shape(myT->Shape());
}

}
}
}