#pragma once

class GeomAPI_ProjectPointOnCurve;
namespace OCCTK {
namespace OCC {
namespace gp {
value struct Pnt;
}
namespace Geom {
ref class Curve;
}
}
}


namespace OCCTK {
namespace OCC {
namespace GeomAPI {

public ref class ProjectPointOnCurve {
public:
	ProjectPointOnCurve(gp::Pnt p, Geom::Curve^ curve);
	ProjectPointOnCurve(gp::Pnt p, Geom::Curve^ curve, double inferior, double superior);
	GeomAPI_ProjectPointOnCurve GetOCC();
public:
	double LowerDistanceParameter();
private:
	GeomAPI_ProjectPointOnCurve* myPPC;
private:
	// 析构函数用于清理非托管资源
	!ProjectPointOnCurve() {
		if (myPPC != 0) {
			delete myPPC;
			myPPC = 0;
		}
	}

	// 终结器（finalizer）用于垃圾回收时的清理
	~ProjectPointOnCurve() {
		// 调用析构函数来清理非托管资源
		this->!ProjectPointOnCurve();
	}
};

}
}
}

