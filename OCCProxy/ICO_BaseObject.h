#pragma once
#include <Standard_Handle.hxx>
//包装C++类到托管类
#include <NCollection_Haft.h> 

namespace OCCTK {
namespace OCC {

public ref class BaseObject {
public:
    BaseObject() { myHandle() = new Standard_Transient(); }
    BaseObject(const Handle(Standard_Transient)& nativeHandle) { myHandle() = nativeHandle; }
    property Handle(Standard_Transient) NativeHandle {
        Handle(Standard_Transient) get() { return myHandle(); }
        void set(Handle(Standard_Transient) handle) { myHandle() = handle; }
    };
public:
    bool IsNull() { return myHandle().IsNull(); }
protected:
    NCollection_Haft<Handle(Standard_Transient)> myHandle;
};

}
}