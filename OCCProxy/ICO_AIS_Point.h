﻿#pragma once
#include <AIS_InteractiveObject.hxx>
#include <AIS_Point.hxx>
//Local
#include "ICO_InteractiveObject.h"

namespace OCCTK {
namespace OCC {
namespace gp {
value struct Trsf;
value struct Pnt;
}
namespace Aspect {
enum class TypeOfMarker;
}
}
}

namespace OCCTK {
namespace OCC {
namespace AIS {

public ref class APoint :public InteractiveObject {
private:
	Handle(AIS_Point) myPnt() { return Handle(AIS_Point)::DownCast(NativeHandle); }
public:
	APoint(const Handle(AIS_Point) aPnt) : InteractiveObject(aPnt) {};
	APoint(gp::Pnt aPnt);
public:
	void SetWidth(double width);
	void SetMakerType(Aspect::TypeOfMarker type);
public:
	Handle(AIS_Point) GetOCC() { return myPnt(); }
};

}
}
}
