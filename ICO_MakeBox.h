#pragma once
#include <BRepPrimAPI_MakeBox.hxx>
//Local
#include "ICO_TopoDS_Shape.h"
#include "ICO_Pnt.h"
#include "ICO_Ax2.h"

using namespace OCCTK::OCC::TopoDS;

namespace OCCTK {
namespace OCC {
namespace BRepPrimAPI {

public ref class MakeBox {
public:
	MakeBox(double X, double Y, double Z);
	MakeBox(Pnt^ left, Pnt^ right);
	MakeBox(Ax2^ axis, double ldX, double ldY, double ldZ, double rdX, double rdY, double rdZ);
	TopoDS::TShape^ Shape();
private:
	BRepPrimAPI_MakeBox* myMaker;
};

}
}
}