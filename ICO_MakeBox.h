#pragma once
#include <BRepPrimAPI_MakeBox.hxx>
//包装C++类到托管类
#include <NCollection_Haft.h> 
//Local
#include "ICO_TopoDS_Shape.h"
using namespace OCCTK::OCC::TopoDS;

namespace OCCTK {
namespace OCC {
namespace BRepPrimAPI {

public ref class MakeBox {
public:
	MakeBox(double X, double Y, double Z);
	TopoDS::TShape^ Shape();
private:
	BRepPrimAPI_MakeBox* myMaker;
};

}
}
}