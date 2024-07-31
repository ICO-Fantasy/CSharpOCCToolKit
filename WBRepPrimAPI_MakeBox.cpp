#include "WBRepPrimAPI_MakeBox.h"

namespace OCCTK {
namespace OCC {
namespace BRepPrimAPI {

WBRepPrimAPI_MakeBox::WBRepPrimAPI_MakeBox(double dX, double dY, double dZ) {
	myMaker = new BRepPrimAPI_MakeBox(dX, dY, dZ);
}

WTopoDS_Shape^ WBRepPrimAPI_MakeBox::Shape()
{
	return gcnew WTopoDS_Shape(myMaker->Shape());
}

}
}
}