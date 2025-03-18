#pragma once
#include <Standard_Handle.hxx>
//包装C++类到托管类
#include <NCollection_Haft.h> 

namespace OCCTK {
namespace OCC {

public ref class BaseObject {
public:
    BaseObject() {
        myHandle() = new Standard_Transient();
    }
    BaseObject(const Handle(Standard_Transient)& nativeHandle) {
        myHandle() = nativeHandle;
    }
    bool IsNull() {
        return myHandle().IsNull();
    }

    /// <summary>
    /// 获取Handle的指针地址并委托为C#指针
    /// </summary>
    /// <returns></returns>
    /// <remarks>
    /// !!! 不要轻易使用指针传递 !!!
    /// </remarks>
    System::IntPtr GetIntPtr() {
        return System::IntPtr(&myHandle());
    }

    property Handle(Standard_Transient) NativeHandle {
        Handle(Standard_Transient) get() { return myHandle(); } void set(Handle(Standard_Transient) handle) { myHandle() = handle; }
    };
protected:
    NCollection_Haft<Handle(Standard_Transient)> myHandle;
};

}
}