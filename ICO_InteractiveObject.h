#pragma once
#include <AIS_InteractiveObject.hxx>
//Local
#include "ICO_BaseObject.h"


namespace OCCTK {
namespace OCC {
namespace AIS {
ref class AShape;
ref class InteractiveContext;
}
}
}

namespace OCCTK {
namespace OCC {
namespace AIS {

public ref class InteractiveObject : BaseObject {
private:
	Handle(AIS_InteractiveObject) myObj() { return Handle(AIS_InteractiveObject)::DownCast(NativeHandle); }
public:
	InteractiveObject() :BaseObject() {}
	InteractiveObject(Handle(AIS_InteractiveObject) theAISObject);
	Handle(AIS_InteractiveObject) GetOCC() { return Handle(AIS_InteractiveObject)::DownCast(NativeHandle); };
	bool IsShape();
	void RemoveSelf(bool update);
	bool HasInteractiveContext();
	InteractiveContext^ GetContext();
};

}
}
}

