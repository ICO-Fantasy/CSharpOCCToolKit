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
	InteractiveObject() :BaseObject() {}
	InteractiveObject(Handle(AIS_InteractiveObject) theAISObject);
	bool IsShape();
	Handle(AIS_InteractiveObject) GetOCC() { return Handle(AIS_InteractiveObject)::DownCast(NativeHandle); };
	//protected:
	//	// 析构函数用于清理非托管资源
	//	!InteractiveObject() {
	//		myAISObject() = 0;
	//	}
	//
	//	// 终结器（finalizer）用于垃圾回收时的清理
	//	~InteractiveObject() {
	//		// 调用析构函数来清理非托管资源
	//		this->!InteractiveObject();
	//	}
};

}
}
}

