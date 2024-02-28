#pragma once
#include <Standard_Handle.hxx>
//包装C++类到托管类
#include <NCollection_Haft.h> 

//namespace OCCTK::OCC::AIS{
//    public ref class InteractiveObject{
//    public:
//        InteractiveObject(Handle(AIS_InteractiveObject) aAISObject);
//        // 重载 operator() 方法，使其返回原始的非托管对象
//    private:
//        NCollection_Haft<Handle(AIS_InteractiveObject)> myAISObject;
//    };
//}
