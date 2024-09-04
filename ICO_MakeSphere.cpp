#include "ICO_MakeSphere.h"
#include <BRepPrimAPI_MakeSphere.hxx>

namespace OCCTK {
namespace OCC {
namespace BRepPrimAPI {

MakeSphere::MakeSphere(double X, double Y, double Z, double R) {
	myMaker = new BRepPrimAPI_MakeSphere(gp_Pnt(X, Y, Z), R);
}

MakeSphere::MakeSphere(Pnt^ originPnt, double R) {
	myMaker = new BRepPrimAPI_MakeSphere(originPnt->GetOCC(), R);
}

MakeSphere::MakeSphere(gp::Ax2^ theAxis, double R) {
	myMaker = new BRepPrimAPI_MakeSphere(theAxis->GetOCC(), R);
}

TShape^ MakeSphere::Shape() {
	return gcnew TShape(myMaker->Shape());
}


}
}
}
