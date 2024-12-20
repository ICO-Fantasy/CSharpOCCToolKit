﻿#pragma once
#include<Poly_Triangulation.hxx>
//local
#include "ICO_BaseObject.h"

namespace OCCTK {
namespace OCC {
namespace Poly {

public ref class PTriangulation :public BaseObject {
private:
	Handle(Poly_Triangulation) myPT();
public:
	PTriangulation(const Handle(Poly_Triangulation) polyTriangulation);
	//! 隐式转换为 Handle(Poly_Triangulation)
	static operator Handle(Poly_Triangulation) (PTriangulation^ pt) { return pt->GetOCC(); }
public:
	Handle(Poly_Triangulation) GetOCC() { return myPT(); }
};

}
}
}
