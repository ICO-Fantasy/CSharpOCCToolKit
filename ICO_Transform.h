#pragma once
#include <BRepBuilderAPI_Transform.hxx>
//包装C++类到托管类
#include <NCollection_Haft.h> 
//Local
#include "ICO_TopoDS_Shape.h"

namespace OCCTK {
namespace OCC {
namespace BRepBuilderAPI {

public ref class Transform {
public:
	Transform(TopoDS::Shape^ theShape, Trsf^ theTrsf);
	TopoDS::Shape^ Shape();
private:
	BRepBuilderAPI_Transform* myT;
};

}
}
}