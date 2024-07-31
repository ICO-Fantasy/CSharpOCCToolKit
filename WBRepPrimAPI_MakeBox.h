#pragma once
#include <BRepPrimAPI_MakeBox.hxx>
//包装C++类到托管类
#include <NCollection_Haft.h> 

#include "WTopoDS_Shape.h"
using namespace OCCTK::OCC::TopoDS;

namespace OCCTK {
namespace OCC {
namespace BRepPrimAPI {

public ref class WBRepPrimAPI_MakeBox
{
public:
	WBRepPrimAPI_MakeBox(double X, double Y, double Z);
	TopoDS::WTopoDS_Shape^ Shape();
private:
	BRepPrimAPI_MakeBox* myMaker;
};

}
}
}