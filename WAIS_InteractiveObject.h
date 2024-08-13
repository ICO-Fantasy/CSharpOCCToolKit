#pragma once
#include <AIS_InteractiveObject.hxx>
#include <Standard_Handle.hxx>
//包装C++类到托管类
#include <NCollection_Haft.h> 

#include "WBaseObject.h"
namespace OCCTK {
namespace OCC {
namespace AIS {

public ref class WAIS_InteractiveObject : WBaseObject {
public:
	WAIS_InteractiveObject();
	WAIS_InteractiveObject(Handle(AIS_InteractiveObject) theAISObject);
	WAIS_InteractiveObject(Handle(Standard_Transient)nativeHandle);
	bool IsNull();
	Handle(AIS_InteractiveObject) GetOCC();
	// 实现虚函数
	Handle(Standard_Transient) GetStd() override;
protected:
	NCollection_Haft<Handle(AIS_InteractiveObject)>myAISObject;
};

}
}
}

