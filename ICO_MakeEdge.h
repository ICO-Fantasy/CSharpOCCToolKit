#pragma once
#include <BRepBuilderAPI_MakeEdge.hxx>

namespace OCCTK {
namespace OCC {
namespace gp {
ref class Trsf;
ref class Pnt;
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
	MakeEdge(gp::Pnt^ P1, gp::Pnt^ P2);
	MakeEdge(Topo::TVertex^ P1, Topo::TVertex^ P2);
	MakeEdge(Geom::Curve^ curve, double min, double max);
public:
	Topo::TShape^ Shape();
	Topo::TEdge^ Edge();
	//! 隐式转换为 TEdge
	static operator Topo::TEdge ^ (MakeEdge^ makeEdge) {
		return makeEdge->Edge();  // 隐式调用 Edge 方法
	}
	bool Error();
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