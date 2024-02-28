#include <BRepPrimAPI_MakeBox.hxx>
#include <TopoDS_Shape.hxx>
#include <AIS_Shape.hxx>
//包装C++类到托管类
#include <NCollection_Haft.h> 

#include "InteractiveObject.cpp"
#include "test.h"

namespace OCCTK::TEST{
    void TEST::MakeBox(){
        TopoDS_Shape aShape = BRepPrimAPI_MakeBox(10,10,10).Shape();
    }
}
