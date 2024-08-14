#include "ICO_MakeBox.h"
#include "ICO_TopoDS_Shape.h"
using namespace OCCTK::OCC::TopoDS;

namespace OCCTK {
namespace OCC {
namespace BRepPrimAPI {

MakeBox::MakeBox(double dX, double dY, double dZ) {
	myMaker = new BRepPrimAPI_MakeBox(dX, dY, dZ);
}

TopoDS::Shape^ MakeBox::Shape() {
	return gcnew TopoDS::Shape(myMaker->Shape());
}

}
}
}