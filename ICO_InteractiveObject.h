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
	Handle(AIS_InteractiveObject) GetOCC() { return Handle(AIS_InteractiveObject)::DownCast(NativeHandle); };
	bool IsShape();
	void RemoveSelf();
};

}
}
}

