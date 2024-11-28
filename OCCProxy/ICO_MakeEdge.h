#pragma once
#include <BRepBuilderAPI_MakeEdge.hxx>

namespace OCCTK {
namespace OCC {
namespace gp {
ref class Trsf;
ref class Circle;
value struct Pnt;
}
namespace Topo {
ref class TShape;
ref class TVertex;
ref class TEdge;
}
namespace Geom {
ref class Curve;
}
}
}

namespace OCCTK {
namespace OCC {
namespace BRepBuilderAPI {

public ref class MakeEdge {
public:
	MakeEdge();
	MakeEdge(gp::Pnt p1, gp::Pnt p2);
	MakeEdge(gp::Circle^ circle);
	MakeEdge(gp::Circle^ circle, gp::Pnt p1, gp::Pnt p2);
	MakeEdge(Topo::TVertex^ p1, Topo::TVertex^ p2);
	MakeEdge(Geom::Curve^ curve, double min, double max);
public:
	Topo::TShape^ Shape();
	Topo::TEdge^ Edge();
	//! 隐式转换为 TEdge
	static operator Topo::TEdge ^ (MakeEdge^ makeEdge) {
		return makeEdge->Edge();  // 隐式调用 Edge 方法
	}
	bool Error();
	bool IsDone();
private:
	BRepBuilderAPI_MakeEdge* myMaker;
protected:
	// 析构函数用于清理非托管资源
	!MakeEdge() {
		if (myMaker != 0) {
			delete myMaker;
			myMaker = 0;
		}
	}

	// 终结器（finalizer）用于垃圾回收时的清理
	~MakeEdge() {
		// 调用析构函数来清理非托管资源
		this->!MakeEdge();
	}
};

}
}
}