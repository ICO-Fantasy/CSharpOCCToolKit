#pragma once
#include <AIS_InteractiveObject.hxx>
#include <Standard_Handle.hxx>
//包装C++类到托管类
#include <NCollection_Haft.h> 
namespace OCCTK
{
namespace OCC
{
namespace AIS {

public ref class WAIS_InteractiveObject abstract
{
public:
	WAIS_InteractiveObject();
	WAIS_InteractiveObject(Handle(Standard_Transient)nativeHandle);
	bool IsNull();
	// 纯虚函数，必须在派生类中实现
	virtual Handle(Standard_Transient) GetStd() abstract;
protected:
	NCollection_Haft<Handle(Standard_Transient)> nativeHandle;
};

}
}
}

