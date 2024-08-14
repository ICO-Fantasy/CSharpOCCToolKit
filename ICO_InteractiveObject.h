#pragma once
#include <AIS_InteractiveObject.hxx>
#include <Standard_Handle.hxx>
//包装C++类到托管类
#include <NCollection_Haft.h> 
//Local
#include "ICO_BaseObject.h"

namespace OCCTK {
namespace OCC {
namespace AIS {

public ref class InteractiveObject : BaseObject {
public:
	InteractiveObject();
	InteractiveObject(Handle(AIS_InteractiveObject) theAISObject);
	InteractiveObject(Handle(Standard_Transient)nativeHandle);
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

