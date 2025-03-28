﻿#pragma once
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
value struct Trsf;
}
namespace Graphic3d {
enum class ZLayerId;
}
}
}

namespace OCCTK {
namespace OCC {
namespace AIS {

public ref class InteractiveObject : BaseObject {
private:
    //动态转换，类方法中使用转换后的 Handle(AIS_InteractiveObject) 而非 NativeHandle
    Handle(AIS_InteractiveObject) myObj() { return Handle(AIS_InteractiveObject)::DownCast(NativeHandle); }
public:
    InteractiveObject() :BaseObject() {}
    InteractiveObject(Handle(AIS_InteractiveObject) theAISObject);
    Handle(AIS_InteractiveObject) GetOCC() { return Handle(AIS_InteractiveObject)::DownCast(NativeHandle); };
    //! 隐式转换为 Handle(AIS_InteractiveObject)
    static operator Handle(AIS_InteractiveObject) (InteractiveObject^ ais) { return ais->GetOCC(); }
public:
    void RemoveSelf(bool update);
    void SetLocalTransformation(gp::Trsf trsf);
    void SetZLayer(Graphic3d::ZLayerId id);
    void SetZLayer(int Zlayerid);
    gp::Trsf LocalTransformation();
    bool IsShape();
    bool HasInteractiveContext();
    InteractiveContext^ GetContext();
};

}
}
}

