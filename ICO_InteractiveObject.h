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
ref class AShape;
}
}
}

namespace OCCTK {
namespace OCC {
namespace AIS {

public ref class InteractiveObject : BaseObject {
public:
	InteractiveObject(Handle(AIS_InteractiveObject) theAISObject);
	bool IsNull();
	bool IsShape();
	AShape^ AsShape();
	Handle(AIS_InteractiveObject) GetOCC();
	Handle(Standard_Transient) GetStd() override;
protected:
	NCollection_Haft<Handle(AIS_InteractiveObject)> myAISObject;
protected:
	// 析构函数用于清理非托管资源
	!InteractiveObject() {
		myAISObject() = 0;
		myHandle() = 0;
	}

	// 终结器（finalizer）用于垃圾回收时的清理
	~InteractiveObject() {
		// 调用析构函数来清理非托管资源
		this->!InteractiveObject();
	}
};

}
}
}

