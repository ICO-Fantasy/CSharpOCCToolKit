#pragma once
#include <BRepPrimAPI_MakeSphere.hxx>
//Local
#include "ICO_TopoDS_Shape.h"
#include "ICO_Pnt.h"
#include "ICO_Ax2.h"

using namespace OCCTK::OCC::TopoDS;

namespace OCCTK {
namespace OCC {
namespace BRepPrimAPI {

public ref class MakeSphere {
public:
	MakeSphere(double X, double Y, double Z, double R);
	MakeSphere(Pnt^ originPnt, double R);
	MakeSphere(gp::Ax2^ theAxis, double R);
	TShape^ Shape();
private:
	BRepPrimAPI_MakeSphere* myMaker;
};

}
}
}