#pragma once
#include <Standard_Handle.hxx>

//包装C++类到托管类
#include <NCollection_Haft.h> 
namespace OCCTK {
namespace OCC {

public ref class WBaseObject abstract
{
public:
	// 纯虚函数，必须在派生类中实现
	virtual Handle(Standard_Transient) GetStd() abstract;
protected:
	NCollection_Haft<Handle(Standard_Transient)> nativeHandle;
};

}
}