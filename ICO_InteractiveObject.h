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
namespace gp {
ref class Trsf;
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
	void RemoveSelf(bool update);
public:
	void SetLocalTransformation(gp::Trsf^ trsf);
	gp::Trsf^ LocalTransformation();
	bool IsShape();
	bool HasInteractiveContext();
	InteractiveContext^ GetContext();
};

}
}
}

